/****************************************************************************
** Meta object code from reading C++ file 'TestNRMDataSetImporter.h'
**
** Created
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/TestNRMDataSetImporter.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'TestNRMDataSetImporter.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_TestNRMDataSetImporter[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      24,   23,   23,   23, 0x08,
      34,   23,   23,   23, 0x08,
      41,   23,   23,   23, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_TestNRMDataSetImporter[] = {
    "TestNRMDataSetImporter\0\0cleanup()\0"
    "init()\0testLoadDataSet()\0"
};

const QMetaObject TestNRMDataSetImporter::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_TestNRMDataSetImporter,
      qt_meta_data_TestNRMDataSetImporter, 0 }
};

const QMetaObject *TestNRMDataSetImporter::metaObject() const
{
    return &staticMetaObject;
}

void *TestNRMDataSetImporter::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_TestNRMDataSetImporter))
        return static_cast<void*>(const_cast< TestNRMDataSetImporter*>(this));
    return QObject::qt_metacast(_clname);
}

int TestNRMDataSetImporter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: cleanup(); break;
        case 1: init(); break;
        case 2: testLoadDataSet(); break;
        }
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
