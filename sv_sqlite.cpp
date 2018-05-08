#include "sv_sqlite.h"
//#include "log.h"

SvSQLITE *SQLITE;
//QMutex mutex;

//int CURRENT_USER_ID = -1;
//struct_user_info USER_INFO;

//using namespace log_ns;
//using namespace sql_create_defs;
//using namespace sql_insert_defs;

SvSQLITE::SvSQLITE(QObject *parent, QString fileName) :
  QObject(parent)
{
  this->setParent(parent);
  _db_name = fileName;
}

SvSQLITE::SvSQLITE(SvSQLITE* sqlite, QObject *parent) :
  QObject(parent)
{
  setParent(parent);
  
  if(sqlite)
  {
    _db_name = sqlite->currentDBName();
    _host_name = sqlite->currentHostName();
    _password = sqlite->currentPassword();
    _port = sqlite->currentPort();
    _user_name = sqlite->currentUserName();
  }
}

QSqlError SvSQLITE::connectToDB(QString dbName,
                              QString host,
                              quint16 port,
                              QString userName,
                              QString pass)
{
  try
  {
    db = QSqlDatabase::addDatabase("QSQLITE", "sqlitedb");
    db.setDatabaseName(_db_name);
    
    /* ------- ловим ошибки драйвера ---------*/
    if(!this->db.isValid())
      QSqlDatabase::removeDatabase("sqlitedb");

    /* ------- назначаем параметры подключения ---------*/
//    if(dbName != "") _db_name = dbName; 
//    if(host != "") _host_name = host;
//    if(port) _port = port;
//    if(userName != "") _user_name = userName;
//    if(pass != "") _password = pass;
    
//    this->db.setDatabaseName(this->p_db_name);
//    this->db.setHostName(this->p_host_name);
//    this->db.setPort(this->p_port);
    
//    this->db.setUserName(this->p_user_name);
//    this->db.setPassword(this->p_password);
    
    /* ---------- подключаемся -------------*/
    else
      db.open();
    
    return db.lastError();
    
  }
  catch(...)
  {
//    log(m_Err, "Error in SvPGDB");
  }
}

QSqlError SvSQLITE::connectToDB()
{
  QSqlError result;
  
  try
  {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(_db_name);
    
    /* ------- ловим ошибки драйвера ---------*/
    if(!db.isValid()) _exception.raise(db.lastError().databaseText());
//      QSqlDatabase::removeDatabase("sqlitedb");

    
//    this->db.setDatabaseName(this->p_db_name);
//    this->db.setHostName(this->p_host_name);
//    this->db.setPort(this->p_port);
    
//    this->db.setUserName(this->p_user_name);
//    this->db.setPassword(this->p_password);
    
    /* ---------- подключаемся -------------*/
    else
      db.open();
    
    return db.lastError();
    
  }
  catch(SvException &exception)
  {
    result.setType(QSqlError::ConnectionError);
    result.setDatabaseText(exception.err);
    return result;
  }
}

SvSQLITE::~SvSQLITE()
{
  disconnectFromDb();
//  QSqlError err = this->disconnectFromDb();
//  if(err.type() != QSqlError::NoError)
//    log_ns::log_SQL_error(err);
  
  this->deleteLater();
  
}

void SvSQLITE::setConnectionParams(QString dbName,
                                 QString host,
                                 quint16 port,
                                 QString userName,
                                 QString pass)
{ 
//  if(dbName != "") _db_name = dbName; 
//  if(host != "") _host_name = host;
//  if(port) _port = port;
//  if(userName != "") p_user_name = userName;
//  if(pass != "") p_password = pass;
}

QSqlError SvSQLITE::execSQL(QString queryText)
{
  QSqlError err;
  if(queryText == "")
  {  
    err.setType(QSqlError::UnknownError);
    err.setDatabaseText("Query is empty");
    return err;
  }
  
  _mutex.lock();
  QSqlQuery query = db.exec(queryText);

  err = query.lastError();
  query.finish();
  _mutex.unlock();
  
  return err;  

}

QSqlError SvSQLITE::execSQL(QString queryText, QSqlQuery* query)
{
  QSqlError err;
  if(queryText == "")
  {  
    err.setType(QSqlError::UnknownError);
    err.setDatabaseText("Query is empty");
    return err;
  }
  
  _mutex.lock();
  *query = db.exec(queryText);
  _mutex.unlock();
  
  return query->lastError();
  
}


QSqlError SvSQLITE::disconnectFromDb()
{
  db.close();
  return db.lastError();
}

