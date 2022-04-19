#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    std::shared_ptr<MainWindow> mainWindow = std::make_shared<MainWindow>();
    mainWindow->show();
    return QApplication::exec();
}
