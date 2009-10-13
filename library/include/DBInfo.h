#ifndef DBINFO_H
#define DBINFO_H

//Qt includes
#include <QString>

namespace spikestream {

    class DBInfo {
	public:
	    DBInfo(QString host, QString user, QString password, QString database);
	    DBInfo();
	    DBInfo(const DBInfo& dbInfo);
	    ~DBInfo();
	    DBInfo& operator=(const DBInfo &rhs);
	    QString toString();

	    QString getHost() const {return host;}
	    QString getUser() const {return user;}
	    QString getPassword() const {return password;}
	    QString getDatabase() const {return database;}

	private:
	    QString host;
	    QString user;
	    QString password;
	    QString database;

    };

}

#endif//DBINFO_H


