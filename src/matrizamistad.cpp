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
        for (int i = 0; i < n * n; i++) {
            Medidor::contar();
            celdas[i] = otra.celdas[i];
        }
    }
}

void MatrizAmistad::inicializar(int cantidadUsuarios) {
    delete[] celdas;
    celdas = nullptr;
    n = cantidadUsuarios;
    if (n > 0) {
        celdas = new bool[n * n];
        for (int i = 0; i < n * n; i++) {
            Medidor::contar();
            celdas[i] = false;
        }
    }
}

int MatrizAmistad::cantidad() const {
    return n;
}

void MatrizAmistad::agregar(int a, int b) {
    celdas[a * n + b] = true;
    celdas[b * n + a] = true;
}

bool MatrizAmistad::sonAmigos(int a, int b) const {
    return celdas[a * n + b];
}

int MatrizAmistad::comunes(int a, int b) const {
    int total = 0;
    for (int k = 0; k < n; k++) {
        Medidor::contar();
        if (celdas[a * n + k] && celdas[b * n + k]) {
            total++;
        }
    }
    return total;
}

void MatrizAmistad::amigosDe(int a, Vector<int>& salida) const {
    salida.vaciar();
    for (int k = 0; k < n; k++) {
        Medidor::contar();
        if (celdas[a * n + k]) {
            salida.agregar(k);
        }
    }
}

unsigned long long MatrizAmistad::memoriaDinamica() const {
    return (unsigned long long)n * (unsigned long long)n * sizeof(bool);
}
