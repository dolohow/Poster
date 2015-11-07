#ifndef WSQLQUERY_H
#define WSQLQUERY_H

#include <QtSql>
#include <QMessageBox>

class WSqlQuery : public QSqlQuery
{
public:
    bool exec();
    bool exec(const QString& query);
    bool prepare(const QString& query);
};

#endif // WSQLQUERY_H
