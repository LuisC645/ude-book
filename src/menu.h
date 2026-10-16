#ifndef MENU_H
#define MENU_H

#include <string>
#include "fecha.h"

class Plataforma;

// Interaccion con el usuario por consola: lee y valida entradas, llama a
// Plataforma y muestra los resultados y las metricas de cada funcionalidad.
// Nunca usa cin >>: todo se lee como linea completa y luego se valida.
class Menu {
public:
    explicit Menu(Plataforma& plataforma);

    void ejecutar();

private:
    Plataforma& plataforma;
    bool cerrada;        // la entrada estandar se termino (Ctrl+D / Ctrl+Z)

    bool leer(const char* mensaje, string& linea);
    bool leerEntero(const char* mensaje, int minimo, int maximo, int& valor);
    void reportar() const;

    // RF-II
    void mostrarEncabezado() const;
    void menuPrincipal();
    void alternarFotos();

    // RF-III
    void menuAmistades();
    void enviarSolicitud();
    void responderSolicitudes(bool aceptar);
    void mostrarResultadoSolicitud(int codigo) const;

    // RF-IV
    void visitarPerfil();
    void recorrerAlbum(int amigo, int album);

    // RF-V
    void verSugerencias();

    // RF-VI
    void menuMensajes();
    void enviarMensaje();
    void leerNuevos();
    void buscarPorFechas();
    void mostrarMensaje(int indice) const;
    bool leerFecha(const char* mensaje, Fecha& fecha);
};

#endif
