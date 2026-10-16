#ifndef MENSAJE_H
#define MENSAJE_H

#include <string>
#include "fecha.h"

class Mensaje
{
public:

    static const int MAX_TEXTO = 512;

    Mensaje();
    Mensaje(unsigned int emisor, unsigned int receptor, const string& texto, const Fecha& fecha, bool leido);

    // getters
    unsigned int getEmisor() const;
    unsigned int getReceptor() const;

    const string& getTexto() const;
    const Fecha& getFecha() const;

    // Demas
    bool estaLeido() const;
    void marcarLeido();

    // setter

private:

    unsigned int emisor;
    unsigned int receptor;
    string texto;
    Fecha fecha;
    bool leido;
};

#endif // MENSAJE_H
