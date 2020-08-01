#include "sv_widget_log.h"

QMutex wlogmutex;

/** SvWidgetLogger **/
sv::SvWidgetLogger::~SvWidgetLogger()
{
  delete _log_edit;
  delete _dockWidget;
}

QDockWidget *sv::SvWidgetLogger::createDockWidgetLog(QMainWindow *parent, const QString &title, QDockWidget::DockWidgetFeature feature)
{
  if (!parent) {

      _log_edit = new QTextEdit;
      _log_edit->show();

      return nullptr;

  }
  else {

    _dockWidget = new QDockWidget(parent);
//    _dockWidget->setObjectName(QStringLiteral("logDockWidget"));
    _dockWidget->setWindowTitle(title);
    _dockWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    _dockWidget->setFeatures(feature);
    _dockWidget->setMinimumSize(QSize(100, 50));
    _dockWidget->setMaximumSize(QSize(16635, 16635));

//    QWidget *wDockContents = new QWidget();
//    wDockContents->setObjectName(QStringLiteral("wDockContents"));

//    QVBoxLayout *verticalLayout = new QVBoxLayout(wDockContents);
//    verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
//    verticalLayout->setMargin(2);

    _log_edit = new QTextEdit(_dockWidget);
//    _log_edit->setObjectName(QStringLiteral("_log_edit"));

//    verticalLayout->addWidget(_log_edit);

//    _dockWidget->setWidget(wDockContents);
//    window->addDockWidget(static_cast<Qt::DockWidgetArea>(4), _dockWidget);

    return _dockWidget;

  }

}

void sv::SvWidgetLogger::bindTo(QTextEdit *widget)
{
  if (widget)
    _log_edit = widget;

}

void sv::SvWidgetLogger::log(sv::log::Level level, sv::log::MessageTypes type, const QString text, log::sender sender, bool newline)
{
  if(p_options.logging)
  {

    if(p_check_log_level && level > p_options.log_level)
      return;

    if(!_log_edit) {
        qInfo() << text;
        return;
    }

    wlogmutex.lock();
    _log_edit->setTextColor(typesColors.value(type));
    _log_edit->append(text); // setPlainText(text + (newline ? "\n" : "")); // append(text);
    wlogmutex.unlock();

    if(newline)
      p_current_line_num++;

    p_current_line = "";

    if(type == sv::log::mtCritical)
      QMessageBox::critical(0, "Error", text, QMessageBox::Ok);

  }

  resetCurrentData();

}

