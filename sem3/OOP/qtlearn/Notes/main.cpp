#include "mainwindow.h"
#include "notebook.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps, true);
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling, true);

    QApplication a(argc, argv);

    Notebook notebook;
    MainWindow w{notebook};
    w.show();
    return a.exec();
}
