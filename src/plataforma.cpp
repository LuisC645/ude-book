#include <iostream>
#include <random>
#include "plataforma.h"
#include "menu.h"
#include "medidor.h"
#include "texto.h"

using namespace std;

const char* const Plataforma::SUBRUTA = "udeabook";
const char* const Plataforma::RUTA_DATOS = "data/";

const int Plataforma::OK;
const int Plataforma::NO_EXISTE;
const int Plataforma::MISMO_USUARIO;
const int Plataforma::YA_AMIGOS;
const int Plataforma::SOLICITUD_RECIBIDA;
const int Plataforma::SOLICITUD_ENVIADA;
const int Plataforma::NO_ES_AMIGO;
const int Plataforma::TEXTO_VACIO;
const int Plataforma::TEXTO_LARGO;
const int Plataforma::TAM_GRUPO;
const int Plataforma::MIN_AMIGOS_ALTERNAR;

Plataforma::Plataforma() : reloj(1, 1, 2026, 0, 0), actual(-1), alternado(false) {}

// ---------------------------------------------------------------------------
// Ejecucion y carga (RF-I)
// ---------------------------------------------------------------------------

int Plataforma::ejecutar() {
    Medidor::reiniciar();
    cout << "Cargando datos desde " << RUTA_DATOS << " ...\n";
    if (!cargar()) {
        cout << "No se pudo iniciar Ude@Book: corrija los datos e intente de nuevo.\n";
        return 1;
    }
    cout << "Datos cargados: " << usuarios.tamano() << " usuarios, " << mensajes.tamano() << " mensajes.\n";
    Medidor::reportar(memoriaTotal());

    Menu menu(*this);
    menu.ejecutar();
    return 0;
}

bool Plataforma::cargar() {
    // Semilla real del generador de aleatorios.
    random_device semilla;
    Medidor::externoAleatorio();
    generador.seed(semilla());
    Medidor::registrarLocal(sizeof(semilla));

    persistencia.fijarCarpeta(RUTA_DATOS);
    if (!persistencia.cargarUsuarios(usuarios)) {
        return false;
    }
    amistades.inicializar(usuarios.tamano());
    if (!persistencia.cargarAmistades(amistades) || !persistencia.cargarSolicitudes(usuarios)) {
        return false;
    }

    // Tablas temporales: numero de album -> (dueno, posicion entre sus albumes).
    Vector<int> albumDueno;
    Vector<int> albumPosicion;
    if (!persistencia.cargarAlbumes(usuarios, albumDueno, albumPosicion)
        || !persistencia.cargarFotos(usuarios, albumDueno, albumPosicion)) {
        return false;
    }
    Medidor::registrarLocal(sizeof(albumDueno) + albumDueno.memoriaArreglo()
                            + sizeof(albumPosicion) + albumPosicion.memoriaArreglo());

    if (!persistencia.cargarMensajes(usuarios, mensajes)) {
        return false;
    }
    // Los mensajes nuevos se fechan despues del mas reciente cargado.
    if (mensajes.tamano() > 0) {
        reloj = mensajes[mensajes.tamano() - 1].getFecha();
    }
    return true;
}

// ---------------------------------------------------------------------------
// Busqueda de usuarios
// ---------------------------------------------------------------------------

int Plataforma::buscarUsuario(const string& nombre) const {
    int bajo = 0;
    int alto = usuarios.tamano() - 1;
    while (bajo <= alto) {
        Medidor::contar();
        int medio = (bajo + alto) / 2;
        const string& actualNombre = usuarios[medio].getNombre();
        if (actualNombre == nombre) {
            return medio;
        }
        if (actualNombre < nombre) {
            bajo = medio + 1;
        } else {
            alto = medio - 1;
        }
    }
    return -1;
}

const Usuario& Plataforma::buscarUsuario(int indice) const {
    return usuarios[indice];
}

// ---------------------------------------------------------------------------
// RF-II: ingreso y mejores amigos
// ---------------------------------------------------------------------------

int Plataforma::pesoPorComunes(int comunes) {
    if (comunes <= 10) {
        return 2;
    }
    if (comunes <= 20) {
        return 5;
    }
    return 7;
}

// Ruleta ponderada: devuelve la posicion elegida entre los amigos no usados, o -1.
int Plataforma::girarRuleta(const Vector<bool>& usado) {
    int total = 0;
    for (int i = 0; i < pesosSesion.tamano(); i++) {
        Medidor::contar();
        if (!usado[i]) {
            total += pesosSesion[i];
        }
    }
    if (total == 0) {
        return -1;
    }
    uniform_int_distribution<int> distribucion(0, total - 1);
    Medidor::externoAleatorio();
    int r = distribucion(generador);
    int acumulado = 0;
    for (int i = 0; i < pesosSesion.tamano(); i++) {
        Medidor::contar();
        if (!usado[i]) {
            acumulado += pesosSesion[i];
            if (r < acumulado) {
                return i;
            }
        }
    }
    return -1;
}

bool Plataforma::iniciarSesion(const string& nombre, const string& contrasena) {
    int u = buscarUsuario(nombre);
    if (u < 0 || !usuarios[u].contrasenaCorrecta(contrasena)) {
        return false;
    }
    actual = u;
    alternado = false;
    likesSesion.vaciar();
    grupo.vaciar();

    // Amigos y pesos se calculan una sola vez por sesion.
    amistades.amigosDe(u, amigosSesion);
    pesosSesion.vaciar();
    pesosSesion.reservar(amigosSesion.tamano());
    for (int i = 0; i < amigosSesion.tamano(); i++) {
        Medidor::contar();
        pesosSesion.agregar(pesoPorComunes(amistades.comunes(u, amigosSesion[i])));
    }

    // Seleccion de hasta 3 amigos distintos sin reemplazo.
    Vector<bool> usado;
    usado.agregar(false, amigosSesion.tamano());
    int meta = TAM_GRUPO;
    if (amigosSesion.tamano() < meta) {
        meta = amigosSesion.tamano();
    }
    while (grupo.tamano() < meta) {
        Medidor::contar();
        int pos = girarRuleta(usado);
        usado[pos] = true;
        grupo.agregar(pos);
    }
    Medidor::registrarLocal(sizeof(usado) + usado.memoriaArreglo() + sizeof(meta));
    return true;
}

void Plataforma::cerrarSesion() {
    actual = -1;
    alternado = false;
    amigosSesion.vaciar();
    pesosSesion.vaciar();
    grupo.vaciar();
    likesSesion.vaciar();
}

int Plataforma::getActual() const {
    return actual;
}

int Plataforma::cantidadAmigos() const {
    return amigosSesion.tamano();
}

int Plataforma::tamanoGrupo() const {
    return grupo.tamano();
}

int Plataforma::amigoDelGrupo(int i) const {
    return amigosSesion[grupo[i]];
}

bool Plataforma::yaAlterno() const {
    return alternado;
}

bool Plataforma::puedeAlternar() const {
    return !alternado && amigosSesion.tamano() >= MIN_AMIGOS_ALTERNAR;
}

// Opcion N: nuevo grupo distinto del primero, sin repetidos internos.
bool Plataforma::alternarGrupo() {
    if (!puedeAlternar()) {
        return false;
    }
    int f = amigosSesion.tamano();
    Vector<bool> usado;
    Vector<bool> enPrimero;
    usado.agregar(false, f);
    enPrimero.agregar(false, f);
    for (int i = 0; i < grupo.tamano(); i++) {
        Medidor::contar();
        usado[grupo[i]] = true;
        enPrimero[grupo[i]] = true;
    }

    // 1) Sorteo entre los amigos que no estaban en el primer grupo.
    Vector<int> nuevo;
    while (nuevo.tamano() < TAM_GRUPO) {
        Medidor::contar();
        int pos = girarRuleta(usado);
        if (pos < 0) {
            break;
        }
        usado[pos] = true;
        nuevo.agregar(pos);
    }
    // 2) Con 4 o 5 amigos se completa con los del primer grupo
    //    (sin repetir los ya elegidos en el nuevo grupo).
    if (nuevo.tamano() < TAM_GRUPO) {
        for (int i = 0; i < f; i++) {
            Medidor::contar();
            if (enPrimero[i]) {
                usado[i] = false;
            }
        }
        while (nuevo.tamano() < TAM_GRUPO) {
            Medidor::contar();
            int pos = girarRuleta(usado);
            usado[pos] = true;
            nuevo.agregar(pos);
        }
    }
    Medidor::registrarLocal(sizeof(usado) + usado.memoriaArreglo() + sizeof(enPrimero)
                            + enPrimero.memoriaArreglo() + sizeof(nuevo) + nuevo.memoriaArreglo());
    grupo = nuevo;
    alternado = true;
    return true;
}

string Plataforma::rutaAlbumes(int usuario) const {
    return string("/users/") + SUBRUTA + "/" + usuarios[usuario].getNombre() + "/albums/";
}

string Plataforma::rutaPerfil(int usuario) const {
    return rutaAlbumes(usuario) + "profile.png";
}

// ---------------------------------------------------------------------------
// RF-III: solicitudes de amistad
// ---------------------------------------------------------------------------

bool Plataforma::hayPendiente(int a, int b) const {
    return usuarios[a].buscarSolicitudDe(b) >= 0 || usuarios[b].buscarSolicitudDe(a) >= 0;
}

// Las 5 validaciones de III-a, en orden.
int Plataforma::enviarSolicitud(int destino) {
    if (destino < 0) {
        return NO_EXISTE;
    }
    if (destino == actual) {
        return MISMO_USUARIO;
    }
    if (amistades.sonAmigos(actual, destino)) {
        return YA_AMIGOS;
    }
    if (usuarios[actual].buscarSolicitudDe(destino) >= 0) {
        return SOLICITUD_RECIBIDA;
    }
    if (usuarios[destino].buscarSolicitudDe(actual) >= 0) {
        return SOLICITUD_ENVIADA;
    }
    usuarios[destino].agregarSolicitud(actual);
    persistencia.guardarSolicitudes(usuarios);
    return OK;
}

int Plataforma::cantidadSolicitudes() const {
    return usuarios[actual].cantidadSolicitudes();
}

int Plataforma::solicitante(int i) const {
    return usuarios[actual].getSolicitudOrigen(i);
}

void Plataforma::aceptarSolicitud(int i) {
    int origen = usuarios[actual].getSolicitudOrigen(i);
    amistades.agregar(actual, origen);
    usuarios[actual].quitarSolicitud(i);
    persistencia.agregarAmistad(actual, origen);
    persistencia.guardarSolicitudes(usuarios);
}

void Plataforma::rechazarSolicitud(int i) {
    usuarios[actual].quitarSolicitud(i);
    persistencia.guardarSolicitudes(usuarios);
}

// ---------------------------------------------------------------------------
// RF-IV: perfil de un amigo
// ---------------------------------------------------------------------------

int Plataforma::validarAmigo(int usuario) const {
    if (usuario < 0) {
        return NO_EXISTE;
    }
    if (usuario == actual) {
        return MISMO_USUARIO;
    }
    if (!amistades.sonAmigos(actual, usuario)) {
        return NO_ES_AMIGO;
    }
    return OK;
}

void Plataforma::albumesVisibles(int amigo, Vector<int>& salida) const {
    salida.vaciar();
    const Usuario& u = usuarios[amigo];
    for (int i = 0; i < u.cantidadAlbumes(); i++) {
        Medidor::contar();
        if (u.getAlbum(i).esVisibleParaAmigos()) {
            salida.agregar(i);
        }
    }
}

const Album& Plataforma::getAlbum(int usuario, int album) const {
    return usuarios[usuario].getAlbum(album);
}

string Plataforma::rutaFoto(int usuario, int album, int foto) const {
    const Album& a = usuarios[usuario].getAlbum(album);
    return a.getFoto(foto).getRuta(a.getRuta(rutaAlbumes(usuario)));
}

int Plataforma::buscarLike(const Foto* foto) const {
    for (int i = 0; i < likesSesion.tamano(); i++) {
        Medidor::contar();
        if (likesSesion[i] == foto) {
            return i;
        }
    }
    return -1;
}

bool Plataforma::tieneLike(int usuario, int album, int foto) const {
    return buscarLike(&usuarios[usuario].getAlbum(album).getFoto(foto)) >= 0;
}

// Alterna el like. Devuelve true si la foto queda con like.
// La marca de "ya di like" solo dura la sesion; el conteo se guarda en el archivo.
bool Plataforma::alternarLike(int usuario, int album, int foto) {
    Foto& f = usuarios[usuario].getAlbum(album).getFoto(foto);
    int pos = buscarLike(&f);
    bool quedaConLike;
    if (pos >= 0) {
        likesSesion.quitarEn(pos);
        if (f.getLikes() > 0) {
            f.setLikes(f.getLikes() - 1);
        }
        quedaConLike = false;
    } else {
        likesSesion.agregar(&f);
        f.setLikes(f.getLikes() + 1);
        quedaConLike = true;
    }
    persistencia.guardarFotos(usuarios);
    return quedaConLike;
}

// ---------------------------------------------------------------------------
// RF-V: sugerencias de amistad
// ---------------------------------------------------------------------------

// Mas comunes primero; si empatan, orden alfabetico.
bool Plataforma::esMejorCandidato(int comunesA, int usuarioA, int comunesB, int usuarioB) const {
    if (comunesA != comunesB) {
        return comunesA > comunesB;
    }
    return usuarios[usuarioA].getNombre() < usuarios[usuarioB].getNombre();
}

// Candidatos: usuarios que no son yo, ni mis amigos, ni tienen solicitud pendiente
// conmigo, y que tienen al menos un amigo en comun conmigo (amigos de mis amigos).
// Se guardan los k mejores en dos arreglos dinamicos de tamano k, ordenados por
// insercion: cada candidato nuevo entra al fondo y sube mientras sea mejor.
void Plataforma::sugerencias(int k, Vector<int>& candidatos, Vector<int>& comunes) {
    int* mejores = new int[k];
    int* mejoresComunes = new int[k];
    int llenos = 0;
    Medidor::registrarLocal(2ULL * (unsigned long long)k * sizeof(int));

    for (int c = 0; c < usuarios.tamano(); c++) {
        Medidor::contar();
        if (c == actual || amistades.sonAmigos(actual, c) || hayPendiente(actual, c)) {
            continue;
        }
        int com = amistades.comunes(actual, c);
        if (com == 0) {
            continue;
        }
        int pos;
        if (llenos < k) {
            pos = llenos;
            llenos++;
        } else if (esMejorCandidato(com, c, mejoresComunes[k - 1], mejores[k - 1])) {
            pos = k - 1;   // reemplaza al peor
        } else {
            continue;
        }
        mejores[pos] = c;
        mejoresComunes[pos] = com;
        while (pos > 0 && esMejorCandidato(mejoresComunes[pos], mejores[pos],
                                           mejoresComunes[pos - 1], mejores[pos - 1])) {
            Medidor::contar();
            int temporal = mejores[pos];
            mejores[pos] = mejores[pos - 1];
            mejores[pos - 1] = temporal;
            temporal = mejoresComunes[pos];
            mejoresComunes[pos] = mejoresComunes[pos - 1];
            mejoresComunes[pos - 1] = temporal;
            pos--;
        }
    }

    candidatos.vaciar();
    comunes.vaciar();
    for (int i = 0; i < llenos; i++) {
        Medidor::contar();
        candidatos.agregar(mejores[i]);
        comunes.agregar(mejoresComunes[i]);
    }
    delete[] mejores;
    delete[] mejoresComunes;
}

// ---------------------------------------------------------------------------
// RF-VI: mensajes
// ---------------------------------------------------------------------------

int Plataforma::enviarMensaje(int destino, const string& texto) {
    int validacion = validarAmigo(destino);
    if (validacion != OK) {
        return validacion;
    }
    if (Texto::esVacio(texto)) {
        return TEXTO_VACIO;
    }
    if ((int)texto.length() > Mensaje::MAX_TEXTO) {
        return TEXTO_LARGO;
    }
    // Fecha de envio: hora local del sistema. Si el reloj del PC esta antes
    // que el ultimo mensaje, se conserva la fecha de ese mensaje para que la
    // lista siga ordenada por fecha (la busqueda binaria por rango lo necesita).
    Fecha hora = Fecha::ahora();
    Medidor::externoReloj();
    if (reloj < hora) {
        reloj = hora;
    }
    int indice = mensajes.tamano();
    mensajes.agregar(Mensaje(actual, destino, texto, reloj, false));
    usuarios[actual].agregarEnviado(indice);
    usuarios[destino].agregarRecibido(indice);
    usuarios[destino].agregarNoLeido(indice);
    persistencia.guardarMensajes(mensajes);
    return OK;
}

const Vector<int>& Plataforma::noLeidosActual() const {
    return usuarios[actual].getNoLeidos();
}

void Plataforma::marcarNoLeidosComoLeidos() {
    const Vector<int>& noLeidos = usuarios[actual].getNoLeidos();
    for (int i = 0; i < noLeidos.tamano(); i++) {
        Medidor::contar();
        mensajes[noLeidos[i]].marcarLeido();
    }
    usuarios[actual].vaciarNoLeidos();
    persistencia.guardarMensajes(mensajes);
}

const Vector<int>& Plataforma::recibidosActual() const {
    return usuarios[actual].getRecibidos();
}

// Posiciones [desde, hasta) de los recibidos con fecha en [inicio, fin].
void Plataforma::buscarRango(const Fecha& inicio, const Fecha& fin, int& desde, int& hasta) const {
    const Vector<int>& recibidos = usuarios[actual].getRecibidos();
    // Busqueda binaria del primer mensaje con fecha >= inicio.
    int bajo = 0;
    int alto = recibidos.tamano();
    while (bajo < alto) {
        Medidor::contar();
        int medio = (bajo + alto) / 2;
        if (mensajes[recibidos[medio]].getFecha() < inicio) {
            bajo = medio + 1;
        } else {
            alto = medio;
        }
    }
    desde = bajo;
    hasta = bajo;
    while (hasta < recibidos.tamano() && mensajes[recibidos[hasta]].getFecha() <= fin) {
        Medidor::contar();
        hasta++;
    }
}

const Mensaje& Plataforma::getMensaje(int i) const {
    return mensajes[i];
}

// ---------------------------------------------------------------------------
// RF-VII: memoria. Los bucles de esta funcion no se cuentan como iteraciones
// porque son parte de la medicion, no de la funcionalidad.
// ---------------------------------------------------------------------------

unsigned long long Plataforma::memoriaTotal() const {
    unsigned long long total = sizeof(*this);   // incluye el generador mt19937 y el reloj
    total += usuarios.memoriaArreglo();
    for (int i = 0; i < usuarios.tamano(); i++) {
        total += usuarios[i].memoriaDinamica();
    }
    total += mensajes.memoriaArreglo();
    total += amistades.memoriaDinamica();
    total += amigosSesion.memoriaArreglo() + pesosSesion.memoriaArreglo();
    total += grupo.memoriaArreglo() + likesSesion.memoriaArreglo();
    return total;
}
