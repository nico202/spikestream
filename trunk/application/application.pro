TEMPLATE = app

TARGET = spikestream

VERSION = 0.2

DESTDIR = $(SPIKESTREAM_ROOT)/bin

OBJECTS_DIR = build/objects

MOC_DIR = build/moc

LIBS += -lqwt -L/usr/local/qwt/lib \
        -lspikestream -L$(SPIKESTREAM_ROOT)/lib \
        -lmysqlpp \
        -lpvm3 

INCLUDEPATH += $(SPIKESTREAM_ROOT)/library/include \
                 /usr/include/mysql \
                 /usr/local/include/mysql++ \
                 /usr/local/qwt/include

CONFIG += debug \
          warn_on \
          qt \
          opengl \
          thread \
          exceptions \
          stl 

QT += xml  opengl qt3support

HEADERS += src/ArchiveManager.h \
           src/ArchiveStatistic.h \
           src/ArchiveStatisticsDialog.h \
           src/ArchiveWidget.h \
           src/BusyDialog.h \
           src/ConnectionCheckTableItem.h \
           src/ConnectionGroupHolder.h \
           src/ConnectionHolder.h \
           src/ConnectionManager.h \
           src/ConnectionParameterTable.h \
           src/ConnectionParameterViewer.h \
           src/ConnectionPropertiesDialog.h \
           src/ConnectionTableItem.h \
           src/ConnectionWidget.h \
           src/Debug.h \
           src/EditNeuronParametersDialog.h \
           src/EditSynapseParametersDialog.h \
           src/GlobalParametersDialog.h \
           src/LayerManager.h \
           src/LayerPropertiesDialog.h \
           src/LayerWidget.h \
           src/LoadArchiveDialog.h \
           src/MonitorArea.h \
           src/MonitorWindow.h \
           src/NameDialog.h \
           src/NetworkDataXmlHandler.h \
           src/NetworkModelXmlHandler.h \
           src/NetworkMonitor.h \
           src/NetworkViewer.h \
           src/NetworkViewerProperties.h \
           src/NeuronGroup.h \
           src/NeuronGroupHolder.h \
           src/NeuronParametersDialog.h \
           src/NoiseParametersDialog.h \
           src/ParameterTable.h \
           src/PatternManager.h \
           src/ScriptRunner.h \
           src/SimulationManager.h \
           src/SimulationWidget.h \
           src/SpikeStreamApplication.h \
           src/SpikeStreamMainWindow.h \
           src/SynapseParametersDialog.h \
           src/Tester.h \
           src/DatabaseManager.h \
           src/PatternDialog.h \
           src/MonitorDataset.h \
           src/MonitorDataPlotter.h \
           src/MonitorXmlHandler.h \
           src/ProbeDialog.h \
           src/ViewModelDialog.h \
           src/ConnectionMatrixLoader.h \
           src/HighlightDialog.h \
            src/testQApplication.h

SOURCES += src/ArchiveManager.cpp \
           src/ArchiveStatistic.cpp \
           src/ArchiveStatisticsDialog.cpp \
           src/ArchiveWidget.cpp \
           src/BusyDialog.cpp \
           src/ConnectionCheckTableItem.cpp \
           src/ConnectionGroupHolder.cpp \
           src/ConnectionManager.cpp \
           src/ConnectionParameterTable.cpp \
           src/ConnectionParameterViewer.cpp \
           src/ConnectionPropertiesDialog.cpp \
           src/ConnectionTableItem.cpp \
           src/ConnectionWidget.cpp \
           src/EditNeuronParametersDialog.cpp \
           src/EditSynapseParametersDialog.cpp \
           src/GlobalParametersDialog.cpp \
           src/LayerManager.cpp \
           src/LayerPropertiesDialog.cpp \
           src/LayerWidget.cpp \
           src/LoadArchiveDialog.cpp \
           src/MonitorArea.cpp \
           src/MonitorWindow.cpp \
           src/NameDialog.cpp \
           src/NetworkDataXmlHandler.cpp \
           src/NetworkModelXmlHandler.cpp \
           src/NetworkMonitor.cpp \
           src/NetworkViewer.cpp \
           src/NetworkViewerProperties.cpp \
           src/NeuronGroupHolder.cpp \
           src/NeuronParametersDialog.cpp \
           src/NoiseParametersDialog.cpp \
           src/ParameterTable.cpp \
           src/PatternManager.cpp \
           src/ScriptRunner.cpp \
           src/SimulationManager.cpp \
           src/SimulationWidget.cpp \
           src/SpikeStreamApplication.cpp \
           src/SpikeStreamMainWindow.cpp \
           src/SynapseParametersDialog.cpp \
           src/Tester.cpp \
           src/Main.cpp \
           src/DatabaseManager.cpp \
           src/PatternDialog.cpp \
           src/MonitorDataset.cpp \
           src/MonitorDataPlotter.cpp \
           src/MonitorXmlHandler.cpp \
           src/ProbeDialog.cpp \
           src/ViewModelDialog.cpp \
           src/ConnectionMatrixLoader.cpp \
           src/HighlightDialog.cpp \
            src/testQApplication.cpp

