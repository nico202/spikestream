/****************************************************************************
** Meta object code from reading C++ file 'NetworkDisplay.h'
**
** Created
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/NetworkDisplay.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'NetworkDisplay.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_spikestream__NetworkDisplay[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      29,   28,   28,   28, 0x05,

 // slots: signature, parameters, type, tag, flags
      53,   28,   28,   28, 0x0a,
      70,   28,   28,   28, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_spikestream__NetworkDisplay[] = {
    "spikestream::NetworkDisplay\0\0"
    "networkDisplayChanged()\0networkChanged()\0"
    "clearZoom()\0"
};

const QMetaObject spikestream::NetworkDisplay::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_spikestream__NetworkDisplay,
      qt_meta_data_spikestream__NetworkDisplay, 0 }
};

const QMetaObject *spikestream::NetworkDisplay::metaObject() const
{
    return &staticMetaObject;
}

void *spikestream::NetworkDisplay::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_spikestream__NetworkDisplay))
        return static_cast<void*>(const_cast< NetworkDisplay*>(this));
    return QObject::qt_metacast(_clname);
}

int spikestream::NetworkDisplay::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: networkDisplayChanged(); break;
        case 1: networkChanged(); break;
        case 2: clearZoom(); break;
        }
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void spikestream::NetworkDisplay::networkDisplayChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
