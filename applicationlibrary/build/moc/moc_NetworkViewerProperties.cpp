/****************************************************************************
** Meta object code from reading C++ file 'NetworkViewerProperties.h'
**
** Created
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/NetworkViewerProperties.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'NetworkViewerProperties.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_NetworkViewerProperties[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      25,   24,   24,   24, 0x08,
      48,   24,   24,   24, 0x08,
      70,   24,   24,   24, 0x08,
      95,   24,   24,   24, 0x08,
     124,   24,   24,   24, 0x08,
     149,   24,   24,   24, 0x08,
     173,   24,   24,   24, 0x08,
     194,   24,   24,   24, 0x08,
     224,   24,   24,   24, 0x08,
     255,   24,   24,   24, 0x08,
     280,   24,   24,   24, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_NetworkViewerProperties[] = {
    "NetworkViewerProperties\0\0"
    "cancelRenderProgress()\0connCheckBoxChanged()\0"
    "filterCombosChanged(int)\0"
    "firstNeuronComboChanged(int)\0"
    "highlightButtonPressed()\0"
    "renderDelayChanged(int)\0renderStateChanged()\0"
    "secondNeuronComboChanged(int)\0"
    "showConnectionDetailsChanged()\0"
    "showConnectionsChanged()\0sortRows(int)\0"
};

const QMetaObject NetworkViewerProperties::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_NetworkViewerProperties,
      qt_meta_data_NetworkViewerProperties, 0 }
};

const QMetaObject *NetworkViewerProperties::metaObject() const
{
    return &staticMetaObject;
}

void *NetworkViewerProperties::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_NetworkViewerProperties))
        return static_cast<void*>(const_cast< NetworkViewerProperties*>(this));
    return QWidget::qt_metacast(_clname);
}

int NetworkViewerProperties::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: cancelRenderProgress(); break;
        case 1: connCheckBoxChanged(); break;
        case 2: filterCombosChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: firstNeuronComboChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: highlightButtonPressed(); break;
        case 5: renderDelayChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: renderStateChanged(); break;
        case 7: secondNeuronComboChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: showConnectionDetailsChanged(); break;
        case 9: showConnectionsChanged(); break;
        case 10: sortRows((*reinterpret_cast< int(*)>(_a[1]))); break;
        }
        _id -= 11;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
