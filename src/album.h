#ifndef ALBUM_H
#define ALBUM_H

#include <string>
#include "Foto.h"
#include "Vector.h"

class Album
{
public:

    // Privacidad
    static const int PUBLICO = 0;
    static const int AMIGOS = 1;
    static const int PRIVADO = 2;

    Album();
    Album(const string& nombre, unsigned short privacidad);

    // getters
    const string& getNombre() const;
    unsigned short getPrivacidad() const;
    const Foto& getFoto(int i) const;

    // Guardar foto en album
    Foto& getFoto(int i);

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
