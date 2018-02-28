#include <QApplication>
#include <QSurfaceFormat>
#include <QtWidgets>
#include <QGLWidget>

//#include "mainwindow.h"

class GraphicsView : public QGraphicsView
{
public:
    GraphicsView()
    {
        setWindowTitle(tr("Boxes"));
        setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
        //setRenderHints(QPainter::SmoothPixmapTransform);
        QMenuBar *bar= new QMenuBar(0);

        QMenu* file = bar->addMenu("File");
        QAction* about = new QAction("about");
        file->addAction(about);
        QAction* config = new QAction("config");
        file->addAction(config);
        QObject::connect(config, &QAction::triggered, [&]()
        {
            QMessageBox box;
            box.setText("config it");
            box.exec();
        });
        QAction* exit = new QAction("exit");
        file->addAction(exit);
        QObject::connect(exit, &QAction::triggered, [&]()
        {
            qApp->exit(0);
        });

        QAction* open = new QAction("Open");
        file->addAction(open);
        QObject::connect(open, &QAction::triggered, [&]()
        {
            QMessageBox box;
            box.setText("open it");
            box.exec();
        });
    }

protected:
    void resizeEvent(QResizeEvent *event) override {
        if (scene())
            scene()->setSceneRect(QRect(QPoint(0, 0), event->size()));
        QGraphicsView::resizeEvent(event);
    }
};

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


        QGLWidget *widget = new QGLWidget(QGLFormat(QGL::SampleBuffers));
        widget->makeCurrent();

        QGraphicsScene scene;
        GraphicsView view;
        view.setViewport(widget);
        view.setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
        view.setScene(&scene);
        view.show();
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
