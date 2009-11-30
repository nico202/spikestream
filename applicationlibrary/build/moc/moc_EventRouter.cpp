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
      39,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      39,   26,   25,   25, 0x05,
      70,   25,   25,   25, 0x05,
      94,   25,   25,   25, 0x05,
     118,   25,   25,   25, 0x05,
     141,   25,   25,   25, 0x05,
     172,   25,   25,   25, 0x05,
     193,   25,   25,   25, 0x05,
     213,   25,   25,   25, 0x05,
     228,   25,   25,   25, 0x05,
     245,   25,   25,   25, 0x05,
     262,   25,   25,   25, 0x05,
     280,   25,   25,   25, 0x05,
     298,   25,   25,   25, 0x05,
     315,   25,   25,   25, 0x05,
     334,   25,   25,   25, 0x05,
     353,   25,   25,   25, 0x05,
     373,   25,   25,   25, 0x05,
     396,   25,   25,   25, 0x05,
     426,   25,   25,   25, 0x05,
     453,   25,   25,   25, 0x05,

 // slots: signature, parameters, type, tag, flags
     468,   25,   25,   25, 0x0a,
     486,   25,   25,   25, 0x0a,
     504,   25,   25,   25, 0x0a,
     533,   25,   25,   25, 0x0a,
     546,   25,   25,   25, 0x08,
     567,   25,   25,   25, 0x08,
     586,   25,   25,   25, 0x08,
     604,   25,   25,   25, 0x08,
     617,   25,   25,   25, 0x08,
     632,   25,   25,   25, 0x08,
     647,   25,   25,   25, 0x08,
     663,   25,   25,   25, 0x08,
     679,   25,   25,   25, 0x08,
     694,   25,   25,   25, 0x08,
     711,   25,   25,   25, 0x08,
     728,   25,   25,   25, 0x08,
     746,   25,   25,   25, 0x08,
     767,   25,   25,   25, 0x08,
     795,   25,   25,   25, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_spikestream__EventRouter[] = {
    "spikestream::EventRouter\0\0isNotRunning\0"
    "analysisNotRunningSignal(bool)\0"
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
    "analysisStarted()\0analysisStopped()\0"
    "archiveTimeStepChangedSlot()\0reloadSlot()\0"
    "archiveChangedSlot()\0moveBackwardSlot()\0"
    "moveForwardSlot()\0moveUpSlot()\0"
    "moveDownSlot()\0moveLeftSlot()\0"
    "moveRightSlot()\0resetViewSlot()\0"
    "rotateUpSlot()\0rotateDownSlot()\0"
    "rotateLeftSlot()\0rotateRightSlot()\0"
    "networkChangedSlot()\0networkDisplayChangedSlot()\0"
    "networkListChangedSlot()\0"
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
        case 0: analysisNotRunningSignal((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: analysisStartedSignal(); break;
        case 2: analysisStoppedSignal(); break;
        case 3: archiveChangedSignal(); break;
        case 4: archiveTimeStepChangedSignal(); break;
        case 5: moveBackwardSignal(); break;
        case 6: moveForwardSignal(); break;
        case 7: moveUpSignal(); break;
        case 8: moveDownSignal(); break;
        case 9: moveLeftSignal(); break;
        case 10: moveRightSignal(); break;
        case 11: resetViewSignal(); break;
        case 12: rotateUpSignal(); break;
        case 13: rotateDownSignal(); break;
        case 14: rotateLeftSignal(); break;
        case 15: rotateRightSignal(); break;
        case 16: networkChangedSignal(); break;
        case 17: networkDisplayChangedSignal(); break;
        case 18: networkListChangedSignal(); break;
        case 19: reloadSignal(); break;
        case 20: analysisStarted(); break;
        case 21: analysisStopped(); break;
        case 22: archiveTimeStepChangedSlot(); break;
        case 23: reloadSlot(); break;
        case 24: archiveChangedSlot(); break;
        case 25: moveBackwardSlot(); break;
        case 26: moveForwardSlot(); break;
        case 27: moveUpSlot(); break;
        case 28: moveDownSlot(); break;
        case 29: moveLeftSlot(); break;
        case 30: moveRightSlot(); break;
        case 31: resetViewSlot(); break;
        case 32: rotateUpSlot(); break;
        case 33: rotateDownSlot(); break;
        case 34: rotateLeftSlot(); break;
        case 35: rotateRightSlot(); break;
        case 36: networkChangedSlot(); break;
        case 37: networkDisplayChangedSlot(); break;
        case 38: networkListChangedSlot(); break;
        }
        _id -= 39;
    }
    return _id;
}

// SIGNAL 0
void spikestream::EventRouter::analysisNotRunningSignal(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void spikestream::EventRouter::analysisStartedSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void spikestream::EventRouter::analysisStoppedSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void spikestream::EventRouter::archiveChangedSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 3, 0);
}

// SIGNAL 4
void spikestream::EventRouter::archiveTimeStepChangedSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 4, 0);
}

// SIGNAL 5
void spikestream::EventRouter::moveBackwardSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 5, 0);
}

// SIGNAL 6
void spikestream::EventRouter::moveForwardSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 6, 0);
}

// SIGNAL 7
void spikestream::EventRouter::moveUpSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 7, 0);
}

// SIGNAL 8
void spikestream::EventRouter::moveDownSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 8, 0);
}

// SIGNAL 9
void spikestream::EventRouter::moveLeftSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 9, 0);
}

// SIGNAL 10
void spikestream::EventRouter::moveRightSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 10, 0);
}

// SIGNAL 11
void spikestream::EventRouter::resetViewSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 11, 0);
}

// SIGNAL 12
void spikestream::EventRouter::rotateUpSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 12, 0);
}

// SIGNAL 13
void spikestream::EventRouter::rotateDownSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 13, 0);
}

// SIGNAL 14
void spikestream::EventRouter::rotateLeftSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 14, 0);
}

// SIGNAL 15
void spikestream::EventRouter::rotateRightSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 15, 0);
}

// SIGNAL 16
void spikestream::EventRouter::networkChangedSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 16, 0);
}

// SIGNAL 17
void spikestream::EventRouter::networkDisplayChangedSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 17, 0);
}

// SIGNAL 18
void spikestream::EventRouter::networkListChangedSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 18, 0);
}

// SIGNAL 19
void spikestream::EventRouter::reloadSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 19, 0);
}
QT_END_MOC_NAMESPACE
