/****************************************************************************
** Meta object code from reading C++ file 'NRMImportDialog.h'
**
** Created
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/dialogs/NRMImportDialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'NRMImportDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_NRMImportDialog[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      17,   16,   16,   16, 0x05,

 // slots: signature, parameters, type, tag, flags
      38,   16,   16,   16, 0x08,
      51,   16,   16,   16, 0x08,
      60,   16,   16,   16, 0x08,
      76,   16,   16,   16, 0x08,
      93,   16,   16,   16, 0x08,
     111,   16,   16,   16, 0x08,
     138,  134,   16,   16, 0x08,
     160,   16,   16,   16, 0x08,
     172,   16,   16,   16, 0x08,
     184,   16,   16,   16, 0x08,
     202,   16,   16,   16, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_NRMImportDialog[] = {
    "NRMImportDialog\0\0networkListChanged()\0"
    "addNetwork()\0cancel()\0getConfigFile()\0"
    "getDatasetFile()\0getTrainingFile()\0"
    "loadNetworkFromFiles()\0msg\0"
    "showBusyPage(QString)\0showPage1()\0"
    "showPage2()\0showSuccessPage()\0"
    "threadFinished()\0"
};

const QMetaObject NRMImportDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_NRMImportDialog,
      qt_meta_data_NRMImportDialog, 0 }
};

const QMetaObject *NRMImportDialog::metaObject() const
{
    return &staticMetaObject;
}

void *NRMImportDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_NRMImportDialog))
        return static_cast<void*>(const_cast< NRMImportDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int NRMImportDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: networkListChanged(); break;
        case 1: addNetwork(); break;
        case 2: cancel(); break;
        case 3: getConfigFile(); break;
        case 4: getDatasetFile(); break;
        case 5: getTrainingFile(); break;
        case 6: loadNetworkFromFiles(); break;
        case 7: showBusyPage((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 8: showPage1(); break;
        case 9: showPage2(); break;
        case 10: showSuccessPage(); break;
        case 11: threadFinished(); break;
        }
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
void NRMImportDialog::networkListChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
