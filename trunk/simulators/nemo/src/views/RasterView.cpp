//SpikeStream includes
#include "RasterView.h"
using namespace spikestream;

//Qwt includes
#include <qwt_color_map.h>

//Qt includes
#include <QDebug>


/*! Constructor */
RasterView::RasterView (RasterModel* rasterModel, QWidget* parent) : QwtPlot( parent ){
	//Store reference to data
	this->rasterModel = rasterModel;

	//Set up the spectrogram
	qwtSpectrogram = new QwtPlotSpectrogram();
	qwtSpectrogram->setDisplayMode(QwtPlotSpectrogram::ContourMode, false);
	QwtLinearColorMap colorMap();
	qwtSpectrogram->setColorMap(colorMap);
	qwtSpectrogram->setData(*rasterModel);
	qwtSpectrogram->attach(this);

	//setAxisScale(QwtPlot::yRight, d_spectrogram->data().range().minValue(), d_spectrogram->data().range().maxValue() );
	//enableAxis(QwtPlot::yRight);

	//Only show major markings on axes
	setAxisMaxMinor( QwtPlot::yLeft, 0);
	setAxisMaxMinor( QwtPlot::xBottom, 0);

	rescale();
//	plotLayout()->setAlignCanvasToScales(true);

	//Change canvas background
	setCanvasBackground(Qt::white);

	//Set titles
	//setAxisTitle(QwtPlot::yRight, "Information Integration");

	//Change fonts
	QFont axisFont("Arial", 12, QFont::Normal);
	setAxisFont(QwtPlot::yLeft, axisFont);
	setAxisFont(QwtPlot::xBottom, axisFont);

	// Avoid jumping when labels with more/less digits appear/disappear when scrolling vertically
//	const QFontMetrics fm(axisWidget(QwtPlot::yLeft)->font());
//	QwtScaleDraw *sd = axisScaleDraw(QwtPlot::yLeft);
//	sd->setMinimumExtent( fm.width("100.00") );

	// Show the plots
	replot();
}


/*! Destructor */
RasterView::~RasterView(){
}


/*----------------------------------------------------------*/
/*-----                 PUBLIC METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Rescales the plot */
void RasterView::rescale(){
	setAxisScale(QwtPlot::xBottom, rasterModel->getMinX(), rasterModel->getMaxX(), 1);
	setAxisScale(QwtPlot::yLeft, rasterModel->getMinY(),rasterModel->getMaxY(), 1);
}


/*! Prints the plot to a pdf file
	FIXME: THIS PROBABLY DOESN'T WORK. */
void RasterView::printPlot() {
//	QPrinter printer;
//	printer.setOrientation(QPrinter::Landscape);
//	printer.setOutputFileName("/tmp/spectrogram.pdf");
//	QPrintDialog dialog(&printer);
//	if ( dialog.exec() ){
//		print(printer);
//	}
}
