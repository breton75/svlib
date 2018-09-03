#include "sv_settings.h"

QString AppParams::saveLayout(QMainWindow *mainWindow)
{
  SvException except;
  QFile file;
  QString result = "";
  
  try {
    
    QFileInfo fi = QFileInfo(getFileInfoFromApp("layout"));
    file.setFileName(fi.absoluteFilePath());
    
    if (!file.open(QFile::WriteOnly)) 
      except.raise(QString("Ошибка при открытии файла %1: %2").arg(fi.fileName()).arg(file.errorString()));
    
    QByteArray geometry_data = mainWindow->saveGeometry();
    QByteArray layout_data = mainWindow->saveState();
    
    if(!file.putChar((uchar)geometry_data.size()))
      except.raise(QString("Ошибка записи в файл %1: %2").arg(fi.fileName()).arg(file.errorString()));
      
    if(file.write(geometry_data) != geometry_data.size())
      except.raise(QString("Ошибка записи в файл %1: %2").arg(fi.fileName()).arg(file.errorString()));
    
    if(file.write(layout_data) != layout_data.size())
      except.raise(QString("Ошибка записи в файл %1: %2").arg(fi.fileName()).arg(file.errorString()));
    
    file.close();
    
  }
  
  catch(SvException& e) {
    
    if(file.isOpen())
      file.close();
    
    result = e.error;
    
  }
  
  return result;
}

QString AppParams::saveLayoutWidget(QWidget *widget) 
{
  SvException except;
  QFile file;
  QString result = "";
  
  try {
    
    QFileInfo fi = QFileInfo(QString("%1.layout").arg(widget->accessibleName()));
    file.setFileName(fi.absoluteFilePath());
    
    if (!file.open(QFile::WriteOnly)) 
      except.raise(QString("Ошибка при открытии файла %1: %2").arg(fi.fileName()).arg(file.errorString()));
    
    QByteArray geometry_data = widget->saveGeometry();
    
    if(!file.putChar((uchar)geometry_data.size()))
      except.raise(QString("Ошибка записи в файл %1: %2").arg(fi.fileName()).arg(file.errorString()));
      
    if(file.write(geometry_data) != geometry_data.size())
      except.raise(QString("Ошибка записи в файл %1: %2").arg(fi.fileName()).arg(file.errorString()));
    
    file.close();
    
  }
  
  catch(SvException& e) {
    
    if(file.isOpen())
      file.close();
    
    result = e.error;
    
  }
  
  return result;
}

QString AppParams::loadLayout(QMainWindow *mainWindow)
{
  SvException except;
  QFile file;
  QString result = "";
  
  try {
    
    QFileInfo fi = QFileInfo(getFileInfoFromApp("layout"));
    file.setFileName(fi.absoluteFilePath());
    
    if(!fi.exists())
      except.raise(QString("Файл %1 не найден. Будет создан новый.").arg(fi.fileName()));
    
    if (!file.open(QFile::ReadOnly)) 
      except.raise(QString("Ошибка при открытии файла %1: %2").arg(fi.fileName()).arg(file.errorString()));
    
    uchar geometry_size;
    QByteArray geometry_data;
    QByteArray layout_data;
 
    if(!file.getChar((char*)&geometry_size))
      except.raise(QString("Ошибка чтения из файла %1: %2").arg(fi.fileName()).arg(file.errorString()));
      
    geometry_data = file.read(geometry_size);
    
    if(geometry_data.size() != geometry_size)
      except.raise(QString("Неверный размер данных в файле %1. Ожидалось %2, прочитано %3")
                   .arg(fi.fileName())
                   .arg(geometry_size)
                   .arg(geometry_data.size()));

    
    layout_data = file.readAll();
    if(layout_data.size() <= 0)
      except.raise(QString("Неверный размер данных в файле %1 (layout_data = 0)")
                   .arg(fi.fileName()));
    
    mainWindow->restoreGeometry(geometry_data);
    mainWindow->restoreState(layout_data);
    
    file.close();
    
  }
  
  catch(SvException& e) {
    
    if(file.isOpen())
      file.close();
    
    result = e.error;
    
  }
  
  return result;
}

QString AppParams::loadLayoutWidget(QWidget *widget)
{
  SvException except;
  QFile file;
  QString result = "";
  
  try {
    
    QFileInfo fi = QFileInfo(QString("%1.layout").arg(widget->accessibleName()));
    file.setFileName(fi.absoluteFilePath());
    
    if(!fi.exists())
      except.raise(QString("Файл %1 не найден. Будет создан новый.").arg(fi.fileName()));
    
    if (!file.open(QFile::ReadOnly)) 
      except.raise(QString("Ошибка при открытии файла %1: %2").arg(fi.fileName()).arg(file.errorString()));
    
    uchar geometry_size;
    QByteArray geometry_data;
 
    if(!file.getChar((char*)&geometry_size))
      except.raise(QString("Ошибка чтения из файла %1: %2").arg(fi.fileName()).arg(file.errorString()));
      
    geometry_data = file.read(geometry_size);
    
    if(geometry_data.size() != geometry_size)
      except.raise(QString("Неверный размер данных в файле %1. Ожидалось %2, прочитано %3")
                   .arg(fi.fileName())
                   .arg(geometry_size)
                   .arg(geometry_data.size()));
   
    widget->restoreGeometry(geometry_data);
    
    file.close();
    
  }
  
  catch(SvException& e) {
    
    if(file.isOpen())
      file.close();
    
    result = e.error;
    
  }
  
  return result;
}

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

QFileInfo AppParams::getFileInfoFromApp(const QString& extention)
{
  QFileInfo result(QString("%1/%2.%3")
                   .arg(qApp->applicationDirPath())
                   .arg(qApp->applicationName())
                   .arg(extention));
  
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
