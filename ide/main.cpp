#include <QApplication>
#include <QSurfaceFormat>
#include <QtWidgets>
#include <QGLWidget>

#include "graphicsview.h"
#include "twosidedgraphicswidget.h"
#include "candlestickchart.h"
#include <QDebug>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    bool usegl = true;

    if ((QGLFormat::openGLVersionFlags() & QGLFormat::OpenGL_Version_1_5) == 0) {
        QMessageBox::critical(0, "OpenGL features missing",
                              "OpenGL version 1.5 or higher is required to run this demo.\n"
                              "The program will now exit.");
        usegl = false;
    }

    QGLWidget *widget = NULL;

    qDebug()<<"begin "<<usegl;

    if(usegl)
    {
        QSurfaceFormat format;
        format.setDepthBufferSize(24);
        format.setStencilBufferSize(8);
        format.setVersion(3, 2);
        format.setProfile(QSurfaceFormat::CoreProfile);
        QSurfaceFormat::setDefaultFormat(format);


        widget = new QGLWidget(QGLFormat(QGL::SampleBuffers|QGL::DoubleBuffer));
        widget->makeCurrent();

    }

    QGraphicsScene scene;
    GraphicsView view;
    if(usegl)
    {
        view.setViewport(widget);
        view.setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    }
    view.setScene(&scene);
    view.show();


    CandleStickChart *canchart = new CandleStickChart(&scene);
    canchart = new CandleStickChart(&scene);


    return app.exec();


}
