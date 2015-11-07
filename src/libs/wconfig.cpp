#include "wconfig.h"

WConfig::WConfig()
{
    settingsModel.setTable("settings");
    settingsModel.setSort(7, Qt::AscendingOrder);
    settingsModel.select();
    QSqlQuery q = settingsModel.query();
    if(!q.exec())
    {
        qDebug() << q.lastError().text();
    }
    while(q.next())
    {
        QMap<QString, QString> map;
        map.insert("key", q.value(1).toString());
        map.insert("value", q.value(2).toString());
        map.insert("type", q.value(3).toString());
        map.insert("label", q.value(4).toString());
        map.insert("description", q.value(5).toString());
        map.insert("options", q.value(6).toString());
        map.insert("ordering", q.value(7).toString());
        this->insert(map["key"], map);
    }
}

void WConfig::save()
{
    WSqlQuery q;
    q.prepare("UPDATE settings SET value = ? WHERE key = ?");
    WConfig::iterator i;
    for(i = this->begin(); i != this->end(); i++)
    {
        q.addBindValue(i.value()["value"]);
        q.addBindValue(i.key());
        q.exec();
    }
}
