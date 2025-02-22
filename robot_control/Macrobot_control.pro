TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp
SOURCES += includes/FuzzyController.cpp
SOURCES += includes/LidarMarbleDetector.cpp
SOURCES += includes/ImageMarbleDetector.cpp
SOURCES += includes/ParticleFilter.cpp
SOURCES += includes/Particle.cpp


QT_CONFIG -= no-pkg-config
CONFIG += link_pkgconfig

mac {
  PKG_CONFIG = /usr/local/bin/pkg-config
}

LIBS += -L$$PWD/includes/fuzzylite-6.0/fuzzylite/release/bin -lfuzzylite-static 
INCLUDEPATH += "$$PWD/includes/fuzzylite-6.0/fuzzylite"

PKGCONFIG += gazebo
PKGCONFIG += opencv4
