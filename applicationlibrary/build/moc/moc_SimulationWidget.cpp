/****************************************************************************
** Meta object code from reading C++ file 'SimulationWidget.h'
**
** Created
**      by: The Qt Meta Object Compiler version 59 (Qt 4.4.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../src/SimulationWidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SimulationWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 59
#error "This file was generated using the moc from 4.4.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_SimulationWidget[] = {

 // content:
       1,       // revision
       0,       // classname
       0,    0, // classinfo
      30,   10, // methods
       0,    0, // properties
       0,    0, // enums/sets

 // slots: signature, parameters, type, tag, flags
      18,   17,   17,   17, 0x08,
      45,   17,   17,   17, 0x08,
      69,   17,   17,   17, 0x08,
      93,   17,   17,   17, 0x08,
     117,   17,   17,   17, 0x08,
     140,   17,   17,   17, 0x08,
     173,  166,   17,   17, 0x08,
     207,   17,   17,   17, 0x08,
     234,   17,   17,   17, 0x08,
     261,   17,   17,   17, 0x08,
     287,   17,   17,   17, 0x08,
     317,   17,   17,   17, 0x08,
     344,   17,   17,   17, 0x08,
     367,   17,   17,   17, 0x08,
     396,  166,   17,   17, 0x08,
     433,   17,   17,   17, 0x08,
     463,   17,   17,   17, 0x08,
     490,   17,   17,   17, 0x08,
     516,   17,   17,   17, 0x08,
     542,   17,   17,   17, 0x08,
     571,   17,   17,   17, 0x08,
     611,  598,   17,   17, 0x08,
     640,   17,   17,   17, 0x08,
     666,   17,   17,   17, 0x08,
     691,   17,   17,   17, 0x08,
     719,   17,   17,   17, 0x08,
     742,   17,   17,   17, 0x08,
     765,   17,   17,   17, 0x08,
     793,   17,   17,   17, 0x08,
     818,   17,   17,   17, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_SimulationWidget[] = {
    "SimulationWidget\0\0checkSimManagerForErrors()\0"
    "checkViewWeightsSaved()\0checkWeightsLoadState()\0"
    "checkWeightsSaveState()\0dockAllButtonClicked()\0"
    "fireNeuronButtonPressed()\0string\0"
    "fireNeuronComboActivated(QString)\0"
    "frameRateComboChanged(int)\0"
    "globalParamButtonPressed()\0"
    "hideGraphsButtonClicked()\0"
    "initialiseButtonToggled(bool)\0"
    "injectNoiseButtonPressed()\0"
    "monitorButtonPressed()\0"
    "monitorNeuronButtonPressed()\0"
    "monitorNeuronComboActivated(QString)\0"
    "monitorSynapseButtonPressed()\0"
    "neuronParamButtonPressed()\0"
    "noiseParamButtonPressed()\0"
    "recordButtonToggled(bool)\0"
    "reloadWeightsButtonPressed()\0"
    "saveWeightsButtonPressed()\0simStartTime\0"
    "setSimulationStartTime(uint)\0"
    "showGraphsButtonClicked()\0"
    "simModeComboChanged(int)\0"
    "simStartButtonToggled(bool)\0"
    "simStepButtonPressed()\0simStopButtonPressed()\0"
    "synapseParamButtonPressed()\0"
    "undockAllButtonClicked()\0"
    "viewWeightsButtonPressed()\0"
};

const QMetaObject SimulationWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_SimulationWidget,
      qt_meta_data_SimulationWidget, 0 }
};

const QMetaObject *SimulationWidget::metaObject() const
{
    return &staticMetaObject;
}

void *SimulationWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SimulationWidget))
        return static_cast<void*>(const_cast< SimulationWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int SimulationWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: checkSimManagerForErrors(); break;
        case 1: checkViewWeightsSaved(); break;
        case 2: checkWeightsLoadState(); break;
        case 3: checkWeightsSaveState(); break;
        case 4: dockAllButtonClicked(); break;
        case 5: fireNeuronButtonPressed(); break;
        case 6: fireNeuronComboActivated((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 7: frameRateComboChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: globalParamButtonPressed(); break;
        case 9: hideGraphsButtonClicked(); break;
        case 10: initialiseButtonToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 11: injectNoiseButtonPressed(); break;
        case 12: monitorButtonPressed(); break;
        case 13: monitorNeuronButtonPressed(); break;
        case 14: monitorNeuronComboActivated((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 15: monitorSynapseButtonPressed(); break;
        case 16: neuronParamButtonPressed(); break;
        case 17: noiseParamButtonPressed(); break;
        case 18: recordButtonToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 19: reloadWeightsButtonPressed(); break;
        case 20: saveWeightsButtonPressed(); break;
        case 21: setSimulationStartTime((*reinterpret_cast< uint(*)>(_a[1]))); break;
        case 22: showGraphsButtonClicked(); break;
        case 23: simModeComboChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 24: simStartButtonToggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 25: simStepButtonPressed(); break;
        case 26: simStopButtonPressed(); break;
        case 27: synapseParamButtonPressed(); break;
        case 28: undockAllButtonClicked(); break;
        case 29: viewWeightsButtonPressed(); break;
        }
        _id -= 30;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
