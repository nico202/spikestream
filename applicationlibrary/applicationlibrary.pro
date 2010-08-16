SPIKESTREAM_ROOT_DIR = ..

include( $${SPIKESTREAM_ROOT_DIR}/spikestream.pri )

TEMPLATE = lib

TARGET = spikestreamapplication


#----------------------------------------------#
#---          INSTALLATION LOCATION         ---#
#----------------------------------------------#
unix {
	target.path = /usr/local/lib
	INSTALLS += target
}
win32 {
	# Add a copy of the libary to the bin directory
	target.path = $${SPIKESTREAM_ROOT_DIR}/bin
	INSTALLS += target
}

DESTDIR = $${SPIKESTREAM_ROOT_DIR}/lib

QT += xml opengl sql


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
#---             INCLUDE PATH               ---#
#----------------------------------------------#
INCLUDEPATH += include $${SPIKESTREAM_ROOT_DIR}/library/include


#----------------------------------------------#
#---               LIBRARIES                ---#
#----------------------------------------------#
unix {
	LIBS += -lspikestream -L$${SPIKESTREAM_ROOT_DIR}/lib
}
win32 {
	LIBS += -lspikestream0 -L$${SPIKESTREAM_ROOT_DIR}/lib
}



#----------------------------------------------#
#---                Database                ---#
#----------------------------------------------#
HEADERS += include/DatabaseManager.h
SOURCES += src/database/DatabaseManager.cpp


#----------------------------------------------#
#---                Managers                ---#
#----------------------------------------------#
HEADERS += include/ConnectionManager.h
SOURCES += src/managers/ConnectionManager.cpp


#----------------------------------------------#
#---                 NRM                    ---#
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
HEADERS += include/AbstractParametersEditDialog.h \
			include/NRMImportDialog.h \
			include/LoadAnalysisDialog.h \
			include/PluginsDialog.h \
			include/TruthTableDialog.h \
			include/NewNetworkDialog.h \
			include/NeuronParametersDialog.h \
			include/NeuronParametersEditDialog.h \
			include/SynapseParametersDialog.h \
			include/SynapseParametersEditDialog.h
SOURCES += src/dialogs/AbstractParametersEditDialog.cpp \
			src/dialogs/NRMImportDialog.cpp \
			src/dialogs/LoadAnalysisDialog.cpp \
			src/dialogs/PluginsDialog.cpp \
			src/dialogs/TruthTableDialog.cpp \
			src/dialogs/NewNetworkDialog.cpp \
			src/dialogs/NeuronParametersDialog.cpp \
			src/dialogs/NeuronParametersEditDialog.cpp \
			src/dialogs/SynapseParametersDialog.cpp \
			src/dialogs/SynapseParametersEditDialog.cpp


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
			include/TruthTableModel.h \
			include/NeuronParametersModel.h \
			include/SynapseParametersModel.h
SOURCES += src/models/NeuronGroupModel.cpp \
			src/models/ConnectionGroupModel.cpp \
			src/models/ConnectionsModel.cpp \
			src/models/AnalysesModel.cpp \
			src/models/TruthTableModel.cpp \
			src/models/NeuronParametersModel.cpp \
			src/models/SynapseParametersModel.cpp


#----------------------------------------------#
#---                   Views                ---#
#----------------------------------------------#
HEADERS += include/NeuronGroupTableView.h \
			include/ConnectionGroupTableView.h \
			include/ConnectionsTableView.h \
			include/AnalysesTableView.h \
			include/TruthTableView.h \
			include/NeuronParametersView.h \
			include/SynapseParametersView.h
SOURCES += src/views/NeuronGroupTableView.cpp \
			src/views/ConnectionGroupTableView.cpp \
			src/views/ConnectionsTableView.cpp \
			src/views/AnalysesTableView.cpp \
			src/views/TruthTableView.cpp \
			src/views/NeuronParametersView.cpp \
			src/views/SynapseParametersView.cpp


#----------------------------------------------#
#---                 Widgets                ---#
#----------------------------------------------#
HEADERS += include/NetworksWidget.h \
			include/ArchiveWidget.h \
			include/NeuronGroupWidget.h \
			include/ConnectionWidget.h \
			include/NetworkViewerProperties.h \
			include/SimulationLoaderWidget.h
SOURCES += src/widgets/NetworksWidget.cpp \
			src/widgets/ArchiveWidget.cpp \
			src/widgets/NeuronGroupWidget.cpp \
			src/widgets/ConnectionWidget.cpp \
			src/widgets/NetworkViewerProperties.cpp \
			src/widgets/SimulationLoaderWidget.cpp


#----------------------------------------------#
#---             Miscellaneous              ---#
#----------------------------------------------#
HEADERS += include/SpikeStreamApplication.h \
		   include/SpikeStreamMainWindow.h \
		   include/PluginManager.h \
		   include/Globals.h \
		   include/EventRouter.h \
		   include/NetworkViewer.h \
		   include/NetworkDisplay.h \
		   include/ArchivePlayerThread.h \
		   include/NetworksBuilder.h
SOURCES += src/SpikeStreamApplication.cpp \
		   src/SpikeStreamMainWindow.cpp \
		   src/PluginManager.cpp \
		   src/Globals.cpp \
		   src/NetworkViewer.cpp \
		   src/NetworkDisplay.cpp \
		   src/ArchivePlayerThread.cpp \
		   src/NetworksBuilder.cpp


