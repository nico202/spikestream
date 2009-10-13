/****************************************************************************
** Meta object code from reading C++ file 'ArchiveWidget_V2.h'
**
** Created
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/widgets/ArchiveWidget_V2.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ArchiveWidget_V2.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_spikestream__ArchiveWidget_V2[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // signals: signature, parameters, type, tag, flags
      31,   30,   30,   30, 0x05,

 // slots: signature, parameters, type, tag, flags
      48,   30,   30,   30, 0x08,
      73,   30,   30,   30, 0x08,
      96,   30,   30,   30, 0x08,
     112,   30,   30,   30, 0x08,
     126,   30,   30,   30, 0x08,
     144,   30,   30,   30, 0x08,
     169,  166,   30,   30, 0x08,
     193,   30,   30,   30, 0x08,
     213,  166,   30,   30, 0x08,
     244,   30,   30,   30, 0x08,
     264,   30,   30,   30, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_spikestream__ArchiveWidget_V2[] = {
    "spikestream::ArchiveWidget_V2\0\0"
    "archiveChanged()\0archiveTimeStepChanged()\0"
    "archivePlayerStopped()\0deleteArchive()\0"
    "loadArchive()\0loadArchiveList()\0"
    "rewindButtonPressed()\0on\0"
    "playButtonToggled(bool)\0stepButtonPressed()\0"
    "fastForwardButtonToggled(bool)\0"
    "stopButtonPressed()\0frameRateComboChanged(int)\0"
};

const QMetaObject spikestream::ArchiveWidget_V2::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_spikestream__ArchiveWidget_V2,
      qt_meta_data_spikestream__ArchiveWidget_V2, 0 }
};

const QMetaObject *spikestream::ArchiveWidget_V2::metaObject() const
{
    return &staticMetaObject;
}

void *spikestream::ArchiveWidget_V2::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_spikestream__ArchiveWidget_V2))
        return static_cast<void*>(const_cast< ArchiveWidget_V2*>(this));
    return QWidget::qt_metacast(_clname);
}

int spikestream::ArchiveWidget_V2::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: archiveChanged(); break;
        case 1: archiveTimeStepChanged(); break;
        case 2: archivePlayerStopped(); break;
        case 3: deleteArchive(); break;
        case 4: loadArchive(); break;
        case 5: loadArchiveList(); break;
        case 6: rewindButtonPressed(); break;
        case 7: playButtonToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 8: stepButtonPressed(); break;
        case 9: fastForwardButtonToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 10: stopButtonPressed(); break;
        case 11: frameRateComboChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        }
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
void spikestream::ArchiveWidget_V2::archiveChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
