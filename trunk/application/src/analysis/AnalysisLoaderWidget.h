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
        AnalysisLoaderWidget(QWidget* parent);
        ~AnalysisLoaderWidget();

    private slots:
        void setAnalysisWidget();

    private:
        //=========================  VARIABLES  =============================
        /*! Combo box to select plugin */
        QComboBox* pluginsCombo;

        /*! Map of all the dynamically loaded analysis widgets */
        QHash<QString, QWidget*> pluginWidgetMap;

        /*! The widget that is currently visible */
        QWidget* currentAnalysisWidget;

        /*! Vertical box organizing layout */
        QVBoxLayout *mainVerticalBox;
};



#endif // ANALYSISLOADERWIDGET_CPP
