/****************************************************************************
** Meta object code from reading C++ file 'NetworksWidget.h'
**
** Created
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/widgets/NetworksWidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'NetworksWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_spikestream__NetworksWidget[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      29,   28,   28,   28, 0x05,

 // slots: signature, parameters, type, tag, flags
      46,   28,   28,   28, 0x08,
      60,   28,   28,   28, 0x08,
      83,   28,   28,   28, 0x08,
      99,   28,   28,   28, 0x08,
     113,   28,   28,   28, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_spikestream__NetworksWidget[] = {
    "spikestream::NetworksWidget\0\0"
    "networkChanged()\0addNetworks()\0"
    "checkLoadingProgress()\0deleteNetwork()\0"
    "loadNetwork()\0loadNetworkList()\0"
};

const QMetaObject spikestream::NetworksWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_spikestream__NetworksWidget,
      qt_meta_data_spikestream__NetworksWidget, 0 }
};

const QMetaObject *spikestream::NetworksWidget::metaObject() const
{
    return &staticMetaObject;
}

void *spikestream::NetworksWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_spikestream__NetworksWidget))
        return static_cast<void*>(const_cast< NetworksWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int spikestream::NetworksWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: networkChanged(); break;
        case 1: addNetworks(); break;
        case 2: checkLoadingProgress(); break;
        case 3: deleteNetwork(); break;
        case 4: loadNetwork(); break;
        case 5: loadNetworkList(); break;
        }
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void spikestream::NetworksWidget::networkChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
