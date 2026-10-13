#ifndef VECTOR_H
#define VECTOR_H

template<typename T>
class Vector {
public:

    Vector() : datos(nullptr), tam(0), cap(0) {}

    Vector(const Vector& otro) : datos(nullptr), tam(0), cap(0) {
        copiarDe(otro);
    }

    void reservar(int nuevaCap) {
        if (nuevaCap <= cap) {
            return;
        }

        T* nuevo = new T[nuevaCap];
        for (int i = 0; i < tam; i++) {
            nuevo[i] = datos[i];
        }

        delete[] datos;
        datos = nuevo;
        cap = nuevaCap;
    }

    void agregar(const T& x) {

        if (tam == cap) {
            int nuevaCap = (cap == 0) ? 4 : cap * 2;
            T* nuevo = new T[nuevaCap];

            for (int i = 0; i < tam; i++) {
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
    void agregar(const T& x, int veces) {
        reservar(tam + veces);
        for (int i = 0; i < veces; i++) {
            datos[tam] = x;
            tam++;
        }
    }

    void quitarEn(int i) {
        for (int j = i; j < tam - 1; j++) {
            datos[j] = datos[j + 1];
        }
        tam--;
    }

    void vaciar() {
        tam = 0;
    }

    int tamano() const {
        return tam;
    }

    explicit Vector(int capacidadInicial) : datos(nullptr), tam(0), cap(0) {
        reservar(capacidadInicial);
    }

    unsigned long long memoriaArreglo() const {
        return (unsigned long long)cap * sizeof(T);
    }

    // Sobrecarga []
    T& operator[](int i) {
        return datos[i];
    }
    const T& operator[](int i) const {
        return datos[i];
    }

private:
    T* datos;
    int tam;
    int cap;

    void copiarDe(const Vector& otro) {
        if (otro.cap > 0) {
            datos = new T[otro.cap];
            cap = otro.cap;
        }
        for (int i = 0; i < otro.tam; i++) {
            datos[i] = otro.datos[i];
        }
        tam = otro.tam;
    }
};

#endif // VECTOR_H
