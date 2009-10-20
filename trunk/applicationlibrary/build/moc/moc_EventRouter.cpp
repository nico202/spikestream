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
      38,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      26,   25,   25,   25, 0x05,
      50,   25,   25,   25, 0x05,
      74,   25,   25,   25, 0x05,
      97,   25,   25,   25, 0x05,
     128,   25,   25,   25, 0x05,
     149,   25,   25,   25, 0x05,
     169,   25,   25,   25, 0x05,
     184,   25,   25,   25, 0x05,
     201,   25,   25,   25, 0x05,
     218,   25,   25,   25, 0x05,
     236,   25,   25,   25, 0x05,
     254,   25,   25,   25, 0x05,
     271,   25,   25,   25, 0x05,
     290,   25,   25,   25, 0x05,
     309,   25,   25,   25, 0x05,
     329,   25,   25,   25, 0x05,
     352,   25,   25,   25, 0x05,
     382,   25,   25,   25, 0x05,
     409,   25,   25,   25, 0x05,

 // slots: signature, parameters, type, tag, flags
     424,   25,   25,   25, 0x0a,
     442,   25,   25,   25, 0x0a,
     471,   25,   25,   25, 0x08,
     492,   25,   25,   25, 0x08,
     510,   25,   25,   25, 0x08,
     529,   25,   25,   25, 0x08,
     547,   25,   25,   25, 0x08,
     560,   25,   25,   25, 0x08,
     575,   25,   25,   25, 0x08,
     590,   25,   25,   25, 0x08,
     606,   25,   25,   25, 0x08,
     622,   25,   25,   25, 0x08,
     637,   25,   25,   25, 0x08,
     654,   25,   25,   25, 0x08,
     671,   25,   25,   25, 0x08,
     689,   25,   25,   25, 0x08,
     710,   25,   25,   25, 0x08,
     738,   25,   25,   25, 0x08,
     763,   25,   25,   25, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_spikestream__EventRouter[] = {
    "spikestream::EventRouter\0\0"
    "analysisStartedSignal()\0analysisStoppedSignal()\0"
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
    "analysisStarted()\0archiveTimeStepChangedSlot()\0"
    "archiveChangedSlot()\0analysisStopped()\0"
    "moveBackwardSlot()\0moveForwardSlot()\0"
    "moveUpSlot()\0moveDownSlot()\0moveLeftSlot()\0"
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
        case 0: analysisStartedSignal(); break;
        case 1: analysisStoppedSignal(); break;
        case 2: archiveChangedSignal(); break;
        case 3: archiveTimeStepChangedSignal(); break;
        case 4: moveBackwardSignal(); break;
        case 5: moveForwardSignal(); break;
        case 6: moveUpSignal(); break;
        case 7: moveDownSignal(); break;
        case 8: moveLeftSignal(); break;
        case 9: moveRightSignal(); break;
        case 10: resetViewSignal(); break;
        case 11: rotateUpSignal(); break;
        case 12: rotateDownSignal(); break;
        case 13: rotateLeftSignal(); break;
        case 14: rotateRightSignal(); break;
        case 15: networkChangedSignal(); break;
        case 16: networkDisplayChangedSignal(); break;
        case 17: networkListChangedSignal(); break;
        case 18: reloadSignal(); break;
        case 19: analysisStarted(); break;
        case 20: archiveTimeStepChangedSlot(); break;
        case 21: archiveChangedSlot(); break;
        case 22: analysisStopped(); break;
        case 23: moveBackwardSlot(); break;
        case 24: moveForwardSlot(); break;
        case 25: moveUpSlot(); break;
        case 26: moveDownSlot(); break;
        case 27: moveLeftSlot(); break;
        case 28: moveRightSlot(); break;
        case 29: resetViewSlot(); break;
        case 30: rotateUpSlot(); break;
        case 31: rotateDownSlot(); break;
        case 32: rotateLeftSlot(); break;
        case 33: rotateRightSlot(); break;
        case 34: networkChangedSlot(); break;
        case 35: networkDisplayChangedSlot(); break;
        case 36: networkListChangedSlot(); break;
        case 37: reloadSlot(); break;
        }
        _id -= 38;
    }
    return _id;
}

// SIGNAL 0
void spikestream::EventRouter::analysisStartedSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void spikestream::EventRouter::analysisStoppedSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void spikestream::EventRouter::archiveChangedSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void spikestream::EventRouter::archiveTimeStepChangedSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}

// SIGNAL 4
void spikestream::EventRouter::moveBackwardSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 4, 0);
}

// SIGNAL 5
void spikestream::EventRouter::moveForwardSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 5, 0);
}

// SIGNAL 6
void spikestream::EventRouter::moveUpSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 6, 0);
}

// SIGNAL 7
void spikestream::EventRouter::moveDownSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 7, 0);
}

// SIGNAL 8
void spikestream::EventRouter::moveLeftSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 8, 0);
}

// SIGNAL 9
void spikestream::EventRouter::moveRightSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 9, 0);
}

// SIGNAL 10
void spikestream::EventRouter::resetViewSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 10, 0);
}

// SIGNAL 11
void spikestream::EventRouter::rotateUpSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 11, 0);
}

// SIGNAL 12
void spikestream::EventRouter::rotateDownSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 12, 0);
}

// SIGNAL 13
void spikestream::EventRouter::rotateLeftSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 13, 0);
}

// SIGNAL 14
void spikestream::EventRouter::rotateRightSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 14, 0);
}

// SIGNAL 15
void spikestream::EventRouter::networkChangedSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 15, 0);
}

// SIGNAL 16
void spikestream::EventRouter::networkDisplayChangedSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 16, 0);
}

// SIGNAL 17
void spikestream::EventRouter::networkListChangedSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 17, 0);
}

// SIGNAL 18
void spikestream::EventRouter::reloadSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 18, 0);
}
QT_END_MOC_NAMESPACE
