/****************************************************************************
** Meta object code from reading C++ file 'EventRouter.h'
**
** Created
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/EventRouter.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'EventRouter.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_spikestream__EventRouter[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      34,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      26,   25,   25,   25, 0x05,
      49,   25,   25,   25, 0x05,
      80,   25,   25,   25, 0x05,
     101,   25,   25,   25, 0x05,
     121,   25,   25,   25, 0x05,
     136,   25,   25,   25, 0x05,
     153,   25,   25,   25, 0x05,
     170,   25,   25,   25, 0x05,
     188,   25,   25,   25, 0x05,
     206,   25,   25,   25, 0x05,
     223,   25,   25,   25, 0x05,
     242,   25,   25,   25, 0x05,
     261,   25,   25,   25, 0x05,
     281,   25,   25,   25, 0x05,
     304,   25,   25,   25, 0x05,
     334,   25,   25,   25, 0x05,
     361,   25,   25,   25, 0x05,

 // slots: signature, parameters, type, tag, flags
     376,   25,   25,   25, 0x0a,
     405,   25,   25,   25, 0x08,
     426,   25,   25,   25, 0x08,
     445,   25,   25,   25, 0x08,
     463,   25,   25,   25, 0x08,
     476,   25,   25,   25, 0x08,
     491,   25,   25,   25, 0x08,
     506,   25,   25,   25, 0x08,
     522,   25,   25,   25, 0x08,
     538,   25,   25,   25, 0x08,
     553,   25,   25,   25, 0x08,
     570,   25,   25,   25, 0x08,
     587,   25,   25,   25, 0x08,
     605,   25,   25,   25, 0x08,
     626,   25,   25,   25, 0x08,
     654,   25,   25,   25, 0x08,
     679,   25,   25,   25, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_spikestream__EventRouter[] = {
    "spikestream::EventRouter\0\0"
    "archiveChangedSignal()\0"
    "archiveTimeStepChangedSignal()\0"
    "moveBackwardSignal()\0moveForwardSignal()\0"
    "moveUpSignal()\0moveDownSignal()\0"
    "moveLeftSignal()\0moveRightSignal()\0"
    "resetViewSignal()\0rotateUpSignal()\0"
    "rotateDownSignal()\0rotateLeftSignal()\0"
    "rotateRightSignal()\0networkChangedSignal()\0"
    "networkDisplayChangedSignal()\0"
    "networkListChangedSignal()\0reloadSignal()\0"
    "archiveTimeStepChangedSlot()\0"
    "archiveChangedSlot()\0moveBackwardSlot()\0"
    "moveForwardSlot()\0moveUpSlot()\0"
    "moveDownSlot()\0moveLeftSlot()\0"
    "moveRightSlot()\0resetViewSlot()\0"
    "rotateUpSlot()\0rotateDownSlot()\0"
    "rotateLeftSlot()\0rotateRightSlot()\0"
    "networkChangedSlot()\0networkDisplayChangedSlot()\0"
    "networkListChangedSlot()\0reloadSlot()\0"
};

const QMetaObject spikestream::EventRouter::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_spikestream__EventRouter,
      qt_meta_data_spikestream__EventRouter, 0 }
};

const QMetaObject *spikestream::EventRouter::metaObject() const
{
    return &staticMetaObject;
}

void *spikestream::EventRouter::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_spikestream__EventRouter))
        return static_cast<void*>(const_cast< EventRouter*>(this));
    return QObject::qt_metacast(_clname);
}

int spikestream::EventRouter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: archiveChangedSignal(); break;
        case 1: archiveTimeStepChangedSignal(); break;
        case 2: moveBackwardSignal(); break;
        case 3: moveForwardSignal(); break;
        case 4: moveUpSignal(); break;
        case 5: moveDownSignal(); break;
        case 6: moveLeftSignal(); break;
        case 7: moveRightSignal(); break;
        case 8: resetViewSignal(); break;
        case 9: rotateUpSignal(); break;
        case 10: rotateDownSignal(); break;
        case 11: rotateLeftSignal(); break;
        case 12: rotateRightSignal(); break;
        case 13: networkChangedSignal(); break;
        case 14: networkDisplayChangedSignal(); break;
        case 15: networkListChangedSignal(); break;
        case 16: reloadSignal(); break;
        case 17: archiveTimeStepChangedSlot(); break;
        case 18: archiveChangedSlot(); break;
        case 19: moveBackwardSlot(); break;
        case 20: moveForwardSlot(); break;
        case 21: moveUpSlot(); break;
        case 22: moveDownSlot(); break;
        case 23: moveLeftSlot(); break;
        case 24: moveRightSlot(); break;
        case 25: resetViewSlot(); break;
        case 26: rotateUpSlot(); break;
        case 27: rotateDownSlot(); break;
        case 28: rotateLeftSlot(); break;
        case 29: rotateRightSlot(); break;
        case 30: networkChangedSlot(); break;
        case 31: networkDisplayChangedSlot(); break;
        case 32: networkListChangedSlot(); break;
        case 33: reloadSlot(); break;
        }
        _id -= 34;
    }
    return _id;
}

// SIGNAL 0
void spikestream::EventRouter::archiveChangedSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void spikestream::EventRouter::archiveTimeStepChangedSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void spikestream::EventRouter::moveBackwardSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void spikestream::EventRouter::moveForwardSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}

// SIGNAL 4
void spikestream::EventRouter::moveUpSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 4, 0);
}

// SIGNAL 5
void spikestream::EventRouter::moveDownSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 5, 0);
}

// SIGNAL 6
void spikestream::EventRouter::moveLeftSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 6, 0);
}

// SIGNAL 7
void spikestream::EventRouter::moveRightSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 7, 0);
}

// SIGNAL 8
void spikestream::EventRouter::resetViewSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 8, 0);
}

// SIGNAL 9
void spikestream::EventRouter::rotateUpSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 9, 0);
}

// SIGNAL 10
void spikestream::EventRouter::rotateDownSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 10, 0);
}

// SIGNAL 11
void spikestream::EventRouter::rotateLeftSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 11, 0);
}

// SIGNAL 12
void spikestream::EventRouter::rotateRightSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 12, 0);
}

// SIGNAL 13
void spikestream::EventRouter::networkChangedSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 13, 0);
}

// SIGNAL 14
void spikestream::EventRouter::networkDisplayChangedSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 14, 0);
}

// SIGNAL 15
void spikestream::EventRouter::networkListChangedSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 15, 0);
}

// SIGNAL 16
void spikestream::EventRouter::reloadSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 16, 0);
}
QT_END_MOC_NAMESPACE
