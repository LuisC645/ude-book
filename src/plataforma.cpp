#include <iostream>
#include <random>
#include "plataforma.h"
#include "menu.h"
#include "medidor.h"
#include "texto.h"

using namespace std;

const char* const Plataforma::SUBRUTA = "udeabook";
const char* const Plataforma::RUTA_DATOS = "data/";

const unsigned short Plataforma::OK;
const unsigned short Plataforma::NO_EXISTE;
const unsigned short Plataforma::MISMO_USUARIO;
const unsigned short Plataforma::YA_AMIGOS;
const unsigned short Plataforma::SOLICITUD_RECIBIDA;
const unsigned short Plataforma::SOLICITUD_ENVIADA;
const unsigned short Plataforma::NO_ES_AMIGO;
const unsigned short Plataforma::TEXTO_VACIO;
const unsigned short Plataforma::TEXTO_LARGO;
const unsigned short Plataforma::TAM_GRUPO;
const unsigned short Plataforma::MIN_AMIGOS_ALTERNAR;

// Inicializa la plataforma sin sesion activa.
Plataforma::Plataforma() : reloj(2026, 1, 1, 0, 0), actual(NINGUNO), alternado(false) {}

// Carga los datos, muestra el consumo de recursos e inicia el menu.
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

// Lee los archivos de datos y deja la informacion en memoria.
bool Plataforma::cargar() {
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

    Vector<unsigned int> albumDueno;
    Vector<unsigned int> albumPosicion;
    if (!persistencia.cargarAlbumes(usuarios, albumDueno, albumPosicion)
        || !persistencia.cargarFotos(usuarios, albumDueno, albumPosicion)) {
        return false;
    }
    Medidor::registrarLocal(sizeof(albumDueno) + albumDueno.memoriaArreglo()
                            + sizeof(albumPosicion) + albumPosicion.memoriaArreglo());

    if (!persistencia.cargarMensajes(usuarios, mensajes)) {
        return false;
    }
    if (mensajes.tamano() > 0) {
        reloj = mensajes[mensajes.tamano() - 1].getFecha();
    }
    return true;
}

// Busca un usuario por nombre y devuelve su indice o NINGUNO.
unsigned int Plataforma::buscarUsuario(const string& nombre) const {
    unsigned int bajo = 0;
    unsigned int alto = usuarios.tamano();
    while (bajo < alto) {
        Medidor::contar();
        unsigned int medio = (bajo + alto) / 2;
        const string& actualNombre = usuarios[medio].getNombre();
        if (actualNombre == nombre) {
            return medio;
        }
        if (actualNombre < nombre) {
            bajo = medio + 1;
        } else {
            alto = medio;
        }
    }

    return NINGUNO;
}

// Devuelve el usuario que esta en el indice dado.
const Usuario& Plataforma::buscarUsuario(unsigned int indice) const {
    return usuarios[indice];
}

// Calcula el peso de un amigo segun la cantidad de amigos en comun.
unsigned short Plataforma::pesoPorComunes(unsigned int comunes) {
    if (comunes <= 10) {
        return 2;
    }
    if (comunes <= 20) {
        return 5;
    }
    return 7;
}

// Elige al azar un amigo no usado segun su peso y devuelve su posicion o NINGUNO.
unsigned int Plataforma::girarRuleta(const Vector<bool>& usado) {
    unsigned int total = 0;
    for (unsigned int i = 0; i < pesosSesion.tamano(); i++) {
        Medidor::contar();
        if (!usado[i]) {
            total += pesosSesion[i];
        }
    }
    if (total == 0) {
        return NINGUNO;
    }
    uniform_int_distribution<unsigned int> distribucion(0, total - 1);
    Medidor::externoAleatorio();
    unsigned int r = distribucion(generador);
    unsigned int acumulado = 0;
    for (unsigned int i = 0; i < pesosSesion.tamano(); i++) {
        Medidor::contar();
        if (!usado[i]) {
            acumulado += pesosSesion[i];
            if (r < acumulado) {
                return i;
            }
        }
    }
    return NINGUNO;
}

// Valida las credenciales e inicializa los datos de la sesion.
bool Plataforma::iniciarSesion(const string& nombre, const string& contrasena) {
    unsigned int u = buscarUsuario(nombre);
    if (u == NINGUNO || !usuarios[u].contrasenaCorrecta(contrasena)) {
        return false;
    }
    actual = u;
    alternado = false;
    likesSesion.vaciar();
    grupo.vaciar();

    amistades.amigosDe(u, amigosSesion);
    pesosSesion.vaciar();
    pesosSesion.reservar(amigosSesion.tamano());
    for (unsigned int i = 0; i < amigosSesion.tamano(); i++) {
        Medidor::contar();
        pesosSesion.agregar(pesoPorComunes(amistades.comunes(u, amigosSesion[i])));
    }

    Vector<bool> usado;
    usado.agregar(false, amigosSesion.tamano());
    unsigned int meta = TAM_GRUPO;
    if (amigosSesion.tamano() < meta) {
        meta = amigosSesion.tamano();
    }
    while (grupo.tamano() < meta) {
        Medidor::contar();
        unsigned int pos = girarRuleta(usado);
        usado[pos] = true;
        grupo.agregar(pos);
    }
    Medidor::registrarLocal(sizeof(usado) + usado.memoriaArreglo() + sizeof(meta));
    return true;
}

// Limpia los datos de la sesion actual.
void Plataforma::cerrarSesion() {
    actual = NINGUNO;
    alternado = false;
    amigosSesion.vaciar();
    pesosSesion.vaciar();
    grupo.vaciar();
    likesSesion.vaciar();
}

// Devuelve el indice del usuario con sesion activa.
unsigned int Plataforma::getActual() const {
    return actual;
}

// Devuelve la cantidad de amigos del usuario actual.
unsigned int Plataforma::cantidadAmigos() const {
    return amigosSesion.tamano();
}

// Devuelve la cantidad de amigos del grupo mostrado.
unsigned int Plataforma::tamanoGrupo() const {
    return grupo.tamano();
}

// Devuelve el indice del amigo en la posicion dada del grupo.
unsigned int Plataforma::amigoDelGrupo(unsigned int i) const {
    return amigosSesion[grupo[i]];
}

// Indica si el grupo ya se alterno en esta sesion.
bool Plataforma::yaAlterno() const {
    return alternado;
}

// Indica si el grupo se puede alternar.
bool Plataforma::puedeAlternar() const {
    return !alternado && amigosSesion.tamano() >= MIN_AMIGOS_ALTERNAR;
}

// Elige un grupo de amigos distinto al primero.
bool Plataforma::alternarGrupo() {
    if (!puedeAlternar()) {
        return false;
    }
    unsigned int f = amigosSesion.tamano();
    Vector<bool> usado;
    Vector<bool> enPrimero;
    usado.agregar(false, f);
    enPrimero.agregar(false, f);
    for (unsigned int i = 0; i < grupo.tamano(); i++) {
        Medidor::contar();
        usado[grupo[i]] = true;
        enPrimero[grupo[i]] = true;
    }

    Vector<unsigned int> nuevo;
    while (nuevo.tamano() < TAM_GRUPO) {
        Medidor::contar();
        unsigned int pos = girarRuleta(usado);
        if (pos == NINGUNO) {
            break;
        }
        usado[pos] = true;
        nuevo.agregar(pos);
    }
    if (nuevo.tamano() < TAM_GRUPO) {
        for (unsigned int i = 0; i < f; i++) {
            Medidor::contar();
            if (enPrimero[i]) {
                usado[i] = false;
            }
        }
        while (nuevo.tamano() < TAM_GRUPO) {
            Medidor::contar();
            unsigned int pos = girarRuleta(usado);
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

// Devuelve la ruta de la carpeta de albumes de un usuario.
string Plataforma::rutaAlbumes(unsigned int usuario) const {
    return string("/users/") + SUBRUTA + "/" + usuarios[usuario].getNombre() + "/albums/";
}

// Devuelve la ruta de la foto de perfil de un usuario.
string Plataforma::rutaPerfil(unsigned int usuario) const {
    return rutaAlbumes(usuario) + "profile.png";
}

// Indica si hay una solicitud pendiente entre dos usuarios.
bool Plataforma::hayPendiente(unsigned int a, unsigned int b) const {
    return usuarios[a].buscarSolicitudDe(b) != NINGUNO || usuarios[b].buscarSolicitudDe(a) != NINGUNO;
}

// Valida y registra una solicitud de amistad hacia el destino.
unsigned short Plataforma::enviarSolicitud(unsigned int destino) {
    if (destino == NINGUNO) {
        return NO_EXISTE;
    }
    if (destino == actual) {
        return MISMO_USUARIO;
    }
    if (amistades.sonAmigos(actual, destino)) {
        return YA_AMIGOS;
    }
    if (usuarios[actual].buscarSolicitudDe(destino) != NINGUNO) {
        return SOLICITUD_RECIBIDA;
    }
    if (usuarios[destino].buscarSolicitudDe(actual) != NINGUNO) {
        return SOLICITUD_ENVIADA;
    }
    usuarios[destino].agregarSolicitud(actual);
    persistencia.guardarSolicitudes(usuarios);
    return OK;
}

// Devuelve la cantidad de solicitudes pendientes del usuario actual.
unsigned int Plataforma::cantidadSolicitudes() const {
    return usuarios[actual].cantidadSolicitudes();
}

// Devuelve el indice de quien envio la solicitud en la posicion dada.
unsigned int Plataforma::solicitante(unsigned int i) const {
    return usuarios[actual].getSolicitudOrigen(i);
}

// Acepta la solicitud en la posicion dada y guarda la nueva amistad.
void Plataforma::aceptarSolicitud(unsigned int i) {
    unsigned int origen = usuarios[actual].getSolicitudOrigen(i);
    amistades.agregar(actual, origen);
    usuarios[actual].quitarSolicitud(i);
    persistencia.agregarAmistad(actual, origen);
    persistencia.guardarSolicitudes(usuarios);
}

// Elimina la solicitud en la posicion dada.
void Plataforma::rechazarSolicitud(unsigned int i) {
    usuarios[actual].quitarSolicitud(i);
    persistencia.guardarSolicitudes(usuarios);
}

// Verifica que el usuario exista y sea amigo del usuario actual.
unsigned short Plataforma::validarAmigo(unsigned int usuario) const {
    if (usuario == NINGUNO) {
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

// Obtiene las posiciones de los albumes visibles de un amigo.
void Plataforma::albumesVisibles(unsigned int amigo, Vector<unsigned int>& salida) const {
    salida.vaciar();
    const Usuario& u = usuarios[amigo];
    for (unsigned int i = 0; i < u.cantidadAlbumes(); i++) {
        Medidor::contar();
        if (u.getAlbum(i).esVisibleParaAmigos()) {
            salida.agregar(i);
        }
    }
}

// Devuelve un album de un usuario.
const Album& Plataforma::getAlbum(unsigned int usuario, unsigned int album) const {
    return usuarios[usuario].getAlbum(album);
}

// Devuelve la ruta de una foto de un album.
string Plataforma::rutaFoto(unsigned int usuario, unsigned int album, unsigned int foto) const {
    const Album& a = usuarios[usuario].getAlbum(album);
    return a.getFoto(foto).getRuta(a.getRuta(rutaAlbumes(usuario)));
}

// Busca una foto entre los likes de la sesion y devuelve su posicion o NINGUNO.
unsigned int Plataforma::buscarLike(const Foto* foto) const {
    for (unsigned int i = 0; i < likesSesion.tamano(); i++) {
        Medidor::contar();
        if (likesSesion[i] == foto) {
            return i;
        }
    }
    return NINGUNO;
}

// Indica si el usuario actual dio like a la foto en esta sesion.
bool Plataforma::tieneLike(unsigned int usuario, unsigned int album, unsigned int foto) const {
    return buscarLike(&usuarios[usuario].getAlbum(album).getFoto(foto)) != NINGUNO;
}

// Da o quita el like de una foto y devuelve si queda con like.
bool Plataforma::alternarLike(unsigned int usuario, unsigned int album, unsigned int foto) {
    Foto& f = usuarios[usuario].getAlbum(album).getFoto(foto);
    unsigned int pos = buscarLike(&f);
    bool quedaConLike;
    if (pos != NINGUNO) {
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

// Compara dos candidatos por amigos en comun y luego por nombre.
bool Plataforma::esMejorCandidato(unsigned int comunesA, unsigned int usuarioA,
                                  unsigned int comunesB, unsigned int usuarioB) const {
    if (comunesA != comunesB) {
        return comunesA > comunesB;
    }
    return usuarios[usuarioA].getNombre() < usuarios[usuarioB].getNombre();
}

// Obtiene los k mejores candidatos a amistad y sus amigos en comun.
void Plataforma::sugerencias(unsigned short k, Vector<unsigned int>& candidatos, Vector<unsigned int>& comunes) {
    unsigned int* mejores = new unsigned int[k];
    unsigned int* mejoresComunes = new unsigned int[k];
    unsigned int llenos = 0;
    Medidor::registrarLocal(2ULL * (unsigned long long)k * sizeof(unsigned int));

    for (unsigned int c = 0; c < usuarios.tamano(); c++) {
        Medidor::contar();
        if (c == actual || amistades.sonAmigos(actual, c) || hayPendiente(actual, c)) {
            continue;
        }
        unsigned int com = amistades.comunes(actual, c);
        if (com == 0) {
            continue;
        }
        unsigned int pos;
        if (llenos < k) {
            pos = llenos;
            llenos++;
        } else if (esMejorCandidato(com, c, mejoresComunes[k - 1], mejores[k - 1])) {
            pos = k - 1;
        } else {
            continue;
        }
        mejores[pos] = c;
        mejoresComunes[pos] = com;
        while (pos > 0 && esMejorCandidato(mejoresComunes[pos], mejores[pos],
                                           mejoresComunes[pos - 1], mejores[pos - 1])) {
            Medidor::contar();
            unsigned int temporal = mejores[pos];
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
    for (unsigned int i = 0; i < llenos; i++) {
        Medidor::contar();
        candidatos.agregar(mejores[i]);
        comunes.agregar(mejoresComunes[i]);
    }
    delete[] mejores;
    delete[] mejoresComunes;
}

// Valida y guarda un mensaje para un amigo.
unsigned short Plataforma::enviarMensaje(unsigned int destino, const string& texto) {
    unsigned short validacion = validarAmigo(destino);
    if (validacion != OK) {
        return validacion;
    }
    if (Texto::esVacio(texto)) {
        return TEXTO_VACIO;
    }
    if (texto.length() > Mensaje::MAX_TEXTO) {
        return TEXTO_LARGO;
    }
    Fecha hora = Fecha::ahora();
    Medidor::externoReloj();
    if (reloj < hora) {
        reloj = hora;
    }
    unsigned int indice = mensajes.tamano();
    mensajes.agregar(Mensaje(actual, destino, texto, reloj, false));
    usuarios[actual].agregarEnviado(indice);
    usuarios[destino].agregarRecibido(indice);
    usuarios[destino].agregarNoLeido(indice);
    persistencia.guardarMensajes(mensajes);
    return OK;
}

// Devuelve los mensajes no leidos del usuario actual.
const Vector<unsigned int>& Plataforma::noLeidosActual() const {
    return usuarios[actual].getNoLeidos();
}

// Marca como leidos los mensajes nuevos del usuario actual.
void Plataforma::marcarNoLeidosComoLeidos() {
    const Vector<unsigned int>& noLeidos = usuarios[actual].getNoLeidos();
    for (unsigned int i = 0; i < noLeidos.tamano(); i++) {
        Medidor::contar();
        mensajes[noLeidos[i]].marcarLeido();
    }
    usuarios[actual].vaciarNoLeidos();
    persistencia.guardarMensajes(mensajes);
}

// Devuelve los mensajes recibidos por el usuario actual.
const Vector<unsigned int>& Plataforma::recibidosActual() const {
    return usuarios[actual].getRecibidos();
}

// Obtiene las posiciones de los mensajes recibidos entre dos fechas.
void Plataforma::buscarRango(const Fecha& inicio, const Fecha& fin,
                             unsigned int& desde, unsigned int& hasta) const {
    const Vector<unsigned int>& recibidos = usuarios[actual].getRecibidos();
    unsigned int bajo = 0;
    unsigned int alto = recibidos.tamano();
    while (bajo < alto) {
        Medidor::contar();
        unsigned int medio = (bajo + alto) / 2;
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

// Devuelve el mensaje en la posicion dada.
const Mensaje& Plataforma::getMensaje(unsigned int i) const {
    return mensajes[i];
}

// Calcula la memoria usada por las estructuras de la plataforma.
unsigned long long Plataforma::memoriaTotal() const {
    unsigned long long total = sizeof(*this);
    total += usuarios.memoriaArreglo();
    for (unsigned int i = 0; i < usuarios.tamano(); i++) {
        total += usuarios[i].memoriaDinamica();
    }
    total += mensajes.memoriaArreglo();
    total += amistades.memoriaDinamica();
    total += amigosSesion.memoriaArreglo() + pesosSesion.memoriaArreglo();
    total += grupo.memoriaArreglo() + likesSesion.memoriaArreglo();
    return total;
}
