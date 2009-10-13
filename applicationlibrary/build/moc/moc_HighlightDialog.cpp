/****************************************************************************
** Meta object code from reading C++ file 'HighlightDialog.h'
**
** Created
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/HighlightDialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'HighlightDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_HighlightDialog[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      17,   16,   16,   16, 0x08,
      45,   16,   16,   16, 0x08,
      66,   16,   16,   16, 0x08,
      87,   16,   16,   16, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_HighlightDialog[] = {
    "HighlightDialog\0\0addHighlightButtonPressed()\0"
    "clearButtonPressed()\0closeButtonPressed()\0"
    "colorButtonPressed()\0"
};

const QMetaObject HighlightDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_HighlightDialog,
      qt_meta_data_HighlightDialog, 0 }
};

const QMetaObject *HighlightDialog::metaObject() const
{
    return &staticMetaObject;
}

void *HighlightDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_HighlightDialog))
        return static_cast<void*>(const_cast< HighlightDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int HighlightDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: addHighlightButtonPressed(); break;
        case 1: clearButtonPressed(); break;
        case 2: closeButtonPressed(); break;
        case 3: colorButtonPressed(); break;
        }
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
