/****************************************************************************
** Meta object code from reading C++ file 'LayerPropertiesDialog.h'
**
** Created
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/LayerPropertiesDialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'LayerPropertiesDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_LayerPropertiesDialog[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      34,   23,   22,   22, 0x08,
      61,   22,   22,   22, 0x08,
     106,   89,   22,   22, 0x08,
     132,   22,   22,   22, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_LayerPropertiesDialog[] = {
    "LayerPropertiesDialog\0\0comboIndex\0"
    "componentComboChanged(int)\0"
    "inputLayerComboChanged(int)\0"
    "currentSelection\0neuronGrpTypeChanged(int)\0"
    "okButtonPressed()\0"
};

const QMetaObject LayerPropertiesDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_LayerPropertiesDialog,
      qt_meta_data_LayerPropertiesDialog, 0 }
};

const QMetaObject *LayerPropertiesDialog::metaObject() const
{
    return &staticMetaObject;
}

void *LayerPropertiesDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_LayerPropertiesDialog))
        return static_cast<void*>(const_cast< LayerPropertiesDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int LayerPropertiesDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: componentComboChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: inputLayerComboChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: neuronGrpTypeChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: okButtonPressed(); break;
        }
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
