/****************************************************************************
** Meta object code from reading C++ file 'ArchiveWidget.h'
**
** Created
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/ArchiveWidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ArchiveWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ArchiveWidget[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      22,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x0a,
      31,   14,   14,   14, 0x08,
      73,   55,   14,   14, 0x08,
     109,   14,   14,   14, 0x08,
     126,   14,   14,   14, 0x08,
     153,   14,   14,   14, 0x08,
     179,  176,   14,   14, 0x08,
     210,   14,   14,   14, 0x08,
     237,   14,   14,   14, 0x08,
     253,   14,   14,   14, 0x08,
     273,   14,   14,   14, 0x08,
     297,   14,   14,   14, 0x08,
     319,   14,   14,   14, 0x08,
     339,   14,   14,   14, 0x08,
     367,  359,   14,   14, 0x08,
     403,  393,   14,   14, 0x08,
     441,   14,   14,   14, 0x08,
     461,   14,   14,   14, 0x08,
     481,   14,   14,   14, 0x08,
     499,   14,   14,   14, 0x08,
     524,   14,   14,   14, 0x08,
     548,   14,   14,   14, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_ArchiveWidget[] = {
    "ArchiveWidget\0\0unloadArchive()\0"
    "addStatsButtonClicked()\0timeStamp,newName\0"
    "archiveNameChanged(QString,QString)\0"
    "archiveStopped()\0deleteStatsButtonClicked()\0"
    "dockAllButtonClicked()\0on\0"
    "fastForwardButtonToggled(bool)\0"
    "frameRateComboChanged(int)\0freezeArchive()\0"
    "loadButtonPressed()\0playButtonToggled(bool)\0"
    "rewindButtonPressed()\0setSpikeCount(uint)\0"
    "setSpikeTotal(uint)\0message\0"
    "showArchiveError(QString)\0row,col,,\0"
    "statsTableClicked(int,int,int,QPoint)\0"
    "stepButtonPressed()\0stopButtonPressed()\0"
    "unfreezeArchive()\0undockAllButtonClicked()\0"
    "updateStatisticsTable()\0"
    "viewModelButtonClicked()\0"
};

const QMetaObject ArchiveWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_ArchiveWidget,
      qt_meta_data_ArchiveWidget, 0 }
};

const QMetaObject *ArchiveWidget::metaObject() const
{
    return &staticMetaObject;
}

void *ArchiveWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ArchiveWidget))
        return static_cast<void*>(const_cast< ArchiveWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int ArchiveWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: unloadArchive(); break;
        case 1: addStatsButtonClicked(); break;
        case 2: archiveNameChanged((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 3: archiveStopped(); break;
        case 4: deleteStatsButtonClicked(); break;
        case 5: dockAllButtonClicked(); break;
        case 6: fastForwardButtonToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 7: frameRateComboChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: freezeArchive(); break;
        case 9: loadButtonPressed(); break;
        case 10: playButtonToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 11: rewindButtonPressed(); break;
        case 12: setSpikeCount((*reinterpret_cast< uint(*)>(_a[1]))); break;
        case 13: setSpikeTotal((*reinterpret_cast< uint(*)>(_a[1]))); break;
        case 14: showArchiveError((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 15: statsTableClicked((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3])),(*reinterpret_cast< const QPoint(*)>(_a[4]))); break;
        case 16: stepButtonPressed(); break;
        case 17: stopButtonPressed(); break;
        case 18: unfreezeArchive(); break;
        case 19: undockAllButtonClicked(); break;
        case 20: updateStatisticsTable(); break;
        case 21: viewModelButtonClicked(); break;
        }
        _id -= 22;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
