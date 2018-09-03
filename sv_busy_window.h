#ifndef SV_BUSY_WINDOW_H
#define SV_BUSY_WINDOW_H

#include <QObject>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLabel>


class SvBusyWindow : public QDialog
{
  Q_OBJECT
  
public:
  explicit SvBusyWindow(QString caption, QWidget *parent = 0)
  {
    setParent(parent);
    setFixedSize(325, 80);
  
    /***********  создаем форму ********************/
    sm_verticalLayout = new QVBoxLayout(this);
    sm_verticalLayout->setObjectName(QStringLiteral("sm_verticalLayout"));
  
    lblCaption = new QLabel(this);
    lblCaption->setObjectName(QStringLiteral("lblCaption"));
    lblCaption->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    lblCaption->setText(caption);
    lblCaption->setAlignment(Qt::AlignCenter);
    QFont fnt = lblCaption->font();
    fnt.setBold(true);
    lblCaption->setFont(fnt);
    sm_verticalLayout->addWidget(lblCaption);
    
  /************************************************************/
    
    setWindowFlags(Qt::Popup);
    setModal(true);
    show();
  
  }
  
  ~SvBusyWindow() { delete sm_verticalLayout; delete lblCaption; deleteLater();  }
    
private:
  QVBoxLayout *sm_verticalLayout;
  QLabel *lblCaption;

};

#endif // SV_BUSY_WINDOW_H
