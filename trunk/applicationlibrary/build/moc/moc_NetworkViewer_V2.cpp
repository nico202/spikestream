/****************************************************************************
** Meta object code from reading C++ file 'NetworkViewer_V2.h'
**
** Created
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/NetworkViewer_V2.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'NetworkViewer_V2.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_spikestream__NetworkViewer_V2[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      15,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      31,   30,   30,   30, 0x0a,
      41,   30,   30,   30, 0x0a,
      49,   30,   30,   30, 0x08,
      64,   30,   30,   30, 0x08,
      78,   30,   30,   30, 0x08,
      87,   30,   30,   30, 0x08,
      98,   30,   30,   30, 0x08,
     109,   30,   30,   30, 0x08,
     121,   30,   30,   30, 0x08,
     133,   30,   30,   30, 0x08,
     144,   30,   30,   30, 0x08,
     157,   30,   30,   30, 0x08,
     170,   30,   30,   30, 0x08,
     195,  184,   30,   30, 0x08,
     231,  184,   30,   30, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_spikestream__NetworkViewer_V2[] = {
    "spikestream::NetworkViewer_V2\0\0refresh()\0"
    "reset()\0moveBackward()\0moveForward()\0"
    "moveUp()\0moveDown()\0moveLeft()\0"
    "moveRight()\0resetView()\0rotateUp()\0"
    "rotateDown()\0rotateLeft()\0rotateRight()\0"
    "clipVolume\0viewClippingVolume_Horizontal(Box&)\0"
    "viewClippingVolume_Vertical(Box&)\0"
};

const QMetaObject spikestream::NetworkViewer_V2::staticMetaObject = {
    { &QGLWidget::staticMetaObject, qt_meta_stringdata_spikestream__NetworkViewer_V2,
      qt_meta_data_spikestream__NetworkViewer_V2, 0 }
};

const QMetaObject *spikestream::NetworkViewer_V2::metaObject() const
{
    return &staticMetaObject;
}

void *spikestream::NetworkViewer_V2::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_spikestream__NetworkViewer_V2))
        return static_cast<void*>(const_cast< NetworkViewer_V2*>(this));
    return QGLWidget::qt_metacast(_clname);
}

int spikestream::NetworkViewer_V2::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGLWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: refresh(); break;
        case 1: reset(); break;
        case 2: moveBackward(); break;
        case 3: moveForward(); break;
        case 4: moveUp(); break;
        case 5: moveDown(); break;
        case 6: moveLeft(); break;
        case 7: moveRight(); break;
        case 8: resetView(); break;
        case 9: rotateUp(); break;
        case 10: rotateDown(); break;
        case 11: rotateLeft(); break;
        case 12: rotateRight(); break;
        case 13: viewClippingVolume_Horizontal((*reinterpret_cast< Box(*)>(_a[1]))); break;
        case 14: viewClippingVolume_Vertical((*reinterpret_cast< Box(*)>(_a[1]))); break;
        }
        _id -= 15;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
