#include "sv_secondmeter.h"

#include <QDebug>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QApplication>

QVBoxLayout *sm_verticalLayout;
QGroupBox *sm_groupBox;
QHBoxLayout *sm_horizontalLayout;
QSpacerItem *sm_horizontalSpacer_2;
QSpacerItem *sm_horizontalSpacer;

SvSecondMeter::SvSecondMeter(int interval,
                             QString labelCaption,
                             QString btnCaption,
                             bool runOnCreate,
                             bool showModal,
                             QObject *parent)
{
  
  this->setWindowTitle("Awaiting");
  this->setFixedSize(325, 80);

  /***********  создаем форму ********************/
  sm_verticalLayout = new QVBoxLayout(this);
  sm_verticalLayout->setObjectName(QStringLiteral("sm_verticalLayout"));
  
  sm_groupBox = new QGroupBox(this);
  sm_groupBox->setObjectName(QStringLiteral("sm_groupBox"));
  QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
  sizePolicy.setHorizontalStretch(0);
  sizePolicy.setVerticalStretch(0);
  sizePolicy.setHeightForWidth(sm_groupBox->sizePolicy().hasHeightForWidth());
  sm_groupBox->setSizePolicy(sizePolicy);
  
  sm_horizontalLayout = new QHBoxLayout(sm_groupBox);
  sm_horizontalLayout->setObjectName(QStringLiteral("sm_horizontalLayout"));

  QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Preferred);
  sizePolicy1.setHorizontalStretch(0);
  sizePolicy1.setVerticalStretch(0);
  
  
  lblCaption = new QLabel(sm_groupBox);
  lblCaption->setObjectName(QStringLiteral("lblCaption"));
  sizePolicy1.setHeightForWidth(lblCaption->sizePolicy().hasHeightForWidth());
  lblCaption->setSizePolicy(sizePolicy1);
  lblCaption->setText(labelCaption);
  sm_horizontalLayout->addWidget(lblCaption);
  
  sm_horizontalSpacer_2 = new QSpacerItem(19, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);
  sm_horizontalLayout->addItem(sm_horizontalSpacer_2);
  
  lblTimer = new QLabel(sm_groupBox);
  lblTimer->setObjectName(QStringLiteral("lblTimer"));
  sizePolicy1.setHeightForWidth(lblTimer->sizePolicy().hasHeightForWidth());
  lblTimer->setSizePolicy(sizePolicy1);
  lblTimer->setText("00:00:00");
  
//  QFont font;
//  font.setBold(false);
//  font.setWeight(50);
//  lblTimer->setFont(font);
  sm_horizontalLayout->addWidget(lblTimer);
  
  sm_horizontalSpacer = new QSpacerItem(19, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);
  sm_horizontalLayout->addItem(sm_horizontalSpacer);
  
  btn = new QPushButton(sm_groupBox);
  btn->setObjectName(QStringLiteral("btn"));
  QSizePolicy sizePolicy2(QSizePolicy::Fixed, QSizePolicy::Fixed);
  sizePolicy2.setHorizontalStretch(0);
  sizePolicy2.setVerticalStretch(0);
  sizePolicy2.setHeightForWidth(btn->sizePolicy().hasHeightForWidth());
  btn->setSizePolicy(sizePolicy2);
  btn->setDefault(true);
  btn->setText(btnCaption);
  sm_horizontalLayout->addWidget(btn);
  
  
sm_verticalLayout->addWidget(sm_groupBox);
  
/************************************************************/
  
  this->setModal(showModal);

  /* форму сразу не показываем. показываем, только если ответа 
   * нет в течение 1 секунды и более при срабатывании сигнала sigNewSecond */
//  this->show();
  
  connect(btn, SIGNAL(clicked()), this, SLOT(slotCanceled()));
  
  timers = new SvSecondMeterTimers(interval);
  
  connect(timers, SIGNAL(sigNewSecond()), this, SLOT(slotNewSecond()));
  connect(timers, SIGNAL(sigTimeout()), this, SLOT(slotTimeout()));
  
  if(runOnCreate) timers->start();
  
}

SvSecondMeter::~SvSecondMeter()
{ 
  close();
  deleteLater();
}

void SvSecondMeter::start()
{
  timers->start();
  this->status = smsRunned;
}

void SvSecondMeter::slotStopTimers()
{
  disconnect(timers, SIGNAL(sigNewSecond()), this, SLOT(slotNewSecond()));
  disconnect(timers, SIGNAL(sigTimeout()), this, SLOT(slotTimeout()));
  
  timers->terminate();
  while(!timers->wait()) QApplication::processEvents();
  free(timers);
}

void SvSecondMeter::slotNewSecond()
{
  this->show();
  
  current_second++;
  
  current_time = current_time.addSecs(1);
  
  this->lblTimer->setText(QString("%1").arg(current_time.toString("hh:mm:ss")));
}

void SvSecondMeter::slotTimeout()
{
  this->slotStopTimers();
  this->status = smsTimeout;
}

void SvSecondMeter::slotCanceled()
{
  this->slotStopTimers();
  this->status = smsCanceled;
}

void SvSecondMeter::slotDone()
{
  this->slotStopTimers();
  this->status = smsDoneOk;
}

void SvSecondMeterTimers::run()
{
  
   QTimer secondMeter;
   secondMeter.setInterval(1000);
   secondMeter.setSingleShot(false);
   connect(&secondMeter, SIGNAL(timeout()), this, SIGNAL(sigNewSecond()));
   
   QTimer generalTimer;
   generalTimer.setInterval(thread_interval);
   generalTimer.setSingleShot(true);
   connect(&generalTimer, SIGNAL(timeout()), this, SIGNAL(sigTimeout()));
   
   secondMeter.start();
   generalTimer.start();

   exec();
}

SvSecondMeterTimers::~SvSecondMeterTimers()
{

}
