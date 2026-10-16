#ifndef PLATAFORMA_H
#define PLATAFORMA_H

#include <random>
#include <string>
#include "fecha.h"
#include "Vector.h"
#include "usuario.h"
#include "mensaje.h"
#include "album.h"
#include "matrizAmistad.h"
#include "persistencia.h"

using namespace std;

// Nucleo de Ude@Book: agrega todos los datos y resuelve la logica de cada
// funcionalidad en RAM. No lee de consola; la interaccion vive en Menu.
class Plataforma {
public:
    static const char* const SUBRUTA;      // sub-ruta fija de las fotos
    static const char* const RUTA_DATOS;   // carpeta de los archivos

    // Resultados de las validaciones
    static const int OK = 0;
    static const int NO_EXISTE = 1;
    static const int MISMO_USUARIO = 2;
    static const int YA_AMIGOS = 3;
    static const int SOLICITUD_RECIBIDA = 4;   // el destino ya me envio una
    static const int SOLICITUD_ENVIADA = 5;    // yo ya le envie una
    static const int NO_ES_AMIGO = 6;
    static const int TEXTO_VACIO = 7;
    static const int TEXTO_LARGO = 8;

    static const int TAM_GRUPO = 3;          // fotos de amigos en el ingreso
    static const int MIN_AMIGOS_ALTERNAR = 4;

    Plataforma();

    // Carga + menu. Devuelve el codigo de salida del programa.
    int ejecutar();

    // Busqueda de usuarios (sobrecarga de metodos): por nombre devuelve el
    // indice (busqueda binaria, los usuarios estan en orden alfabetico);
    // por indice devuelve el usuario.
    int buscarUsuario(const string& nombre) const;
    const Usuario& buscarUsuario(int indice) const;

    // RF-II: sesion y mejores amigos
    bool iniciarSesion(const string& nombre, const string& contrasena);
    void cerrarSesion();
    int getActual() const;
    int cantidadAmigos() const;
    int tamanoGrupo() const;
    int amigoDelGrupo(int i) const;
    bool yaAlterno() const;
    bool puedeAlternar() const;
    bool alternarGrupo();
    string rutaPerfil(int usuario) const;

    // RF-III: solicitudes
    int enviarSolicitud(int destino);
    int cantidadSolicitudes() const;
    int solicitante(int i) const;
    void aceptarSolicitud(int i);
    void rechazarSolicitud(int i);

    // RF-IV: perfil de amigo
    int validarAmigo(int usuario) const;
    void albumesVisibles(int amigo, Vector<int>& salida) const;
    const Album& getAlbum(int usuario, int album) const;
    string rutaFoto(int usuario, int album, int foto) const;
    bool tieneLike(int usuario, int album, int foto) const;
    bool alternarLike(int usuario, int album, int foto);

    // RF-V: sugerencias
    void sugerencias(int k, Vector<int>& candidatos, Vector<int>& comunes);

    // RF-VI: mensajes
    int enviarMensaje(int destino, const string& texto);
    const Vector<int>& noLeidosActual() const;
    void marcarNoLeidosComoLeidos();
    const Vector<int>& recibidosActual() const;
    void buscarRango(const Fecha& inicio, const Fecha& fin, int& desde, int& hasta) const;
    const Mensaje& getMensaje(int i) const;

    // RF-VII
    unsigned long long memoriaTotal() const;

private:
    Vector<Usuario> usuarios;
    Vector<Mensaje> mensajes;
    MatrizAmistad amistades;
    Persistencia persistencia;
    mt19937 generador;
    Fecha reloj;                      // fecha del mensaje mas reciente

    // Estado de la sesion
    int actual;
    Vector<int> amigosSesion;         // amigos al iniciar sesion
    Vector<int> pesosSesion;          // peso 2/5/7 de cada amigo
    Vector<int> grupo;                // posiciones en amigosSesion
    bool alternado;                   // la opcion N ya se uso
    Vector<const Foto*> likesSesion;  // fotos a las que di like en esta sesion

    bool cargar();
    static int pesoPorComunes(int comunes);
    int girarRuleta(const Vector<bool>& usado);
    bool hayPendiente(int a, int b) const;
    bool esMejorCandidato(int comunesA, int usuarioA, int comunesB, int usuarioB) const;
    string rutaAlbumes(int usuario) const;
    int buscarLike(const Foto* foto) const;
};

#endif
