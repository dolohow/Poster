#ifndef REPORTDIALOG_H
#define REPORTDIALOG_H

#include <QDialog>
#include "ui_reportdialog.h"
namespace Ui {
    class ReportDialog;
}

class ReportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ReportDialog(QWidget *parent = 0);
    ~ReportDialog();

    Ui::ReportDialog *ui;

private slots:
    void on_pushButton_pressed();
};

#endif // REPORTDIALOG_H
