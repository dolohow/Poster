#include "templateslistwindow.h"
#include "ui_templateslistwindow.h"
#include <QMessageBox>

TemplatesListWindow::TemplatesListWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TemplatesListWindow)
{
    ui->setupUi(this);
    setWindowIcon(QIcon("uploader.ico"));
    ui->templatesTable->horizontalHeader()->setStretchLastSection(true);
}

TemplatesListWindow::~TemplatesListWindow()
{
    delete ui;
}

void TemplatesListWindow::setup(QSqlTableModel *model, AddTemplateDialog *dialog)
{
    templatesModel = model;
    templatesModel->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
    templatesModel->setHeaderData(1, Qt::Horizontal, QObject::tr("Nazwa szablonu"));
    templatesModel->setHeaderData(2, Qt::Horizontal, QObject::tr("Szablon"));
    templatesModel->setEditStrategy(QSqlTableModel::OnFieldChange);
    templatesModel->select();

    ui->templatesTable->setItemDelegate(new QSqlRelationalDelegate(ui->templatesTable));
    ui->templatesTable->setSortingEnabled(true);
    ui->templatesTable->setModel(templatesModel);
    ui->templatesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->templatesTable->hideColumn(0);

    templateDialog = dialog;
}

void TemplatesListWindow::resizeEvent(QResizeEvent *event)
{
    ui->templatesTable->resizeColumnsToContents();
    ui->templatesTable->horizontalHeader()->setStretchLastSection(true);
}

void TemplatesListWindow::on_addButton_pressed()
{
    templateDialog = new AddTemplateDialog;
    templateDialog->setup();
    templateDialog->show();
    connect(templateDialog, SIGNAL(changed()), this, SLOT(refresh()));
}

void TemplatesListWindow::refresh()
{
    templatesModel->select();
    ui->templatesTable->setModel(templatesModel);
}

void TemplatesListWindow::on_editButton_pressed()
{
    QModelIndexList indexes = ui->templatesTable->selectionModel()->selection().indexes();
    for (int i = 0; i < indexes.count(); ++i)
    {
        QModelIndex index = indexes.at(i);
        if(index.column() == 0)
        {
            AddTemplateDialog *d = new AddTemplateDialog;
            d->setup(index.data().toInt());
            d->move(300+10*i,100+10*i);
            d->show();
            connect(d, SIGNAL(changed()), this, SLOT(refresh()));
        }
    }
}

void TemplatesListWindow::on_deleteButton_pressed()
{
    QModelIndexList indexes = ui->templatesTable->selectionModel()->selection().indexes();
    for (int i = 0; i < indexes.count(); ++i)
    {
        WSqlQuery q;
        q.prepare("DELETE FROM templates WHERE id = ?");
        QModelIndex index = indexes.at(i);
        if(index.column() == 0)
        {
            q.addBindValue(index.data().toInt());
            q.exec();
        }
    }
    refresh();
}
