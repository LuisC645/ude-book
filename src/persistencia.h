#ifndef PERSISTENCIA_H
#define PERSISTENCIA_H

#include <fstream>
#include <string>
#include "Vector.h"
#include "usuario.h"
#include "mensaje.h"
#include "matrizAmistad.h"

using namespace std;

// Lectura y escritura del almacenamiento permanente
// Todos los archivos son de texto: un registro por linea, campos separados por '|'
//   usuarios.txt     usuario|AAAAMMDD nacimiento|correo|contrasena|ciudad|pais|AAAAMMDD inscripcion
//                    (ordenados alfabeticamente y el indice del usuario es su numero de linea)
//   amistades.txt    idA|idB
//   solicitudes.txt  idOrigen|idDestino (solo las pendientes)
//   albumes.txt      idDueno|nombre|privacidad (agrupados por dueno, en orden)
//   fotos.txt        idAlbum|nombre|AAAAMMDD|tamano|likes
//   mensajes.txt     idEmisor|idReceptor|AAAAMMDDhhmm|leido(0/1)|texto (ordenados por fecha)
// Los archivos solo se leen al iniciar. Despues, cada cambio se guarda de inmediato
// reescribiendo el archivo afectado con los datos que ya estan en memoria.

class Persistencia {
public:
    void fijarCarpeta(const string& rutaCarpeta);

    // Carga. Ante un error muestran archivo, linea y motivo, y devuelven false.
    bool cargarUsuarios(Vector<Usuario>& usuarios);
    bool cargarAmistades(MatrizAmistad& matriz);
    bool cargarSolicitudes(Vector<Usuario>& usuarios);
    bool cargarAlbumes(Vector<Usuario>& usuarios, Vector<int>& albumDueno, Vector<int>& albumPosicion);
    bool cargarFotos(Vector<Usuario>& usuarios, const Vector<int>& albumDueno,
                     const Vector<int>& albumPosicion);
    bool cargarMensajes(Vector<Usuario>& usuarios, Vector<Mensaje>& mensajes);

    // Guardado inmediato de cada cambio.
    void agregarAmistad(int a, int b);
    void guardarSolicitudes(const Vector<Usuario>& usuarios);
    void guardarFotos(const Vector<Usuario>& usuarios);
    void guardarMensajes(const Vector<Mensaje>& mensajes);

private:
    string carpeta;

    bool abrirLectura(ifstream& archivo, const char* nombre);
    bool abrirEscritura(ofstream& archivo, const char* nombre, bool anexar);
    static bool leerLinea(ifstream& archivo, string& registro);
    static int dividir(const string& registro, int maxCampos, Vector<string>& campos);
    bool error() const;
};

#endif
