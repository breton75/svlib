#include "sv_log.h"

//using namespace svlog;

QMutex logmutex;

QDockWidget *svlog::SvLog::createLog(QMainWindow *window)
{
  if (!window) {
    
      _log_edit = new QTextEdit;
      _log_edit->show();
  }
  else {
    
    _dockWidget = new QDockWidget(window);
    _dockWidget->setObjectName(QStringLiteral("logDockWidget"));
    _dockWidget->setWindowTitle("Лог");
    _dockWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
//    _dockWidget->setFeatures(QDockWidget::DockWidgetVerticalTitleBar);
    _dockWidget->setMinimumSize(QSize(100, 50));
    _dockWidget->setMaximumSize(QSize(16635, 16635));
    
    QWidget *wDockContents = new QWidget();
    wDockContents->setObjectName(QStringLiteral("wDockContents"));
    
    QVBoxLayout *verticalLayout = new QVBoxLayout(wDockContents);
    verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
    verticalLayout->setMargin(2);
    
    _log_edit = new QTextEdit(wDockContents);
    _log_edit->setObjectName(QStringLiteral("_log_edit"));
  
    verticalLayout->addWidget(_log_edit);
  
    _dockWidget->setWidget(wDockContents);
    window->addDockWidget(static_cast<Qt::DockWidgetArea>(4), _dockWidget);

  }
  
}

void svlog::SvLog::assignLog(QTextEdit *widget)
{
  if (!widget)
  {
    _log_edit = new QTextEdit;
    _log_edit->show();
  }
  else
  {
    _log_edit = widget;
  }
}
  
//void log_ns::log(int mode, QString text, QWidget *parent)
void svlog::SvLog::log(svlog::MessageTypes type, QString text)
{
  if(!_log_edit)
    return;
  
  QColor color;
  
  switch (type) {
      
    case svlog::Simple:
      color = Qt::black;
      break;
      
    case svlog::Data:
        color = QColor(223344);
        break;

    case svlog::Error:
        color = Qt::red;
        break;
      
    case svlog::Info:
        color = QColor(0x009A8832);// Qt::green);
        break;
      
    case svlog::Duty:
        color = QColor(0x003A6098);
        break;
      
    case svlog::Attention:
        color = Qt::darkMagenta;
        break;
      
    case svlog::NewData:
        color = QColor(0x00BD6311);
        break;
      
    case svlog::Fail:
        color = QColor(0x00173EF4);
        break;
      
    case svlog::Debug:
        color = QColor(0x008A8A8A);
        break;
      
    case svlog::Success:
        color = QColor(0x0020A230);
        break;
      
    case svlog::Critical:
      color = Qt::red;
      break;
    }
  
  
  logmutex.lock();
  _log_edit->setTextColor(color);
  _log_edit->append(text);
  logmutex.unlock();
  
  _current_line_num++;
  _current_line = "";
  
  QApplication::processEvents();
  
  if(type == svlog::Critical)
    QMessageBox::critical(0, "Error", text, QMessageBox::Ok);
  
}

//void log_ns::log(int mode, QStringList list, QWidget *parent)
void svlog::SvLog::log(svlog::MessageTypes type, QStringList list)
{
  for(int i = 0; i < list.count(); i++)
    log(type, list.at(i));

}

void svlog::SvLog::log(svlog::MessageTypes type, QString text, QTextEdit *textedit)
{
  
}

//void log_ns::log_SQL_error(QSqlError error, QString noErrorMsg)
//{
//  switch (error.type())
//  {
//    case QSqlError::NoError:
//      log(m_Success, noErrorMsg);
//      break;
  
//    case QSqlError::ConnectionError:
//      log(m_Critical, QString("Connection Error: %1").arg(error.text()));
//      break;      
  
//    case QSqlError::StatementError:
//      log(m_Critical, QString("Statement Error: %1").arg(error.text()));
//      break;  
      
//    case QSqlError::TransactionError:
//      log(m_Critical, QString("Transaction Error: %1").arg(error.text()));
//      break;      
      
//    case QSqlError::UnknownError:
//      log(m_Critical, QString("Error: %1").arg(error.text()));
//      break;       
//  }
//}

QString svlog::bytesToText(const QByteArray* b, QString splitter)
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

QString svlog::bytesToHex(const QByteArray* b, QString splitter)
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
