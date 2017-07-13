#include "sv_tcpserverclient.h"
#include "../../Common/log.h"
#include "sv_secondmeter.h"

#include <QApplication>
#include <QByteArray>
#include <QDataStream>
#include <QDebug>
#include <QMessageBox>

using namespace log_ns;

QString char2hex(char c, bool toUpper = true, bool add0x = false);


QString char2hex(char c, bool toUpper, bool add0x)
{
    QString s = "";
    int d = 0;
    memcpy(&d, &c, 1);

    s.setNum(d, 16);
    if(s.length() == 1) s = '0' + s;

    return (add0x ? "0x" : "") + (toUpper ? s.toUpper() : s) + " ";

}

/*******************************************************
 *                    SvTcpServer                      *
 * *****************************************************/

SvTcpServer::SvTcpServer(quint16 port,
                         bool runServer,
                         QObject *parent,
                         bool logRequestData,
                         bool showRequestDataInHex,
                         bool logResponseData,
                         bool showResponseDataInHex,
                         bool showLog,
                         bool advancedStream,
                         int streamVersion) :
  QObject(parent)
{
  this->port = port;
  this->logRequestData = logRequestData;
  this->showRequestDataInHex = showRequestDataInHex;
  this->logResponseData = logResponseData;
  this->showResponseDataInHex = showResponseDataInHex;
  this->showLog = showLog;
  
  this->advancedStream = advancedStream;
  this->streamVersion = streamVersion;
  
  if(runServer && (this->port != 0))
    this->startServer(this->port, parent);
 
}

void SvTcpServer::slotNewConnection()
{
  QTcpSocket* pClientSocket = this->server->nextPendingConnection();
  connect(pClientSocket, SIGNAL(disconnected()), this, SLOT(slotClientDisconnected()));
  connect(pClientSocket, SIGNAL(readyRead()), this, SLOT(slotReadClient()));
  
  if(showLog) log(m_Info, "Connected client: " + pClientSocket->peerAddress().toString());
  //this->sendToClient(pClientSocket, "Connected to " + pClientSocket->localAddress().toString());
}

void SvTcpServer::slotClientDisconnected()
{
  QTcpSocket* pClientSocket = (QTcpSocket*)sender();
  if(showLog) log(m_Info, QString("Client %1 disconnected").arg(pClientSocket->peerAddress().toString()));
  
  pClientSocket->close();
  pClientSocket->deleteLater();
  
  emit sigClientDisconnected();
}

void SvTcpServer::sendToClient(QTcpSocket* pSocket, const QString& str)
{
  QByteArray  arrBlock;
  QString s = str;

  arrBlock.append(s);
  
  pSocket->write(arrBlock);
  
  /******** выводим в лог *********/
  if(logResponseData)
  {
    if(showResponseDataInHex)
    {
      s = "";
      for (int i=0; i < arrBlock.size(); i++)
        s = s + char2hex(arrBlock.at(i));
    }
    
    if(showLog) log(m_Data, " << " + s);
  }
  else if(showLog) log(m_Data, QString(" << %1 bytes sent").arg(arrBlock.size()));
}

void SvTcpServer::slotReadClient()
{
  QTcpSocket* pClientSocket = (QTcpSocket*)sender();
  QString message = "";
  
  this->last_message.clear();
  
  if(pClientSocket->bytesAvailable() > 0)
    this->last_message = pClientSocket->readAll();
  
  this->lastClientIp = pClientSocket->peerAddress().toString();
  
  /******** выводим в лог *********/
  if(this->logRequestData & showLog)
  {
    message = "";
    if(this->showRequestDataInHex)
    {
      for (int i=0; i < this->last_message.size(); i++)
        message = message + char2hex(this->last_message.at(i));
    }
    else message.append(this->last_message);
    
    log(m_Data, " >> " + message);
  }
  
  /********** отправляем клиенту OK ************/
//  if(this->last_message.toUpper().contains(QString("ERROR").toLatin1()))
//  { 
//    pClientSocket->close();
//    pClientSocket->deleteLater();
//  }
//  else
    this->sendToClient(pClientSocket, "OK");
    
  emit this->sigGotMessage();
  
}

int SvTcpServer::startServer(quint16 port, QObject* parent)
{
  if(this->isRunned)
  {
    if(showLog) log(m_Err, "Already runned");
    return 0;
  }
  
  this->server = new QTcpServer(parent);
  this->port = port;
  
  if(!this->server->listen(QHostAddress::Any, this->port))
  {
    log(m_Err, "Server not runned");
    this->server->close();
    return 1;
  }
  
  connect(this->server, SIGNAL(acceptError(QAbstractSocket::SocketError)), this, SLOT(slotSocketError(QAbstractSocket::SocketError)));
  connect(this->server, SIGNAL(newConnection()), this, SLOT(slotNewConnection()));

  if(showLog) log(m_Info, "Server runned");
  
  return 0;
}

void SvTcpServer::stopServer()
{
  while(this->server->hasPendingConnections())
  {
    QTcpSocket* connection = this->server->nextPendingConnection();
    connection->close();
    connection->deleteLater();
  }
  
  this->server->close();
  this->isRunned = false;
  if(showLog) log(m_Info, "Server stopped");

}

void SvTcpServer::slotSocketError(QAbstractSocket::SocketError err)
{
  switch (err)
  {
    case QAbstractSocket::ConnectionRefusedError:
      log(m_Err, "Connection Refused Error");
      break;

    case QAbstractSocket::RemoteHostClosedError :
      log(m_Err, QString("Remote Host Closed Error"));
      break;

    case QAbstractSocket::HostNotFoundError :
      log(m_Err, QString("Host Not Found Error"));
      break;

    case QAbstractSocket::SocketAccessError :
      log(m_Err, QString("Socket Access Error"));
      break;

    case QAbstractSocket::SocketResourceError :
      log(m_Err, QString("Socket Resource Error"));
      break;

    case QAbstractSocket::SocketTimeoutError :
      log(m_Err, QString("Socket Timeout Error"));
      break;

    case QAbstractSocket::DatagramTooLargeError :
      log(m_Err, QString("DatagramTooLargeError"));
      break;

    case QAbstractSocket::NetworkError :
      log(m_Err, QString("Network Error"));
      break;

    case QAbstractSocket::AddressInUseError :
      log(m_Err, QString("Address In Use Error"));
      break;

    case QAbstractSocket::SocketAddressNotAvailableError :
      log(m_Err, QString("Socket Address Not Available Error"));
      break;

    case QAbstractSocket::UnsupportedSocketOperationError :
      log(m_Err, QString("Unsupported Socket Operation Error"));
      break;

    case QAbstractSocket::UnfinishedSocketOperationError :
      log(m_Err, QString("Unfinished Socket Operation Error"));
      break;

    case QAbstractSocket::ProxyAuthenticationRequiredError :
      log(m_Err, QString("Proxy Authentication Required Error"));
      break;

    case QAbstractSocket::SslHandshakeFailedError :
      log(m_Err, QString("Ssl Handshake Failed Error"));
      break;

    case QAbstractSocket::ProxyConnectionRefusedError :
      log(m_Err, QString("Proxy Connection Refused Error"));
      break;

    case QAbstractSocket::ProxyConnectionClosedError :
      log(m_Err, QString("Proxy Connection Closed Error"));
      break;

    case QAbstractSocket::ProxyConnectionTimeoutError :
      log(m_Err, QString("Proxy Connection Timeout Error"));
      break;

    case QAbstractSocket::ProxyNotFoundError :
      log(m_Err, QString("Proxy Not Found Error"));
      break;

    case QAbstractSocket::ProxyProtocolError :
      log(m_Err, QString("Proxy Protocol Error"));
      break;

    case QAbstractSocket::OperationError :
      log(m_Err, QString("Operation Error"));
      break;

    case QAbstractSocket::SslInternalError :
      log(m_Err, QString("SslInternal Error"));
      break;

    case QAbstractSocket::SslInvalidUserDataError :
      log(m_Err, QString("Ssl Invalid User Data Error"));
      break;

    case QAbstractSocket::TemporaryError :
      log(m_Err, QString("Temporary Error"));
      break;

    case QAbstractSocket::UnknownSocketError :
      log(m_Err, QString("Unknown Socket Error"));
      break;

  }

}

/*******************************************************
 *                      SvTcpClient                    *
 * *****************************************************/

SvTcpClient::SvTcpClient(svlog::SvLog &log,
                         QString ip,
                         quint16 port,
                         QObject *parent,
                         bool logRequestData,
                         bool showRequestDataInHex,
                         bool logResponseData,
                         bool showResponseDataInHex,
                         bool advancedStream,
                         int streamVersion):
  QObject(parent)
{
  this->ip = ip;
  this->port = port;
  this->logRequestData = logRequestData;
  this->showRequestDataInHex = showRequestDataInHex;
  this->logResponseData = logResponseData;
  this->showResponseDataInHex = showResponseDataInHex;
  
  this->advancedStream = advancedStream;
  this->streamVersion = streamVersion;
  
  this->socket = new QTcpSocket(this);
  connect(this->socket, SIGNAL(readyRead()), this, SLOT(slotReadyRead()));
  connect(this->socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slotSocketError(QAbstractSocket::SocketError)));
  connect(this->socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(slotStateChanged(QAbstractSocket::SocketState)));
  
  log << svlog::Data << "dsdsdsds" << svlog::endl;
}

int SvTcpClient::connectToHost()
{
  try
  {
    if (QHostAddress(this->ip).toIPv4Address() == 0) throw SOCKET_WRONG_IP;
    if ((port < 1) || (port > 65535)) throw(SOCKET_WRONG_PORT);
    
    if (!this->connected)
      {    
        this->socket->connectToHost(this->ip, this->port);
        if (this->socket->waitForConnected(2000))
          {
            this->connected = true;
            return SOCKET_OK;
          }
        else return SOCKET_BAD_CONNECTION; // throw(SOCKET_BAD_CONNECTION);
      }
    return SOCKET_OK;
  }
 
  catch (int err)
  {
    switch (err)
    {  
      case SOCKET_BAD_CONNECTION:
        log(m_Err, QString("Can not connect to %1:%2\n").arg(this->ip).arg(this->port));
        break;      
      
      case SOCKET_WRONG_IP:
        log(m_Err, "Wrong IP adress\n");
        break;
        
      case SOCKET_WRONG_PORT:
        log(m_Err, "Wrong port\n");
        break;        
    }
    
    return err;
  }
}

void SvTcpClient::disconnectFromHost()
{
  this->socket->disconnectFromHost();
  this->socket->close();
  this->connected  = false;
}

void SvTcpClient::slotSocketError(QAbstractSocket::SocketError err)
{
  switch (err)
  {
    case QAbstractSocket::ConnectionRefusedError:
      log(m_Err, "Connection Refused Error");
      this->connected = false;
      QApplication::processEvents();
      break;
      
    case QAbstractSocket::RemoteHostClosedError :
      log(m_Err, QString("Remote Host Closed Error"));
      QApplication::processEvents();
      break;   
      
    case QAbstractSocket::HostNotFoundError :
      log(m_Err, QString("Host Not Found Error"));
      QApplication::processEvents();
      break; 
      
    case QAbstractSocket::SocketAccessError :
      log(m_Err, QString("Socket Access Error"));
      QApplication::processEvents();
      break; 
      
    case QAbstractSocket::SocketResourceError :
      log(m_Err, QString("Socket Resource Error"));
      QApplication::processEvents();
      break; 
      
    case QAbstractSocket::SocketTimeoutError :
      log(m_Err, QString("Socket Timeout Error"));
      QApplication::processEvents();
      break; 
      
    case QAbstractSocket::DatagramTooLargeError :
      log(m_Err, QString("DatagramTooLargeError"));
      QApplication::processEvents();
      break; 
      
    case QAbstractSocket::NetworkError :
      log(m_Err, QString("Network Error"));
      QApplication::processEvents();
      break; 
      
    case QAbstractSocket::AddressInUseError :
      log(m_Err, QString("Address In Use Error"));
      QApplication::processEvents();
      break; 
      
    case QAbstractSocket::SocketAddressNotAvailableError :
      log(m_Err, QString("Socket Address Not Available Error"));
      QApplication::processEvents();
      break; 
      
    case QAbstractSocket::UnsupportedSocketOperationError :
      log(m_Err, QString("Unsupported Socket Operation Error"));
      QApplication::processEvents();
      break; 
      
    case QAbstractSocket::UnfinishedSocketOperationError :
      log(m_Err, QString("Unfinished Socket Operation Error"));
      QApplication::processEvents();
      break; 
      
    case QAbstractSocket::ProxyAuthenticationRequiredError :
      log(m_Err, QString("Proxy Authentication Required Error"));
      QApplication::processEvents();
      break; 
      
    case QAbstractSocket::SslHandshakeFailedError :
      log(m_Err, QString("Ssl Handshake Failed Error"));
      QApplication::processEvents();
      break; 
      
    case QAbstractSocket::ProxyConnectionRefusedError :
      log(m_Err, QString("Proxy Connection Refused Error"));
      QApplication::processEvents();
      break; 
      
    case QAbstractSocket::ProxyConnectionClosedError :
      log(m_Err, QString("Proxy Connection Closed Error"));
      QApplication::processEvents();
      break; 
      
    case QAbstractSocket::ProxyConnectionTimeoutError :
      log(m_Err, QString("Proxy Connection Timeout Error"));
      QApplication::processEvents();
      break; 
      
    case QAbstractSocket::ProxyNotFoundError :
      log(m_Err, QString("Proxy Not Found Error"));
      QApplication::processEvents();
      break;
      
    case QAbstractSocket::ProxyProtocolError :  
      log(m_Err, QString("Proxy Protocol Error"));
      QApplication::processEvents();
      break; 
      
    case QAbstractSocket::OperationError :
      log(m_Err, QString("Operation Error"));
      QApplication::processEvents();
      break; 
      
    case QAbstractSocket::SslInternalError :
      log(m_Err, QString("SslInternal Error"));
      QApplication::processEvents();
      break; 
      
    case QAbstractSocket::SslInvalidUserDataError :
      log(m_Err, QString("Ssl Invalid User Data Error"));
      QApplication::processEvents();
      break;
      
    case QAbstractSocket::TemporaryError :
      log(m_Err, QString("Temporary Error"));
      QApplication::processEvents();
      break; 
      
    case QAbstractSocket::UnknownSocketError :
      log(m_Err, QString("Unknown Socket Error"));
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

void SvTcpClient::slotStateChanged(QAbstractSocket::SocketState state)
{
  switch (state)
    {
    case QAbstractSocket::UnconnectedState:
      log(m_Info, "No connection");
      this->connected = false;
      QApplication::processEvents();
      break;
      
    case QAbstractSocket::HostLookupState:
      log(m_Info, QString("Looking up for %1").arg(this->ip));
      QApplication::processEvents();
      break;   

    case QAbstractSocket::ConnectingState:
      log(m_Info, QString("Connecting to %1").arg(this->ip));
      QApplication::processEvents();
      break;
      
    case QAbstractSocket::ConnectedState:
      this->connected = true;
      log(m_Info, QString("Connected to %1").arg(this->ip));
      QApplication::processEvents();
      break;

    case QAbstractSocket::BoundState:
      log(m_Info, QString("Bounded for %1:%2").arg(this->ip).arg(this->port));
      QApplication::processEvents();
      break; 
      
    case QAbstractSocket::ClosingState:
      log(m_Info, "Closing connection");
      QApplication::processEvents();
      break;       
  
    default:
      log(m_Info, "state ");// + QString("%1").arg(int(state)));
  }
}

void SvTcpClient::slotReadyRead()
{
  try
  {
    QString str;
    
    if (!this->advancedStream)
    {
      if (this->socket->bytesAvailable() > 0)
      {
        responseData = this->socket->readAll();
        responseSize = responseData.size();
      }
    }
     
    else
    {
       responseSize = 0;
       QDataStream in(this->socket);
       
       in.setVersion(this->streamVersion);/*QDataStream::Qt_5_3*/
       for (;;)
       {
         if (!responseSize)
         {
           if (this->socket->bytesAvailable() < (quint64)sizeof(quint64)) break;
           in >> responseSize;
         }
         if (this->socket->bytesAvailable() < responseSize) break;
         
         responseData = this->socket->readAll();

         if(logResponseData)
         {
           QDataStream in1(responseData);
           in1.setVersion(this->streamVersion);
           in1 >> str;
//           in >> responseData;// /*time >>*/ str;
         }
       }
     }
    
    
    /***** выводим лог *****/
    if(this->logResponseData)
    {
      
      if(!showResponseDataInHex)
      {
        for (int i=0; i < responseData.size(); i++)
        {
          if((responseData.at(i) > 31) || 
             (responseData.at(i) == 10) || 
              (responseData.at(i) == 13)) 
             str = str + responseData.at(i);
          else str = str + char2hex(responseData.at(i), true, false) + " ";
        }
      }
      else
      {
        for (int i=0; i < responseData.size(); i++)
          str = str + char2hex(responseData.at(i), true, false) + " ";
      }
      
      log(m_Data, " >> " + str + '\n');
    }
    else log(m_Data, QString(" >> %1 bytes got\n").arg(responseSize));
    
    emit sigGotNewData();
  }

  catch(...)
  {
    log(m_Err, "slotReadyRead error: ");  
  }
    
}

void SvTcpClient::sendData(QString text, int msecWaitForAnswer, QObject* parent)
{
  if(!this->socket->isOpen()) return;
  
  QByteArray arrBlock;
  SvSecondMeter *t;

  /***** готовимся ждать ответ *****/
  if(msecWaitForAnswer != dontWait)
  {
    t = new SvSecondMeter(msecWaitForAnswer, "Waiting for response", "Cancel", false, true, parent);
    connect(this, SIGNAL(sigGotNewData()), t, SLOT(slotDone()));
    
    this->responseSize = 0;
    this->responseData.clear();   
  }
  
  /**** готовим данные к отправке *******/
  if(!this->advancedStream) {
      arrBlock.append(text);
  }
  else
  {
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(this->streamVersion);
    out << quint16(0) /*<< QTime::currentTime()*/ << text;
    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));
  }
  
  /***** отправляем данные *****/
  this->socket->write(arrBlock);
  
  if(t) t->start();
  
  /***** выводим лог *****/
  if(this->logRequestData)
  {
    log(m_Data, " << " + text);
  }
  else log(m_Data, QString(" << %1 bytes sent").arg(text.length()));
  
  if(msecWaitForAnswer == dontWait) return;
  
  /*********************** ждем ответ ***********************/
  while((t->status == SvSecondMeter::smsRunned)&&(this->socket->isOpen())) QApplication::processEvents();
  
  if(!this->socket->isOpen()) t->status = SvSecondMeter::smsUnfinished;
  
  if(!t) return;
  
  switch (t->status)
  {
    case SvSecondMeter::smsCanceled:
      this->socket->abort();
      log(m_Err, QString("Response awaiting was canceled by user at %1").
            arg(t->current_time.toString("hh:mm:ss")));
      this->responseStatus = SOCKET_AWAITING_CANCELED;
      break;
      
    case SvSecondMeter::smsTimeout:
      this->socket->abort();
      log(m_Err, QString("Response awaiting timeout %1").
            arg(t->current_time.toString("hh:mm:ss")));
      this->responseStatus = SOCKET_TIMEOUT;
      break;
      
    case SvSecondMeter::smsUnfinished:
      log(m_Err, QString("Qwaiting not finished %1").
            arg(t->current_time.toString("hh:mm:ss")));
      this->responseStatus = SOCKET_CONNECTION_CLOSED;
      break;
      
    case SvSecondMeter::smsDoneOk:
      this->responseStatus = SOCKET_OK;      
  }
  
  t->~SvSecondMeter();
  
}
 
void SvTcpClient::sendData(QByteArray* data, int msecWaitForAnswer, QObject* parent)
{
    if(!this->socket->isOpen()) return;
  
    SvSecondMeter *t;
    QByteArray arrBlock;
    
    arrBlock.append(*data);
    
    /***** готовимся ждать ответ *****/
    if(msecWaitForAnswer != dontWait)
    {
      t = new SvSecondMeter(msecWaitForAnswer, "Waiting for response", "Cancel", false, true, parent);
      connect(this, SIGNAL(sigGotNewData()), t, SLOT(slotDone()));
      
      this->responseSize = 0;
      this->responseData.clear();   
    }
    
    /***** отправляем данные *****/
    this->socket->write(arrBlock);
    
    t->start();
    
    /***** выводим лог *****/
    QString str = "";
    if(this->logRequestData)
    {
      if(!showRequestDataInHex)
      {
        for (int i=0; i < arrBlock.size(); i++)
        {
          if((arrBlock.at(i) > 31) || 
             (arrBlock.at(i) == 10) || 
              (arrBlock.at(i) == 13)) 
             str = str + arrBlock.at(i);
          else str = str + char2hex(arrBlock.at(i), true, false) + " ";
        }
      }
      else
      {
        for (int i=0; i < arrBlock.size(); i++)
          str = str + char2hex(arrBlock.at(i), true, false) + " ";
      }

    }
    else str = QString("%1 bytes sent").arg(arrBlock.length());
      
    log(m_Data, " << " + str);
      
    if(msecWaitForAnswer == dontWait) return;
    
    /*********************** ждем ответ ***********************/
    while((t->status == SvSecondMeter::smsRunned)&&(this->socket->isOpen())) QApplication::processEvents();
    if(!this->socket->isOpen()) t->status = SvSecondMeter::smsUnfinished;
    
    switch (t->status)
    {
      case SvSecondMeter::smsCanceled:
        this->socket->reset();//abort();
        log(m_Err, QString("Response awaiting was canceled by user at %1").
              arg(t->current_time.toString("hh:mm:ss")));
        this->responseStatus = SOCKET_AWAITING_CANCELED;
        break;
        
      case SvSecondMeter::smsTimeout:
        this->socket->abort();
        log(m_Err, QString("Response awaiting timeout %1").
              arg(t->current_time.toString("hh:mm:ss")));
        this->responseStatus = SOCKET_TIMEOUT;
        break;
        
      case SvSecondMeter::smsUnfinished:
        log(m_Err, QString("Awaiting not finished %1").
              arg(t->current_time.toString("hh:mm:ss")));
        this->responseStatus = SOCKET_CONNECTION_CLOSED;
        break;
        
      case SvSecondMeter::smsDoneOk:
        this->responseStatus = SOCKET_OK;      
    }
    
    t->~SvSecondMeter();
}
 
void SvTcpClient::sendData(int msecWaitForAnswer)
{
  try
  {
    /************* для отладки таймера ************/
    
    /*********************** ждем ответ ***********************/
    this->responseSize = 0;
    this->responseData.clear();
    
    SvSecondMeter *t = new SvSecondMeter(msecWaitForAnswer,
                                         "Waiting for response",
                                         "Cancel",
                                         true,
                                         true);
    
//    connect(this, SIGNAL(sigGotNewData()), t, SLOT(slotDone()));
    
    while(t->status == SvSecondMeter::smsRunned) QApplication::processEvents();
    
    switch (t->status)
    {
      case SvSecondMeter::smsCanceled:
//        this->socket->abort();
        log(m_Err, QString("Response awaiting was canceled by user at %1").
              arg(t->current_time.toString("hh:mm:ss")));
        this->responseStatus = SOCKET_AWAITING_CANCELED;
        break;
        
      case SvSecondMeter::smsTimeout:
//        this->socket->abort();
        log(m_Err, QString("Response awaiting timeout %1").
              arg(t->current_time.toString("hh:mm:ss")));
        this->responseStatus = SOCKET_TIMEOUT;
        break;
        
      case SvSecondMeter::smsDoneOk:
        this->responseStatus = SOCKET_OK;      
    }
    
    t->~SvSecondMeter();
  }
  
  catch(...)
  {
    log(m_Err, "sayToServer: ");
  }
}

void SvTcpClient::simpleSendData(QString text)
{
  if(!this->socket->isOpen()) return;
  
  QByteArray arrBlock;

  /***** отправляем данные *****/
  arrBlock.append(text);
  arrBlock.append('\n');
  this->socket->write(arrBlock);
  
  /***** выводим лог *****/
  if(this->logRequestData)
  {
    log(m_Data, " << " + text);
  }
  else log(m_Data, QString(" << %1 bytes sent").arg(text.length()));

  
}
