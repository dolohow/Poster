#ifndef ADDFILEDIALOG_H
#define ADDFILEDIALOG_H

#include <QDialog>
#include <QCloseEvent>
#include "libs/wsqlquery.h"

namespace Ui {
    class AddFileDialog;
}

class AddFileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddFileDialog(QWidget *parent = 0);
    ~AddFileDialog();
    void clearFields();
    void setup(int edit = 0);

signals:
    void enableParent(bool enable);

private:
    Ui::AddFileDialog *ui;
    void addFile();

    int editId;
private slots:
    void on_buttonBox_rejected();
    void on_buttonBox_accepted();
signals:
    void changed();

protected:
    void closeEvent(QCloseEvent *event);
};

#endif // ADDFILEDIALOG_H
