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

    void inicializar(unsigned int cantidadUsuarios);
    unsigned int cantidad() const;

    void agregar(unsigned int a, unsigned int b); // amistad mutua
    bool sonAmigos(unsigned int a, unsigned int b) const;
    unsigned int comunes(unsigned int a, unsigned int b) const;
    void amigosDe(unsigned int a, Vector<unsigned int>& salida) const;

    unsigned long long memoriaDinamica() const;

private:
    unsigned int n;
    bool* celdas;

    void copiarDe(const MatrizAmistad& otra);
};

#endif // MATRIZAMISTAD_H
