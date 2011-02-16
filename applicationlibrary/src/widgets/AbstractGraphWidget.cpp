//SpikeStream includes
#include "Globals.h"
#include "AbstractGraphWidget.h"
#include "SpikeStreamException.h"
#include "Util.h"
using namespace spikestream;

//Qt includes
#include <QFileDialog>
#include <QMessageBox>
#include <QPainter>
#include <QResizeEvent>
#include <QDebug>


/*! Constructor */
AbstractGraphWidget::AbstractGraphWidget(QWidget* parent) : QWidget(parent){
	//Initialize with empty default values
	numTimeSteps = 1000;
	minTimeStep = 0;
	setYRange(-10, 10);
	xAxisTickLength = 2;
	yAxisTickLength = 2;
	axesLabelFontSize = 10;
	labelFontSize = 10;
	backgroundColor = qRgb(255,255,255);
	axesColor = qRgb(0,0,0);
	bufferImage = NULL;

	//Build default buffer image
	buildBufferImage();

	//Painting attributes
	setAttribute(Qt::WA_OpaquePaintEvent);
	setAttribute(Qt::WA_PaintOnScreen);
}


/*! Destructor */
AbstractGraphWidget::~AbstractGraphWidget(){
	if(bufferImage != NULL)
		delete bufferImage;
}


/*----------------------------------------------------------*/
/*------               PUBLIC METHODS                 ------*/
/*----------------------------------------------------------*/

/*! Checks and sets y range */
void AbstractGraphWidget::setYRange(int minY, int maxY){
	if(minY >= maxY)
		throw SpikeStreamException("MinY must be less than maxY.");
	this->minY = minY;
	this->maxY = maxY;
}



/*----------------------------------------------------------*/
/*------              PROTECTED METHODS               ------*/
/*----------------------------------------------------------*/

/*! Builds the buffer image.
	Should be called after changing the parameters. */
void AbstractGraphWidget::buildBufferImage(){
	if(bufferImage != NULL)
		delete bufferImage;

	yAxisPadding = yAxisTickLength + 10 + axesLabelFontSize * QString::number(maxY-minY).length();
	xAxisPadding = xAxisTickLength + 1 + axesLabelFontSize;
	imageWidth = numTimeSteps + yAxisPadding + 1;
	imageHeight = maxY - minY;
	bufferImage = new QImage(imageWidth, imageHeight, QImage::Format_RGB32);
	bufferImage->fill(backgroundColor);
	updateAxes = true;

	//Set up widget graphic properties
	if(imageWidth > 500)
		setMinimumWidth(500);
	else
		setMinimumWidth(imageWidth);
	if(imageHeight > 500)
		setMinimumHeight(500);
	else
		setMinimumHeight(imageHeight);

	widgetWidth = this->size().width();
	widgetHeight = this->size().height();
}


/*! Prompts user to save image as a file. */
void AbstractGraphWidget::mouseDoubleClickEvent (QMouseEvent*){
	QString fileType = "BMP";
	QString filePath = getFilePath("*." + fileType.toLower());

	//Fix extension
	if(!filePath.endsWith(fileType.toLower()))
		filePath += "." + fileType.toLower();

	//If file exists, check to see if user wants to overwrite file
	if(QFile::exists(filePath)){
		int response = QMessageBox::warning(this, "Overwrite File?", filePath + " already exists.\nAre you sure that you want to overwrite it?", QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);
		if(response != QMessageBox::Ok)
			return;
	}

	//Save file
	bufferImage->save(filePath, fileType.toAscii(), 100);
}


//Inherited from QWidget
void AbstractGraphWidget::paintEvent(QPaintEvent*){
	QPainter painter(this);
	if(bufferImage != NULL){
		QPainter imagePainter(bufferImage);
		if(updateAxes){
			paintAxes(imagePainter);
			paintLabels(imagePainter);
		}
		paintData(imagePainter);
		painter.drawImage(0, 0, bufferImage->scaled(widgetWidth, widgetHeight));
	}
	painter.end();
}


//Inherited from QWidget
void AbstractGraphWidget::resizeEvent(QResizeEvent*) {
	widgetWidth = this->size().width();
	widgetHeight = this->size().height();
}


/*----------------------------------------------------------*/
/*------              PRIVATE METHODS                 ------*/
/*----------------------------------------------------------*/

/*! Enables user to enter a file path */
QString AbstractGraphWidget::getFilePath(QString fileFilter){
	QFileDialog dialog(this);
	dialog.setDirectory(Globals::getWorkingDirectory());
	dialog.setFileMode(QFileDialog::AnyFile);
	dialog.setNameFilter( QString("Image file (" + fileFilter + ")") );
	dialog.setWindowTitle("Save");
	dialog.setViewMode(QFileDialog::Detail);
	QStringList fileNames;
	if (dialog.exec())
		fileNames = dialog.selectedFiles();
	if(fileNames.size() > 0)
		return fileNames[0];
	else
		return QString("");
}


/*! Advances the time step and redraws axes if necessary */
void AbstractGraphWidget::increaseTimeStep(int currentTimeStep){
	//Check to see if we have moved out of the X axis range
	if( ((currentTimeStep /numTimeSteps) * numTimeSteps) != minTimeStep){
		minTimeStep = (currentTimeStep/numTimeSteps) * numTimeSteps;
		bufferImage->fill(backgroundColor);
		updateAxes = true;
	}
}


/*! Paints the X and Y axes */
void AbstractGraphWidget::paintAxes(QPainter& painter) {
	//Set up drawing attributes
	painter.setPen( QPen(axesColor) );
	QFont font;
	font.setFamily("Helvetica");
	font.setWeight(QFont::Light);
	font.setPixelSize(axesLabelFontSize);
	painter.setFont(font);

	//Paint axes
	paintYAxis(painter);
	paintXAxis(painter);

	//Set flag recording that axes are updated
	updateAxes = false;
}


/*! Paints the X axis and its ticks */
void AbstractGraphWidget::paintXAxis(QPainter& painter) {
	//Draw X axis line
	if(minY < 0 && Util::toPositive(minY) > xAxisPadding)
		painter.drawLine(0, imageHeight + minY, imageWidth, imageHeight + minY);
	else
		painter.drawLine(0, imageHeight - xAxisPadding, imageWidth, imageHeight - xAxisPadding);

	//Draw ticks
	for(int i=0; i<numTimeSteps; i += numTimeSteps/10)
		paintXAxisTick(painter, i + yAxisPadding, minTimeStep + i);
}


/*! Paints a single tick on the X axis */
void AbstractGraphWidget::paintXAxisTick(QPainter& painter, int xPos, int labelX) {
	int xAxisLocation;
	if(minY < 0 && Util::toPositive(minY) > xAxisPadding)
		xAxisLocation = imageHeight + minY;
	else
		xAxisLocation = imageHeight - xAxisPadding;

	painter.drawLine(xPos,
					 xAxisLocation,
					 xPos,
					 xAxisLocation + xAxisTickLength
	);
	int textW = axesLabelFontSize*5;
	painter.drawText(
			xPos - textW/2, xAxisLocation + xAxisTickLength,
			textW, axesLabelFontSize,
			Qt::AlignHCenter,
			QString::number(labelX) + "ms"
	);
}


/*! Paints a single tick on the Y axis */
void AbstractGraphWidget::paintYAxisTick(QPainter& painter, int yPos, int label) {
	painter.drawLine(yAxisPadding, yPos, yAxisPadding - yAxisTickLength, yPos);
	painter.drawText(
			0, yPos + axesLabelFontSize/2,
			QString::number(label)
	);
}


/*! Paints the Y axis */
void AbstractGraphWidget::paintYAxis(QPainter& painter){
	//Draw Y axis line
	painter.drawLine(yAxisPadding, 0, yAxisPadding, imageHeight);

	//Draw ticks
	for(int i=0; i< maxY-minY; i += (maxY - minY)/10){
		paintYAxisTick(painter, imageHeight - i, i+minY);
	}
}


