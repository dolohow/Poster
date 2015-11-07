#include "settingswindow.h"
#include "ui_settingswindow.h"

SettingsWindow::SettingsWindow(WConfig *conf, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);
    config = conf;

    WConfig::iterator i;
    for(i = config->begin(); i != config->end(); i++)
    {
        QWidget *widget;
        QString type = i.value()["type"];

        qDebug() << type;

        if(type == "password")
        {
            QLineEdit *lineEdit = new QLineEdit(i.value()["value"]);
            lineEdit->setEchoMode(QLineEdit::Password);
            widget = lineEdit;
        }
        if(type == "varchar")
        {
             widget = new QLineEdit(i.value()["value"]);
        }
        if(type == "text")
        {
            widget = new QPlainTextEdit(i.value()["value"]);
        }
        if(type == "spinbox")
        {
            QSpinBox *spinBox = new QSpinBox();
            spinBox->setMaximum(1000);
            spinBox->setValue(i.value()["value"].toInt());
            widget = spinBox;
        }
        if(type == "combobox")
        {
            QComboBox *comboBox = new QComboBox();
            comboBox->addItem(i.value()["value"]);
            comboBox->addItems(i.value()["options"].split(","));
            widget = comboBox;
        }
        ui->formLayout->addRow(i.value()["label"], widget);
        map.insert(i.value()["key"], widget);
    }
}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}

void SettingsWindow::on_pushButton_clicked()
{
    QMap<QString, QWidget*>::iterator i;
    for(i = map.begin(); i != map.end(); i++)
    {
        QString type = (*config)[i.key()]["type"];
        if(type == "password" || type == "varchar")
        {
            QLineEdit *widget = (QLineEdit*)i.value();
            (*config)[i.key()]["value"] = widget->text();
        }
        else if(type == "text")
        {
            QPlainTextEdit *widget = (QPlainTextEdit*)i.value();
            (*config)[i.key()]["value"] = widget->toPlainText();
        }
        else if(type == "spinbox")
        {
            QSpinBox *widget = (QSpinBox*)i.value();
            (*config)[i.key()]["value"] = QString::number(widget->value());
        }
        else if(type == "combobox")
        {
            QComboBox *widget = (QComboBox*)i.value();
            (*config)[i.key()]["value"] = widget->itemText(widget->currentIndex());
        }
        qDebug() << i.key() << (*config)[i.key()]["value"];
    }
    config->save();
    close();
}
