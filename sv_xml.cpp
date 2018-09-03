#include "sv_xml.h"

SvXML::SvXML(QObject *parent) : QObject(parent)
{
  
}

SvXML::~SvXML()
{
  
}

bool SvXML::open(const QString fileName)
{
  try {
    
    _file = new QFile(fileName);  
    
    if (!_file->open(QIODevice::ReadOnly | QIODevice::Text)) _e.raise(_file->errorString());
          
    _xml.setDevice(_file);
    _current_line = 0;
    
    return true;
    
  }
  
  catch(SvException& e) {
    
    _last_error = e.error;
    
    delete _file;
    return false;
    
  }
}

bool SvXML::readTable(QString tableName)
{
  try {
    
    _column_names.clear();
    
    for(Row r: _table)
      r.clear();
    
    _table.clear();
      
    _current_line = 0;
    
    int k = 0;
   
    while(_xml.readNext() != QXmlStreamReader::Invalid) {
      
      
      bool b = _xml.isStartElement() && _xml.name() == "table";
      k++;
      if(k > 13) break;
      
      if(!b) {
        qDebug() << k << _xml.name() << b;
        continue;
      }
//      else
//        break;
      
      
      while (_xml.readNext()) {
          
        if(_xml.isStartElement() && _xml.name() == "name")
          break;
       
      }
      
      if(_xml.text() == tableName)
        break;
      
    }
    
    if(_xml.atEnd() || _xml.hasError() || _xml.name() != "name")
      _e.raise("1Неверный формат файла конфигурации");
    
    qDebug() << 200 << _xml.name() << _xml.text();
    
    return false;
//      qDebug() << _xml.name() << _xml.attributes().value("name").toString();
//      if(k > 10)
//        break;
//      k++;
//    }
//    if(k > 10) return false;
      
    while(_xml.readNextStartElement()) {
      qDebug() << 300 << _xml.name();
      if(_xml.name() == "columns") break;
    }
      
    if(_xml.atEnd() || _xml.hasError() || _xml.name() != "columns")
      _e.raise("2Неверный формат файла конфигурации");
    
    while(_xml.readNext()) {
      
      if(_xml.isStartElement() && _xml.name() == "column") {
       
        while(_xml.readNext())
          if(_xml.isStartElement() && _xml.name() == "name") break;
        
        _column_names << _xml.text().toString();
        qDebug() << _xml.text();        
      }
      else if(_xml.isEndElement() && _xml.name() == "columns")
        break;
  
    }
    
    if(_xml.atEnd() || _xml.hasError() || _column_names.isEmpty())
      _e.raise("Неверный формат файла конфигурации");
    
    return false;
    
//    while(_xml.readNextStartElement())
//      if(_xml.name() == "rows") break;
    
//    while(true) {
        
//      _xml.readNext();
      
//      if(_xml.isStartElement() && _xml.name() == "row") {
       
//        for(int i = 0; i < _xml.attributes().count(); i++)
//          qDebug() << _xml.attributes().at(i).name() << _xml.attributes().at(i).value().toString();
        
//  //      _row << (
//  //                value("name").toString();
//        _xml.skipCurrentElement();
        
//      }
//      else if(_xml.isEndElement() && _xml.name() == "rows")
//        break;
  
//    }
    
    
    
  }
  
  catch(SvException& e) {
    
    _last_error = e.error;
    return false;
    
  }

      
      
      
      
      
//      if(_xml.name() != "row" && _xml.name() != "value")
//        qDebug() << _xml.name();
  
        
  //        if (xml.name() == "Record") {
  //          QXmlStreamAttributes attrib = xmlDoc.attributes();
  //          date = QDate::fromString(attrib.value("Date").toString(), "dd.MM.yyyy");
  //        }
  
//  file->close();
}

bool SvXML::next()
{
  
}

QVariant SvXML::value(QString columnName)
{
  
}



//bool SvXML::readBounds(QString &fileName)
//{
//  bool result = false;
  
//  /* читаем xml */
//  QFile file(fileName);
//  if (!file.open(QFile::ReadOnly | QFile::Text)) {
//      QMessageBox::warning(this, tr("Ошибка чтения XML"),
//                           tr("Не удается прочитать файл %1:\n%2.")
//                           .arg(fileName)
//                           .arg(file.errorString()));
//      return false;
//  }
  
//  QXmlStreamReader xml;
//  xml.setDevice(&file);
  
//  while (!xml.atEnd())
//  {
//    xml.readNextStartElement();

//    if(xml.isStartElement() && xml.name() == "bounds")
//    {
//      bool o = true; bool k;
//      _area_data.geo_bounds.min_lat = xml.attributes().value("minlat").toDouble(&k); o &= k;
//      _area_data.geo_bounds.max_lat = xml.attributes().value("maxlat").toDouble(&k); o &= k;
//      _area_data.geo_bounds.min_lon = xml.attributes().value("minlon").toDouble(&k); o &= k;
//      _area_data.geo_bounds.max_lon = xml.attributes().value("maxlon").toDouble(&k); o &= k;
      
//      result = o;
      
//      break;
//    }
//  }
  
//  return result;
//}
