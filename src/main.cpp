#include <QApplication>

#include "main_window.h"

int main(int argc, char* argv[]) {
    QApplication application(argc, argv);
    QApplication::setApplicationName(QString::fromUtf8(u8"Garbage Panel"));

    MainWindow mainWindow;
    mainWindow.show();

    return application.exec();
}
