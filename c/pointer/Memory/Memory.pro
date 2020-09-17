TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp

include(deployment.pri)
qtcAddDeployment()

HEADERS += \
    bitcopy.h \
    common.h \
    valuecopy.h \
    referencecount.h \
    referencesimple.h

