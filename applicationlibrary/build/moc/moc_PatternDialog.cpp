/****************************************************************************
** Meta object code from reading C++ file 'PatternDialog.h'
**
** Created
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/PatternDialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'PatternDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_PatternDialog[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x08,
      28,   14,   14,   14, 0x08,
      54,   44,   14,   14, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_PatternDialog[] = {
    "PatternDialog\0\0addPattern()\0deletePattern()\0"
    "colNumber\0tableHeaderClicked(int)\0"
};

const QMetaObject PatternDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_PatternDialog,
      qt_meta_data_PatternDialog, 0 }
};

const QMetaObject *PatternDialog::metaObject() const
{
    return &staticMetaObject;
}

void *PatternDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PatternDialog))
        return static_cast<void*>(const_cast< PatternDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int PatternDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: addPattern(); break;
        case 1: deletePattern(); break;
        case 2: tableHeaderClicked((*reinterpret_cast< int(*)>(_a[1]))); break;
        }
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
