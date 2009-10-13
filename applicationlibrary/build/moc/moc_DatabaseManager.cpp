/****************************************************************************
** Meta object code from reading C++ file 'DatabaseManager.h'
**
** Created
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/DatabaseManager.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'DatabaseManager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_DatabaseManager[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      17,   16,   16,   16, 0x08,
      39,   16,   16,   16, 0x08,
      57,   16,   16,   16, 0x08,
      73,   16,   16,   16, 0x08,
      90,   16,   16,   16, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_DatabaseManager[] = {
    "DatabaseManager\0\0cancelButtonPressed()\0"
    "okButtonPressed()\0processExited()\0"
    "readFromStderr()\0readFromStdout()\0"
};

const QMetaObject DatabaseManager::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_DatabaseManager,
      qt_meta_data_DatabaseManager, 0 }
};

const QMetaObject *DatabaseManager::metaObject() const
{
    return &staticMetaObject;
}

void *DatabaseManager::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DatabaseManager))
        return static_cast<void*>(const_cast< DatabaseManager*>(this));
    return QDialog::qt_metacast(_clname);
}

int DatabaseManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: cancelButtonPressed(); break;
        case 1: okButtonPressed(); break;
        case 2: processExited(); break;
        case 3: readFromStderr(); break;
        case 4: readFromStdout(); break;
        }
        _id -= 5;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
