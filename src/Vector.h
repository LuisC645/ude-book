#ifndef VECTOR_H
#define VECTOR_H

#include "medidor.h"

// Posicion inexistente (resultado de una busqueda sin exito)
const unsigned int NINGUNO = 0xFFFFFFFFu;

template<typename T>
class Vector {
public:

    Vector() : datos(nullptr), tam(0), cap(0) {}

    Vector(const Vector& otro) : datos(nullptr), tam(0), cap(0) {
        copiarDe(otro);
    }

    // Sobrecarga =: copia profunda (cada vector tiene su propio arreglo)
    Vector& operator=(const Vector& otro) {
        if (this != &otro) {
            delete[] datos;
            datos = nullptr;
            tam = 0;
            cap = 0;
            copiarDe(otro);
        }
        return *this;
    }

    ~Vector() {
        delete[] datos;
    }

    void reservar(unsigned int nuevaCap) {
        if (nuevaCap <= cap) {
            return;
        }

        T* nuevo = new T[nuevaCap];
        for (unsigned int i = 0; i < tam; i++) {
            Medidor::contar();
            nuevo[i] = datos[i];
        }

        delete[] datos;
        datos = nuevo;
        cap = nuevaCap;
    }

    void agregar(const T& x) {

        if (tam == cap) {
            unsigned int nuevaCap = (cap == 0) ? 4 : cap * 2;
            T* nuevo = new T[nuevaCap];

            for (unsigned int i = 0; i < tam; i++) {
                Medidor::contar();
                nuevo[i] = datos[i];
            }

            nuevo[tam] = x;
            delete[] datos;
            datos = nuevo;
            cap = nuevaCap;

        } else {
            datos[tam] = x;
        }

        tam++;
    }

    // Sobrecarga: agrega el mismo valor varias veces
    void agregar(const T& x, unsigned int veces) {
        reservar(tam + veces);
        for (unsigned int i = 0; i < veces; i++) {
            Medidor::contar();
            datos[tam] = x;
            tam++;
        }
    }

    void quitarEn(unsigned int i) {
        for (unsigned int j = i; j + 1 < tam; j++) {
            Medidor::contar();
            datos[j] = datos[j + 1];
        }
        tam--;
    }

    void vaciar() {
        tam = 0;
    }

    unsigned int tamano() const {
        return tam;
    }

    explicit Vector(unsigned int capacidadInicial) : datos(nullptr), tam(0), cap(0) {
        reservar(capacidadInicial);
    }

    unsigned long long memoriaArreglo() const {
        return (unsigned long long)cap * sizeof(T);
    }

    // Sobrecarga []
    T& operator[](unsigned int i) {
        return datos[i];
    }
    const T& operator[](unsigned int i) const {
        return datos[i];
    }

private:
    T* datos;
    unsigned int tam;
    unsigned int cap;

    void copiarDe(const Vector& otro) {
        if (otro.cap > 0) {
            datos = new T[otro.cap];
            cap = otro.cap;
        }
        for (unsigned int i = 0; i < otro.tam; i++) {
            Medidor::contar();
            datos[i] = otro.datos[i];
        }
        tam = otro.tam;
    }
};

#endif // VECTOR_H
