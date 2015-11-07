#include "addtemplatedialog.h"
#include "ui_addtemplatedialog.h"

AddTemplateDialog::AddTemplateDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddTemplateDialog)
{
    /*
     * Add template options
     */
    ui->setupUi(this);
    setWindowIcon(QIcon("uploader.ico"));
    editId = 0;
}

void AddTemplateDialog::setup(int edit)
{
    WSqlQuery q;
    if(edit)
    {
        q.prepare("SELECT * FROM templates WHERE id = ?");
        q.addBindValue(edit);
        if(q.exec())
        {
            q.next();

            ui->headerLabel->setText(QString("Edycja szablonu \"")+q.value(1).toString()+"\"");
            this->setWindowTitle("Edycja szablonu");

            this->ui->nameEdit->setText(q.value(1).toString());
            this->ui->textEdit->setPlainText(q.value(2).toString());
        }
        else
        {
            QMessageBox::information(0,"B³¹d","Taki szablon nie istnieje!");
        }
    }
    editId = edit;
}

AddTemplateDialog::~AddTemplateDialog()
{
    delete ui;
}

void AddTemplateDialog::on_buttonBox_accepted()
{
    /*
     * TODO: ADD DATA VALIDATION
     */
    this->addTemplate();
    this->close();
    this->clearFields();
}

void AddTemplateDialog::on_buttonBox_rejected()
{
    this->close();
    this->clearFields();
}

void AddTemplateDialog::clearFields()
{
    this->ui->nameEdit->clear();
    this->ui->textEdit->clear();
}


void AddTemplateDialog::closeEvent(QCloseEvent *event)
{
    event->accept();
}

void AddTemplateDialog::addTemplate()
{
    WSqlQuery query;
    if(editId) query.prepare("UPDATE templates SET name = ?, `text` = ? WHERE id = ?");
    else query.prepare("INSERT INTO templates(name, `text`) VALUES(?,?)");
    query.addBindValue(this->ui->nameEdit->text());
    query.addBindValue(this->ui->textEdit->toPlainText());
    if(editId) query.addBindValue(editId);
    query.exec();

    emit changed();
}
