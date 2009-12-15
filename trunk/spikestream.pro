TEMPLATE = subdirs

SUBDIRS =   library \
	    applicationlibrary \
	    application

#============  ANALYSIS PLUGINS  =============
SUBDIRS += analysis/statebasedphi


#============  NETWORKS PLUGINS  =============
SUBDIRS += networks/tononinetworks \
	    networks/aleksandernetworks


#=================  TESTS  ===================
SUBDIRS += testlibrary \
	    library/test \
	    applicationlibrary/test \
	    analysis/statebasedphi/test


