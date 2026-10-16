#include "usuario.h"
#include "medidor.h"

Usuario::Usuario() {}

Usuario::Usuario(const string& nombre, const Fecha& nacimiento, const string& correo,
                 const string& contrasena, const string& ciudad, const string& pais,
                 const Fecha& inscripcion)
    : nombre(nombre), nacimiento(nacimiento), correo(correo), contrasena(contrasena),
    ciudad(ciudad), pais(pais), inscripcion(inscripcion) {}

const string& Usuario::getNombre() const {
    return nombre;
}

bool Usuario::contrasenaCorrecta(const string& intento) const {
    return contrasena == intento;
}

void Usuario::agregarAlbum(const Album& album) {
    albumes.agregar(album);
}

int Usuario::cantidadAlbumes() const {
    return albumes.tamano();
}

const Album& Usuario::getAlbum(int i) const {
    return albumes[i];
}

Album& Usuario::getAlbum(int i) {
    return albumes[i];
}

void Usuario::agregarRecibido(int mensaje) {
    recibidos.agregar(mensaje);
}

void Usuario::agregarEnviado(int mensaje) {
    enviados.agregar(mensaje);
}

void Usuario::agregarNoLeido(int mensaje) {
    noLeidos.agregar(mensaje);
}

const Vector<int>& Usuario::getRecibidos() const {
    return recibidos;
}

const Vector<int>& Usuario::getNoLeidos() const {
    return noLeidos;
}

void Usuario::vaciarNoLeidos() {
    noLeidos.vaciar();
}

void Usuario::agregarSolicitud(int origen) {
    solicitudes.agregar(origen);
}

int Usuario::cantidadSolicitudes() const {
    return solicitudes.tamano();
}

int Usuario::getSolicitudOrigen(int i) const {
    return solicitudes[i];
}

void Usuario::quitarSolicitud(int i) {
    solicitudes.quitarEn(i);
}

int Usuario::buscarSolicitudDe(int origen) const {
    for (int i = 0; i < solicitudes.tamano(); i++) {
        Medidor::contar();
        if (solicitudes[i] == origen) {
            return i;
        }
    }
    return -1;
}

unsigned long long Usuario::memoriaDinamica() const {
    unsigned long long total = albumes.memoriaArreglo();
    for (int i = 0; i < albumes.tamano(); i++) {
        total += albumes[i].memoriaDinamica();
    }
    total += recibidos.memoriaArreglo() + enviados.memoriaArreglo() + noLeidos.memoriaArreglo();
    total += solicitudes.memoriaArreglo();
    return total;
}
