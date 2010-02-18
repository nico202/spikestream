TEMPLATE = subdirs

CONFIG += release warn_off

#===========  CORE SPIKESTREAM COMPONENTS  ===========
SUBDIRS =   library \
			applicationlibrary \
			application


#================  ANALYSIS PLUGINS  =================
SUBDIRS += analysis/statebasedphi \
			analysis/liveliness


#================  NETWORKS PLUGINS  =================
SUBDIRS += networks/tononinetworks \
			networks/aleksandernetworks


#=================  TESTS  ===================
SUBDIRS += testlibrary \
			library/test \
			applicationlibrary/test \
			analysis/statebasedphi/test \
			analysis/liveliness/test


