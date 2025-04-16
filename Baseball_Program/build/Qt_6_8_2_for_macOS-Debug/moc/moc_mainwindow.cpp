/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.8.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/mainwindow.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.8.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN10MainWindowE_t {};
} // unnamed namespace


#ifdef QT_MOC_HAS_STRINGDATA
static constexpr auto qt_meta_stringdata_ZN10MainWindowE = QtMocHelpers::stringData(
    "MainWindow",
    "displayTeamInfo",
    "",
    "displayAllTeamsByTeamName",
    "displayAllTeamsByStadiumName",
    "displayAmericanLeagueTeams",
    "displayNationalLeagueTeams",
    "displayTeamsByTypology",
    "displayOpenRoofTeams",
    "displayTeamsByDateOpened",
    "displayTeamsByCapacity",
    "displayGreatestCenterField",
    "displaySmallestCenterField",
    "onImportCSV",
    "manageSouvenirs"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA

Q_CONSTINIT static const uint qt_meta_data_ZN10MainWindowE[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   92,    2, 0x08,    1 /* Private */,
       3,    0,   93,    2, 0x08,    2 /* Private */,
       4,    0,   94,    2, 0x08,    3 /* Private */,
       5,    0,   95,    2, 0x08,    4 /* Private */,
       6,    0,   96,    2, 0x08,    5 /* Private */,
       7,    0,   97,    2, 0x08,    6 /* Private */,
       8,    0,   98,    2, 0x08,    7 /* Private */,
       9,    0,   99,    2, 0x08,    8 /* Private */,
      10,    0,  100,    2, 0x08,    9 /* Private */,
      11,    0,  101,    2, 0x08,   10 /* Private */,
      12,    0,  102,    2, 0x08,   11 /* Private */,
      13,    0,  103,    2, 0x08,   12 /* Private */,
      14,    0,  104,    2, 0x08,   13 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_ZN10MainWindowE.offsetsAndSizes,
    qt_meta_data_ZN10MainWindowE,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_tag_ZN10MainWindowE_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<MainWindow, std::true_type>,
        // method 'displayTeamInfo'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'displayAllTeamsByTeamName'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'displayAllTeamsByStadiumName'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'displayAmericanLeagueTeams'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'displayNationalLeagueTeams'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'displayTeamsByTypology'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'displayOpenRoofTeams'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'displayTeamsByDateOpened'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'displayTeamsByCapacity'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'displayGreatestCenterField'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'displaySmallestCenterField'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onImportCSV'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'manageSouvenirs'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<MainWindow *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->displayTeamInfo(); break;
        case 1: _t->displayAllTeamsByTeamName(); break;
        case 2: _t->displayAllTeamsByStadiumName(); break;
        case 3: _t->displayAmericanLeagueTeams(); break;
        case 4: _t->displayNationalLeagueTeams(); break;
        case 5: _t->displayTeamsByTypology(); break;
        case 6: _t->displayOpenRoofTeams(); break;
        case 7: _t->displayTeamsByDateOpened(); break;
        case 8: _t->displayTeamsByCapacity(); break;
        case 9: _t->displayGreatestCenterField(); break;
        case 10: _t->displaySmallestCenterField(); break;
        case 11: _t->onImportCSV(); break;
        case 12: _t->manageSouvenirs(); break;
        default: ;
        }
    }
    (void)_a;
}

const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ZN10MainWindowE.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 13;
    }
    return _id;
}
QT_WARNING_POP
