#include <QtGui/QApplication>
#include "uploader.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Uploader w;
    w.show();

    return a.exec();
}
