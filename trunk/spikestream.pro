# Components of the SpikeStream build
TEMPLATE = subdirs

#===========  CORE SPIKESTREAM COMPONENTS  ===========
SUBDIRS =   library applicationlibrary application


#================  ANALYSIS PLUGINS  =================
SUBDIRS += analysis/statebasedphi analysis/liveliness


#================  NETWORKS PLUGINS  =================
#SUBDIRS += networks/tononinetworks networks/aleksandernetworks networks/alekgameznets2
SUBDIRS += networks/connectionmatriximporter


#================  NEURON GROUP PLUGINS  =================
SUBDIRS += neurongroups/cuboid


#================  CONNECTION GROUP PLUGINS  =================
SUBDIRS += connectiongroups/random1 connectiongroups/topographic1


#================  SIMULATION PLUGINS  =================
SUBDIRS += simulators/nemo
	
	
#================  INSTALLATION  =====================
SUBDIRS += installation/dbconfigtool


#=================  TESTS  ===================
#SUBDIRS += testlibrary
#SUBDIRS += library/test
#SUBDIRS += applicationlibrary/test
#SUBDIRS += analysis/statebasedphi/test
#SUBDIRS += analysis/liveliness/test
#SUBDIRS += simulators/nemo/test


