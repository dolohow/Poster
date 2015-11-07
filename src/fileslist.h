#ifndef FILESLIST_H
#define FILESLIST_H

#include <QDialog>

namespace Ui {
    class FilesList;
}

class FilesList : public QDialog
{
    Q_OBJECT

public:
    explicit FilesList(QWidget *parent = 0);
    ~FilesList();

private:
    Ui::FilesList *ui;
};

#endif // FILESLIST_H
