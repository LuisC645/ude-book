#include <iostream>
#include <fstream>
#include "persistencia.h"
#include "medidor.h"
#include "fecha.h"
#include "album.h"
#include "foto.h"
#include "texto.h"

using namespace std;

void Persistencia::fijarCarpeta(const string& rutaCarpeta) {
    carpeta = rutaCarpeta;
}

// ---------------------------------------------------------------------------
// Auxiliares
// ---------------------------------------------------------------------------

bool Persistencia::abrirLectura(ifstream& archivo, const char* nombre) {
    string ruta = carpeta + nombre;
    archivo.open(ruta);
    Medidor::externoArchivo();
    if (!archivo.is_open()) {
        cout << "Error: no se pudo abrir el archivo " << ruta << "\n";
        return false;
    }
    return true;
}

bool Persistencia::abrirEscritura(ofstream& archivo, const char* nombre, bool anexar) {
    string ruta = carpeta + nombre;
    if (anexar) {
        archivo.open(ruta, ios::app);
    } else {
        archivo.open(ruta, ios::trunc);
    }
    Medidor::externoArchivo();
    if (!archivo.is_open()) {
        cout << "Advertencia: no se pudo guardar el cambio en " << ruta << "\n";
        return false;
    }
    return true;
}

bool Persistencia::leerLinea(ifstream& archivo, string& registro) {
    Medidor::externoArchivo();
    return Texto::leerLinea(archivo, registro);
}

// Separar por |
int Persistencia::dividir(const string& registro, int maxCampos, Vector<string>& campos) {
    campos.vaciar();
    int inicio = 0;
    for (int i = 0; i < (int)registro.length(); i++) {
        Medidor::contar();
        if (registro[i] == '|' && campos.tamano() < maxCampos - 1) {
            campos.agregar(registro.substr(inicio, i - inicio));
            inicio = i + 1;
        }
    }
    campos.agregar(registro.substr(inicio));
    return campos.tamano();
}

bool Persistencia::error() const {
    cout << "Error" << "\n";
    return false;
}

// ---------------------------------------------------------------------------
// Carga
// ---------------------------------------------------------------------------

bool Persistencia::cargarUsuarios(Vector<Usuario>& usuarios) {
    const char* nombreArchivo = "usuarios.txt";
    ifstream archivo;
    if (!abrirLectura(archivo, nombreArchivo)) {
        return false;
    }

    Vector<string> campos;
    string registro;
    int linea = 0;

    while (leerLinea(archivo, registro)) {
        Medidor::contar();
        linea++;
        if (dividir(registro, 7, campos) != 7) {
            return error();
        }
        string nombre = Texto::recortado(campos[0]);
        if (nombre.empty() || Texto::contiene(nombre, ' ')) {
            return error();
        }

        Fecha nacimiento;
        Fecha inscripcion;

        if (!Fecha::desdeCompacta(Texto::recortado(campos[1]), nacimiento)
            || !Fecha::desdeCompacta(Texto::recortado(campos[6]), inscripcion)) {
            return error();
        }
        string contrasena = Texto::recortado(campos[3]);
        if (contrasena.empty()) {
            return error();
        }
        usuarios.agregar(Usuario(nombre, nacimiento, Texto::recortado(campos[2]), contrasena,
                                 Texto::recortado(campos[4]), Texto::recortado(campos[5]), inscripcion));
    }
    if (usuarios.tamano() == 0) {
        return error();
    }
    return true;
}

bool Persistencia::cargarAmistades(MatrizAmistad& matriz) {
    const char* nombreArchivo = "amistades.txt";
    ifstream archivo;
    if (!abrirLectura(archivo, nombreArchivo)) {
        return false;
    }
    Vector<string> campos;
    string registro;
    int linea = 0;

    while (leerLinea(archivo, registro)) {
        Medidor::contar();
        linea++;
        int a = -1;
        int b = -1;

        if (dividir(registro, 2, campos) != 2 || !Texto::aEntero(campos[0], a)
            || !Texto::aEntero(campos[1], b)) {
            return error();
        }
        if (a >= matriz.cantidad() || b >= matriz.cantidad() || a == b) {
            return error();
        }
        matriz.agregar(a, b);
    }
    return true;
}

bool Persistencia::cargarSolicitudes(Vector<Usuario>& usuarios) {
    const char* nombreArchivo = "solicitudes.txt";
    ifstream archivo;
    if (!abrirLectura(archivo, nombreArchivo)) {
        return false;
    }
    Vector<string> campos;
    string registro;
    int linea = 0;
    while (leerLinea(archivo, registro)) {
        Medidor::contar();
        linea++;
        int origen = -1;
        int destino = -1;
        if (dividir(registro, 2, campos) != 2 || !Texto::aEntero(campos[0], origen)
            || !Texto::aEntero(campos[1], destino)) {
            return error();
        }
        if (origen >= usuarios.tamano() || destino >= usuarios.tamano() || origen == destino) {
            return error();
        }
        usuarios[destino].agregarSolicitud(origen);
    }
    return true;
}

bool Persistencia::cargarAlbumes(Vector<Usuario>& usuarios, Vector<int>& albumDueno,
                                 Vector<int>& albumPosicion) {
    const char* nombreArchivo = "albumes.txt";
    ifstream archivo;
    if (!abrirLectura(archivo, nombreArchivo)) {
        return false;
    }
    Vector<string> campos;
    string registro;
    int linea = 0;
    while (leerLinea(archivo, registro)) {
        Medidor::contar();
        linea++;
        int dueno = -1;
        int privacidad = -1;
        if (dividir(registro, 3, campos) != 3 || !Texto::aEntero(campos[0], dueno)
            || !Texto::aEntero(campos[2], privacidad)) {
            return error();
        }
        if (dueno >= usuarios.tamano()) {
            return error();
        }
        // Agrupados por dueno: asi guardarFotos puede reconstruir el numero de cada album.
        if (albumDueno.tamano() > 0 && dueno < albumDueno[albumDueno.tamano() - 1]) {
            return error();
        }
        if (privacidad > Album::PRIVADO) {
            return error();
        }
        string nombre = Texto::recortado(campos[1]);
        if (nombre.empty()) {
            return error();
        }
        albumDueno.agregar(dueno);
        albumPosicion.agregar(usuarios[dueno].cantidadAlbumes());
        usuarios[dueno].agregarAlbum(Album(nombre, privacidad));
    }
    return true;
}

bool Persistencia::cargarFotos(Vector<Usuario>& usuarios, const Vector<int>& albumDueno,
                               const Vector<int>& albumPosicion) {
    const char* nombreArchivo = "fotos.txt";
    ifstream archivo;
    if (!abrirLectura(archivo, nombreArchivo)) {
        return false;
    }
    Vector<string> campos;
    string registro;
    int linea = 0;
    while (leerLinea(archivo, registro)) {
        Medidor::contar();
        linea++;
        int album = -1;
        int tamano = -1;
        int likes = -1;
        Fecha captura;
        if (dividir(registro, 5, campos) != 5 || !Texto::aEntero(campos[0], album)
            || !Texto::aEntero(campos[3], tamano) || !Texto::aEntero(campos[4], likes)) {
            return error();
        }
        if (album >= albumDueno.tamano()) {
            return error();
        }
        if (!Fecha::desdeCompacta(Texto::recortado(campos[2]), captura)) {
            return error();
        }
        string nombre = Texto::recortado(campos[1]);
        if (nombre.empty()) {
            return error();
        }
        usuarios[albumDueno[album]].getAlbum(albumPosicion[album])
            .agregarFoto(Foto(nombre, captura, tamano, likes));
    }
    return true;
}

bool Persistencia::cargarMensajes(Vector<Usuario>& usuarios, Vector<Mensaje>& mensajes) {
    const char* nombreArchivo = "mensajes.txt";
    ifstream archivo;
    if (!abrirLectura(archivo, nombreArchivo)) {
        return false;
    }
    Vector<string> campos;
    string registro;
    int linea = 0;

    while (leerLinea(archivo, registro)) {
        Medidor::contar();
        linea++;
        int emisor = -1;
        int receptor = -1;
        int leido = -1;
        Fecha fecha;
        if (dividir(registro, 5, campos) != 5 || !Texto::aEntero(campos[0], emisor)
            || !Texto::aEntero(campos[1], receptor) || !Texto::aEntero(campos[3], leido)) {
            return error();
        }
        if (emisor >= usuarios.tamano() || receptor >= usuarios.tamano() || emisor == receptor) {
            return error();
        }
        if (!Fecha::desdeCompacta(Texto::recortado(campos[2]), fecha)) {
            return error();
        }
        // Orden por fecha: permite la busqueda binaria del rango de fechas.
        if (mensajes.tamano() > 0 && fecha < mensajes[mensajes.tamano() - 1].getFecha()) {
            return error();
        }
        if (leido > 1) {
            return error();
        }
        const string& texto = campos[4];
        if (Texto::esVacio(texto) || (int)texto.length() > Mensaje::MAX_TEXTO) {
            return error();
        }
        int indice = mensajes.tamano();
        mensajes.agregar(Mensaje(emisor, receptor, texto, fecha, leido == 1));
        usuarios[emisor].agregarEnviado(indice);
        usuarios[receptor].agregarRecibido(indice);
        if (leido == 0) {
            usuarios[receptor].agregarNoLeido(indice);
        }
    }
    return true;
}

// ---------------------------------------------------------------------------
// Guardado
// ---------------------------------------------------------------------------

// Una amistad nueva solo se anexa como ultima linea del archivo.
void Persistencia::agregarAmistad(int a, int b) {
    ofstream archivo;
    if (abrirEscritura(archivo, "amistades.txt", true)) {
        archivo << a << '|' << b << '\n';
        Medidor::externoArchivo();
    }
}

void Persistencia::guardarSolicitudes(const Vector<Usuario>& usuarios) {
    ofstream archivo;
    if (!abrirEscritura(archivo, "solicitudes.txt", false)) {
        return;
    }
    for (int destino = 0; destino < usuarios.tamano(); destino++) {
        Medidor::contar();
        for (int i = 0; i < usuarios[destino].cantidadSolicitudes(); i++) {
            Medidor::contar();
            archivo << usuarios[destino].getSolicitudOrigen(i) << '|' << destino << '\n';
            Medidor::externoArchivo();
        }
    }
}

// Los albumes se numeran recorriendo usuarios y albumes en el mismo orden de albumes.txt.
void Persistencia::guardarFotos(const Vector<Usuario>& usuarios) {
    ofstream archivo;
    if (!abrirEscritura(archivo, "fotos.txt", false)) {
        return;
    }
    int numeroAlbum = 0;
    for (int u = 0; u < usuarios.tamano(); u++) {
        Medidor::contar();
        for (int a = 0; a < usuarios[u].cantidadAlbumes(); a++) {
            Medidor::contar();
            const Album& album = usuarios[u].getAlbum(a);
            for (int f = 0; f < album.cantidadFotos(); f++) {
                Medidor::contar();
                const Foto& foto = album.getFoto(f);
                archivo << numeroAlbum << '|' << foto.getNombre() << '|';
                foto.getCaptura().escribirCompacta(archivo, false);
                archivo << '|' << foto.getTamano() << '|' << foto.getLikes() << '\n';
                Medidor::externoArchivo();
            }
            numeroAlbum++;
        }
    }
}

void Persistencia::guardarMensajes(const Vector<Mensaje>& mensajes) {
    ofstream archivo;
    if (!abrirEscritura(archivo, "mensajes.txt", false)) {
        return;
    }
    for (int i = 0; i < mensajes.tamano(); i++) {
        Medidor::contar();
        const Mensaje& m = mensajes[i];
        archivo << m.getEmisor() << '|' << m.getReceptor() << '|';
        m.getFecha().escribirCompacta(archivo, true);
        archivo << '|' << (m.estaLeido() ? 1 : 0) << '|' << m.getTexto() << '\n';
        Medidor::externoArchivo();
    }
}
