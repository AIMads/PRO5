TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp


QT_CONFIG -= no-pkg-config
CONFIG += link_pkgconfig

PKGCONFIG += gazebo
PKGCONFIG += opencv
