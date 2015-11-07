#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QCloseEvent>
#include "libs/wconfig.h"

namespace Ui {
    class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(WConfig *config, QWidget *parent);
    ~LoginDialog();

signals:
    void dataSend(QString login, QString password, QString url, QString forumType);
    void enableParent(bool enable = true);

private:
    Ui::LoginDialog *ui;
    WConfig *config;
private slots:
    void on_comboBox_currentIndexChanged(QString );
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

protected:
    void closeEvent(QCloseEvent *event);
};

#endif // LOGINDIALOG_H
