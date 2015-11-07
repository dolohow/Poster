#include "addfiledialog.h"
#include "ui_addfiledialog.h"

AddFileDialog::AddFileDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddFileDialog)
{
    /*
     * Add template options
     */
    ui->setupUi(this);
    setWindowIcon(QIcon("uploader.ico"));
    editId = 0;
}

void AddFileDialog::setup(int edit)
{
    WSqlQuery q;
    q.exec("SELECT * FROM templates");
    while(q.next())
    {
        this->ui->templatesBox->addItem(q.value(1).toString(), q.value(0).toString());
    }
    if(edit)
    {
        q.prepare("SELECT * FROM files WHERE id = ?");
        q.addBindValue(edit);
        if(q.exec())
        {
            q.next();

            ui->headerLabel->setText(QString("Edycja pliku \"")+q.value(1).toString()+"\"");
            this->setWindowTitle("Edycja pliku");

            this->ui->fileNameEdit->setText(q.value(1).toString());
            this->ui->forumLinkEdit->setText(q.value(3).toString());
            this->ui->linksEdit->clear();
            this->ui->patternEdit->setText(q.value(2).toString());
            this->ui->imgEdit->setPlainText(q.value(5).toString());
            this->ui->descEdit->setPlainText(q.value(4).toString());
            this->ui->templatesBox->setCurrentIndex(ui->templatesBox->findData(q.value(6).toInt()));
            qDebug() << q.value(6).toInt();
        }
        else
        {
            QMessageBox::information(0,"B³¹d","Taki plik nie istnieje!");
        }
    }
    editId = edit;
}

AddFileDialog::~AddFileDialog()
{
    delete ui;
}

void AddFileDialog::on_buttonBox_accepted()
{
    /*
     * TODO: ADD DATA VALIDATION
     */
    this->addFile();
    this->close();
    this->clearFields();
}

void AddFileDialog::on_buttonBox_rejected()
{
    this->close();
    this->clearFields();

    emit enableParent(true);
}

void AddFileDialog::clearFields()
{
    this->ui->fileNameEdit->clear();
    this->ui->forumLinkEdit->clear();
    this->ui->linksEdit->clear();
    this->ui->patternEdit->clear();
    this->ui->imgEdit->clear();
    this->ui->descEdit->clear();

    emit enableParent(true);
}


void AddFileDialog::closeEvent(QCloseEvent *event)
{
    emit enableParent(true);
    event->accept();
}

void AddFileDialog::addFile()
{
    WSqlQuery query;
    if(editId) query.prepare("UPDATE files SET name = ?, pattern = ?, url = ?, description = ?, screenshots = ?, template_id = ? WHERE id = ?");
    else query.prepare("INSERT INTO files(name, pattern, url, description, screenshots, template_id) VALUES(?,?,?,?,?,?)");
    query.addBindValue(this->ui->fileNameEdit->text());
    query.addBindValue(this->ui->patternEdit->text());
    query.addBindValue(this->ui->forumLinkEdit->text());
    query.addBindValue(this->ui->descEdit->toPlainText());
    query.addBindValue(this->ui->imgEdit->toPlainText());
    query.addBindValue(this->ui->templatesBox->itemData(this->ui->templatesBox->currentIndex()).toString());
    if(editId) query.addBindValue(editId);
    query.exec();

    emit changed();
}
