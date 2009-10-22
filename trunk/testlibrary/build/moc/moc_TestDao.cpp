/****************************************************************************
** Meta object code from reading C++ file 'TestDao.h'
**
** Created
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../include/TestDao.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'TestDao.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_TestDao[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
       9,    8,    8,    8, 0x09,
      19,    8,    8,    8, 0x09,
      37,    8,    8,    8, 0x09,
      44,    8,    8,    8, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_TestDao[] = {
    "TestDao\0\0cleanup()\0cleanupTestCase()\0"
    "init()\0initTestCase()\0"
};

const QMetaObject TestDao::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_TestDao,
      qt_meta_data_TestDao, 0 }
};

const QMetaObject *TestDao::metaObject() const
{
    return &staticMetaObject;
}

void *TestDao::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_TestDao))
        return static_cast<void*>(const_cast< TestDao*>(this));
    return QObject::qt_metacast(_clname);
}

int TestDao::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: cleanup(); break;
        case 1: cleanupTestCase(); break;
        case 2: init(); break;
        case 3: initTestCase(); break;
        }
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
