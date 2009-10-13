/****************************************************************************
** Meta object code from reading C++ file 'GlobalParametersDialog.h'
**
** Created
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/GlobalParametersDialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'GlobalParametersDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GlobalParametersDialog[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      24,   23,   23,   23, 0x08,
      45,   23,   23,   23, 0x08,
      67,   23,   23,   23, 0x08,
      95,   23,   23,   23, 0x08,
     123,   23,   23,   23, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_GlobalParametersDialog[] = {
    "GlobalParametersDialog\0\0applyButtonPressed()\0"
    "cancelButtonPressed()\0loadDefaultsButtonPressed()\0"
    "makeDefaultsButtonPressed()\0"
    "okButtonPressed()\0"
};

const QMetaObject GlobalParametersDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_GlobalParametersDialog,
      qt_meta_data_GlobalParametersDialog, 0 }
};

const QMetaObject *GlobalParametersDialog::metaObject() const
{
    return &staticMetaObject;
}

void *GlobalParametersDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GlobalParametersDialog))
        return static_cast<void*>(const_cast< GlobalParametersDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int GlobalParametersDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: applyButtonPressed(); break;
        case 1: cancelButtonPressed(); break;
        case 2: loadDefaultsButtonPressed(); break;
        case 3: makeDefaultsButtonPressed(); break;
        case 4: okButtonPressed(); break;
        }
        _id -= 5;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
