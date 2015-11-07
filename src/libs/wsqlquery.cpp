#include "wsqlquery.h"

bool WSqlQuery::exec()
{
    bool test;
    if(!(test = QSqlQuery::exec()))
    {
        QMessageBox::information(0, "B��d SQL", this->lastError().text()+this->lastQuery());
    }
    return test;
}

bool WSqlQuery::exec(const QString& q)
{
    bool test;
    if(!(test = QSqlQuery::exec(q)))
    {
        QMessageBox::information(0, "B��d SQL", this->lastError().text()+this->lastQuery());
    }
    return test;
}

bool WSqlQuery::prepare(const QString& q)
{
    bool test;
    if(!(test = QSqlQuery::prepare(q)))
    {
        QMessageBox::information(0, "B��d SQL (podpinanie parametru)", this->lastError().text());
    }
    return test;
}
