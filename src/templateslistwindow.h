#ifndef TEMPLATESLISTWINDOW_H
#define TEMPLATESLISTWINDOW_H

#include <QMainWindow>
#include <QtSql>
#include "addtemplatedialog.h"

namespace Ui {
    class TemplatesListWindow;
}

class TemplatesListWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit TemplatesListWindow(QWidget *parent = 0);
    ~TemplatesListWindow();
    void setup(QSqlTableModel *model, AddTemplateDialog *dialog);
private:
    Ui::TemplatesListWindow *ui;
    AddTemplateDialog *templateDialog;

    QSqlTableModel *templatesModel;
protected:
    void resizeEvent(QResizeEvent *event);

public slots:
    void refresh();
private slots:
    void on_deleteButton_pressed();
    void on_editButton_pressed();
    void on_addButton_pressed();
};

#endif // TEMPLATESLISTWINDOW_H
