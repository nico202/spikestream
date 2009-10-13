/****************************************************************************
** Meta object code from reading C++ file 'ParameterTable.h'
**
** Created
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/ParameterTable.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ParameterTable.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ParameterTable[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      31,   16,   15,   15, 0x05,
      77,   16,   15,   15, 0x05,

 // slots: signature, parameters, type, tag, flags
     152,  128,   15,   15, 0x08,
     193,  185,   15,   15, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_ParameterTable[] = {
    "ParameterTable\0\0row,col,typeID\0"
    "parameterTableClicked(int,int,unsigned short)\0"
    "parameterTableValueChanged(int,int,unsigned short)\0"
    "row,col,button,mousePos\0"
    "tableClicked(int,int,int,QPoint)\0"
    "row,col\0tableValueChanged(int,int)\0"
};

const QMetaObject ParameterTable::staticMetaObject = {
    { &Q3Table::staticMetaObject, qt_meta_stringdata_ParameterTable,
      qt_meta_data_ParameterTable, 0 }
};

const QMetaObject *ParameterTable::metaObject() const
{
    return &staticMetaObject;
}

void *ParameterTable::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ParameterTable))
        return static_cast<void*>(const_cast< ParameterTable*>(this));
    return Q3Table::qt_metacast(_clname);
}

int ParameterTable::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Q3Table::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: parameterTableClicked((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< unsigned short(*)>(_a[3]))); break;
        case 1: parameterTableValueChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< unsigned short(*)>(_a[3]))); break;
        case 2: tableClicked((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< const QPoint(*)>(_a[4]))); break;
        case 3: tableValueChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        }
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void ParameterTable::parameterTableClicked(int _t1, int _t2, unsigned short _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ParameterTable::parameterTableValueChanged(int _t1, int _t2, unsigned short _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
