#include "foto.h"

Foto::Foto() : nombre(), captura(), tamano(0), likes(0) {}

Foto::Foto(const string& nombre, const Fecha& captura, unsigned int tamano, unsigned short likes)
    : nombre(nombre), captura(captura), tamano(tamano), likes(likes) {}

const string& Foto::getNombre() const {
    return nombre;
}

unsigned short Foto::getLikes() const {
    return likes;
}

void Foto::setLikes(unsigned short nuevosLikes) {
    likes = nuevosLikes;
}

const Fecha& Foto::getCaptura() const {
    return captura;
}

unsigned int Foto::getTamano() const {
    return tamano;
}

string Foto::getRuta(const string& rutaAlbum) const {
    return rutaAlbum + nombre + ".png";
}
