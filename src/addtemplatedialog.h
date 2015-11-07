#ifndef ADDTEMPLATEDIALOG_H
#define ADDTEMPLATEDIALOG_H

#include <QDialog>
#include <QCloseEvent>
#include "libs/wsqlquery.h"

namespace Ui {
    class AddTemplateDialog;
}

class AddTemplateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddTemplateDialog(QWidget *parent = 0);
    ~AddTemplateDialog();
    void clearFields();
    void setup(int edit = 0);

private:
    Ui::AddTemplateDialog *ui;
    void addTemplate();

    int editId;

private slots:
    void on_buttonBox_rejected();
    void on_buttonBox_accepted();
signals:
    void changed();

protected:
    void closeEvent(QCloseEvent *event);
};

#endif // ADDTEMPLATEDIALOG_H
