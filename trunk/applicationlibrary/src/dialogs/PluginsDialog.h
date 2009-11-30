#ifndef PLUGINSDIALOG_H
#define PLUGINSDIALOG_H

//Qt includes
#include <QDialog>

namespace spikestream {

    /*! Scans the specified plugins folder and displays a list of available plugins in
	a combo box, which enables the user to select one and interact with its GUI */
    class PluginsDialog : public QDialog {
	Q_OBJECT

	public:
	    PluginsDialog(QWidget* parent, const QString pluginFolder, const QString title);
	    ~PluginsDialog();



    };

}

#endif//PLUGINSDIALOG_H

