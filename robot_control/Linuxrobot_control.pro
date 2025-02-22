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
#LIBS += -L/home/mads/fuzzylite/fuzzylite/release/bin -lfuzzylite-static
LIBS += -L$$PWD/includes/fuzzylite-6.0/fuzzylite/release/bin -lfuzzylite-static 

#INCLUDEPATH += "/home/mads/fuzzylite/fuzzylite"
INCLUDEPATH += "$$PWD/includes/fuzzylite-6.0/fuzzylite"
PKGCONFIG += gazebo
PKGCONFIG += opencv
