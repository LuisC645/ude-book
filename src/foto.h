#ifndef FOTO_H
#define FOTO_H

#include <string>
#include "fecha.h"

class Foto
{
public:

    Foto();
    Foto(const string& nombre, const Fecha& captura, unsigned int tamano, unsigned short likes);

    // getters
    const string& getNombre() const;
    unsigned short getLikes() const;
    const Fecha& getCaptura() const;
    unsigned int getTamano() const;

    // rutaAlbum = ".../albums/<album>/"  ->  ".../albums/<album>/<foto>.png"
    string getRuta(const string& rutaAlbum) const;

    // setters
    void setLikes(unsigned short nuevosLikes);

private:
    string nombre;
    Fecha captura;
    unsigned int tamano; // bytes
    unsigned short likes;

};

#endif // FOTO_H
