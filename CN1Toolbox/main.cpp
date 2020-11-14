#include "toolbox.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //a.setStyle("fusion");

    Toolbox w;
    w.show();
    return a.exec();
}
