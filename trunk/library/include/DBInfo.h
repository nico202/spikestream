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

	    QString getHost() {return host;}
	    QString getUser() {return user;}
	    QString getPassword() {return password;}
	    QString getDatabase() {return database;}

	private:
	    QString host;
	    QString user;
	    QString password;
	    QString database;

    };

}

#endif//DBINFO_H


