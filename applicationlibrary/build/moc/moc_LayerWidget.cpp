/****************************************************************************
** Meta object code from reading C++ file 'LayerWidget.h'
**
** Created
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/LayerWidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'LayerWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_LayerWidget[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      13,   12,   12,   12, 0x08,
      27,   12,   12,   12, 0x08,
      49,   12,   12,   12, 0x08,
      64,   60,   12,   12, 0x08,
      97,   12,   12,   12, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_LayerWidget[] = {
    "LayerWidget\0\0deleteLayer()\0"
    "editLayerProperties()\0newLayer()\0,,,\0"
    "tableClicked(int,int,int,QPoint)\0"
    "tableHeaderClicked(int)\0"
};

const QMetaObject LayerWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_LayerWidget,
      qt_meta_data_LayerWidget, 0 }
};

const QMetaObject *LayerWidget::metaObject() const
{
    return &staticMetaObject;
}

void *LayerWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_LayerWidget))
        return static_cast<void*>(const_cast< LayerWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int LayerWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: deleteLayer(); break;
        case 1: editLayerProperties(); break;
        case 2: newLayer(); break;
        case 3: tableClicked((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< const QPoint(*)>(_a[4]))); break;
        case 4: tableHeaderClicked((*reinterpret_cast< int(*)>(_a[1]))); break;
        }
        _id -= 5;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
