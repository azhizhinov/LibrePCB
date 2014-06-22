#-------------------------------------------------
#
# Project created by QtCreator 2013-02-05T16:47:16
#
#-------------------------------------------------

QT += core widgets opengl

TARGET = EDA4U
TEMPLATE = app

CONFIG += c++11

#DEFINES += USE_32BIT_LENGTH_UNITS          # see units.h

RESOURCES += \
    ressources.qrc

SOURCES += \
    src/main.cpp \
    src/common/units.cpp \
    src/common/cadscene.cpp \
    src/common/cadview.cpp

HEADERS += \
    src/common/units.h \
    src/common/cadscene.h \
    src/common/cadview.h

FORMS +=

