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
    src/database.cpp \
    src/souvenirdialog.cpp \
    src/trip.cpp \
    src/tripplanner.cpp \
    src/adminmanager.cpp \
    src/adminlogindialog.cpp \
    src/adminpanel.cpp

HEADERS += \
    src/mainwindow.h \
    src/database.h \
    src/souvenirdialog.h \
    src/trip.h \
    src/tripplanner.h \
    src/hashmap.h \
    src/adminmanager.h \
    src/adminlogindialog.h \
    src/adminpanel.h

FORMS += \
    src/mainwindow.ui \
    src/souvenirdialog.ui \
    src/tripplanner.ui \
    src/adminlogindialog.ui \
    src/adminpanel.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# Ensure the database file is copied to the build directory
DISTFILES += \
    README.md \
    src/style.css

# Create a directory for the database if it doesn't exist
QMAKE_POST_LINK += $$QMAKE_MKDIR $$shell_path($$OUT_PWD/data/)

# Add include paths for Qt and source files
INCLUDEPATH += $$PWD/src
INCLUDEPATH += $$[QT_INSTALL_HEADERS]
DEPENDPATH += $$[QT_INSTALL_HEADERS]

# Ensure proper handling of generated files
UI_DIR = $$OUT_PWD/ui
MOC_DIR = $$OUT_PWD/moc
OBJECTS_DIR = $$OUT_PWD/obj
RCC_DIR = $$OUT_PWD/rcc

# Force update of UI files
system(touch $$PWD/src/*.ui)

RESOURCES += \
    src/resource.qrc
