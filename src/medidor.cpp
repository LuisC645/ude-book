#include <iostream>
#include "medidor.h"

using namespace std;

Medidor::Medidor() {}

void Medidor::contar(){
    iteraciones++;
}

void Medidor::externoAleatorio() {
    usosAleatorio++;
}

void Medidor::externoArchivo() {
    usosArchivo++;
}

void Medidor::externoReloj() {
    usosReloj++;
}

void Medidor::registrarLocal(unsigned long long bytes) {
    bytesLocales += bytes;
}

void Medidor::reiniciar() {
    iteraciones = 0;
    usosAleatorio = 0;
    usosArchivo = 0;
    usosReloj = 0;
    bytesLocales = 0;
}

void Medidor::reporte(unsigned long long memEstruct) {

    cout << "\n------ Consumo de recursos ------\n";
    cout << "Iteraciones: " << iteraciones << "\n";
    cout << "Componentes externos: ";
    bool alguno = false;
    if (usosAleatorio > 0) {
        cout << "generador aleatorio x" << usosAleatorio;
        alguno = true;
    }
    if (usosArchivo > 0) {
        cout << (alguno ? ", " : "") << "archivos (fstream) x" << usosArchivo;
        alguno = true;
    }
    if (usosReloj > 0) {
        cout << (alguno ? ", " : "") << "reloj del sistema (ctime) x" << usosReloj;
        alguno = true;
    }
    if (!alguno) {
        cout << "ninguno";
    }
    cout << "\nMemoria en este momento: " << (memEstruct + bytesLocales) << " bytes\n";
    cout << "---------------------------------\n";
}



