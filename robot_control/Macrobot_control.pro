TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp
SOURCES += includes/FuzzyController.cpp
SOURCES += includes/LidarMarbleDetector.cpp
SOURCES += includes/ImageMarbleDetector.cpp


QT_CONFIG -= no-pkg-config
CONFIG += link_pkgconfig

mac {
  PKG_CONFIG = /usr/local/bin/pkg-config
}

PKGCONFIG += gazebo
PKGCONFIG += opencv4
