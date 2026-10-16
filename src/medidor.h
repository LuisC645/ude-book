#ifndef MEDIDOR_H
#define MEDIDOR_H

class Medidor
{
public:
    Medidor();

    // Para contar bucles en general
    static void contar();

    // Aleatorio es necesario???????
    static void externoAleatorio();
    static void externoArchivo();
    static void externoReloj();

    // Cuanta memoria se usa
    static void registrarLocal(unsigned long long bytes);

    static void reiniciar();
    static void reportar(unsigned long long memEstructuras);


private:

    static unsigned long long iteraciones;

    static unsigned long long usosAleatorio;
    static unsigned long long usosArchivo;
    static unsigned long long usosReloj;

    static unsigned long long bytesLocales;

};

#endif // MEDIDOR_H
