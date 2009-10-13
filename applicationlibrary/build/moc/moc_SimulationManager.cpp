/****************************************************************************
** Meta object code from reading C++ file 'SimulationManager.h'
**
** Created
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/SimulationManager.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SimulationManager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_SimulationManager[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      35,   19,   18,   18, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_SimulationManager[] = {
    "SimulationManager\0\0newSimStartTime\0"
    "simulationStartTimeChanged(uint)\0"
};

const QMetaObject SimulationManager::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_SimulationManager,
      qt_meta_data_SimulationManager, 0 }
};

const QMetaObject *SimulationManager::metaObject() const
{
    return &staticMetaObject;
}

void *SimulationManager::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SimulationManager))
        return static_cast<void*>(const_cast< SimulationManager*>(this));
    return QThread::qt_metacast(_clname);
}

int SimulationManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: simulationStartTimeChanged((*reinterpret_cast< uint(*)>(_a[1]))); break;
        }
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void SimulationManager::simulationStartTimeChanged(unsigned int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
