/****************************************************************************
** Meta object code from reading C++ file 'ArchiveManager.h'
**
** Created
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/ArchiveManager.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ArchiveManager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ArchiveManager[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      24,   16,   15,   15, 0x05,
      46,   15,   15,   15, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_ArchiveManager[] = {
    "ArchiveManager\0\0message\0archiveError(QString)\0"
    "stopped()\0"
};

const QMetaObject ArchiveManager::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_ArchiveManager,
      qt_meta_data_ArchiveManager, 0 }
};

const QMetaObject *ArchiveManager::metaObject() const
{
    return &staticMetaObject;
}

void *ArchiveManager::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ArchiveManager))
        return static_cast<void*>(const_cast< ArchiveManager*>(this));
    return QThread::qt_metacast(_clname);
}

int ArchiveManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: archiveError((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: stopped(); break;
        }
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void ArchiveManager::archiveError(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void ArchiveManager::stopped()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
QT_END_MOC_NAMESPACE
