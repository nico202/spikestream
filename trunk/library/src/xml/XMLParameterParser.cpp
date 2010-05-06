//SpikeStream includes
#include "XMLParameterParser.h"
#include "SpikeStreamXMLException.h"
using namespace spikestream;

//Qt includes
#include <QDebug>


/*! Constructor */
XMLParameterParser::XMLParameterParser(){
}


/*! Destructor */
XMLParameterParser::~XMLParameterParser(){
}


/*----------------------------------------------------------*/
/*-----                  PUBLIC METHODS                -----*/
/*----------------------------------------------------------*/

/*! Parses the XML string and returns a map of parameters */
QHash<QString, double> XMLParameterParser::getParameterMap(const QString& xmlString){
    //Clear parameter map
    parameterMap.clear();

    //Return empty map if paramter string is empty
    if(xmlString == "")
		return parameterMap;

    //Parse the XML
    QXmlSimpleReader xmlReader;
    QXmlInputSource xmlInput;
    xmlReader.setContentHandler(this);
    xmlReader.setErrorHandler(this);
    xmlInput.setData(xmlString);
    xmlReader.parse(xmlInput);

    return parameterMap;
}


/*----------------------------------------------------------*/
/*-----               PROTECTED METHODS                -----*/
/*----------------------------------------------------------*/

/*! Called when parser encounters characters. */
bool XMLParameterParser::characters(const QString& chars){
    if(loadingParameter){
		if(currentElement == "name"){
			currentParamName = chars;
		}
		else if(currentElement == "value"){
			bool ok;
			currentParamValue = chars.toDouble(&ok);
			if(!ok)
				throw SpikeStreamXMLException("Error converting parameter value to double: " + chars);
		}
		else{
			throw SpikeStreamXMLException("Unrecognized element: " + currentElement);
		}
    }
    else
		throw SpikeStreamXMLException("Unexpected characters encountered: '" + chars + "'");
    return true;
}


/*! Called when the parser encounters the end of an element. */
bool XMLParameterParser::endElement( const QString&, const QString&, const QString& elemName){
    if(loadingParameter && elemName == "parameter"){
		//Check that parameter name has been set
		if(currentParamName == "")
			throw SpikeStreamXMLException("Parameter is missing description");

		//Store parameter in map
		parameterMap[currentParamName] = currentParamValue;

		//Have finished loading the parameter
		loadingParameter = false;
    }
    return true;
}


/*! Called when the parser generates an error. */
bool XMLParameterParser::error (const QXmlParseException& parseEx){
    throw SpikeStreamXMLException(parseEx.message());
    return true;
}


/*! Returns a default error string. */
QString XMLParameterParser::errorString (){
	return QString("Default error string");

}


/*! Called when the parser generates a fatal error. */
bool XMLParameterParser::fatalError (const QXmlParseException& parseEx){
    throw SpikeStreamXMLException(parseEx.message());
    return true;
}


/*! Called when parser reaches the start of the document. */
bool XMLParameterParser::startDocument(){
    loadingParameter = false;
    return true;
}


/*! Called when parser reaches the start of an element. */
bool XMLParameterParser::startElement(const QString&, const QString&, const QString& qName, const QXmlAttributes&){
    currentElement = qName;
    if(currentElement == "parameter"){
		//Reset information associated with a parameter
		currentParamName = "";
		currentParamValue = 0.0;
		loadingParameter = true;
    }
    return true;
}


/*! Called when the parser generates a warning. */
bool XMLParameterParser::warning ( const QXmlParseException& ex){
    qWarning()<<ex.message();
    return true;
}



