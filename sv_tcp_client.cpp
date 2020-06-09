#include "sv_tcp_client.h"
//#include "../../Common/log.h"
//#include "sv_secondmeter.h"

#include <QApplication>
#include <QByteArray>
#include <QDataStream>
#include <QDebug>
#include <QMessageBox>

//using namespace log_ns;

//QString char2hex(char c, bool toUpper = true, bool add0x = false, bool addBlank = true);


//QString char2hex(char c, bool toUpper, bool add0x, bool addBlank)
//{
//    QString s = QString("%1").arg(uint(c), 2, 16, QLatin1Char('0'));
  
//    return (add0x ? "0x" : "") + (toUpper ? s.toUpper() : s) + (addBlank ? " " : "");
//}

/*******************************************************
 *                      SvTcpClient                    *
 * *****************************************************/

svtcp::SvTcpClient::SvTcpClient(QString ip, quint16 port,
                                sv::SvAbstractLogger* logger,
                                int flags,
                                QObject *parent) : 
  QObject(parent),
  _logger(logger)
{
  _parent = parent;
  
  _ip = ip;
  _port = port;
  _log_flags = flags;
  
  _socket = new QTcpSocket(this);
//  connect(_socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
  connect(_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));
  connect(_socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(stateChanged(QAbstractSocket::SocketState)));
   
}

svtcp::SvTcpClient::~SvTcpClient()
{
  disconnectFromHost();
  delete _socket;
}

svtcp::SocketErrors svtcp::SvTcpClient::connectToHost()
{
  if (QHostAddress(_ip).toIPv4Address() == 0) {
      _lastError = "Wrong IP";
      socketError(QAbstractSocket::HostNotFoundError);
      return svtcp::SOCKET_WRONG_IP;
  }
      
  if((_port < 1) || (_port > 65535)) {
    _lastError = "Wrong port";
    socketError(QAbstractSocket::HostNotFoundError);
    return svtcp::SOCKET_WRONG_PORT;
  }
  
  if (!_connected) {

      _socket->connectToHost(_ip, _port);
      
      if (_socket->waitForConnected(5000))
        
          _connected = true;
      
      else {
        _lastError = _socket->errorString();
        return svtcp::SOCKET_BAD_CONNECTION;
      }
  }
  
  return svtcp::SOCKET_OK;
  
}

void svtcp::SvTcpClient::disconnectFromHost()
{
  _socket->disconnectFromHost();
  _socket->close();
  _connected  = false;
}

void svtcp::SvTcpClient::socketError(QAbstractSocket::SocketError err)
{
  if(!_socket->errorString().isEmpty())
    _lastError = _socket->errorString();
  
  if(_logger)
    *_logger << sv::log::Time << sv::log::mtError << _lastError << sv::log::endl;
  
  switch (err)
  {
    case QAbstractSocket::ConnectionRefusedError:
//      log(m_Err, "Connection Refused Error");
      _connected = false;
      QApplication::processEvents();
      break;
      
    case QAbstractSocket::RemoteHostClosedError :
//      log(m_Err, QString("Remote Host Closed Error"));
      QApplication::processEvents();
      break;   
      
    case QAbstractSocket::HostNotFoundError :
//      log(m_Err, QString("Host Not Found Error"));
      QApplication::processEvents();
      break; 
      
    case QAbstractSocket::SocketAccessError :
//      log(m_Err, QString("Socket Access Error"));
      QApplication::processEvents();
      break; 
      
    case QAbstractSocket::SocketResourceError :
//      log(m_Err, QString("Socket Resource Error"));
      QApplication::processEvents();
      break; 
      
    case QAbstractSocket::SocketTimeoutError :
//      log(m_Err, QString("Socket Timeout Error"));
      QApplication::processEvents();
      break; 
      
    case QAbstractSocket::DatagramTooLargeError :
//      log(m_Err, QString("DatagramTooLargeError"));
      QApplication::processEvents();
      break; 
      
    case QAbstractSocket::NetworkError :
//      log(m_Err, QString("Network Error"));
      QApplication::processEvents();
      break; 
      
    case QAbstractSocket::AddressInUseError :
//      log(m_Err, QString("Address In Use Error"));
      QApplication::processEvents();
      break; 
      
    case QAbstractSocket::SocketAddressNotAvailableError :
//      log(m_Err, QString("Socket Address Not Available Error"));
      QApplication::processEvents();
      break; 
      
    case QAbstractSocket::UnsupportedSocketOperationError :
//      log(m_Err, QString("Unsupported Socket Operation Error"));
      QApplication::processEvents();
      break; 
      
    case QAbstractSocket::UnfinishedSocketOperationError :
//      log(m_Err, QString("Unfinished Socket Operation Error"));
      QApplication::processEvents();
      break; 
      
    case QAbstractSocket::ProxyAuthenticationRequiredError :
//      log(m_Err, QString("Proxy Authentication Required Error"));
      QApplication::processEvents();
      break; 
      
    case QAbstractSocket::SslHandshakeFailedError :
//      log(m_Err, QString("Ssl Handshake Failed Error"));
      QApplication::processEvents();
      break; 
      
    case QAbstractSocket::ProxyConnectionRefusedError :
//      log(m_Err, QString("Proxy Connection Refused Error"));
      QApplication::processEvents();
      break; 
      
    case QAbstractSocket::ProxyConnectionClosedError :
//      log(m_Err, QString("Proxy Connection Closed Error"));
      QApplication::processEvents();
      break; 
      
    case QAbstractSocket::ProxyConnectionTimeoutError :
//      log(m_Err, QString("Proxy Connection Timeout Error"));
      QApplication::processEvents();
      break; 
      
    case QAbstractSocket::ProxyNotFoundError :
//      log(m_Err, QString("Proxy Not Found Error"));
      QApplication::processEvents();
      break;
      
    case QAbstractSocket::ProxyProtocolError :  
//      log(m_Err, QString("Proxy Protocol Error"));
      QApplication::processEvents();
      break; 
      
    case QAbstractSocket::OperationError :
//      log(m_Err, QString("Operation Error"));
      QApplication::processEvents();
      break; 
      
    case QAbstractSocket::SslInternalError :
//      log(m_Err, QString("SslInternal Error"));
      QApplication::processEvents();
      break; 
      
    case QAbstractSocket::SslInvalidUserDataError :
//      log(m_Err, QString("Ssl Invalid User Data Error"));
      QApplication::processEvents();
      break;
      
    case QAbstractSocket::TemporaryError :
//      log(m_Err, QString("Temporary Error"));
      QApplication::processEvents();
      break; 
      
    case QAbstractSocket::UnknownSocketError :
//      log(m_Err, QString("Unknown Socket Error"));
      QApplication::processEvents();
      break;

  }
  
  
//  QString strError =
//        "Error: " + (err == QAbstractSocket::HostNotFoundError ?
//                     "The host was not found." :
//                     err == QAbstractSocket::RemoteHostClosedError ?
//                     "The remote host is closed." :
//                     err == QAbstractSocket::ConnectionRefusedError ?
//                     "The connection was refused." :
//                     (QString(this->socket->errorString()) != "" ? QString(this->socket->errorString()) :
//                      QString(this->socket->errorString()))
//                    );
//    log(m_Err, strError);
}

void svtcp::SvTcpClient::stateChanged(QAbstractSocket::SocketState state)
{
  QString stateString;
  
  switch (state)
    {
    case QAbstractSocket::UnconnectedState:
      stateString = "No connection";
      _connected = false;
      break;
      
    case QAbstractSocket::HostLookupState:
      stateString = QString("Looking up for %1").arg(_ip);
      break;   

    case QAbstractSocket::ConnectingState:
      stateString = QString("Connecting to %1").arg(_ip);
      break;
      
    case QAbstractSocket::ConnectedState:
      _connected = true;
      stateString = QString("Connected to %1").arg(_ip);
      break;

    case QAbstractSocket::BoundState:
      stateString = QString("Bounded for %1:%2").arg(_ip).arg(_port);
      break; 
      
    case QAbstractSocket::ClosingState:
      stateString = "Closing connection";
      break;       
  
    default:
      stateString = "state ";// + QString("%1").arg(int(state)));
  }
  
  if(_logger)
    *_logger <<  sv::log::Time << sv::log::mtInfo << stateString << sv::log::endl;

  QApplication::processEvents();
  
}

void svtcp::SvTcpClient::readyRead()
{
  if (_socket->bytesAvailable() > 0)
  {
    _response.data = _socket->readAll();
    _response.size = _response.data.size();
    
    /***** выводим лог *****/
    if(_logger) {

      if(_log_flags & svtcp::LogInData) {

        if(_log_flags & svtcp::InDataAsHex)
          *_logger << sv::log::Time << sv::log::mtData << sv::log::in << QString(_response.data.toHex()) << sv::log::endl;

        else
          *_logger << sv::log::Time << sv::log::mtData << sv::log::in << QString(_response.data) << sv::log::endl;

      }
    }

    emit newData();

  }
}

void svtcp::SvTcpClient::sendData(QString text, int msecWaitForAnswer)
{
  QByteArray ba = QByteArray();
  ba.append(text);
  
  sendData(ba, msecWaitForAnswer);
  
}
 
void svtcp::SvTcpClient::sendData(const QByteArray &data, int msecWaitForAnswer)
{
    if(!_socket->isOpen()) return;
  
//    SvSecondMeter *t = nullptr;
    
    /***** готовимся ждать ответ *****/
//    if(msecWaitForAnswer != svtcp::DontWait) {
      
//      t = new SvSecondMeter(msecWaitForAnswer, "Waiting for response", "Cancel", false, true, _parent);
//      connect(this, SIGNAL(newData()), t, SLOT(slotDone()));
      
//      _response.size = 0;
//      _response.data.clear();
//    }
    
    /***** отправляем данные *****/
    _socket->write(data);
    
    
    /***** выводим лог *****/
    if(_logger) {

      if(_log_flags & svtcp::LogOutData) {

          if(_log_flags & svtcp::OutDataAsHex)
            *_logger << sv::log::Time << sv::log::mtData << sv::log::out << QString(data.toHex()) << sv::log::endl;

          else
            *_logger << sv::log::Time << sv::log::mtData << sv::log::out << QString(data) << sv::log::endl;

      }
    }
      
//    if(!t) return;
    
//    t->start();
    
//    /*********************** ждем ответ ***********************/
//    while((t->status == SvSecondMeter::smsRunned)&&(_socket->isOpen())) QApplication::processEvents();
//    if(!_socket->isOpen()) t->status = SvSecondMeter::smsUnfinished;
    
//    switch (t->status)
//    {
//      case SvSecondMeter::smsCanceled:
////        _socket->abort();
        
//        *_logger << sv::log::Time << sv::log::mtInfo
//             << QString("Response awaiting was canceled by user at %1").arg(QTime(t->current_time).toString("hh:mm:ss.zzz"))
//             << sv::log::endl;
        
//        _response.status = SOCKET_AWAITING_CANCELED;
//        _lastError = "Awaiting cancelled by user.";
//        break;
        
//      case SvSecondMeter::smsTimeout:
////        _socket->abort();
        
//        *_logger << sv::log::Time << sv::log::mtError
//             << QString("Response awaiting timeout %1").arg(QTime(t->current_time).toString("hh:mm:ss.zzz"))
//             << sv::log::endl;
        
//        _response.status = SOCKET_TIMEOUT;
//        _lastError = "Awaiting timeout.";
//        break;
        
//      case SvSecondMeter::smsUnfinished:
//        *_logger << sv::log::Time << sv::log::mtError
//             << QString("Awaiting not finished %1.").arg(QTime(t->current_time).toString("hh:mm:ss.zzz"))
//             << t->current_time
//             << sv::log::endl;
        
//        _response.status = SOCKET_CONNECTION_CLOSED;
//        _lastError = "Awaiting not finished.";
//        break;
        
//      case SvSecondMeter::smsDoneOk:
//        _response.status = SOCKET_OK;
//        _lastError = "";
//    }
    
//    t->~SvSecondMeter();

}
 
