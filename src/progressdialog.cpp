#include "progressdialog.h"
#include "ui_progressdialog.h"

ProgressDialog::ProgressDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProgressDialog)
{
    ui->setupUi(this);
    setWindowIcon(QIcon("uploader.ico"));
}

ProgressDialog::~ProgressDialog()
{
    delete ui;
}

void ProgressDialog::on_pushButton_pressed()
{
    emit breakUpload();
    this->hide();
}
