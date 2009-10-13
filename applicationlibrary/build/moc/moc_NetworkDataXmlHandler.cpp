/****************************************************************************
** Meta object code from reading C++ file 'NetworkDataXmlHandler.h'
**
** Created
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/NetworkDataXmlHandler.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'NetworkDataXmlHandler.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_NetworkDataXmlHandler[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      38,   23,   22,   22, 0x05,
      77,   62,   22,   22, 0x05,
     101,   22,   22,   22, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_NetworkDataXmlHandler[] = {
    "NetworkDataXmlHandler\0\0neurSpikeCount\0"
    "spikeCountChanged(uint)\0neurSpikeTotal\0"
    "spikeTotalChanged(uint)\0statisticsChanged()\0"
};

const QMetaObject NetworkDataXmlHandler::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_NetworkDataXmlHandler,
      qt_meta_data_NetworkDataXmlHandler, 0 }
};

const QMetaObject *NetworkDataXmlHandler::metaObject() const
{
    return &staticMetaObject;
}

void *NetworkDataXmlHandler::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_NetworkDataXmlHandler))
        return static_cast<void*>(const_cast< NetworkDataXmlHandler*>(this));
    if (!strcmp(_clname, "QXmlDefaultHandler"))
        return static_cast< QXmlDefaultHandler*>(const_cast< NetworkDataXmlHandler*>(this));
    return QObject::qt_metacast(_clname);
}

int NetworkDataXmlHandler::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: spikeCountChanged((*reinterpret_cast< uint(*)>(_a[1]))); break;
        case 1: spikeTotalChanged((*reinterpret_cast< uint(*)>(_a[1]))); break;
        case 2: statisticsChanged(); break;
        }
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void NetworkDataXmlHandler::spikeCountChanged(unsigned int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void NetworkDataXmlHandler::spikeTotalChanged(unsigned int _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void NetworkDataXmlHandler::statisticsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}
QT_END_MOC_NAMESPACE
