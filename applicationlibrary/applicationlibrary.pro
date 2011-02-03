SPIKESTREAM_ROOT_DIR = ..

include( $${SPIKESTREAM_ROOT_DIR}/spikestream.pri )

TEMPLATE = lib

TARGET = spikestreamapplication


#----------------------------------------------#
#---          INSTALLATION LOCATION         ---#
#----------------------------------------------#
unix:!macx {
	target.path = /usr/local/lib
	INSTALLS += target
}
win32 {
	# Add a copy of the libary to the bin directory
	target.path = $${SPIKESTREAM_ROOT_DIR}/bin
	INSTALLS += target
}
macx {
	# Add a copy of the libary to the lib directory
	target.path = /usr/lib
	INSTALLS += target
}


DESTDIR = $${SPIKESTREAM_ROOT_DIR}/lib

QT += xml opengl sql


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
HEADERS += include/ConnectionManager.h \
			include/NetworkManager.h
SOURCES += src/managers/ConnectionManager.cpp \
			src/managers/NetworkManager.cpp


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
#---                 Builders               ---#
#----------------------------------------------#
HEADERS += include/AbstractConnectionBuilder.h \
			include/AbstractNeuronBuilder.h
SOURCES += src/builders/AbstractConnectionBuilder.cpp \
			src/builders/AbstractNeuronBuilder.cpp

#----------------------------------------------#
#---                Dialogs                 ---#
#----------------------------------------------#
HEADERS += include/AbstractParametersEditDialog.h \
			include/LoadAnalysisDialog.h \
			include/PluginsDialog.h \
			include/TruthTableDialog.h \
			include/NetworkDialog.h \
			include/NeuronParametersDialog.h \
			include/NeuronParametersEditDialog.h \
			include/SynapseParametersDialog.h \
			include/SynapseParametersEditDialog.h \
			include/ViewParametersDialog.h \
			include/ParametersDialog.h \
			include/NeuronGroupDialog.h \
			include/ConnectionGroupDialog.h \
			include/DescriptionDialog.h \
			include/NeuronGroupSelectionDialog.h \
			include/SpikeRasterDialog.h
SOURCES += src/dialogs/AbstractParametersEditDialog.cpp \
			src/dialogs/LoadAnalysisDialog.cpp \
			src/dialogs/PluginsDialog.cpp \
			src/dialogs/TruthTableDialog.cpp \
			src/dialogs/NetworkDialog.cpp \
			src/dialogs/NeuronParametersDialog.cpp \
			src/dialogs/NeuronParametersEditDialog.cpp \
			src/dialogs/SynapseParametersDialog.cpp \
			src/dialogs/SynapseParametersEditDialog.cpp \
			src/dialogs/ViewParametersDialog.cpp \
			src/dialogs/ParametersDialog.cpp \
			src/dialogs/NeuronGroupDialog.cpp \
			src/dialogs/ConnectionGroupDialog.cpp \
			src/dialogs/DescriptionDialog.cpp \
			src/dialogs/NeuronGroupSelectionDialog.cpp \
			src/dialogs/SpikeRasterDialog.cpp


#----------------------------------------------#
#---                Models                  ---#
#----------------------------------------------#
HEADERS += include/NeuronGroupModel.h \
			include/ConnectionGroupModel.h \
			include/ConnectionsModel.h \
			include/AnalysesModel.h \
			include/TruthTableModel.h \
			include/NeuronParametersModel.h \
			include/SynapseParametersModel.h \
			include/NeuronGroupSelectionModel.h
SOURCES += src/models/NeuronGroupModel.cpp \
			src/models/ConnectionGroupModel.cpp \
			src/models/ConnectionsModel.cpp \
			src/models/AnalysesModel.cpp \
			src/models/TruthTableModel.cpp \
			src/models/NeuronParametersModel.cpp \
			src/models/SynapseParametersModel.cpp \
			src/models/NeuronGroupSelectionModel.cpp


#----------------------------------------------#
#---               Simulation               ---#
#----------------------------------------------#
HEADERS += include/AbstractSimulation.h \
			include/AbstractExperimentWidget.h
SOURCES += src/simulation/AbstractSimulation.cpp \
			src/simulation/AbstractExperimentWidget.cpp


#----------------------------------------------#
#---                   Views                ---#
#----------------------------------------------#
HEADERS += include/NeuronGroupTableView.h \
			include/ConnectionGroupTableView.h \
			include/ConnectionsTableView.h \
			include/AnalysesTableView.h \
			include/TruthTableView.h \
			include/NeuronParametersView.h \
			include/SynapseParametersView.h \
			include/NeuronGroupSelectionView.h
SOURCES += src/views/NeuronGroupTableView.cpp \
			src/views/ConnectionGroupTableView.cpp \
			src/views/ConnectionsTableView.cpp \
			src/views/AnalysesTableView.cpp \
			src/views/TruthTableView.cpp \
			src/views/NeuronParametersView.cpp \
			src/views/SynapseParametersView.cpp \
			src/views/NeuronGroupSelectionView.cpp


#----------------------------------------------#
#---                 Widgets                ---#
#----------------------------------------------#
HEADERS += include/NetworksWidget.h \
			include/ArchiveWidget.h \
			include/NeuronGroupWidget.h \
			include/ConnectionWidget.h \
			include/NetworkViewerProperties.h \
			include/SimulationLoaderWidget.h \
			include/AbstractConnectionWidget.h \
			include/SpikeRasterWidget.h
SOURCES += src/widgets/NetworksWidget.cpp \
			src/widgets/ArchiveWidget.cpp \
			src/widgets/NeuronGroupWidget.cpp \
			src/widgets/ConnectionWidget.cpp \
			src/widgets/NetworkViewerProperties.cpp \
			src/widgets/SimulationLoaderWidget.cpp \
			src/widgets/AbstractConnectionWidget.cpp \
			src/widgets/SpikeRasterWidget.cpp


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


