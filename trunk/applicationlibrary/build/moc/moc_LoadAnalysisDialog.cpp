/****************************************************************************
** Meta object code from reading C++ file 'LoadAnalysisDialog.h'
**
** Created
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/dialogs/LoadAnalysisDialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'LoadAnalysisDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_spikestream__LoadAnalysisDialog[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      33,   32,   32,   32, 0x08,
      68,   51,   32,   32, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_spikestream__LoadAnalysisDialog[] = {
    "spikestream::LoadAnalysisDialog\0\0"
    "okButtonPressed()\0current,previous\0"
    "selectionChanged(QModelIndex,QModelIndex)\0"
};

const QMetaObject spikestream::LoadAnalysisDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_spikestream__LoadAnalysisDialog,
      qt_meta_data_spikestream__LoadAnalysisDialog, 0 }
};

const QMetaObject *spikestream::LoadAnalysisDialog::metaObject() const
{
    return &staticMetaObject;
}

void *spikestream::LoadAnalysisDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_spikestream__LoadAnalysisDialog))
        return static_cast<void*>(const_cast< LoadAnalysisDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int spikestream::LoadAnalysisDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: okButtonPressed(); break;
        case 1: selectionChanged((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< const QModelIndex(*)>(_a[2]))); break;
        }
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
