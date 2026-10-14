#ifndef TEXTO_H
#define TEXTO_H

#include <iostream>
#include <string>

using namespace std;

class Texto
{
public:
    Texto();

    // Quitar \r
    static bool leerLinea(istream& entrada, string& linea);

    // Quitar espacio inicio y fin
    static string recortado(const string& texto);

    // Verificar espacios o si si hay algo
    static bool esVacio(const string& texto);

    static bool contiene(const string& texto, char c);

    // Convertir a entero
    static bool aEntero(const string& texto, int& salida);

    static bool esEspacio(char c);

};

#endif // TEXTO_H
