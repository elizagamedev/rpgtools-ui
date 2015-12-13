#include "mainwindow.h"

#include <QApplication>
#include <QMessageBox>
#include <QDir>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //Look for tools
    QDir tooldir(a.applicationDirPath() + "/tools");
    if (!tooldir.exists()) {
        QMessageBox(QMessageBox::Critical, a.tr("Error"), a.tr("No tools folder found."), QMessageBox::Ok).exec();
        return 1;
    }

    MainWindow w(tooldir);
    w.show();

    return a.exec();
}
