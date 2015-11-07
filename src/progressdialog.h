#ifndef PROGRESSDIALOG_H
#define PROGRESSDIALOG_H

#include <QDialog>
#include "forumbot.h"
#include "ui_progressdialog.h"
namespace Ui {
    class ProgressDialog;
}

class ProgressDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ProgressDialog(QWidget *parent = 0);
    ~ProgressDialog();
    Ui::ProgressDialog *ui;

signals:
    void breakUpload();

private slots:
    void on_pushButton_pressed();
};

#endif // PROGRESSDIALOG_H
