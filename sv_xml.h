#ifndef SV_XML_H
#define SV_XML_H


#include <QObject>
#include <QSqlQuery>
#include <QXmlStreamReader>
#include <QFile>
#include <QVariant>
#include <QDebug>

#include "../../svlib/sv_exception.h"

typedef QList<QList<QVariant>> Table;
typedef QList<QVariant> Row;

class SvXML : public QObject
{
  Q_OBJECT
public:
  explicit SvXML(QObject *parent = nullptr);
  ~SvXML();
  
  bool open(const QString fileName);
  
  bool readTable(QString tableName);
  bool next();
  QVariant value(QString columnName);
  
  QString lastError() { return _last_error; }
  
private:
  QFile* _file;
  
  QXmlStreamReader _xml;
  quint32 _current_line = 0;
  
  SvException _e;
  
  QString _last_error = "";
  
  QStringList _column_names;
  Table _table;
  Row _row;
  
signals:
  
public slots:
};

#endif // SV_XML_H
