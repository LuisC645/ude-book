#include "matrizamistad.h"
#include "medidor.h"

MatrizAmistad::MatrizAmistad() : n(0), celdas(nullptr) {}

MatrizAmistad::MatrizAmistad(const MatrizAmistad& otra) : n(0), celdas(nullptr) {
    copiarDe(otra);
}

MatrizAmistad::~MatrizAmistad() {
    delete[] celdas;
}

MatrizAmistad& MatrizAmistad::operator=(const MatrizAmistad& otra) {
    if (this != &otra) {
        delete[] celdas;
        celdas = nullptr;
        copiarDe(otra);
    }
    return *this;
}

void MatrizAmistad::copiarDe(const MatrizAmistad& otra) {
    n = otra.n;
    if (n > 0) {
        celdas = new bool[n * n];
        for (unsigned int i = 0; i < n * n; i++) {
            Medidor::contar();
            celdas[i] = otra.celdas[i];
        }
    }
}

void MatrizAmistad::inicializar(unsigned int cantidadUsuarios) {
    delete[] celdas;
    celdas = nullptr;
    n = cantidadUsuarios;
    if (n > 0) {
        celdas = new bool[n * n];
        for (unsigned int i = 0; i < n * n; i++) {
            Medidor::contar();
            celdas[i] = false;
        }
    }
}

unsigned int MatrizAmistad::cantidad() const {
    return n;
}

void MatrizAmistad::agregar(unsigned int a, unsigned int b) {
    celdas[a * n + b] = true;
    celdas[b * n + a] = true;
}

bool MatrizAmistad::sonAmigos(unsigned int a, unsigned int b) const {
    return celdas[a * n + b];
}

unsigned int MatrizAmistad::comunes(unsigned int a, unsigned int b) const {
    unsigned int total = 0;
    for (unsigned int k = 0; k < n; k++) {
        Medidor::contar();
        if (celdas[a * n + k] && celdas[b * n + k]) {
            total++;
        }
    }
    return total;
}

void MatrizAmistad::amigosDe(unsigned int a, Vector<unsigned int>& salida) const {
    salida.vaciar();
    for (unsigned int k = 0; k < n; k++) {
        Medidor::contar();
        if (celdas[a * n + k]) {
            salida.agregar(k);
        }
    }
}

unsigned long long MatrizAmistad::memoriaDinamica() const {
    return (unsigned long long)n * (unsigned long long)n * sizeof(bool);
}
