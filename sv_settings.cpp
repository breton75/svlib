#include "sv_settings.h"
#include <QVariant>

AppParams::WindowParams AppParams::readWindowParams(QObject* parent, QString group_name, QString file_name)
{
  // читаем размер и положение окна
  QString fname = file_name;
  if(fname.isEmpty())
    fname = qApp->applicationDirPath() + "/" + qApp->applicationName() + ".ini";
  
  WindowParams result;
  
  SvSettings *sett = new SvSettings(fname, parent);
  result.size = sett->readValue(group_name, "Size", QSize(800, 600)).toSize();
  result.position = sett->readValue(group_name, "Position", QPoint(100, 100)).toPoint();
  result.state = Qt::WindowState(sett->readValue(group_name, "WindowState", Qt::WindowNoState).toInt());
  sett->~SvSettings();
  
  return result;
}

void AppParams::saveWindowParams(QObject* parent, QSize size, QPoint position, int state, QString group_name, QString file_name)
{
  // сохраняем параметры окна
  QString fname = file_name;
  if(fname.isEmpty())
    fname = qApp->applicationDirPath() + "/" + qApp->applicationName() + ".ini";

  SvSettings *sett = new SvSettings(fname, parent);
  sett->writeValue(group_name, "WindowState", state);
  if(state == Qt::WindowNoState)
  {
    sett->writeValue(group_name, "Size", size);
    sett->writeValue(group_name, "Position", position);
//    sett->writeValue(group_name, "WindowState", state);
  }
  sett->~SvSettings();

}

QVariant AppParams::readParam(QObject* parent, QString group_name, QString param_name, QVariant default_value, QString file_name)
{
  QString fname = file_name;
  if(fname.isEmpty())
    fname = qApp->applicationDirPath() + "/" + qApp->applicationName() + ".ini";
  
  QVariant result;
  
  SvSettings *sett = new SvSettings(fname, parent);
  result.setValue(sett->readValue(group_name, param_name, default_value));
  sett->~SvSettings();
  
  return result;
}

void AppParams::saveParam(QObject* parent, QString group_name, QString param_name, QVariant value, QString file_name)
{
  // сохраняем параметры окна
  QString fname = file_name;
  if(fname.isEmpty())
    fname = qApp->applicationDirPath() + "/" + qApp->applicationName() + ".ini";

  SvSettings *sett = new SvSettings(fname, parent);
  sett->writeValue(group_name, param_name, value);
  sett->~SvSettings();

}



SvSettings::SvSettings(QString fileName,
                       QObject *parent,
                       QSettings::Format format,
                       QString organizationName,
                       QString organizationDomain,
                       QString setAppName) :
  QObject(parent)
{
  if (fileName != "")
  {
    this->appSettings = new QSettings(fileName, format, parent);
  }
  else
  {
    this->appSettings = new QSettings();
  }

  this->appSettings->setIniCodec(QTextCodec::codecForName("UTF-8")); /** !!! **/
  
}

SvSettings::~SvSettings()
{
  this->deleteLater();
}

void SvSettings::writeValue(QString GroupName, QString ValueName, QVariant Value)
{
  this->appSettings->beginGroup(GroupName);
  this->appSettings->setValue(ValueName, Value);
  this->appSettings->endGroup();
}

void SvSettings::writeValue(QString GroupName, QString ValueName, int Value)
{
  this->appSettings->beginGroup(GroupName);
  this->appSettings->setValue(ValueName, Value);
  this->appSettings->endGroup();
}

void SvSettings::writeValue(QString GroupName, QString ValueName, QString Value)
{
  this->appSettings->beginGroup(GroupName);
  this->appSettings->setValue(ValueName, Value);
  this->appSettings->endGroup(); 
}

void SvSettings::writeValue(QString GroupName, QString ValueName, QSize Value)
{
  this->appSettings->beginGroup(GroupName);
  this->appSettings->setValue(ValueName, Value);
  this->appSettings->endGroup(); 
}

void SvSettings::writeValue(QString GroupName, QString ValueName, QPoint Value)
{
  this->appSettings->beginGroup(GroupName);
  this->appSettings->setValue(ValueName, Value);
  this->appSettings->endGroup(); 
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
  this->appSettings->beginGroup(GroupName);
  v = this->appSettings->value(ValueName, DefaultValue);
  this->appSettings->endGroup(); 
  return v;
}
