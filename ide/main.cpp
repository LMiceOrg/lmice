#include <QApplication>
#include <QSurfaceFormat>
#include <QtWidgets>
#include <QGLWidget>

#include "graphicsview.h"
#include "twosidedgraphicswidget.h"


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

    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setVersion(3, 2);
    format.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(format);

    if(usegl)
    {


        QGLWidget *widget = new QGLWidget(QGLFormat(QGL::SampleBuffers|QGL::DoubleBuffer));
        widget->makeCurrent();

        QGraphicsScene scene;
        GraphicsView view;
        view.setViewport(widget);
        view.setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
        view.setScene(&scene);
        view.show();

        TwoSidedGraphicsWidget *twoSided = new TwoSidedGraphicsWidget(&scene);
        QLabel *w1= new QLabel("hello 1");
        QLabel *w2= new QLabel("hello 2");
        QPixmap *p2 = new QPixmap("/Users/hehao/Documents/002.jpg");

        w2->setPixmap(p2->scaledToWidth(800));
        twoSided->setWidget(0, w1);
        twoSided->setWidget(1, w2);



        return app.exec();
    }
    else
    {
        QGraphicsScene scene;
        GraphicsView view;

        view.setScene(&scene);
        view.show();
        return app.exec();
    }

}
