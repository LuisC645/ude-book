#ifndef MENU_H
#define MENU_H

#include <string>
#include "fecha.h"

class Plataforma;

class Menu {
public:
    explicit Menu(Plataforma& plataforma);

    void ejecutar();

private:
    Plataforma& plataforma;
    bool cerrada;

    bool leer(const char* mensaje, string& linea);
    bool leerEntero(const char* mensaje, unsigned int minimo, unsigned int maximo, unsigned int& valor);
    void reportar() const;

    void mostrarEncabezado() const;
    void menuPrincipal();
    void alternarFotos();

    void menuAmistades();
    void enviarSolicitud();
    void responderSolicitudes(bool aceptar);
    void mostrarResultadoSolicitud(unsigned short codigo) const;

    void visitarPerfil();
    void recorrerAlbum(unsigned int amigo, unsigned int album);

    void verSugerencias();

    void menuMensajes();
    void enviarMensaje();
    void leerNuevos();
    void buscarPorFechas();
    void mostrarMensaje(unsigned int indice) const;
    bool leerFecha(const char* mensaje, Fecha& fecha);
};

#endif
