/****************************************************************************
** Meta object code from reading C++ file 'LoadArchiveDialog.h'
**
** Created
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/LoadArchiveDialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'LoadArchiveDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_LoadArchiveDialog[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      44,   19,   18,   18, 0x05,
      80,   18,   18,   18, 0x05,
     103,   18,   18,   18, 0x05,

 // slots: signature, parameters, type, tag, flags
     127,   18,   18,   18, 0x08,
     152,   18,   18,   18, 0x08,
     174,   18,   18,   18, 0x08,
     192,   18,   18,   18, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_LoadArchiveDialog[] = {
    "LoadArchiveDialog\0\0archiveStartTime,newName\0"
    "archiveNameChanged(QString,QString)\0"
    "deleteArchiveStarted()\0deleteArchiveFinished()\0"
    "deleteAllButtonPressed()\0deleteButtonPressed()\0"
    "okButtonPressed()\0renameButtonPressed()\0"
};

const QMetaObject LoadArchiveDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_LoadArchiveDialog,
      qt_meta_data_LoadArchiveDialog, 0 }
};

const QMetaObject *LoadArchiveDialog::metaObject() const
{
    return &staticMetaObject;
}

void *LoadArchiveDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_LoadArchiveDialog))
        return static_cast<void*>(const_cast< LoadArchiveDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int LoadArchiveDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: archiveNameChanged((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 1: deleteArchiveStarted(); break;
        case 2: deleteArchiveFinished(); break;
        case 3: deleteAllButtonPressed(); break;
        case 4: deleteButtonPressed(); break;
        case 5: okButtonPressed(); break;
        case 6: renameButtonPressed(); break;
        }
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void LoadArchiveDialog::archiveNameChanged(QString _t1, QString _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void LoadArchiveDialog::deleteArchiveStarted()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void LoadArchiveDialog::deleteArchiveFinished()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}
QT_END_MOC_NAMESPACE
