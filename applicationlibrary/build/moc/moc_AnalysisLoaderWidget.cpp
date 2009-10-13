/****************************************************************************
** Meta object code from reading C++ file 'AnalysisLoaderWidget.h'
**
** Created
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/analysis/AnalysisLoaderWidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AnalysisLoaderWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_AnalysisLoaderWidget[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      22,   21,   21,   21, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_AnalysisLoaderWidget[] = {
    "AnalysisLoaderWidget\0\0showAnalysisWidget()\0"
};

const QMetaObject AnalysisLoaderWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_AnalysisLoaderWidget,
      qt_meta_data_AnalysisLoaderWidget, 0 }
};

const QMetaObject *AnalysisLoaderWidget::metaObject() const
{
    return &staticMetaObject;
}

void *AnalysisLoaderWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_AnalysisLoaderWidget))
        return static_cast<void*>(const_cast< AnalysisLoaderWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int AnalysisLoaderWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: showAnalysisWidget(); break;
        }
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
