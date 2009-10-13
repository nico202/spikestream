/****************************************************************************
** Meta object code from reading C++ file 'EditSynapseParametersDialog.h'
**
** Created
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/EditSynapseParametersDialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'EditSynapseParametersDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_EditSynapseParametersDialog[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      29,   28,   28,   28, 0x08,
      51,   28,   28,   28, 0x08,
      79,   28,   28,   28, 0x08,
     107,   28,   28,   28, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_EditSynapseParametersDialog[] = {
    "EditSynapseParametersDialog\0\0"
    "cancelButtonPressed()\0loadDefaultsButtonPressed()\0"
    "makeDefaultsButtonPressed()\0"
    "okButtonPressed()\0"
};

const QMetaObject EditSynapseParametersDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_EditSynapseParametersDialog,
      qt_meta_data_EditSynapseParametersDialog, 0 }
};

const QMetaObject *EditSynapseParametersDialog::metaObject() const
{
    return &staticMetaObject;
}

void *EditSynapseParametersDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_EditSynapseParametersDialog))
        return static_cast<void*>(const_cast< EditSynapseParametersDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int EditSynapseParametersDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: cancelButtonPressed(); break;
        case 1: loadDefaultsButtonPressed(); break;
        case 2: makeDefaultsButtonPressed(); break;
        case 3: okButtonPressed(); break;
        }
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
