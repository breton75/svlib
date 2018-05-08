#include "sv_settings.h"


AppParams::WindowParams AppParams::readWindowParams(QObject* parent, QString group_name, QString file_name)
{
  // читаем размер и положение окна
  QString fname = AppParams::checkFileName(file_name);

  WindowParams result;
  
  SvSettings sett(fname, parent);
  result.size = sett.readValue(group_name, "Size", QSize(800, 600)).toSize();
  result.position = sett.readValue(group_name, "Position", QPoint(100, 100)).toPoint();
  result.state = Qt::WindowState(sett.readValue(group_name, "WindowState", Qt::WindowNoState).toInt());
  
  return result;
}

void AppParams::saveWindowParams(QObject* parent, QSize size, QPoint position, int state, QString group_name, QString file_name)
{
  QString fname = AppParams::checkFileName(file_name);

  SvSettings sett(fname, parent);
  sett.writeValue(group_name, "WindowState", state);
  
  if(state == Qt::WindowNoState) {
    
    sett.writeValue(group_name, "Size", size);
    sett.writeValue(group_name, "Position", position);
    
  }
}

QVariant AppParams::readParam(QObject* parent, QString group_name, QString param_name, QVariant default_value, QString file_name)
{
  QString fname = AppParams::checkFileName(file_name);

  QVariant result;
  
  SvSettings sett(fname, parent);
  result.setValue(sett.readValue(group_name, param_name, default_value));
  
  return result;
}

void AppParams::saveParam(QObject* parent, QString group_name, QString param_name, QVariant value, QString file_name)
{
  QString fname = AppParams::checkFileName(file_name);

  SvSettings sett(fname, parent);
  sett.writeValue(group_name, param_name, value);

}

QString AppParams::checkFileName(QString fname)
{
  QString result = fname;
  
  if(result.isEmpty())
    result = qApp->applicationDirPath() + '/' + qApp->applicationName() + ".ini";
//    result = QDir::toNativeSeparators(qApp->applicationDirPath()) + QDir::separator() + qApp->applicationName() + ".ini";
  
  return result;
}


SvSettings::SvSettings(QString fileName,
                       QObject *parent,
                       QSettings::Format format,
                       QString organizationName,
                       QString organizationDomain,
                       QString setAppName) :
  QObject(parent)
{
  if (!fileName.isEmpty())
    appSettings = new QSettings(fileName, format, parent);
  
  else
    appSettings = new QSettings();

  appSettings->setIniCodec(QTextCodec::codecForName("UTF-8")); /** !!! **/
  
}

SvSettings::~SvSettings()
{
  deleteLater();
}

void SvSettings::writeValue(QString GroupName, QString ValueName, QVariant Value)
{
  appSettings->beginGroup(GroupName);
  appSettings->setValue(ValueName, Value);
  appSettings->endGroup();
}

void SvSettings::writeValue(QString GroupName, QString ValueName, int Value)
{
  appSettings->beginGroup(GroupName);
  appSettings->setValue(ValueName, Value);
  appSettings->endGroup();
}

void SvSettings::writeValue(QString GroupName, QString ValueName, QString Value)
{
  appSettings->beginGroup(GroupName);
  appSettings->setValue(ValueName, Value);
  appSettings->endGroup(); 
}

void SvSettings::writeValue(QString GroupName, QString ValueName, QSize Value)
{
  this->appSettings->beginGroup(GroupName);
  this->appSettings->setValue(ValueName, Value);
  this->appSettings->endGroup(); 
}

void SvSettings::writeValue(QString GroupName, QString ValueName, QPoint Value)
{
  appSettings->beginGroup(GroupName);
  appSettings->setValue(ValueName, Value);
  appSettings->endGroup(); 
}

//int SvSettings::readValue(QString GroupName, QString ValueName)
//{
//  int i;
//  this->appSettings.beginGroup(GroupName);
//  i = this->appSettings.value(ValueName, Value);
//  this->appSettings.endGroup(); 
//  return i;
//}

QVariant SvSettings::readValue(QString GroupName, QString ValueName, QVariant DefaultValue)
{
  QVariant v;
  appSettings->beginGroup(GroupName);
  v = appSettings->value(ValueName, DefaultValue);
  appSettings->endGroup(); 
  return v;
}
