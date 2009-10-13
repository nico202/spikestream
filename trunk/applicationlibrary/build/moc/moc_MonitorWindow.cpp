/****************************************************************************
** Meta object code from reading C++ file 'MonitorWindow.h'
**
** Created
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/MonitorWindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MonitorWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MonitorWindow[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      17,   15,   14,   14, 0x08,
      57,   14,   14,   14, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MonitorWindow[] = {
    "MonitorWindow\0\0p\0"
    "windowPlaceChanged(Q3DockWindow::Place)\0"
    "windowVisibilityChanged(bool)\0"
};

const QMetaObject MonitorWindow::staticMetaObject = {
    { &Q3DockWindow::staticMetaObject, qt_meta_stringdata_MonitorWindow,
      qt_meta_data_MonitorWindow, 0 }
};

const QMetaObject *MonitorWindow::metaObject() const
{
    return &staticMetaObject;
}

void *MonitorWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MonitorWindow))
        return static_cast<void*>(const_cast< MonitorWindow*>(this));
    return Q3DockWindow::qt_metacast(_clname);
}

int MonitorWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Q3DockWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: windowPlaceChanged((*reinterpret_cast< Q3DockWindow::Place(*)>(_a[1]))); break;
        case 1: windowVisibilityChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        }
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
