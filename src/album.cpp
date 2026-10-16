#include "album.h"

const int Album::PUBLICO;
const int Album::AMIGOS;
const int Album::PRIVADO;

Album::Album() : nombre(), privacidad(PRIVADO), fotos() {}

Album::Album(const string& nombre, unsigned short privacidad)
    : nombre(nombre), privacidad(privacidad), fotos() {}

const string& Album::getNombre() const {
    return nombre;
}

unsigned short Album::getPrivacidad() const {
    return privacidad;
}

bool Album::esVisibleParaAmigos() const {
    return privacidad == PUBLICO || privacidad == AMIGOS;
}

void Album::agregarFoto(const Foto& foto) {
    fotos.agregar(foto);
}

unsigned int Album::cantidadFotos() const {
    return fotos.tamano();
}

const Foto& Album::getFoto(int i) const {
    return fotos[i];
}

Foto& Album::getFoto(int i) {
    return fotos[i];
}

string Album::getRuta(const string& rutaUsuario) const {
    return rutaUsuario + nombre + "/";
}

unsigned long long Album::memoriaDinamica() const {
    return fotos.memoriaArreglo();
}
