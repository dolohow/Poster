#ifndef MIRRORSLISTWINDOW_H
#define MIRRORSLISTWINDOW_H

#include <QMainWindow>
#include <QtSql>
#include "libs/wsqlquery.h"

namespace Ui {
    class MirrorsListWindow;
}

class MirrorsListWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MirrorsListWindow(QWidget *parent = 0);
    ~MirrorsListWindow();

    void setup(QSqlRelationalTableModel *model);

private:
    Ui::MirrorsListWindow *ui;

    QSqlRelationalTableModel *mirrorsModel;
protected:
    void resizeEvent(QResizeEvent *event);

public slots:
    void refresh();
private slots:
    void on_deleteButton_pressed();
    void on_addButton_pressed();
};

#endif // MIRRORSLISTWINDOW_H
