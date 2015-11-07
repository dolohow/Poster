#include "fileslistwindow.h"
#include "ui_fileslistwindow.h"
#include <QMessageBox>

FilesListWindow::FilesListWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FilesListWindow)
{
    ui->setupUi(this);
    setWindowIcon(QIcon("uploader.ico"));
    ui->filesTable->horizontalHeader()->setStretchLastSection(true);
}

FilesListWindow::~FilesListWindow()
{
    delete ui;
}

void FilesListWindow::setup(QSqlRelationalTableModel *model, AddFileDialog *dialog)
{
    filesModel = model;
    filesModel->setHeaderData(0, Qt::Horizontal, QObject::tr("ID"));
    filesModel->setHeaderData(1, Qt::Horizontal, QObject::tr("Nazwa pliku"));
    filesModel->setHeaderData(2, Qt::Horizontal, QObject::tr("Wzorzec"));
    filesModel->setHeaderData(3, Qt::Horizontal, QObject::tr("Url tematu"));
    filesModel->setHeaderData(4, Qt::Horizontal, QObject::tr("Opis"));
    filesModel->setHeaderData(5, Qt::Horizontal, QObject::tr("Obrazki"));
    filesModel->setHeaderData(6, Qt::Horizontal, QObject::tr("Szablon"));
    filesModel->setEditStrategy(QSqlTableModel::OnFieldChange);
    filesModel->select();

    ui->filesTable->setItemDelegate(new QSqlRelationalDelegate(ui->filesTable));
    ui->filesTable->setSortingEnabled(true);
    ui->filesTable->setModel(filesModel);
    ui->filesTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->filesTable->hideColumn(0);

    fileDialog = dialog;
}

void FilesListWindow::resizeEvent(QResizeEvent *event)
{
    ui->filesTable->resizeColumnsToContents();
    ui->filesTable->horizontalHeader()->setStretchLastSection(true);
}

void FilesListWindow::on_addButton_pressed()
{
    fileDialog = new AddFileDialog;
    fileDialog->setup();
    fileDialog->show();
    connect(fileDialog, SIGNAL(changed()), this, SLOT(refresh()));
}

void FilesListWindow::refresh()
{
    filesModel->select();
    ui->filesTable->setModel(filesModel);
}

void FilesListWindow::on_editButton_pressed()
{
    QModelIndexList indexes = ui->filesTable->selectionModel()->selection().indexes();
    for (int i = 0; i < indexes.count(); ++i)
    {
        QModelIndex index = indexes.at(i);
        if(index.column() == 0)
        {
            AddFileDialog *d = new AddFileDialog;
            d->setup(index.data().toInt());
            d->move(300+10*i,100+10*i);
            d->show();
            connect(d, SIGNAL(changed()), this, SLOT(refresh()));
        }
    }
}

void FilesListWindow::on_deleteButton_pressed()
{
    QModelIndexList indexes = ui->filesTable->selectionModel()->selection().indexes();
    for (int i = 0; i < indexes.count(); ++i)
    {
        WSqlQuery q, r;
        q.prepare("DELETE FROM files WHERE id = ?");
        r.prepare("DELETE FROM mirrors WHERE file_id = ?");
        QModelIndex index = indexes.at(i);
        if(index.column() == 0)
        {
            q.addBindValue(index.data().toInt());
            q.exec();
            r.addBindValue(index.data().toInt());
            r.exec();
        }
    }
    refresh();
}
