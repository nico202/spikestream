SPIKESTREAM_ROOT_DIR = ..

TEMPLATE = lib

TARGET = spikestreamapplication

win32 {
	# Add a copy of the libary to the bin directory
	target.path = $${SPIKESTREAM_ROOT_DIR}/bin
	INSTALLS += target
}

VERSION = 0.2

DESTDIR = $${SPIKESTREAM_ROOT_DIR}/lib

OBJECTS_DIR = build/objects

MOC_DIR = build/moc

#----------------------------------------------#
#---                DEFINES                 ---#
#----------------------------------------------#
unix {
	DEFINES += LINUX32_SPIKESTREAM
}
win32 {
	DEFINES += WIN32_SPIKESTREAM
}

#----------------------------------------------#
#---               LIBRARIES                ---#
#----------------------------------------------#
unix {
	LIBS += -lspikestream -L$${SPIKESTREAM_ROOT_DIR}/lib -lgmp
}
win32 {
	LIBS += -lspikestream0 -L$${SPIKESTREAM_ROOT_DIR}/lib
	LIBS += -lgmp -L$${SPIKESTREAM_ROOT_DIR}/extlib/gmp/lib
	LIBS += -lqwt5 -L$${SPIKESTREAM_ROOT_DIR}/extlib/qwt/lib
}

#----------------------------------------------#
#---                INCLUDE                 ---#
#----------------------------------------------#
INCLUDEPATH += include $${SPIKESTREAM_ROOT_DIR}/library/include
win32 {
	INCLUDEPATH += $${SPIKESTREAM_ROOT_DIR}/extlib/gmp/include  $${SPIKESTREAM_ROOT_DIR}/extlib/qwt/include
}


CONFIG += debug console thread exceptions

QT += xml opengl sql


#----------------------------------------------#
#---          database folder               ---#
#----------------------------------------------#
HEADERS += include/DatabaseManager.h
SOURCES += src/database/DatabaseManager.cpp


#----------------------------------------------#
#---            nrm folder                  ---#
#----------------------------------------------#
HEADERS += include/NRMNetwork.h \
			include/NRMConfigLoader.h \
			include/NRMTrainingLoader.h \
			include/NRMConstants.h \
			include/NRMLayer.h \
			include/NRMNeuralLayer.h \
			include/NRMInputLayer.h \
			include/NRMConnection.h \
			include/NRMNeuron.h \
			include/NRMRandom.h \
			include/NRMFileLoader.h \
			include/NRMDataImporter.h \
			include/NRMDataSetImporter.h \
			include/NRMDataSet.h
SOURCES += src/nrm/NRMNetwork.cpp \
			src/nrm/NRMConfigLoader.cpp \
			src/nrm/NRMTrainingLoader.cpp \
			src/nrm/NRMLayer.cpp \
			src/nrm/NRMNeuralLayer.cpp \
			src/nrm/NRMInputLayer.cpp \
			src/nrm/NRMConnection.cpp \
			src/nrm/NRMNeuron.cpp \
			src/nrm/NRMRandom.cpp \
			src/nrm/NRMFileLoader.cpp \
			src/nrm/NRMDataImporter.cpp \
			src/nrm/NRMDataSetImporter.cpp \
			src/nrm/NRMDataSet.cpp


#----------------------------------------------#
#---                 Analysis               ---#
#----------------------------------------------#
HEADERS += include/AnalysisLoaderWidget.h \
			include/AbstractAnalysisWidget.h \
			include/AnalysisRunner.h \
			include/AnalysisParameterDialog.h \
			include/AbstractExportAnalysisDialog.h \
			include/AbstractAnalysisTimeStepThread.h \
			include/ProgressWidget.h \
			include/HeatColorBar.h \
			include/AbstractGraphDialog.h \
			include/AbstractSpectrogramData.h \
			include/AnalysisSpectrogram.h
SOURCES += src/analysis/AnalysisLoaderWidget.cpp \
			src/analysis/AbstractAnalysisWidget.cpp \
			src/analysis/AnalysisRunner.cpp \
			src/analysis/AnalysisParameterDialog.cpp \
			src/analysis/AbstractExportAnalysisDialog.cpp \
			src/analysis/AbstractAnalysisTimeStepThread.cpp \
			src/analysis/ProgressWidget.cpp \
			src/analysis/HeatColorBar.cpp \
			src/analysis/AbstractGraphDialog.cpp \
			src/analysis/AbstractSpectrogramData.cpp \
			src/analysis/AnalysisSpectrogram.cpp


#----------------------------------------------#
#---                Dialogs                 ---#
#----------------------------------------------#
HEADERS += include/NRMImportDialog.h \
			include/LoadAnalysisDialog.h \
			include/PluginsDialog.h \
			include/TruthTableDialog.h \
			include/NewNetworkDialog.h
SOURCES += src/dialogs/NRMImportDialog.cpp \
			src/dialogs/LoadAnalysisDialog.cpp \
			src/dialogs/PluginsDialog.cpp \
			src/dialogs/TruthTableDialog.cpp \
			src/dialogs/NewNetworkDialog.cpp


#----------------------------------------------#
#---               Exceptions               ---#
#----------------------------------------------#
HEADERS += include/NRMException.h
SOURCES += src/exceptions/NRMException.cpp


#----------------------------------------------#
#---                Models                  ---#
#----------------------------------------------#
HEADERS += include/NeuronGroupModel.h \
			include/ConnectionGroupModel.h \
			include/ConnectionsModel.h \
			include/AnalysesModel.h \
			include/TruthTableModel.h
SOURCES += src/models/NeuronGroupModel.cpp \
			src/models/ConnectionGroupModel.cpp \
			src/models/ConnectionsModel.cpp \
			src/models/AnalysesModel.cpp \
			src/models/TruthTableModel.cpp


#----------------------------------------------#
#---                   Views                ---#
#----------------------------------------------#
HEADERS += include/NeuronGroupTableView.h \
			include/ConnectionGroupTableView.h \
			include/ConnectionsTableView.h \
			include/AnalysesTableView.h \
			include/TruthTableView.h
SOURCES += src/views/NeuronGroupTableView.cpp \
			src/views/ConnectionGroupTableView.cpp \
			src/views/ConnectionsTableView.cpp \
			src/views/AnalysesTableView.cpp \
			src/views/TruthTableView.cpp


#----------------------------------------------#
#---                 Widgets                ---#
#----------------------------------------------#
HEADERS += include/NetworksWidget.h \
			include/ArchiveWidget_V2.h \
			include/NeuronGroupWidget.h \
			include/ConnectionWidget.h \
			include/NetworkViewerProperties_V2.h \
			include/SimulationLoaderWidget.h
SOURCES += src/widgets/NetworksWidget.cpp \
			src/widgets/ArchiveWidget_V2.cpp \
			src/widgets/NeuronGroupWidget.cpp \
			src/widgets/ConnectionWidget.cpp \
			src/widgets/NetworkViewerProperties_V2.cpp \
			src/widgets/SimulationLoaderWidget.cpp


#----------------------------------------------#
#---             Miscellaneous              ---#
#----------------------------------------------#
HEADERS += include/SpikeStreamApplication.h \
		   include/SpikeStreamMainWindow.h \
		   include/PluginManager_V2.h \
		   include/Globals.h \
		   include/EventRouter.h \
		   include/NetworkViewer_V2.h \
		   include/NetworkDisplay.h \
		   include/ArchivePlayerThread.h \
		   include/NetworksBuilder.h
SOURCES += src/SpikeStreamApplication.cpp \
		   src/SpikeStreamMainWindow.cpp \
		   src/PluginManager_V2.cpp \
		   src/Globals.cpp \
		   src/EventRouter.cpp \
		   src/NetworkViewer_V2.cpp \
		   src/NetworkDisplay.cpp \
		   src/ArchivePlayerThread.cpp \
		   src/NetworksBuilder.cpp

