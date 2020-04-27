#ifndef SV_FNT
#define SV_FNT

#include <QString>
#include <QDateTime>
#include <QDir>
#include <QList>
#include <QPair>


namespace svfnt {

const QString RE_EXT = "{EXT}";
const QString RE_DATE = "{DATE}";
const QString RE_TIME = "{TIME}";
const QString RE_DATE_TIME = "{DATETIME}";
const QString RE_USER = "{USER}";
const QString RE_DEVICE = "{DEVICE}";

const QString DATE_FORMAT1 = "yyyy_MM_dd";
const QString TIME_FORMAT1 = "hh_mm_ss";
const QString DATE_TIME_FORMAT1 = "ddMMyy_hhmmss";

typedef QPair<QString, QString> REPair;
typedef QList<svfnt::REPair> REList;


struct SvRE {
  
  SvRE() { }
  SvRE(QDateTime curDateTime) { date_time = curDateTime; }
  
  QString date_format = svfnt::DATE_FORMAT1;
  QString time_format = svfnt::TIME_FORMAT1;
  QString date_time_format = svfnt::DATE_TIME_FORMAT1;
  
  svfnt::REList relist = svfnt::REList(QList<svfnt::REPair>());
  QDateTime date_time = QDateTime::currentDateTime();
  
};

QString replace_re(QString raw_string, svfnt::SvRE re);

QString get_folder_path(QString raw_path, svfnt::SvRE re);

QString get_file_path(QString raw_path, QString raw_file, svfnt::SvRE re);


//def get_path(config, extention, **kwargs):
//    try:
//        if not (c_workdir in config or c_filename_template in config):
//            raise Exception('config must contains params "filename_template" and "workdir" specifing path and names of created files\nexample: filename_template = home/user/  workdir=test_main')

//        if len(config[c_workdir]) == 0:
//            raise Exception("Work directory not specified")

//        workdir = ''
//        if ('only_filename' not in kwargs) or (bool(kwargs['only_filename']) == False):
//            workdir = get_folder_name(config, extention)

//        filename = replace_re(config, extention, config[c_filename_template])

//        return workdir + filename + '.' + extention


//    except Exception as E:
//        print('error in func get_path(): %s' % E, file=sys.stderr)
//        return None
}


#endif // SV_FNT

