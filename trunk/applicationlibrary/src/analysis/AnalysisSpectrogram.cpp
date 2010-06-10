#include "AnalysisSpectrogram.h"
using namespace spikestream;


/*! Constructor */
AnalysisSpectrogram::AnalysisSpectrogram (QWidget* parent, QwtRasterData* data) : QwtPlot( parent ){
	// Show a title
	//setTitle( "This is an Example" );

	d_spectrogram = new QwtPlotSpectrogram();

	QwtLinearColorMap colorMap(Qt::darkCyan, Qt::red);
	colorMap.addColorStop(0.1, Qt::cyan);
	colorMap.addColorStop(0.6, Qt::green);
	colorMap.addColorStop(0.95, Qt::yellow);

	d_spectrogram->setColorMap(colorMap);

	d_spectrogram->setData(*data);
	d_spectrogram->attach(this);

	QwtValueList contourLevels;
	for ( double level = 0.5; level < 10.0; level += 1.0 )
		contourLevels += level;
	d_spectrogram->setContourLevels(contourLevels);

	// A color bar on the right axis
	QwtScaleWidget *rightAxis = axisWidget(QwtPlot::yRight);
	rightAxis->setColorBarEnabled(true);
	rightAxis->setColorMap(d_spectrogram->data().range(), d_spectrogram->colorMap());

	setAxisScale(QwtPlot::yRight, d_spectrogram->data().range().minValue(), d_spectrogram->data().range().maxValue() );
	enableAxis(QwtPlot::yRight);

	//Rescale to match data
	setAxisMaxMinor( QwtPlot::yLeft, 0);
	setAxisMaxMinor( QwtPlot::xBottom, 0);
	rescale();

	//Change canvas background
	setCanvasBackground(Qt::white);

	//Set titles
	//setAxisTitle(QwtPlot::yRight, "Information Integration");

	//Change fonts
	QFont axisFont("Arial", 12, QFont::Normal);
	setAxisFont(QwtPlot::yLeft, axisFont);
	setAxisFont(QwtPlot::yRight, axisFont);
	setAxisFont(QwtPlot::xBottom, axisFont);

	plotLayout()->setAlignCanvasToScales(true);
	replot();

	// LeftButton for the zooming
	// MidButton for the panning
	// RightButton: zoom out by 1
	// Ctrl+RighButton: zoom out to full size

	QwtPlotZoomer* zoomer = new MyZoomer(canvas());
	zoomer->setMousePattern(QwtEventPattern::MouseSelect2, Qt::RightButton, Qt::ControlModifier);
	zoomer->setMousePattern(QwtEventPattern::MouseSelect3, Qt::RightButton);

	QwtPlotPanner *panner = new QwtPlotPanner(canvas());
	panner->setAxisEnabled(QwtPlot::yRight, false);
	panner->setMouseButton(Qt::MidButton);

	// Avoid jumping when labels with more/less digits
	// appear/disappear when scrolling vertically

	const QFontMetrics fm(axisWidget(QwtPlot::yLeft)->font());
	QwtScaleDraw *sd = axisScaleDraw(QwtPlot::yLeft);
	sd->setMinimumExtent( fm.width("100.00") );

	const QColor c(Qt::darkBlue);
	zoomer->setRubberBandPen(c);
	zoomer->setTrackerPen(c);

	// Show the plots
	replot();
}


/*! Destructor */
AnalysisSpectrogram::~AnalysisSpectrogram(){
}


/*----------------------------------------------------------*/
/*-----                 PUBLIC METHODS                 -----*/
/*----------------------------------------------------------*/

/*! Rescales the plot */
void AnalysisSpectrogram::rescale(){
	setAxisScale(QwtPlot::yLeft, d_spectrogram->data().boundingRect().y(),d_spectrogram->data().boundingRect().y() + d_spectrogram->data().boundingRect().height(), 1);
	setAxisScale(QwtPlot::xBottom, d_spectrogram->data().boundingRect().x(),d_spectrogram->data().boundingRect().x() + d_spectrogram->data().boundingRect().width(), 1);

}


/*! Shows the contours of the plot */
void AnalysisSpectrogram::showContour(bool on){
	d_spectrogram->setDisplayMode(QwtPlotSpectrogram::ContourMode, on);
	replot();
}


/*! Makes the spectrogram visible */
void AnalysisSpectrogram::showSpectrogram(bool on){
	d_spectrogram->setDisplayMode(QwtPlotSpectrogram::ImageMode, on);
	d_spectrogram->setDefaultContourPen(on ? QPen() : QPen(Qt::NoPen));
	replot();
}


/*! Prints the plot to a pdf file
	FIXME: THIS PROBABLY DOESN'T WORK. */
void AnalysisSpectrogram::printPlot() {
	QPrinter printer;
	printer.setOrientation(QPrinter::Landscape);
	printer.setOutputFileName("/tmp/spectrogram.pdf");
	QPrintDialog dialog(&printer);
	if ( dialog.exec() ){
		print(printer);
	}
}
