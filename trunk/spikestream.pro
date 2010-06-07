TEMPLATE = subdirs

#===========  CORE SPIKESTREAM COMPONENTS  ===========
SUBDIRS =   library applicationlibrary application


#================  ANALYSIS PLUGINS  =================
#SUBDIRS += analysis/statebasedphi analysis/liveliness


#================  NETWORKS PLUGINS  =================
#SUBDIRS += networks/tononinetworks networks/aleksandernetworks networks/alekgameznets2


#================  NEURON GROUP PLUGINS  =================
#SUBDIRS += neurongroups/cuboid


#================  SIMULATION PLUGINS  =================
SUBDIRS += simulators/nemo
	
	
#================  INSTALLATION  =====================
#SUBDIRS += installation/dbconfigtool


#=================  TESTS  ===================
#SUBDIRS += testlibrary library/test
#applicationlibrary/test analysis/statebasedphi/test analysis/liveliness/test


