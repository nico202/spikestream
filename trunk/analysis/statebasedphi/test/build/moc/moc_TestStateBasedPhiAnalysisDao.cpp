/****************************************************************************
** Meta object code from reading C++ file 'TestStateBasedPhiAnalysisDao.h'
**
** Created
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/TestStateBasedPhiAnalysisDao.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'TestStateBasedPhiAnalysisDao.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_TestStateBasedPhiAnalysisDao[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      30,   29,   29,   29, 0x08,
      47,   29,   29,   29, 0x08,
      69,   29,   29,   29, 0x08,
      91,   29,   29,   29, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_TestStateBasedPhiAnalysisDao[] = {
    "TestStateBasedPhiAnalysisDao\0\0"
    "testAddComplex()\0testDeleteTimeSteps()\0"
    "testGetComplexCount()\0"
    "testGetStateBasedPhiDataTableModel()\0"
};

const QMetaObject TestStateBasedPhiAnalysisDao::staticMetaObject = {
    { &TestDao::staticMetaObject, qt_meta_stringdata_TestStateBasedPhiAnalysisDao,
      qt_meta_data_TestStateBasedPhiAnalysisDao, 0 }
};

const QMetaObject *TestStateBasedPhiAnalysisDao::metaObject() const
{
    return &staticMetaObject;
}

void *TestStateBasedPhiAnalysisDao::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_TestStateBasedPhiAnalysisDao))
        return static_cast<void*>(const_cast< TestStateBasedPhiAnalysisDao*>(this));
    return TestDao::qt_metacast(_clname);
}

int TestStateBasedPhiAnalysisDao::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = TestDao::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: testAddComplex(); break;
        case 1: testDeleteTimeSteps(); break;
        case 2: testGetComplexCount(); break;
        case 3: testGetStateBasedPhiDataTableModel(); break;
        }
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
