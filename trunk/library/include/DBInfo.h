#ifndef DBINFO_H
#define DBINFO_H

//Qt includes
#include <QString>

namespace spikestream {

	/*! Holds information about a database.  */
    class DBInfo {
		public:
			DBInfo(QString host, QString user, QString password, QString database);
			DBInfo();
			DBInfo(const DBInfo& dbInfo);
			~DBInfo();
			DBInfo& operator=(const DBInfo &rhs);
			QString toString() const;

			QString getHost() const {return host;}
			QString getUser() const {return user;}
			QString getPassword() const {return password;}
			QString getDatabase() const {return database;}

			void setHost(const QString& host) { this->host = host;}
			void setUser(const QString& user) { this->user = user;}
			void setPassword(const QString& password) { this->password = password;}
			void setDatabase(const QString& database) { this->database = database; }

			static QString UNDEFINED;

		private:
			QString host;
			QString user;
			QString password;
			QString database;

	};

}

#endif//DBINFO_H


