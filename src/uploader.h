#ifndef UPLOADER_H
#define UPLOADER_H

#include <QMainWindow>
#include <QtSql>

/*
 * Additional windows
 */
#include "fileslistwindow.h"
#include "templateslistwindow.h"
#include "mirrorslistwindow.h"

/*
 * Additional dialogs
 */
#include "addfiledialog.h"
#include "logindialog.h"
#include "addtemplatedialog.h"
#include "progressdialog.h"
#include "reportdialog.h"
#include "settingswindow.h"

/*
 * Libraries
 */
#include "forumbot.h"
#include "libs/wconfig.h"
#include "stdlib.h"

namespace Ui {
    class Uploader;
}


struct AddEntry
{
    QString topicUrl;
    QString title;
    QString body;
};



class Uploader : public QMainWindow
{
    Q_OBJECT

public:
    explicit Uploader(QWidget *parent = 0);
    ~Uploader();

    /*
     * Database object
     */
    QSqlDatabase db;

    /*
     * Models
     */
    QSqlTableModel *settingsModel;
    QSqlTableModel *templatesModel;
    QSqlRelationalTableModel *filesModel;
    QSqlRelationalTableModel *mirrorsModel;


private:
    /*
     * Qt UI Class
     */
    Ui::Uploader *ui;

    /*
     * Dialogs
     */
    AddFileDialog *fileDialog;
    LoginDialog *loginDialog;
    AddTemplateDialog *templateDialog;
    ProgressDialog progressDialog;
    ReportDialog reportDialog;
    /*
     * Windows
     */
    FilesListWindow filesListWindow;
    TemplatesListWindow templatesListWindow;
    MirrorsListWindow mirrorsListWindow;
    SettingsWindow *settingsWindow;

    /*
     * Libraries
     */
    ForumBot forumBot;
    WConfig *config;

    /*
     * Forum posts to add
     */
    QList< AddEntry > entries;
    QList< AddEntry > unfinished;

    /*
     * Functions
     */

    void sendData();

    /*
     * Other
     */
    int time, randomTime, addCount, totalCount, errorCount;
    QTimer timer;
    QString linksLeft;
    bool uploadBreak;
    QList<QString> authServers;

private slots:
    void on_actionSettings_triggered();
    void on_actionEditMirror_triggered();
    void on_actionEditTemplate_triggered();
    void on_actionAddTemplate_triggered();
    void on_actionEditFile_triggered();
    void on_pushButton_pressed();
    void on_actionClose_triggered();
    void on_actionAddFile_triggered();
    void afterLoginForm(QString login, QString password, QString url, QString forumType);

    void prepareData();

    void sendNext();
    void sendNextSuccess();
    void sendNextFailure();

    void getSimple();

    void onLoginFailure();
    void displayReport();

protected:
    void closeEvent(QCloseEvent *event);

signals:
    void sendingFinished();
    void enableParent(bool enable = true);
};
#endif // UPLOADER_H
