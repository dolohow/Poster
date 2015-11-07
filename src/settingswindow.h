#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QMainWindow>
#include <QtSql/QSqlTableModel>
#include <QtSql/QSqlQuery>
#include <QDebug>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QSpinBox>
#include "libs/wconfig.h"

namespace Ui {
    class SettingsWindow;
}

class SettingsWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SettingsWindow(WConfig *conf, QWidget *parent = 0);
    ~SettingsWindow();

private:
    QSqlTableModel *model;
    Ui::SettingsWindow *ui;
    WConfig *config;
    QMap<QString, QWidget*> map;
private slots:
    void on_pushButton_clicked();
};

#endif // SETTINGSWINDOW_H
