#include "logindialog.h"
#include "ui_logindialog.h"

LoginDialog::LoginDialog(WConfig *cfg, QWidget *parent = 0) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    setWindowIcon(QIcon("uploader.ico"));

    config = cfg;

    QString forumType = (*config)["default_forum"]["value"];
    qDebug() << forumType;
    ui->loginField->setText((*config)[forumType+"_default_login"]["value"]);
    ui->passField->setText((*config)[forumType+"_default_password"]["value"]);
    ui->urlField->setText((*config)[forumType+"_default_url"]["value"]);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::on_buttonBox_rejected()
{
    emit enableParent(true);
    this->close();
}

void LoginDialog::on_buttonBox_accepted()
{
    emit dataSend(this->ui->loginField->text(), this->ui->passField->text(), this->ui->urlField->text(), this->ui->comboBox->itemText(this->ui->comboBox->currentIndex()).toLower());
    emit enableParent(true);
    this->close();
}


void LoginDialog::closeEvent(QCloseEvent *event)
{
    emit enableParent(true);
    event->accept();
}

void LoginDialog::on_comboBox_currentIndexChanged(QString text)
{
    QString forumType = text.toLower();
    qDebug() << "CHANGE TYPE: " << forumType;
    ui->loginField->setText((*config)[forumType+"_default_login"]["value"]);
    ui->passField->setText((*config)[forumType+"_default_password"]["value"]);
    ui->urlField->setText((*config)[forumType+"_default_url"]["value"]);
}
