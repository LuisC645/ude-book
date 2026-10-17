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

unsigned int Usuario::cantidadAlbumes() const {
    return albumes.tamano();
}

const Album& Usuario::getAlbum(unsigned int i) const {
    return albumes[i];
}

Album& Usuario::getAlbum(unsigned int i) {
    return albumes[i];
}

void Usuario::agregarRecibido(unsigned int mensaje) {
    recibidos.agregar(mensaje);
}

void Usuario::agregarEnviado(unsigned int mensaje) {
    enviados.agregar(mensaje);
}

void Usuario::agregarNoLeido(unsigned int mensaje) {
    noLeidos.agregar(mensaje);
}

const Vector<unsigned int>& Usuario::getRecibidos() const {
    return recibidos;
}

const Vector<unsigned int>& Usuario::getNoLeidos() const {
    return noLeidos;
}

void Usuario::vaciarNoLeidos() {
    noLeidos.vaciar();
}

void Usuario::agregarSolicitud(unsigned int origen) {
    solicitudes.agregar(origen);
}

unsigned int Usuario::cantidadSolicitudes() const {
    return solicitudes.tamano();
}

unsigned int Usuario::getSolicitudOrigen(unsigned int i) const {
    return solicitudes[i];
}

void Usuario::quitarSolicitud(unsigned int i) {
    solicitudes.quitarEn(i);
}

unsigned int Usuario::buscarSolicitudDe(unsigned int origen) const {
    for (unsigned int i = 0; i < solicitudes.tamano(); i++) {
        Medidor::contar();
        if (solicitudes[i] == origen) {
            return i;
        }
    }
    return NINGUNO;
}

unsigned long long Usuario::memoriaDinamica() const {
    unsigned long long total = albumes.memoriaArreglo();
    for (unsigned int i = 0; i < albumes.tamano(); i++) {
        total += albumes[i].memoriaDinamica();
    }
    total += recibidos.memoriaArreglo() + enviados.memoriaArreglo() + noLeidos.memoriaArreglo();
    total += solicitudes.memoriaArreglo();
    return total;
}
