#include "sv_fnt.h"

QString svfnt::replace_re(QString raw_string, svfnt::SvRE re)
{
    QString date_fmt = re.date_format.isEmpty() ? svfnt::DATE_FORMAT1 : re.date_format;
    QString time_fmt = re.time_format.isEmpty() ? svfnt::TIME_FORMAT1 : re.time_format;
    QString date_time_fmt = re.date_time_format.isEmpty() ? svfnt::DATE_TIME_FORMAT1 : re.date_time_format;

    QString result = raw_string.replace(svfnt::RE_DATE, re.date_time.date().toString(date_fmt))
                               .replace(svfnt::RE_TIME, re.date_time.time().toString(time_fmt))
                               .replace(svfnt::RE_DATE_TIME, re.date_time.toString(date_time_fmt));

    for(svfnt::REPair val: re.relist)
      result = result.replace(val.first, val.second);

    return result;

}

QString svfnt::get_folder_path(QString raw_path, svfnt::SvRE re)
{
    QString path = svfnt::replace_re(raw_path, re);

    QDir dir(QDir::currentPath());

    if(!dir.mkpath(path))
        return "";

    dir.setPath(path);

    path = QDir::toNativeSeparators(dir.absolutePath());
    path += QDir::separator();

    return path;

}

QString svfnt::get_file_path(QString raw_path, QString raw_file, svfnt::SvRE re)
{
    QString path = svfnt::get_folder_path(raw_path, re);
    if(path.isEmpty()) return path;

    QString fn = svfnt::replace_re(raw_file, re);

    path += fn;

    return path;

}
