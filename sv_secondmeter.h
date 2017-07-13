#ifndef SVSECONDMETER_H
#define SVSECONDMETER_H

#include <QThread>
#include <QTimer>
#include <QDialog>
#include <QLabel>
#include <QWidget>
#include <QTime>
#include <QPushButton>

class SvSecondMeterTimers;

class SvSecondMeter : public QDialog
{
  Q_OBJECT
    
  private:
    int thread_interval = 0;
  
  public:
    enum STATUSES
    {
      smsUndefined = -1,
      smsDoneOk = 0,
      smsRunned,
      smsCanceled,
      smsTimeout,
      smsUnfinished
    };
    
    
    explicit SvSecondMeter(int interval,
                           QString labelCaption,
                           QString btnCaption,
                           bool runOnCreate,
                           bool showModal,
                           QObject *parent = 0);

    ~SvSecondMeter();
    
    void setLabelCaption(QString labelCaption);
    void setBtnCaption(QString btnCaption);
    void start();
    
    SvSecondMeterTimers *timers;
    
    QLabel *lblCaption;
    QLabel *lblTimer;
    QPushButton *btn;

    int status = smsUndefined;
    
    QTime current_time = QTime(0, 0, 1);
    int current_second = 0;
    
  public slots:
    void slotNewSecond();
    void slotTimeout();
    void slotCanceled();
    void slotDone();
    void slotStopTimers();

};

class SvSecondMeterTimers : public QThread
{
  Q_OBJECT
  
  private:
    int thread_interval = 0;
  
  public:
   
    explicit SvSecondMeterTimers(int interval)
    {
      thread_interval = interval;
    }

    ~SvSecondMeterTimers();

    virtual void run() Q_DECL_OVERRIDE;
    
  signals:
    void sigNewSecond();
    void sigTimeout();
    
  public slots:
//    void slotOnNewSecond();
//    void on_Timeout();

};

#endif // SVSECONDMETER_H
