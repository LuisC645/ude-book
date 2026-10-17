# Generador de datos de prueba para Ude@Book (programa aparte, FUERA de la solucion).
# Version en Python de generador.cpp: escribe los 6 archivos de data/ con los
# formatos que documenta Persistencia.h. Semilla fija: siempre produce los mismos datos.
#
# Ejecutar desde la raiz del proyecto:
#   python tools/generador.py data/
#
# Usuarios de prueba (contrasena de TODOS: 1234):
#   ana      -> 45 amigos (pesos 2/5/7), >= 12 sugerencias con un maximo unico,
#               4 solicitudes recibidas, mensajes en ~10 semanas, 6 no leidos.
#   bruno    -> amigo de ana: albumes publico (6 fotos), amigos (7 fotos),
#               privado (3 fotos) y uno vacio.
#   cero, uno, tres, cuatro, cinco, seis -> 0, 1, 3, 4, 5 y 6 amigos.
#   sinalbum -> usuario sin albumes (amigo de ana).
#
# Internamente se trabaja con un "id de generacion"; al escribir, cada id se
# traduce a su posicion en usuarios.txt, que va ordenado alfabeticamente.
import os
import random
import sys
from datetime import datetime, timedelta

N = 1000                 # usuarios (el enunciado exige minimo 1000)
ANA = 0
SINALBUM = 2
BRUNO = 31
SEMILLA = 20261016
TOTAL_SOLICITUDES = 50
TOTAL_MENSAJES = 300     # sin contar los 6 no leidos de ana

gen = random.Random(SEMILLA)


def azar(a, b):
    return gen.randint(a, b)


BASES = [
    "laura", "carlos", "maria", "jorge", "sofia", "andres", "valentina", "felipe",
    "camila", "santiago", "daniela", "mateo", "isabella", "sebastian", "paula",
    "nicolas", "juliana", "alejandro", "mariana", "david", "natalia", "esteban",
    "catalina", "miguel", "luisa", "tomas", "sara", "julian", "manuela", "simon",
]
CIUDADES = ["Medellin", "Bogota", "Cali", "Barranquilla", "Lima", "Quito", "Madrid", "Santiago"]
PAISES = ["Colombia", "Colombia", "Colombia", "Colombia", "Peru", "Ecuador", "Espana", "Chile"]


def fecha8(a, m, d):
    return f"{a:04d}{m:02d}{d:02d}"


def main():
    carpeta = sys.argv[1] if len(sys.argv) > 1 else "data/"
    os.makedirs(carpeta, exist_ok=True)

    def ruta(nombre):
        return os.path.join(carpeta, nombre)

    def abrir(nombre):
        return open(nombre, "w", encoding="utf-8", newline="\n")

    # ----- Nombres -----
    nombres = [f"{BASES[i % 30]}_{i}" for i in range(N)]
    especiales = {0: "ana", 2: "sinalbum", 31: "bruno", 70: "cero", 71: "uno",
                  72: "tres", 73: "cuatro", 74: "cinco", 75: "seis"}
    for i, nombre in especiales.items():
        nombres[i] = nombre

    # Orden alfabetico por bytes (igual que operator< de string en el programa).
    # orden[k] = id en la posicion k; pos[id] = k.
    orden = sorted(range(N), key=lambda i: nombres[i].encode("utf-8"))
    pos = [0] * N
    for k, i in enumerate(orden):
        pos[i] = k

    # ----- Amistades -----
    adj = [set() for _ in range(N)]

    def amigos(a, b):
        return b in adj[a]

    def unir(a, b):
        if a != b:
            adj[a].add(b)
            adj[b].add(a)

    # ana: amigos 1..45 en tres grupos
    for f in range(1, 46):
        unir(ANA, f)
    # Grupo A (1..15): anillo -> 2 amigos en comun con ana  (peso 2)
    for i in range(15):
        unir(1 + i, 1 + (i + 1) % 15)
    # Grupo C (31..45): todos entre si + todo B -> 14 + 15 = 29 comunes (peso 7)
    for a in range(31, 46):
        for b in range(a + 1, 46):
            unir(a, b)
    # Grupo B (16..30): conectado a todo C -> 15 comunes (peso 5)
    for b in range(16, 31):
        for c in range(31, 46):
            unir(b, c)

    # Candidatos a sugerencia 46..65: el 46 tiene el maximo unico (12 comunes).
    metas = [12, 9, 8, 8, 7, 7, 6, 5, 5, 4, 3, 3, 2, 2, 1, 1, 1, 1, 1, 1]
    for j, meta in enumerate(metas):
        for k in range(meta):
            unir(46 + j, 1 + (j * 3 + k * 2) % 45)

    # Usuarios con 1, 3, 4, 5 y 6 amigos (sus amigos salen del rango 76..99).
    unir(71, 76)
    for k in range(3):
        unir(72, 77 + k)
    for k in range(4):
        unir(73, 80 + k)
    for k in range(5):
        unir(74, 84 + k)
    for k in range(6):
        unir(75, 89 + k)
    for k in range(80, 83):
        unir(k, k + 1)
    for k in range(89, 94):
        unir(k, k + 1)

    # Resto (76..N-1): ~4 amistades aleatorias por usuario.
    for i in range(76, N):
        for _ in range(4):
            unir(i, azar(100, N - 1))
    for i in range(66, 70):
        unir(i, azar(100, N - 1))

    # ----- usuarios.txt (en orden alfabetico) -----
    with abrir(ruta("usuarios.txt")) as fu:
        for i in orden:
            ciudad = azar(0, 7)
            nacimiento = fecha8(azar(1960, 2008), azar(1, 12), azar(1, 28))
            ingreso = fecha8(azar(2015, 2025), azar(1, 12), azar(1, 28))
            fu.write(f"{nombres[i]}|{nacimiento}|{nombres[i]}@udea.edu.co|1234|"
                     f"{CIUDADES[ciudad]}|{PAISES[ciudad]}|{ingreso}\n")

    # ----- amistades.txt -----
    total_amistades = 0
    with abrir(ruta("amistades.txt")) as fa:
        for a in range(N):
            for b in sorted(adj[a]):
                if b > a:
                    fa.write(f"{pos[a]}|{pos[b]}\n")
                    total_amistades += 1

    # ----- solicitudes.txt (solo pendientes) -----
    # Recibidas por ana (47 es candidato: queda excluido de sus sugerencias)
    solicitudes = [(o, ANA) for o in (47, 66, 67, 68)]
    # ana ya le envio una a 69
    solicitudes.append((ANA, 69))
    while len(solicitudes) < TOTAL_SOLICITUDES:
        o, d = azar(100, N - 1), azar(100, N - 1)
        if o == d or amigos(o, d):
            continue
        if (o, d) in solicitudes or (d, o) in solicitudes:
            continue
        solicitudes.append((o, d))
    with abrir(ruta("solicitudes.txt")) as fs:
        for o, d in solicitudes:
            fs.write(f"{pos[o]}|{pos[d]}\n")

    # ----- Albumes (se escriben agrupados por dueno, en orden de usuarios.txt) -----
    # Cada album: (dueno, nombre, privacidad, cantidad de fotos)
    albumes = []
    nombres_album = ["Viajes", "Amigos", "Mascotas", "Universidad", "Paisajes", "Privado"]
    for i in range(N):
        if i == SINALBUM or 70 <= i <= 75:
            continue
        if i == BRUNO:
            albumes += [(i, "Vacaciones", 0, 6), (i, "Familia", 1, 7),
                        (i, "Secreto", 2, 3), (i, "Vacio", 1, 0)]
            continue
        if i == ANA:
            albumes.append((i, "MisFotos", 0, 3))
            continue
        tiene = i <= 45 or azar(0, 9) < 3
        if not tiene:
            continue
        for _ in range(azar(1, 2)):
            t = azar(0, 5)
            privacidad = 2 if t == 5 else azar(0, 1)
            albumes.append((i, nombres_album[t], privacidad, azar(1, 5)))
    # sorted es estable: los albumes de un mismo dueno conservan su orden
    albumes.sort(key=lambda alb: pos[alb[0]])
    with abrir(ruta("albumes.txt")) as fal:
        for dueno, nombre, privacidad, _ in albumes:
            fal.write(f"{pos[dueno]}|{nombre}|{privacidad}\n")

    # ----- fotos.txt -----
    nombres_foto = ["playa", "montana", "cumpleanos", "atardecer", "ciudad", "comida", "grupo", "selfie"]
    nf = 0
    with abrir(ruta("fotos.txt")) as ff:
        for k, (_, _, _, fotos) in enumerate(albumes):
            for j in range(fotos):
                fecha = fecha8(azar(2018, 2026), azar(1, 9), azar(1, 28))
                ff.write(f"{k}|{nombres_foto[(k + j) % 8]}_{j + 1:02d}|{fecha}|"
                         f"{azar(150000, 4800000)}|{azar(0, 60)}\n")
                nf += 1

    # ----- mensajes.txt (ordenados por fecha) -----
    textos = [
        "Hola, como vas?",
        "Nos vemos manana en la universidad.",
        "¿Ya hiciste el desafío de Informática II?",
        "Te mando las fotos del paseo.",
        "Feliz cumpleaños!!",
        "Que buena pelicula la de ayer",
        "Me prestas los apuntes de calculo?",
        "Llego en 10 minutos",
        "Gracias por todo :)",
        "Vamos a cine el viernes?",
        "El parcial quedo dificil",
        "Saludos desde Medellín",
    ]
    # texto de exactamente 512 bytes
    inicio = "Mensaje de prueba de 512 bytes exactos: "
    texto512 = inicio + "".join(chr(ord("a") + p % 26) for p in range(len(inicio), 512))

    # Empiezan el 13/07/2026 y terminan unas 10 semanas despues: todas las
    # fechas quedan en el pasado, asi los mensajes nuevos (hora del sistema)
    # siempre van despues de los existentes.
    reloj = datetime(2026, 7, 13, 8, 0)
    # Al final, 6 no leidos para ana de 6 amigos distintos
    emisores_nuevos = [1, 16, 31, 5, 20, 40]
    nm = 0
    with abrir(ruta("mensajes.txt")) as fm:
        for k in range(TOTAL_MENSAJES + 6):
            normal = k < TOTAL_MENSAJES
            reloj += timedelta(minutes=azar(60, 600) if normal else azar(30, 300))

            if not normal:
                n = k - TOTAL_MENSAJES
                e, r, leido = emisores_nuevos[n], ANA, 0
                t = texto512 if n == 3 else textos[n]
            else:
                tipo = azar(0, 9)
                t = textos[azar(0, 11)]
                leido = 1
                if tipo < 4:
                    e, r = azar(1, 45), ANA
                elif tipo == 4:
                    e, r = ANA, azar(1, 45)
                else:
                    r = None
                    while r is None:   # un usuario del resto y uno de sus amigos
                        e = azar(100, N - 1)
                        r = next((q for q in range(100, N) if amigos(e, q)), None)
                    leido = 0 if azar(0, 3) == 0 else 1
            fm.write(f"{pos[e]}|{pos[r]}|{reloj:%Y%m%d%H%M}|{leido}|{t}\n")
            nm += 1

    print(f"Generados en {carpeta}: {N} usuarios, {total_amistades} amistades, "
          f"{len(solicitudes)} solicitudes, {len(albumes)} albumes, {nf} fotos, {nm} mensajes.")
    print(f"Mensajes desde 13/07/2026 hasta {reloj.day}/{reloj.month}/{reloj.year}.")


if __name__ == "__main__":
    main()
