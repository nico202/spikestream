/****************************************************************************
** Meta object code from reading C++ file 'NoiseParametersDialog.h'
**
** Created
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/NoiseParametersDialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'NoiseParametersDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_NoiseParametersDialog[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      23,   22,   22,   22, 0x08,
      44,   22,   22,   22, 0x08,
      66,   22,   22,   22, 0x08,
      90,   22,   22,   22, 0x08,
     116,  108,   22,   22, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_NoiseParametersDialog[] = {
    "NoiseParametersDialog\0\0applyButtonPressed()\0"
    "cancelButtonPressed()\0defaultsButtonPressed()\0"
    "okButtonPressed()\0row,col\0"
    "paramValueChanged(int,int)\0"
};

const QMetaObject NoiseParametersDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_NoiseParametersDialog,
      qt_meta_data_NoiseParametersDialog, 0 }
};

const QMetaObject *NoiseParametersDialog::metaObject() const
{
    return &staticMetaObject;
}

void *NoiseParametersDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_NoiseParametersDialog))
        return static_cast<void*>(const_cast< NoiseParametersDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int NoiseParametersDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
        case 4: paramValueChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        }
        _id -= 5;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
