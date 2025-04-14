QT       += core gui sql widgets

TARGET = Baseball_Program
TEMPLATE = app

CONFIG += c++17

# Platform-specific configurations
win32 {
    LIBS += -L$$[QT_INSTALL_LIBS]
}

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/database.cpp

HEADERS += \
    src/mainwindow.h \
    src/database.h

FORMS += \
    src/mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# Ensure the database file is copied to the build directory
DISTFILES += \
    README.md

# Create a directory for the database if it doesn't exist
QMAKE_POST_LINK += $$QMAKE_MKDIR $$shell_path($$OUT_PWD/data/)

# Add include paths for Qt
INCLUDEPATH += $$[QT_INSTALL_HEADERS]
DEPENDPATH += $$[QT_INSTALL_HEADERS]
