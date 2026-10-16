#include "fecha.h"
#include "medidor.h"
#include "texto.h"

#include <ctime>

Fecha::Fecha() : ano(2000), mes(1), dia(1), hora(0), min(0), conHora(false) {}

Fecha::Fecha(unsigned short ano, unsigned short mes, unsigned short dia, unsigned short hora, unsigned short min) :
    ano(ano), mes(mes), dia(dia), hora(hora), min(min), conHora(true) {}


Fecha Fecha::ahora() {
    time_t segundos = time(nullptr);
    tm* local = localtime(&segundos);
    return Fecha(local->tm_mday, local->tm_mon + 1, local->tm_year + 1900,
                 local->tm_hour, local->tm_min);
}

bool Fecha::esBisiesto(unsigned short ano) {
    return ano % 4 == 0 && (ano % 100 != 0 || ano % 400 == 0);
}


unsigned short Fecha::diasDelMes(unsigned short mes, unsigned short ano) {
    if (mes == 2) {
        return esBisiesto(ano) ? 29 : 28;
    }
    if (mes == 4 || mes == 6 || mes == 9 || mes == 11) {
        return 30;
    }
    return 31;
}

bool Fecha::esValida() const {
    if (ano < 1 || ano > 9999 || mes < 1 || mes > 12) {
        return false;
    }
    if (dia < 1 || dia > diasDelMes(mes, ano)) {
        return false;
    }
    return hora >= 0 && hora <= 23 && min >= 0 && min <= 59;
}

// Valida que el trozo sean solo digitos y lo convierte con stoi (via Texto::aEntero).
bool Fecha::numeroEn(const string& texto, int desde, int cuantos, int& valor) {
    return Texto::aEntero(texto.substr(desde, cuantos), valor);
}

bool Fecha::desdeTexto(const string& texto, Fecha& salida) {

    string t = Texto::recortado(texto);

    if (t.length() != 10 || t[2] != '/' || t[5] != '/') {
        return false;
    }

    int d = 0;
    int m = 0;
    int a = 0;

    if (!numeroEn(t, 0, 2, d) || !numeroEn(t, 3, 2, m) || !numeroEn(t, 6, 4, a)) {
        return false;
    }

    Fecha f(d, m, a, 0, 0);
    f.conHora = false;
    if (!f.esValida()) {
        return false;
    }
    salida = f;
    return true;
}

bool Fecha::desdeCompacta(const string& texto, Fecha& salida) {
    int n = texto.length();

    if (n != 8 && n != 12) {
        return false;
    }
    int a = 0;
    int m = 0;
    int d = 0;
    int h = 0;
    int mi = 0;

    if (!numeroEn(texto, 0, 4, a) || !numeroEn(texto, 4, 2, m) || !numeroEn(texto, 6, 2, d)) {
        return false;
    }
    if (n == 12 && (!numeroEn(texto, 8, 2, h) || !numeroEn(texto, 10, 2, mi))) {
        return false;
    }

    Fecha f(d, m, a, h, mi);
    f.conHora = (n == 12);
    if (!f.esValida()) {
        return false;
    }
    salida = f;
    return true;
}

// Escribe valor con exactamente 'cifras' digitos, rellenando con ceros a la izquierda
void Fecha::escribirCifras(ostream& salida, int valor, int cifras) {
    char anterior = salida.fill('0');   // relleno con '0' (guarda el que habia)
    salida.width(cifras);               // ancho minimo del siguiente dato
    salida << valor;
    salida.fill(anterior);              // deja el flujo como estaba
}

void Fecha::escribirCompacta(ostream& salida, bool incluirHora) const {
    escribirCifras(salida, ano, 4);
    escribirCifras(salida, mes, 2);
    escribirCifras(salida, dia, 2);
    if (incluirHora) {
        escribirCifras(salida, hora, 2);
        escribirCifras(salida, min, 2);
    }
}

void Fecha::fijarHora(int nuevaHora, int nuevoMinuto) {
    hora = nuevaHora;
    min = nuevoMinuto;
    conHora = true;
}

// AAAAMMDDhhmm
unsigned long long Fecha::clave() const {
    unsigned long long c = (unsigned long long)ano;
    c = c * 100 + (unsigned long long)mes;
    c = c * 100 + (unsigned long long)dia;
    c = c * 100 + (unsigned long long)hora;
    c = c * 100 + (unsigned long long)min;
    return c;
}

bool Fecha::operator<(const Fecha& otra) const {
    return clave() < otra.clave();
}

bool Fecha::operator<=(const Fecha& otra) const {
    return clave() <= otra.clave();
}

ostream& operator<<(ostream& salida, const Fecha& fecha) {
    Fecha::escribirCifras(salida, fecha.dia, 2);
    salida << '/';
    Fecha::escribirCifras(salida, fecha.mes, 2);
    salida << '/' << fecha.ano;
    if (fecha.conHora) {
        salida << ' ';
        Fecha::escribirCifras(salida, fecha.hora, 2);
        salida << ':';
        Fecha::escribirCifras(salida, fecha.min, 2);
    }
    return salida;
}
