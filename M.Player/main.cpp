#include "widget.h"
#include <QApplication>
#include "QtSingleApplication"

int main(int argc, char *argv[])
{
    QtSingleApplication a(argc, argv);
    if(a.isRunning())
        return 0;
    Widget w;
    w.setFixedSize(320, 120);
    w.show();
    return a.exec();
}
