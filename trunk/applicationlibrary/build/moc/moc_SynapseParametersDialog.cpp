/****************************************************************************
** Meta object code from reading C++ file 'SynapseParametersDialog.h'
**
** Created
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/SynapseParametersDialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SynapseParametersDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_SynapseParametersDialog[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      25,   24,   24,   24, 0x08,
      46,   24,   24,   24, 0x08,
      68,   24,   24,   24, 0x08,
      92,   24,   24,   24, 0x08,
     125,  110,   24,   24, 0x08,
     171,  110,   24,   24, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_SynapseParametersDialog[] = {
    "SynapseParametersDialog\0\0applyButtonPressed()\0"
    "cancelButtonPressed()\0defaultsButtonPressed()\0"
    "okButtonPressed()\0row,col,typeID\0"
    "parameterTableClicked(int,int,unsigned short)\0"
    "parameterTableValueChanged(int,int,unsigned short)\0"
};

const QMetaObject SynapseParametersDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_SynapseParametersDialog,
      qt_meta_data_SynapseParametersDialog, 0 }
};

const QMetaObject *SynapseParametersDialog::metaObject() const
{
    return &staticMetaObject;
}

void *SynapseParametersDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SynapseParametersDialog))
        return static_cast<void*>(const_cast< SynapseParametersDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int SynapseParametersDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: applyButtonPressed(); break;
        case 1: cancelButtonPressed(); break;
        case 2: defaultsButtonPressed(); break;
        case 3: okButtonPressed(); break;
        case 4: parameterTableClicked((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< unsigned short(*)>(_a[3]))); break;
        case 5: parameterTableValueChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< unsigned short(*)>(_a[3]))); break;
        }
        _id -= 6;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
