#ifndef SPIKESTREAMEXCEPTION_H
#define SPIKESTREAMEXCEPTION_H

#include <QString>

class SpikeStreamException {
    public:
        SpikeStreamException(QString details = QString("No details available"));
        virtual ~SpikeStreamException();
        QString getMessage();

    private:
        QString type;
        QString details;
};

#endif // SPIKESTREAMEXCEPTION_H
