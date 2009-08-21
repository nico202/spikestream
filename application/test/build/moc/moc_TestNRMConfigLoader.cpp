/****************************************************************************
** Meta object code from reading C++ file 'TestNRMConfigLoader.h'
**
** Created
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/TestNRMConfigLoader.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'TestNRMConfigLoader.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_TestNRMConfigLoader[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      21,   20,   20,   20, 0x08,
      31,   20,   20,   20, 0x08,
      38,   20,   20,   20, 0x08,
      69,   20,   20,   20, 0x08,
      86,   20,   20,   20, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_TestNRMConfigLoader[] = {
    "TestNRMConfigLoader\0\0cleanup()\0init()\0"
    "testLoadConnectionParameters()\0"
    "testLoadInputs()\0testLoadNeuralLayers()\0"
};

const QMetaObject TestNRMConfigLoader::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_TestNRMConfigLoader,
      qt_meta_data_TestNRMConfigLoader, 0 }
};

const QMetaObject *TestNRMConfigLoader::metaObject() const
{
    return &staticMetaObject;
}

void *TestNRMConfigLoader::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_TestNRMConfigLoader))
        return static_cast<void*>(const_cast< TestNRMConfigLoader*>(this));
    return QObject::qt_metacast(_clname);
}

int TestNRMConfigLoader::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: cleanup(); break;
        case 1: init(); break;
        case 2: testLoadConnectionParameters(); break;
        case 3: testLoadInputs(); break;
        case 4: testLoadNeuralLayers(); break;
        }
        _id -= 5;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
