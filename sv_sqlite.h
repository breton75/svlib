/* класс только для работы с базами данных SQLite */

#ifndef SV_SQLITE_H
#define SV_SQLITE_H

#include <QObject>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlQueryModel>
#include <QThread>
#include <QDebug>
#include <QMutex>
#include <QException>

#include "sv_exception.h"

struct struct_user_info
{
  int id;
  int employee_id;
  
  bool allowAdd;
  bool allowEdit;
  bool allowDelete;
  bool allowReports;
  
  bool viewDevices;
  bool viewEmployees;
  bool viewUsers;
};

class SvSQLITE : public QObject // QThread //
{
    Q_OBJECT
    
//    void run() Q_DECL_OVERRIDE;
    
  public:
    SvSQLITE(QObject *parent = 0, QString fileName = "");
    SvSQLITE(SvSQLITE* sqlite, QObject *parent = 0);
    ~SvSQLITE();
    
    QSqlDatabase db;
    bool connected;
    
    QSqlError connectToDB(QString dbName,
                          QString host,
                          quint16 port,
                          QString userName,
                          QString pass);
    
    QSqlError connectToDB();
    
    QSqlError disconnectFromDb();
    
//    void lock() { _mutex.lock(); }
//    void unlock() { _mutex.unlock(); }
    
    bool transaction() { return db.transaction(); }
    bool rollback() { return db.rollback(); }
    bool commit() { return db.commit(); }
    
    void setConnectionParams(QString dbName = "",
                             QString host = "",
                             quint16 port = 0,
                             QString userName = "",
                             QString pass = "");
    
    QString currentDBName()   { return _db_name; }
    QString currentHostName() { return _host_name; }
    quint16 currentPort()     { return _port; }
    QString currentUserName() { return _user_name; }
    QString currentPassword() { return _password; }
    
  private:
    QString _db_name = "";
    QString _host_name = "";
    quint16 _port = 0;
    QString _user_name = "";
    QString _password = "";
    
    QMutex _mutex;
    
    SvException _exception;
  
  signals:
    
  public slots:
    QSqlError execSQL(QString queryText);
    QSqlError execSQL(QString queryText, QSqlQuery *query);
//    QSqlError execSQL(QString queryText, QByteArray *data1, QByteArray *data2 = 0, QByteArray *data3 = 0, QSqlQuery* query = 0, bool doLog = true);
//    QSqlError execSQL(QString queryText, QByteArray *data1, QByteArray *data2 = 0, QByteArray *data3 = 0, QSqlQuery* query, bool doLog = true);
//    QSqlError execSQL(QString queryText, QSqlQueryModel *model, bool doLog = true);
    
};

#endif // SV_SQLITE_H
