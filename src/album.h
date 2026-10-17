#ifndef ALBUM_H
#define ALBUM_H

#include <string>
#include "foto.h"
#include "Vector.h"

class Album
{
public:

    // Privacidad
    static const unsigned short PUBLICO = 0;
    static const unsigned short AMIGOS = 1;
    static const unsigned short PRIVADO = 2;

    Album();
    Album(const string& nombre, unsigned short privacidad);

    // getters
    const string& getNombre() const;
    unsigned short getPrivacidad() const;
    const Foto& getFoto(unsigned int i) const;

    // Guardar foto en album
    Foto& getFoto(unsigned int i);

    // rutaUsuario = ".../albums/"  ->  ".../albums/<album>/"
    string getRuta(const string& rutaUsuario) const;

    // Demas
    void agregarFoto(const Foto& foto);
    unsigned int cantidadFotos() const;

    bool esVisibleParaAmigos() const;

    unsigned long long memoriaDinamica() const;

private:
    string nombre;
    unsigned short privacidad;
    Vector<Foto> fotos;

};

#endif // ALBUM_H
