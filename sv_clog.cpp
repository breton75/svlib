#include "sv_clog.h"

//using namespace svlog;

QMutex logmutex;

  
void clog::SvCLog::log(clog::MessageTypes type, QString text, bool newline)
{
//#if !defined (Q_OS_WIN)
//  QTextCodec* codec = QTextCodec::codecForName("CP866");
//  QByteArray encodedString(text.toUtf8());
////  encodedString = codec->fromUnicode(text.toUtf8());
////  std::cout << encodedString.toStdString()/*.data()*/ << std::endl;
//  std::cout << encodedString.data() << std::endl;
//#else
//  std::cout << text.toStdString() << std::endl;
//#endif

  QTextStream stream(stdout);
  stream << QString(text); // << (newline ? '\n' : '');

  if(newline) {
    stream << '\n';
    _current_line_num++;
  }

  _current_line = "";
  
}

void clog::SvCLog::log(clog::MessageTypes type, QStringList list)
{
  for(int i = 0; i < list.count(); i++)
    log(type, list.at(i));

}

QString clog::bytesToText(const QByteArray* b, QString splitter)
{
  QString s;
  
  uint i = 0;
  uint len = b->length();
  while(i < len)
  {
    s += i > 0 ? splitter : "";
    s += uchar(b->at(i)) > 31 ? uchar(b->at(i)) : '.';
    i++;
  }
  
  return s;
}

QString clog::bytesToHex(const QByteArray* b, QString splitter)
{
  QString s;
  
  uint i = 0;
  uint len = b->length();
  while(i < len)
  {
    QString h = QString::number(uchar(b->at(i)), 16);
    
    if(h.length() == 1) h = "0" + h;
    s += i > 0 ? splitter : "";
    s += h.toUpper();
    i++;
  }
  
  return s;
}
