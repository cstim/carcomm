// Copyright (C) Christian Stimming, 2010

#include <QtGui/QApplication>
#include "carmainwindow.hpp"
#include "PositionWGS84.hpp"


// ////////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    qRegisterMetaType<PositionWGS84>();

    CarMainWindow mainwindow;
    mainwindow.show();

    return app.exec();
}

// Local Variables:
// indent-tabs-mode:nil
// c-basic-offset:4
// tab-width:8
// End:
