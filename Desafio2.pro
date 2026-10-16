QT = core

CONFIG += c++17 cmdline

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp \
        src/album.cpp \
        src/fecha.cpp \
        src/foto.cpp \
        src/matrizamistad.cpp \
        src/medidor.cpp \
        src/mensaje.cpp \
        src/menu.cpp \
        src/persistencia.cpp \
        src/plataforma.cpp \
        src/texto.cpp \
        src/usuario.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    src/Vector.h \
    src/album.h \
    src/fecha.h \
    src/foto.h \
    src/matrizamistad.h \
    src/medidor.h \
    src/mensaje.h \
    src/menu.h \
    src/persistencia.h \
    src/plataforma.h \
    src/texto.h \
    src/usuario.h
