//
// Created by Luke Openshaw on 31/01/2026.
//

#include <QApplication>
#include "MainWindow.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    MainWindow w;
    w.resize(1200, 720);
    w.show();

    return app.exec();
}