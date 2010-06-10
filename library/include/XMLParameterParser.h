#ifndef XMLPARAMETERPARSER_H
#define XMLPARAMETERPARSER_H

//Qt includes
#include <qxml.h>
#include <QHash>
#include <QString>

namespace spikestream {

	/*! Extracts parameters stored in double format from an XML string. */
	class XMLParameterParser : public QXmlDefaultHandler {
		public:
			XMLParameterParser();
			~XMLParameterParser();
			QHash<QString, double> getParameterMap(const QString& xmlString);

		protected:
			//Parsing methods inherited from QXmlDefaultHandler
			bool characters ( const QString & ch );
			bool endElement( const QString&, const QString&, const QString& );
			bool startDocument();
			bool startElement( const QString&, const QString&, const QString& , const QXmlAttributes& );

			//Error handling methods inherited from QXmlDefaultHandler
			bool error ( const QXmlParseException & exception );
			QString errorString();
			bool fatalError ( const QXmlParseException & exception );
			bool warning ( const QXmlParseException & exception );


		private:
			/*! Stores the current element. */
			QString currentElement;

			/*! Stores the current parameter name */
			QString currentParamName;

			/*! The current parameter value */
			double currentParamValue;

			/*! Have passed a <parameter> opening tag */
			bool loadingParameter;

			/*! Map holding parameters */
			QHash<QString, double> parameterMap;

		};

}

#endif//XMLPARAMETERPARSER_H

