/****************************************************************************
** Meta object code from reading C++ file 'SpikeStreamMainWindow.h'
**
** Created
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/SpikeStreamMainWindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SpikeStreamMainWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_SpikeStreamMainWindow[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      23,   22,   22,   22, 0x05,

 // slots: signature, parameters, type, tag, flags
      32,   22,   22,   22, 0x0a,
      49,   22,   22,   22, 0x08,
      71,   57,   22,   22, 0x08,
      98,   22,   22,   22, 0x08,
     115,   22,   22,   22, 0x08,
     140,   22,   22,   22, 0x08,
     165,   22,   22,   22, 0x08,
     181,   22,   22,   22, 0x08,
     200,   22,   22,   22, 0x08,
     217,   22,   22,   22, 0x08,
     232,   22,   22,   22, 0x08,
     248,   22,   22,   22, 0x08,
     267,   22,   22,   22, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_SpikeStreamMainWindow[] = {
    "SpikeStreamMainWindow\0\0reload()\0"
    "reloadPatterns()\0about()\0acceleratorID\0"
    "acceleratorKeyPressed(int)\0clearDatabases()\0"
    "closeEvent(QCloseEvent*)\0"
    "importConnectionMatrix()\0loadDatabases()\0"
    "importNRMNetwork()\0managePatterns()\0"
    "manageProbes()\0reloadDevices()\0"
    "reloadEverything()\0saveDatabases()\0"
};

const QMetaObject SpikeStreamMainWindow::staticMetaObject = {
    { &Q3MainWindow::staticMetaObject, qt_meta_stringdata_SpikeStreamMainWindow,
      qt_meta_data_SpikeStreamMainWindow, 0 }
};

const QMetaObject *SpikeStreamMainWindow::metaObject() const
{
    return &staticMetaObject;
}

void *SpikeStreamMainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SpikeStreamMainWindow))
        return static_cast<void*>(const_cast< SpikeStreamMainWindow*>(this));
    return Q3MainWindow::qt_metacast(_clname);
}

int SpikeStreamMainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = Q3MainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: reload(); break;
        case 1: reloadPatterns(); break;
        case 2: about(); break;
        case 3: acceleratorKeyPressed((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: clearDatabases(); break;
        case 5: closeEvent((*reinterpret_cast< QCloseEvent*(*)>(_a[1]))); break;
        case 6: importConnectionMatrix(); break;
        case 7: loadDatabases(); break;
        case 8: importNRMNetwork(); break;
        case 9: managePatterns(); break;
        case 10: manageProbes(); break;
        case 11: reloadDevices(); break;
        case 12: reloadEverything(); break;
        case 13: saveDatabases(); break;
        }
        _id -= 14;
    }
    return _id;
}

// SIGNAL 0
void SpikeStreamMainWindow::reload()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
