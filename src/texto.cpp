#include "texto.h"
#include "medidor.h"

Texto::Texto() {}

bool Texto::esEspacio(char c) {

    if(c == ' ' || c == '\t'){
        return true;
    } else {
        return false;
    }
}

bool Texto::leerLinea(istream& entrada, string& linea) {
    if (!getline(entrada, linea)) {
        return false;
    }
    if (!linea.empty() && linea[linea.length() - 1] == '\r') {
        linea.erase(linea.length() - 1);
    }
    return true;
}

string Texto::recortado(const string& texto) {
    unsigned int lon = (unsigned int)texto.size();
    unsigned int inicio = 0;

    while (inicio < lon && esEspacio(texto[inicio])) {
        Medidor::contar();
        inicio++;
    }

    unsigned int fin = lon;
    while (fin > inicio && esEspacio(texto[fin - 1])) {
        Medidor::contar();
        fin--;
    }

    return texto.substr(inicio, fin - inicio);
}

bool Texto::esVacio(const string& texto) {
    for (unsigned int i = 0; i < texto.size(); i++) {
        Medidor::contar();
        if (!esEspacio(texto[i])) {
            return false;
        }
    }
    return true;
}


bool Texto::contiene(const string& texto, char c) {
    for (unsigned int i = 0; i < texto.size(); i++) {
        Medidor::contar();
        if (texto[i] == c) {
            return true;
        }
    }
    return false;
}

bool Texto::aEntero(const string& texto, unsigned int& salida) {

    string t = recortado(texto);

    if (t.empty() || t.find_first_not_of("0123456789") != string::npos) {
        return false;
    }

    salida = (unsigned int)stoul(t);
    return true;
}




