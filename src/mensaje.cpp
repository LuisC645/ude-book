#include "mensaje.h"

const int Mensaje::MAX_TEXTO;


Mensaje::Mensaje() : emisor(-1), receptor(-1), texto(), fecha(), leido(false) {}


Mensaje::Mensaje(unsigned int emisor, unsigned int receptor, const string& texto, const Fecha& fecha, bool leido)
    : emisor(emisor), receptor(receptor), texto(texto), fecha(fecha), leido(leido) {}

unsigned int Mensaje::getEmisor() const {
    return emisor;
}

unsigned int Mensaje::getReceptor() const {
    return receptor;
}

const string& Mensaje::getTexto() const {
    return texto;
}

const Fecha& Mensaje::getFecha() const {
    return fecha;
}

bool Mensaje::estaLeido() const {
    return leido;
}

void Mensaje::marcarLeido() {
    leido = true;
}