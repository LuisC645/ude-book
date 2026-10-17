#ifndef PLATAFORMA_H
#define PLATAFORMA_H

#include <random>
#include <string>
#include "fecha.h"
#include "Vector.h"
#include "usuario.h"
#include "mensaje.h"
#include "album.h"
#include "matrizamistad.h"
#include "persistencia.h"

using namespace std;

class Plataforma {
public:
    static const char* const SUBRUTA;
    static const char* const RUTA_DATOS;

    static const unsigned short OK = 0;
    static const unsigned short NO_EXISTE = 1;
    static const unsigned short MISMO_USUARIO = 2;
    static const unsigned short YA_AMIGOS = 3;
    static const unsigned short SOLICITUD_RECIBIDA = 4;
    static const unsigned short SOLICITUD_ENVIADA = 5;
    static const unsigned short NO_ES_AMIGO = 6;
    static const unsigned short TEXTO_VACIO = 7;
    static const unsigned short TEXTO_LARGO = 8;

    static const unsigned short TAM_GRUPO = 3;
    static const unsigned short MIN_AMIGOS_ALTERNAR = 4;

    Plataforma();

    int ejecutar();

    unsigned int buscarUsuario(const string& nombre) const;
    const Usuario& buscarUsuario(unsigned int indice) const;

    bool iniciarSesion(const string& nombre, const string& contrasena);
    void cerrarSesion();
    unsigned int getActual() const;
    unsigned int cantidadAmigos() const;
    unsigned int tamanoGrupo() const;
    unsigned int amigoDelGrupo(unsigned int i) const;
    bool yaAlterno() const;
    bool puedeAlternar() const;
    bool alternarGrupo();
    string rutaPerfil(unsigned int usuario) const;

    unsigned short enviarSolicitud(unsigned int destino);
    unsigned int cantidadSolicitudes() const;
    unsigned int solicitante(unsigned int i) const;
    void aceptarSolicitud(unsigned int i);
    void rechazarSolicitud(unsigned int i);

    unsigned short validarAmigo(unsigned int usuario) const;
    void albumesVisibles(unsigned int amigo, Vector<unsigned int>& salida) const;
    const Album& getAlbum(unsigned int usuario, unsigned int album) const;
    string rutaFoto(unsigned int usuario, unsigned int album, unsigned int foto) const;
    bool tieneLike(unsigned int usuario, unsigned int album, unsigned int foto) const;
    bool alternarLike(unsigned int usuario, unsigned int album, unsigned int foto);

    void sugerencias(unsigned short k, Vector<unsigned int>& candidatos, Vector<unsigned int>& comunes);

    unsigned short enviarMensaje(unsigned int destino, const string& texto);
    const Vector<unsigned int>& noLeidosActual() const;
    void marcarNoLeidosComoLeidos();
    const Vector<unsigned int>& recibidosActual() const;
    void buscarRango(const Fecha& inicio, const Fecha& fin, unsigned int& desde, unsigned int& hasta) const;
    const Mensaje& getMensaje(unsigned int i) const;

    unsigned long long memoriaTotal() const;

private:
    Vector<Usuario> usuarios;
    Vector<Mensaje> mensajes;
    MatrizAmistad amistades;
    Persistencia persistencia;
    mt19937 generador;
    Fecha reloj;

    unsigned int actual;
    Vector<unsigned int> amigosSesion;
    Vector<unsigned short> pesosSesion;
    Vector<unsigned int> grupo;
    bool alternado;
    Vector<const Foto*> likesSesion;

    bool cargar();
    static unsigned short pesoPorComunes(unsigned int comunes);
    unsigned int girarRuleta(const Vector<bool>& usado);
    bool hayPendiente(unsigned int a, unsigned int b) const;
    bool esMejorCandidato(unsigned int comunesA, unsigned int usuarioA,
                          unsigned int comunesB, unsigned int usuarioB) const;
    string rutaAlbumes(unsigned int usuario) const;
    unsigned int buscarLike(const Foto* foto) const;
};

#endif
