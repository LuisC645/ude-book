#ifndef USUARIO_H
#define USUARIO_H

#include <string>
#include "fecha.h"
#include "album.h"
#include "Vector.h"

class Usuario
{
public:
    Usuario();
    Usuario(const string& nombre, const Fecha& nacimiento, const string& correo,
            const string& contrasena, const string& ciudad, const string& pais,
            const Fecha& inscripcion);

    const string& getNombre() const;
    bool contrasenaCorrecta(const string& intento) const;

    // Albumes
    void agregarAlbum(const Album& album);
    unsigned int cantidadAlbumes() const;
    const Album& getAlbum(unsigned int i) const;
    Album& getAlbum(unsigned int i);

    // Mensajes (indices al conjunto global, ordenados por fecha)
    void agregarRecibido(unsigned int mensaje);
    void agregarEnviado(unsigned int mensaje);
    void agregarNoLeido(unsigned int mensaje);
    const Vector<unsigned int>& getRecibidos() const;
    const Vector<unsigned int>& getNoLeidos() const;
    void vaciarNoLeidos();

    // Solicitudes de amistad pendientes que este usuario ha recibido
    void agregarSolicitud(unsigned int origen);
    unsigned int cantidadSolicitudes() const;
    unsigned int getSolicitudOrigen(unsigned int i) const;
    void quitarSolicitud(unsigned int i);
    unsigned int buscarSolicitudDe(unsigned int origen) const;   // posicion o NINGUNO

    unsigned long long memoriaDinamica() const;

private:
    string nombre;
    Fecha nacimiento;
    string correo;
    string contrasena;
    string ciudad;
    string pais;
    Fecha inscripcion;

    Vector<Album> albumes;
    Vector<unsigned int> recibidos;
    Vector<unsigned int> enviados;
    Vector<unsigned int> noLeidos;
    Vector<unsigned int> solicitudes;

};

#endif // USUARIO_H
