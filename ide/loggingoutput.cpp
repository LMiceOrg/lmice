#include "loggingoutput.h"

#include <QRegularExpression>
#include <QCoreApplication>
#include <QApplication>
#include <QtWidgets>

LoggingOutput::LoggingOutput(QGraphicsScene *scene) : QObject(scene)
{
    m_scene = scene;
    m_widget = new TwoSidedGraphicsWidget(scene);

    m_model =new LoggingModel(this);
    m_view1 = new QTreeView();
    m_view1->setModel(m_model);


    //m_view1->setPalette(Qt::red);
    //m_view1->viewport()->setPalette(Qt::red);

    m_widget->setWidget(0, m_view1);
    m_setting= new CandleStickSetting();
    m_widget->setWidget(1, m_setting);

    //QPalette pal = m_view1->palette();

//    m_widget->proxyWidget()[0]
//            ->setContentsMargins(20,20,20,20);
    //m_view1->viewport()->setContentsMargins(20,20,20,-1);
    //m_setting->setContentsMargins(1,1,1,1);
    QSize size = qApp->primaryScreen()->availableSize()/4;
    m_view1->resize(size);

    this->outputMessage("2018-03-11 04:33:50.4338190 INFO:[7373:0x7fff9e8fb3c0]updated setting\nhello world!\n\n");
    this->outputMessage("[7373:0x7fff9e8fb3c0]updated setting");

    //m_view1->expandAll();
}

LoggingOutput::~LoggingOutput()
{

}

QList<GraphicsWidget *> LoggingOutput::graphicsWidgets() const
{
    return m_widget->proxyWidget();
}

void LoggingOutput::outputMessage(const QString &str)
{

    QRegularExpression pattern = QRegularExpression("^([0-9-]+)\\s"
                                                    "([0-9:.]+)\\s"
                                                    "(\\w+)[:]"
                                                    "[\\[]([0-9:a-zA-Z_]+)[\\]]"
                                                    "(.*)");
    //2018-03-11 04:33:50.4338190 INFO:[7373:0x7fff9e8fb3c0]updated setting
    QStringList sl = str.split("\n");
    foreach(QString s, sl)
    {
        QString date;
        QString time;
        QString level;
        QString program;
        QString text;
        QRegularExpressionMatch match;

        if(s.isEmpty())
            continue;

        match = pattern.match(s);

        if(match.hasMatch())
        {
            date = match.captured(1);
            time = match.captured(2);
            level = match.captured(3);
            program = match.captured(4);
            text = match.captured(5);
        }
        else
        {
            text = s;
        }

        QStringList sl;
        sl<<date<<time<<level<<program<<text;
        m_model->appendModelData(sl);
    }


}
