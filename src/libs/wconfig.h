#ifndef WCONFIG_H
#define WCONFIG_H

#include <QWidget>
#include <QMap>
#include <QtSql>
#include "wsqlquery.h"

class WConfig : public QMap<QString, QMap<QString,QString> >
{
    QSqlTableModel settingsModel;

public:
    explicit WConfig();
    void save();

signals:

public slots:

};

#endif // WCONFIG_H
