#ifndef ANALYSISLOADERWIDGET_CPP
#define ANALYSISLOADERWIDGET_CPP

//Qt includes
#include <QWidget>
#include <QHash>
#include <QLayout>
#include <QComboBox>

class AnalysisLoaderWidget : public QWidget {
  Q_OBJECT

    public:
	AnalysisLoaderWidget(QWidget* parent=0);
	~AnalysisLoaderWidget();

    private slots:
	void showAnalysisWidget();

    private:
	//=========================  VARIABLES  =============================
	/*! Combo box to select plugin */
	QComboBox* pluginsCombo;

	/*! Stores the position in the stacked widget where the analysis plugins are stored. */
	QHash<QString, int> pluginWidgetMap;

	/*! Vertical box organizing layout */
	QVBoxLayout *mainVerticalBox;
};



#endif // ANALYSISLOADERWIDGET_CPP
