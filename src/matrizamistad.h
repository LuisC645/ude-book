#ifndef MATRIZAMISTAD_H
#define MATRIZAMISTAD_H

#include "Vector.h"

class MatrizAmistad {
public:

    // Constructores
    MatrizAmistad();
    MatrizAmistad(const MatrizAmistad& otra);

    // Destructor
    ~MatrizAmistad();

    MatrizAmistad& operator=(const MatrizAmistad& otra);

    void inicializar(int cantidadUsuarios);
    int cantidad() const;

    void agregar(int a, int b); // amistad mutua
    bool sonAmigos(int a, int b) const;
    int comunes(int a, int b) const;
    void amigosDe(int a, Vector<int>& salida) const;

    unsigned long long memoriaDinamica() const;

private:
    int n;
    bool* celdas;

    void copiarDe(const MatrizAmistad& otra);
};

#endif // MATRIZAMISTAD_H
