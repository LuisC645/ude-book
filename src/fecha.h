#ifndef FECHA_H
#define FECHA_H

#include <iostream>
#include <string>

using namespace std;

class Fecha
{
public:
    Fecha();
    Fecha(unsigned short ano, unsigned short mes, unsigned short dia, unsigned short hora, unsigned short min);

    // Fecha del sistema
    static Fecha ahora();

    static bool esBisiesto(unsigned short ano);
    static unsigned short diasDelMes(unsigned short mes, unsigned short ano);

    bool esValida() const;

    // "DD/MM/AAAA"
    static bool desdeTexto(const string& texto, Fecha& salida);

    // "AAAAMMDD" o "AAAAMMDDhhmm" (formato de los archivos)
    static bool desdeCompacta(const string& texto, Fecha& salida);

    // Escribe "AAAAMMDD" o "AAAAMMDDhhmm" en un archivo.
    void escribirCompacta(ostream& salida, bool incluirHora) const;


    void fijarHora(int nuevaHora, int nuevoMinuto);

    bool operator<(const Fecha& otra) const;
    bool operator<=(const Fecha& otra) const;

    friend ostream& operator<<(ostream& salida, const Fecha& fecha);

private:

    unsigned short ano;
    unsigned short mes;
    unsigned short dia;
    unsigned short hora;
    unsigned short min;
    bool conHora;

    unsigned long long clave() const;   // AAAAMMDDhhmm como un solo numero
    static bool numeroEn(const string& texto, int desde, int cuantos, int& valor);
    static void escribirCifras(ostream& salida, int valor, int cifras);
};

#endif // FECHA_H
