// Copyright (C) Christian Stimming, 2010

#include <QtGui/QApplication>
#include "carmainwindow.hpp"


// ////////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    CarMainWindow mainwindow;
    mainwindow.show();

    return app.exec();
}

// Local Variables:
// indent-tabs-mode:nil
// c-basic-offset:4
// tab-width:8
// End:
