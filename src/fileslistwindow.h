#ifndef FILESLISTWINDOW_H
#define FILESLISTWINDOW_H

#include <QMainWindow>
#include <QtSql>
#include "addfiledialog.h"

namespace Ui {
    class FilesListWindow;
}

class FilesListWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit FilesListWindow(QWidget *parent = 0);
    ~FilesListWindow();

    void setup(QSqlRelationalTableModel *model, AddFileDialog *dialog);

private:
    Ui::FilesListWindow *ui;
    AddFileDialog *fileDialog;

    QSqlRelationalTableModel *filesModel;
protected:
    void resizeEvent(QResizeEvent *event);

public slots:
    void refresh();
private slots:
    void on_deleteButton_pressed();
    void on_editButton_pressed();
    void on_addButton_pressed();
};

#endif // FILESLISTWINDOW_H
