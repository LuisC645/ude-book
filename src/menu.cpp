#include <iostream>
#include "menu.h"
#include "plataforma.h"
#include "medidor.h"
#include "fecha.h"
#include "album.h"
#include "mensaje.h"
#include "texto.h"

using namespace std;

// Asocia el menu a la plataforma.
Menu::Menu(Plataforma& plataforma) : plataforma(plataforma), cerrada(false) {}

// Muestra un mensaje y lee una linea de maximo 512 caracteres.
bool Menu::leer(const char* mensaje, string& linea) {
    while (!cerrada) {
        Medidor::contar();
        cout << mensaje;
        if (!Texto::leerLinea(cin, linea)) {
            cerrada = true;
            break;
        }
        if (linea.length() <= Mensaje::MAX_TEXTO) {
            return true;
        }
        cout << "La entrada supera los " << Mensaje::MAX_TEXTO << " caracteres. Intente de nuevo.\n";
    }
    cout << "\n";
    return false;
}

// Lee un numero entero dentro de un rango.
bool Menu::leerEntero(const char* mensaje, unsigned int minimo, unsigned int maximo, unsigned int& valor) {
    string linea;
    while (leer(mensaje, linea)) {
        Medidor::contar();
        if (Texto::aEntero(linea, valor) && valor >= minimo && valor <= maximo) {
            return true;
        }
        cout << "Entrada invalida: ingrese un numero entre " << minimo << " y " << maximo << ".\n";
    }
    return false;
}

// Muestra el consumo de recursos actual.
void Menu::reportar() const {
    Medidor::reportar(plataforma.memoriaTotal() + sizeof(*this));
}

// Muestra la pantalla de ingreso hasta que el usuario decide salir.
void Menu::ejecutar() {
    string nombre;
    string contrasena;
    while (!cerrada) {
        Medidor::contar();
        cout << "\n========== Ude@Book ==========\n";
        if (!leer("Usuario (ENTER vacio para salir): ", nombre)) {
            break;
        }
        nombre = Texto::recortado(nombre);
        if (nombre.empty()) {
            break;
        }
        if (!leer("Contrasena: ", contrasena)) {
            break;
        }
        Medidor::reiniciar();
        Medidor::registrarLocal(sizeof(nombre) + sizeof(contrasena));
        if (!plataforma.iniciarSesion(nombre, contrasena)) {
            cout << "Usuario o contrasena incorrectos.\n";
            reportar();
            continue;
        }
        mostrarEncabezado();
        reportar();
        menuPrincipal();
        plataforma.cerrarSesion();
        cout << "Sesion cerrada.\n";
    }
    cout << "Hasta pronto.\n";
}

// Muestra las fotos de perfil del grupo de amigos y el saludo.
void Menu::mostrarEncabezado() const {
    cout << "\n";
    if (plataforma.cantidadAmigos() == 0) {
        cout << "Aun no tienes amigos.\n";
    }
    for (unsigned int i = 0; i < plataforma.tamanoGrupo(); i++) {
        Medidor::contar();
        cout << "Foto perfil de amigo " << (i + 1) << ": "
             << plataforma.rutaPerfil(plataforma.amigoDelGrupo(i)) << "\n";
    }
    cout << "\nBienvenido a UdeaBook "
         << plataforma.buscarUsuario(plataforma.getActual()).getNombre() << "\n";
}

// Muestra el menu principal y atiende la opcion elegida.
void Menu::menuPrincipal() {
    string opcion;
    while (!cerrada) {
        Medidor::contar();
        cout << "\nMenu de opciones:\n";
        cout << "1.- Administrar amistades\n";
        cout << "2.- Visitar perfil de un amigo\n";
        cout << "3.- Ver sugerencias de amistad\n";
        cout << "4.- Administrar mensajes\n\n";
        if (plataforma.puedeAlternar()) {
            cout << ".- Ingrese 'N' o 'n' para alternar el grupo de fotos de perfil de amigos.\n";
        }
        cout << ".- Ingrese 'S' o 's' para cerrar su sesion.\n";
        if (!leer("Su opcion: ", opcion)) {
            return;
        }
        opcion = Texto::recortado(opcion);
        if (opcion == "1") {
            menuAmistades();
        } else if (opcion == "2") {
            visitarPerfil();
        } else if (opcion == "3") {
            verSugerencias();
        } else if (opcion == "4") {
            menuMensajes();
        } else if (opcion == "N" || opcion == "n") {
            alternarFotos();
        } else if (opcion == "S" || opcion == "s") {
            return;
        } else {
            cout << "Opcion invalida.\n";
        }
    }
}

// Cambia el grupo de fotos de perfil de amigos.
void Menu::alternarFotos() {
    if (plataforma.cantidadAmigos() < Plataforma::MIN_AMIGOS_ALTERNAR) {
        cout << "Opcion no disponible: necesitas al menos 4 amigos.\n";
        return;
    }
    if (plataforma.yaAlterno()) {
        cout << "La opcion N ya no esta disponible en esta sesion.\n";
        return;
    }
    Medidor::reiniciar();
    plataforma.alternarGrupo();
    mostrarEncabezado();
    reportar();
}

// Muestra el menu de amistades y atiende la opcion elegida.
void Menu::menuAmistades() {
    unsigned int opcion = 0;
    while (!cerrada) {
        Medidor::contar();
        cout << "\n--- Administrar amistades ---\n";
        cout << "1. Enviar solicitud de amistad\n";
        cout << "2. Aceptar solicitudes\n";
        cout << "3. Rechazar solicitudes\n";
        cout << "0. Volver\n";
        if (!leerEntero("Su opcion: ", 0, 3, opcion) || opcion == 0) {
            return;
        }
        if (opcion == 1) {
            enviarSolicitud();
        } else if (opcion == 2) {
            responderSolicitudes(true);
        } else {
            responderSolicitudes(false);
        }
    }
}

// Muestra el resultado de enviar una solicitud.
void Menu::mostrarResultadoSolicitud(unsigned short codigo) const {
    if (codigo == Plataforma::OK) {
        cout << "Solicitud enviada.\n";
    } else if (codigo == Plataforma::NO_EXISTE) {
        cout << "El usuario no existe.\n";
    } else if (codigo == Plataforma::MISMO_USUARIO) {
        cout << "No puedes enviarte una solicitud a ti mismo.\n";
    } else if (codigo == Plataforma::YA_AMIGOS) {
        cout << "Ya son amigos.\n";
    } else if (codigo == Plataforma::SOLICITUD_RECIBIDA) {
        cout << "Ese usuario ya te envio una solicitud: aceptala desde 'Aceptar solicitudes'.\n";
    } else if (codigo == Plataforma::SOLICITUD_ENVIADA) {
        cout << "Ya le enviaste una solicitud que sigue pendiente.\n";
    }
}

// Pide un usuario y le envia una solicitud de amistad.
void Menu::enviarSolicitud() {
    Medidor::reiniciar();
    string nombre;
    if (!leer("Usuario destino: ", nombre)) {
        return;
    }
    Medidor::registrarLocal(sizeof(nombre));
    mostrarResultadoSolicitud(plataforma.enviarSolicitud(plataforma.buscarUsuario(Texto::recortado(nombre))));
    reportar();
}

// Lista las solicitudes pendientes y acepta o rechaza la elegida.
void Menu::responderSolicitudes(bool aceptar) {
    Medidor::reiniciar();
    unsigned int n = plataforma.cantidadSolicitudes();
    if (n == 0) {
        cout << "No tienes solicitudes pendientes.\n";
        reportar();
        return;
    }
    cout << "\nSolicitudes pendientes:\n";
    for (unsigned int i = 0; i < n; i++) {
        Medidor::contar();
        cout << (i + 1) << ". " << plataforma.buscarUsuario(plataforma.solicitante(i)).getNombre() << "\n";
    }
    unsigned int eleccion = 0;
    const char* mensaje = aceptar ? "Numero de la solicitud a aceptar (0 para volver): "
                                  : "Numero de la solicitud a rechazar (0 para volver): ";
    if (!leerEntero(mensaje, 0, n, eleccion)) {
        return;
    }
    Medidor::registrarLocal(sizeof(n) + sizeof(eleccion) + sizeof(mensaje));
    if (eleccion > 0) {
        const string& nombre = plataforma.buscarUsuario(plataforma.solicitante(eleccion - 1)).getNombre();
        cout << (aceptar ? "Ahora eres amigo de " : "Rechazaste la solicitud de ") << nombre << ".\n";
        if (aceptar) {
            plataforma.aceptarSolicitud(eleccion - 1);
        } else {
            plataforma.rechazarSolicitud(eleccion - 1);
        }
    }
    reportar();
}

// Muestra los albumes visibles de un amigo.
void Menu::visitarPerfil() {
    Medidor::reiniciar();
    string nombre;
    if (!leer("Usuario del amigo: ", nombre)) {
        return;
    }
    unsigned int amigo = plataforma.buscarUsuario(Texto::recortado(nombre));
    unsigned short validacion = plataforma.validarAmigo(amigo);
    if (validacion == Plataforma::NO_EXISTE) {
        cout << "El usuario no existe.\n";
    } else if (validacion == Plataforma::MISMO_USUARIO) {
        cout << "Ese es tu propio usuario; ingresa el de un amigo.\n";
    } else if (validacion == Plataforma::NO_ES_AMIGO) {
        cout << "Ese usuario no es tu amigo.\n";
    }
    if (validacion != Plataforma::OK) {
        reportar();
        return;
    }

    Vector<unsigned int> visibles;
    plataforma.albumesVisibles(amigo, visibles);
    Medidor::registrarLocal(sizeof(nombre) + sizeof(amigo)
                            + sizeof(visibles) + visibles.memoriaArreglo());
    const string& nombreAmigo = plataforma.buscarUsuario(amigo).getNombre();
    if (visibles.tamano() == 0) {
        cout << nombreAmigo << " no tiene albumes visibles.\n";
    }
    while (visibles.tamano() > 0 && !cerrada) {
        Medidor::contar();
        cout << "\nAlbumes de " << nombreAmigo << ":\n";
        for (unsigned int i = 0; i < visibles.tamano(); i++) {
            Medidor::contar();
            const Album& a = plataforma.getAlbum(amigo, visibles[i]);
            cout << (i + 1) << ". " << a.getNombre() << " ("
                 << (a.getPrivacidad() == Album::PUBLICO ? "publico" : "amigos") << ", "
                 << a.cantidadFotos() << " fotos)\n";
        }
        unsigned int eleccion = 0;
        if (!leerEntero("Elija un album (0 para volver): ", 0, visibles.tamano(), eleccion) || eleccion == 0) {
            break;
        }
        unsigned int album = visibles[eleccion - 1];
        if (plataforma.getAlbum(amigo, album).cantidadFotos() == 0) {
            cout << "El album no tiene fotos.\n";
        } else {
            recorrerAlbum(amigo, album);
        }
    }
    reportar();
}

// Recorre las fotos de un album y permite dar o quitar like.
void Menu::recorrerAlbum(unsigned int amigo, unsigned int album) {
    const Album& a = plataforma.getAlbum(amigo, album);
    unsigned int n = a.cantidadFotos();
    unsigned int i = 0;
    unsigned int opcion = 0;
    while (!cerrada) {
        Medidor::contar();
        cout << "\nAlbum " << a.getNombre() << " - Foto " << (i + 1) << "/" << n << "\n";
        cout << plataforma.rutaFoto(amigo, album, i) << "\n";
        cout << "Likes: " << a.getFoto(i).getLikes();
        if (plataforma.tieneLike(amigo, album, i)) {
            cout << " (te gusta)";
        }
        cout << "\n1. Siguiente  2. Previa  3. Dar/quitar like  0. Salir del album\n";
        if (!leerEntero("Su opcion: ", 0, 3, opcion) || opcion == 0) {
            return;
        }
        if (opcion == 1) {
            i = (i + 1) % n;
        } else if (opcion == 2) {
            i = (i == 0) ? n - 1 : i - 1;
        } else if (plataforma.alternarLike(amigo, album, i)) {
            cout << "Diste like a la foto.\n";
        } else {
            cout << "Quitaste tu like de la foto.\n";
        }
    }
}

// Muestra sugerencias de amistad y permite enviar una solicitud.
void Menu::verSugerencias() {
    Medidor::reiniciar();
    unsigned int k = 0;
    if (!leerEntero("Cuantas sugerencias desea ver (1 a 20): ", 1, 20, k)) {
        return;
    }
    Vector<unsigned int> candidatos;
    Vector<unsigned int> comunes;
    plataforma.sugerencias((unsigned short)k, candidatos, comunes);
    Medidor::registrarLocal(sizeof(k) + sizeof(candidatos) + candidatos.memoriaArreglo()
                            + sizeof(comunes) + comunes.memoriaArreglo());
    if (candidatos.tamano() == 0) {
        cout << "No hay sugerencias disponibles.\n";
        reportar();
        return;
    }
    if (candidatos.tamano() < k) {
        cout << "Solo hay " << candidatos.tamano() << " sugerencias disponibles.\n";
    }
    cout << "\nSugerencias de amistad:\n";
    for (unsigned int i = 0; i < candidatos.tamano(); i++) {
        Medidor::contar();
        cout << (i + 1) << ". " << plataforma.buscarUsuario(candidatos[i]).getNombre() << " - "
             << comunes[i] << " amigos en comun\n";
    }
    unsigned int eleccion = 0;
    if (!leerEntero("Numero para enviar solicitud (0 para omitir): ", 0, candidatos.tamano(), eleccion)) {
        return;
    }
    if (eleccion > 0) {
        mostrarResultadoSolicitud(plataforma.enviarSolicitud(candidatos[eleccion - 1]));
    }
    reportar();
}

// Muestra el menu de mensajes y atiende la opcion elegida.
void Menu::menuMensajes() {
    unsigned int opcion = 0;
    while (!cerrada) {
        Medidor::contar();
        cout << "\n--- Administrar mensajes ---\n";
        cout << "1. Enviar mensaje\n";
        cout << "2. Leer mensajes nuevos\n";
        cout << "3. Buscar mensajes por rango de fechas\n";
        cout << "0. Volver\n";
        if (!leerEntero("Su opcion: ", 0, 3, opcion) || opcion == 0) {
            return;
        }
        if (opcion == 1) {
            enviarMensaje();
        } else if (opcion == 2) {
            leerNuevos();
        } else {
            buscarPorFechas();
        }
    }
}

// Muestra la fecha, el emisor y el texto de un mensaje.
void Menu::mostrarMensaje(unsigned int indice) const {
    const Mensaje& m = plataforma.getMensaje(indice);
    cout << "[" << m.getFecha() << "] De: " << plataforma.buscarUsuario(m.getEmisor()).getNombre()
         << ": " << m.getTexto() << "\n";
}

// Pide un amigo y un texto y envia el mensaje.
void Menu::enviarMensaje() {
    Medidor::reiniciar();
    string nombre;
    if (!leer("Usuario destinatario: ", nombre)) {
        return;
    }
    unsigned int destino = plataforma.buscarUsuario(Texto::recortado(nombre));
    unsigned short validacion = plataforma.validarAmigo(destino);
    if (validacion == Plataforma::NO_EXISTE) {
        cout << "El usuario no existe.\n";
    } else if (validacion == Plataforma::MISMO_USUARIO) {
        cout << "No puedes enviarte mensajes a ti mismo.\n";
    } else if (validacion == Plataforma::NO_ES_AMIGO) {
        cout << "Solo puedes enviar mensajes a tus amigos.\n";
    }
    if (validacion != Plataforma::OK) {
        reportar();
        return;
    }
    string texto;
    while (true) {
        Medidor::contar();
        if (!leer("Mensaje (maximo 512 caracteres): ", texto)) {
            return;
        }
        if (!Texto::esVacio(texto)) {
            break;
        }
        cout << "El mensaje no puede estar vacio.\n";
    }
    Medidor::registrarLocal(sizeof(nombre) + sizeof(destino) + sizeof(texto));
    if (plataforma.enviarMensaje(destino, texto) == Plataforma::OK) {
        cout << "Mensaje enviado.\n";
    }
    reportar();
}

// Muestra los mensajes nuevos y los marca como leidos.
void Menu::leerNuevos() {
    Medidor::reiniciar();
    const Vector<unsigned int>& noLeidos = plataforma.noLeidosActual();
    if (noLeidos.tamano() == 0) {
        cout << "No tienes mensajes nuevos.\n";
        reportar();
        return;
    }
    cout << "\nMensajes nuevos (" << noLeidos.tamano() << "):\n";
    for (unsigned int i = 0; i < noLeidos.tamano(); i++) {
        Medidor::contar();
        mostrarMensaje(noLeidos[i]);
    }
    plataforma.marcarNoLeidosComoLeidos();
    reportar();
}

// Lee una fecha valida en formato DD/MM/AAAA.
bool Menu::leerFecha(const char* mensaje, Fecha& fecha) {
    string linea;
    while (leer(mensaje, linea)) {
        Medidor::contar();
        if (Fecha::desdeTexto(linea, fecha)) {
            return true;
        }
        cout << "Fecha invalida: use el formato DD/MM/AAAA con una fecha real.\n";
    }
    return false;
}

// Muestra los mensajes recibidos entre dos fechas.
void Menu::buscarPorFechas() {
    Medidor::reiniciar();
    Fecha inicio;
    Fecha fin;
    while (true) {
        Medidor::contar();
        if (!leerFecha("Fecha de inicio (DD/MM/AAAA): ", inicio) || !leerFecha("Fecha de fin (DD/MM/AAAA): ", fin)) {
            return;
        }
        if (inicio <= fin) {
            break;
        }
        cout << "La fecha de inicio debe ser anterior o igual a la fecha de fin.\n";
    }
    inicio.fijarHora(0, 0);
    fin.fijarHora(23, 59);

    unsigned int desde = 0;
    unsigned int hasta = 0;
    plataforma.buscarRango(inicio, fin, desde, hasta);
    Medidor::registrarLocal(sizeof(inicio) + sizeof(fin) + sizeof(desde) + sizeof(hasta));
    if (desde == hasta) {
        cout << "No hay mensajes recibidos en ese rango.\n";
    } else {
        cout << "\nMensajes recibidos entre " << inicio << " y " << fin << " (" << (hasta - desde) << "):\n";
        const Vector<unsigned int>& recibidos = plataforma.recibidosActual();
        for (unsigned int i = desde; i < hasta; i++) {
            Medidor::contar();
            mostrarMensaje(recibidos[i]);
        }
    }
    reportar();
}
