TEMPLATE = lib

TARGET = spikestreamapplication

VERSION = 0.2

DESTDIR = $(SPIKESTREAM_ROOT)/lib

OBJECTS_DIR = build/objects

MOC_DIR = build/moc

LIBS += -lqwt -L/usr/local/qwt/lib \
	-lspikestream -L$(SPIKESTREAM_ROOT)/lib \
	-lgmp

INCLUDEPATH += src \
		src/analysis \
		src/exceptions \
		src/nrm \
		src/delegates \
		src/dialogs \
		src/models \
		src/views \
		src/widgets \
		$(SPIKESTREAM_ROOT)/library/include \
		/usr/local/qwt/include

CONFIG += debug \
	  warn_on \
	  qt \
	  opengl \
	  thread \
	  exceptions \
	  stl

QT += xml opengl qt3support sql


#----------------------------------------------#
#---            nrm folder                  ---#
#----------------------------------------------#
HEADERS += src/nrm/NRMNetwork.h \
	    src/nrm/NRMConfigLoader.h \
	    src/nrm/NRMTrainingLoader.h \
	    src/nrm/NRMConstants.h \
	    src/nrm/NRMLayer.h \
	    src/nrm/NRMNeuralLayer.h \
	    src/nrm/NRMInputLayer.h \
	    src/nrm/NRMConnection.h \
	    src/nrm/NRMNeuron.h \
	    src/nrm/NRMRandom.h \
	    src/nrm/NRMFileLoader.h \
	    src/nrm/NRMDataImporter.h \
	    src/nrm/NRMDataSetImporter.h \
	    src/nrm/NRMDataSet.h
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
#---             analysis folder            ---#
#----------------------------------------------#
HEADERS += src/analysis/AnalysisLoaderWidget.h
SOURCES += src/analysis/AnalysisLoaderWidget.cpp

#----------------------------------------------#
#---              dialogs folder            ---#
#----------------------------------------------#
HEADERS += src/dialogs/NRMImportDialog.h \
	    src/dialogs/LoadAnalysisDialog.h \
	    src/dialogs/PluginsDialog.h
SOURCES += src/dialogs/NRMImportDialog.cpp \
	    src/dialogs/LoadAnalysisDialog.cpp \
	    src/dialogs/PluginsDialog.cpp

#----------------------------------------------#
#---            exceptions folder           ---#
#----------------------------------------------#
HEADERS += src/exceptions/NRMException.h
SOURCES += src/exceptions/NRMException.cpp

#----------------------------------------------#
#---              models folder             ---#
#----------------------------------------------#
HEADERS += src/models/NeuronGroupModel.h \
	    src/models/ConnectionGroupModel.h \
	    src/models/ConnectionsModel.h \
	    src/models/AnalysesModel.h
SOURCES += src/models/NeuronGroupModel.cpp \
	    src/models/ConnectionGroupModel.cpp \
	    src/models/ConnectionsModel.cpp \
	    src/models/AnalysesModel.cpp

#----------------------------------------------#
#---              views folder             ---#
#----------------------------------------------#
HEADERS += src/views/NeuronGroupTableView.h \
	    src/views/ConnectionGroupTableView.h \
	    src/views/ConnectionsTableView.h \
	    src/views/AnalysesTableView.h
SOURCES += src/views/NeuronGroupTableView.cpp \
	    src/views/ConnectionGroupTableView.cpp \
	    src/views/ConnectionsTableView.cpp \
	    src/views/AnalysesTableView.cpp

#----------------------------------------------#
#---              widgets folder            ---#
#----------------------------------------------#
HEADERS += src/widgets/NetworksWidget.h \
	    src/widgets/ArchiveWidget_V2.h \
	    src/widgets/NeuronGroupWidget.h \
	    src/widgets/ConnectionWidget_V2.h \
	    src/widgets/NetworkViewerProperties_V2.h
SOURCES += src/widgets/NetworksWidget.cpp \
	    src/widgets/ArchiveWidget_V2.cpp \
	    src/widgets/NeuronGroupWidget.cpp \
	    src/widgets/ConnectionWidget_V2.cpp \
	    src/widgets/NetworkViewerProperties_V2.cpp

#----------------------------------------------#
#---               src folder               ---#
#----------------------------------------------#
HEADERS += src/SpikeStreamApplication.h \
	   src/SpikeStreamMainWindow.h \
	   src/HighlightDialog.h \
	   src/PluginManager.h \
	   src/Globals.h \
	   src/EventRouter.h \
	   src/NetworkViewer_V2.h \
	   src/NetworkDisplay.h \
	   src/ArchivePlayerThread.h \
	   src/NetworksBuilder.h

SOURCES += src/SpikeStreamApplication.cpp \
	   src/SpikeStreamMainWindow.cpp \
	   src/HighlightDialog.cpp \
	   src/PluginManager.cpp \
	   src/Globals.cpp \
	   src/EventRouter.cpp \
	   src/NetworkViewer_V2.cpp \
	   src/NetworkDisplay.cpp \
	   src/ArchivePlayerThread.cpp \
	   src/NetworksBuilder.cpp
