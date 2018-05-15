#include <QApplication>
#include <QSurfaceFormat>
#include <QtWidgets>
#include <QGLWidget>
#include <QOpenGLWidget>

#include "graphicsview.h"
#include "twosidedgraphicswidget.h"
#include "candlestickchart.h"
#include <QDebug>

#include "pythonhelper.h"

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

    /* Initnalize Python */
    PythonHelper helper;
    helper.callModel("ioredirect");
    helper.exec("print 'hello 1\\nworld!'");
    helper.callModel("ioreset");
    helper.exec("print 'hello 2'");

    //helper.exec("lmice.ioredirect()", helper.SINGLE_LINE);


//    QString s = "import lmice\n"
//                       "import time\n"
//                       "import traceback\n"
//                       "print dir(lmice)\n"
//                       "t1=time.time()\n"
//                       "c = lmice.ChinaL1Source('/Users/hehao/work/shif/package_2018-03-06__20-50-01_guava2.cap')\n"
//                       "t2=time.time()\n"
//                       "print t2-t1\n"
//                       "print 'msg size=',c.msg_size\n"
//"print c.data_path\n"
//            "c.m_inst=None\n"
//                        "print 'vol=', c.m_volume, type(c.m_volume)\n"
//                       "print 'vol=', c.m_volume, type(c.m_volume)\n"
//                       "for i in range(1):\n"
//                       "    c.at(i)\n"
//                       "    print c.m_inst, c.m_bid\n"
//                       "\n";
//    qDebug()<<s;
//    PyRun_SimpleString(s.toLocal8Bit().data());


//    qDebug()<<"begin "<<usegl;
//    qDebug()<<offsetof(LS_ChinaL1Msg, m_data)<<offsetof(Dummy_ChinaL1Msg, m_notional);
//    qDebug()<<(Py_ssize_t)&((LS_ChinaL1Msg*)(0))->m_data.m_notional<<sizeof(LS_ChinaL1Source);

    if(usegl)
    {
        QSurfaceFormat format;
        format.setDepthBufferSize(24);
        format.setStencilBufferSize(8);
        format.setVersion(3, 2);
        format.setProfile(QSurfaceFormat::CoreProfile);
        format.setSamples(4);
        QSurfaceFormat::setDefaultFormat(format);



        widget = new QGLWidget(QGLFormat(QGL::SampleBuffers|QGL::DoubleBuffer));
        //widget->setMouseTracking(true);

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

    return app.exec();


}
