#include "MainWindow.h"


//orientierung nach ubungsblatt9 /Anlage

// main Programm übergibt das QApplication-Objekt
// Mainwindow wird gezeigt

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    MainWindow window;

    window.show();

    return QApplication::exec();
}