/****************************************************************************
** Meta object code from reading C++ file 'ArchiveStatisticsDialog.h'
**
** Created
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/ArchiveStatisticsDialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ArchiveStatisticsDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ArchiveStatisticsDialog[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      25,   24,   24,   24, 0x08,
      47,   24,   24,   24, 0x08,
      75,   24,   24,   24, 0x08,
     102,   24,   24,   24, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_ArchiveStatisticsDialog[] = {
    "ArchiveStatisticsDialog\0\0cancelButtonPressed()\0"
    "neurGrpRadioButtonClicked()\0"
    "neurIDRadioButtonClicked()\0okButtonPressed()\0"
};

const QMetaObject ArchiveStatisticsDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_ArchiveStatisticsDialog,
      qt_meta_data_ArchiveStatisticsDialog, 0 }
};

const QMetaObject *ArchiveStatisticsDialog::metaObject() const
{
    return &staticMetaObject;
}

void *ArchiveStatisticsDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ArchiveStatisticsDialog))
        return static_cast<void*>(const_cast< ArchiveStatisticsDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int ArchiveStatisticsDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: cancelButtonPressed(); break;
        case 1: neurGrpRadioButtonClicked(); break;
        case 2: neurIDRadioButtonClicked(); break;
        case 3: okButtonPressed(); break;
        }
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
