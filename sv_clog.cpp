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

  if(p_options.logging)
  {
    if(p_check_log_level && level > p_options.log_level)
      return;

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
