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
    int cantidadAlbumes() const;
    const Album& getAlbum(int i) const;
    Album& getAlbum(int i);

    // Mensajes (indices al conjunto global, ordenados por fecha)
    void agregarRecibido(int mensaje);
    void agregarEnviado(int mensaje);
    void agregarNoLeido(int mensaje);
    const Vector<int>& getRecibidos() const;
    const Vector<int>& getNoLeidos() const;
    void vaciarNoLeidos();

    // Solicitudes de amistad pendientes que este usuario ha recibido
    void agregarSolicitud(int origen);
    int cantidadSolicitudes() const;
    int getSolicitudOrigen(int i) const;
    void quitarSolicitud(int i);
    int buscarSolicitudDe(int origen) const;   // posicion o -1

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
    Vector<int> recibidos;
    Vector<int> enviados;
    Vector<int> noLeidos;
    Vector<int> solicitudes;

};

#endif // USUARIO_H
