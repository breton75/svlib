#include "sv_userlogon.h"
//#include "ui_userlogon.h"

//#include "sv_pgdb.h"
//#include "sv_log.h"

using namespace sv;

svlog::SvLog ulog;
SvUserLogon* SvUserLogon::p_instance = nullptr;

sv::SvUserLogon::SvUserLogon(QWidget *parent,
                         bool showHost,
                         bool showPort,
                         bool showDBName,
                         bool showUser,
                         bool showPass,
                         bool showLog,
                         bool showRole,
                         QString host,
                         quint16 port,
                         QString dbName,
                         QString user,
                         QString pass,
                         QString role) :
  QDialog(parent),
  ui(new Ui::Ui_Dialog)
{
  ui->ShowDialog(this,
                     showHost, showPort, showDBName, showUser, showPass, showLog,
                     host, port, dbName, user, pass, showRole, role);
  
  ui->connectionErrorCount = 0;

  ulog.assignLog(ui->textEdit);
  
  connect(ui->bnCancel, &QPushButton::pressed, this, &sv::SvUserLogon::reject);
  connect(ui->bnConnect, &QPushButton::pressed, this, &sv::SvUserLogon::on_accept);
  
}

sv::SvUserLogon::~SvUserLogon()
{
  delete ui;
}

void sv::SvUserLogon::on_accept()
{
  ui->bnConnect->setEnabled(false);
  ui->bnCancel->setEnabled(false);
  QApplication::processEvents();

  SvPGDB::instance()->setConnectionParams(ui->editDbName->text(),
                            ui->editHost->text(),
                            ui->editPort->text().toUInt(),
                            ui->editUser->text(),
                            ui->editPass->text(),
                            ui->editRole->text());
  
  ulog << QString("Connecting to database (%1)").arg(connectionErrorCount) << svlog::endl;
  QApplication::processEvents();
  
  QSqlError err = SvPGDB::instance()->connectToDB();
  ulog << err.text() << svlog::endl;
  
  if(err.type() != QSqlError::NoError)
  {
    
    if(++connectionErrorCount == 1)
      ui->showLog(this);
    
    
    if(connectionErrorCount == maxConnectionErrorCount)
      
      reject();
      
    else {
      
      ui->bnCancel->setEnabled(true);
      ui->bnConnect->setEnabled(true);
      QApplication::processEvents();
      
    }
    
  }
  else
  {
    
    accept();
    
  }
}

/** static functions **/
int SvUserLogon::showDbConnectDialog()
{
  bool showHost   = AppParams::readParam(0, "Logon", "showHost", true).toBool();
  bool showPort   = AppParams::readParam(0, "Logon", "showPort", true).toBool();
  bool showDBName = AppParams::readParam(0, "Logon", "showDBName", true).toBool();
  bool showUser   = AppParams::readParam(0, "Logon", "showUser", true).toBool();
  bool showPass   = AppParams::readParam(0, "Logon", "showPass", true).toBool();
  bool showLog    = AppParams::readParam(0, "Logon", "showLog",  false).toBool();
  bool showRole   = AppParams::readParam(0, "Logon", "showRole", true).toBool();
  QString host    = AppParams::readParam(0, "Logon", "host",   "").toString();
  quint16 port    = AppParams::readParam(0, "Logon", "port",   5432).toUInt();
  QString dbName  = AppParams::readParam(0, "Logon", "dbName", "").toString();
  QString user    = AppParams::readParam(0, "Logon", "user",   "").toString();
  QString pass    = AppParams::readParam(0, "Logon", "pass",   "").toString();
  QString role    = AppParams::readParam(0, "Logon", "role",   "").toString();
  
  int result = SvUserLogon::showDbConnectDialog(0, showHost, showPort, showDBName, showUser, showPass, showLog, showRole,
                                     host, port, dbName, user, pass, role);
  
  if(result == QDialog::Accepted) {
    
    AppParams::saveParam(0, "Logon", "showHost",   showHost);
    AppParams::saveParam(0, "Logon", "showPort",   showPort);
    AppParams::saveParam(0, "Logon", "showDBName", showDBName);
    AppParams::saveParam(0, "Logon", "showUser",   showUser);
    AppParams::saveParam(0, "Logon", "showPass",   showPass);
    AppParams::saveParam(0, "Logon", "showLog",    showLog );
    AppParams::saveParam(0, "Logon", "showRole",   showRole);
    
    AppParams::saveParam(0, "Logon", "host",       p_instance->ui->editHost->text());    
    AppParams::saveParam(0, "Logon", "port",       p_instance->ui->editPort->text());   
    AppParams::saveParam(0, "Logon", "dbName",     p_instance->ui->editDbName->text());    
    AppParams::saveParam(0, "Logon", "user",       p_instance->ui->editUser->text());    
    AppParams::saveParam(0, "Logon", "role",       p_instance->ui->editRole->text());    
    
  }
  
  return result;
  
}
    
int SvUserLogon::showDbConnectDialog(QWidget *parent,
                      bool showHost, bool showPort,
                      bool showDBName, bool showUser,
                      bool showPass, bool showLog,
                      bool showRole,
                      QString host, quint16 port, 
                      QString dbName,
                      QString user, QString pass,
                      QString role)
{
  p_instance = new SvUserLogon(parent,
                               showHost, showPort, showDBName, showUser, showPass, showLog, showRole,
                               host, port, dbName, user, pass, role);
  
  return p_instance->exec();
  
}
    
void SvUserLogon::deleteDbConnectDialog()
{
  if(p_instance)
  delete p_instance;
  
  p_instance = nullptr;
  
}
