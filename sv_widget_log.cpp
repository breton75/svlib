#include "sv_clog.h"

QMutex logmutex;

/** SvFileLogger **/

bool sv::SvFileLogger::checkFile()
{

NEW_FILE:

  // если файл еще не открыт, то открываем
  if(!p_log_file) {

    p_re.date_time = QDateTime::currentDateTime();
    QString fn = svfnt::get_file_path(p_options.log_directory, p_options.log_filename, p_re);

    const char* mode = p_options.log_truncate_on_rotation ? "w" : "a";

    if((p_log_file = fopen(fn.toStdString().c_str(), mode)) == NULL) {

      _last_error = QString("Файл не может быть открыт: %!1").arg(fn);
      return false;

    }

    if(p_options.log_rotation_age) {

      p_file_time_watcher = QTime::currentTime();
      p_file_time_watcher.start();

    }
  }

  /****** проверяем не истекло ли время записи текущего файла *****/
  if(p_options.log_rotation_age && (quint32(p_file_time_watcher.elapsed()) >= p_options.log_rotation_age * 1000))
  {
    /* если истекло, то закрываем файл и открываем новый */
    fclose (p_log_file);
    p_log_file = nullptr;

    goto NEW_FILE;
  }

  /****** проверяем не превышен ли размер текущего файла *****/
  // эту проверку выполняем только если log_truncate_on_rotation = on
  if(p_options.log_truncate_on_rotation) {

    if(p_options.log_rotation_size) {

      // заполнение структуры типа stat
      struct stat buff;
      fstat(fileno(p_log_file), &buff);

      if((buff.st_size >= p_options.log_rotation_size))
      {
        /* если превышен, то закрываем файл и открываем новый */
        fclose(p_log_file);
        p_log_file = nullptr;

        goto NEW_FILE;
      }
    }
  }

  return true;

}

void sv::SvFileLogger::log(sv::log::Level level, sv::log::MessageTypes type, const QString &text, bool newline)
{
  Q_UNUSED(type);

  if(p_options.logging && (level <= p_options.log_level)) {

    if(!checkFile())
      std::cout << _last_error.toStdString().c_str();

    else {

//      _log_file.write(text.toUtf8()); // для QFilr так не работает в линуксе. хз почему
//      fwrite(text.toStdString().c_str(), text.length(), 1, _log_file); // для FILE* неверно работает fstat

      QTextStream stream(p_log_file);
      stream << QString(text);
      if(newline) stream << '\n';

    }

    if(newline) p_current_line_num++;

  }

  resetCurrentData();

}


/** SvConcoleLogger **/

void sv::SvConcoleLogger::log(sv::log::Level level, sv::log::MessageTypes type, const QString &text, bool newline)
{
  Q_UNUSED(type);

  if(p_options.logging && (level <= p_options.log_level)) {

    std::cout << text.toStdString().c_str();
    if(newline) {

      std::cout << '\n';
      p_current_line_num++;

    }
  }

  resetCurrentData();
  
}


#ifdef GUI_APP

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

void sv::SvWidgetLogger::log(sv::log::Level level, sv::log::MessageTypes type, const QString &text, bool newline)
{
  if(p_options.logging && (level <= p_options.log_level))
  {

    if(!_log_edit) {
      qInfo() << text;
      return;
    }

    logmutex.lock();
    _log_edit->setTextColor(typesColors.value(type));
    _log_edit->append(text); // setPlainText(text + (newline ? "\n" : "")); // append(text);
    logmutex.unlock();

    if(newline)
      p_current_line_num++;

    p_current_line = "";

    if(type == sv::log::mtCritical)
      QMessageBox::critical(0, "Error", text, QMessageBox::Ok);

  }
}

#endif // GUI_APP
