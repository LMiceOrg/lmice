#ifndef LOGGINGOUTPUT_H
#define LOGGINGOUTPUT_H

#include "candlesticksetting.h"
#include "loggingmodel.h"
#include "twosidedgraphicswidget.h"
#include <QObject>
#include <QTreeView>
#include <QGraphicsScene>
#include <QList>


class LoggingOutput : public QObject
{
    Q_OBJECT
public:
    explicit LoggingOutput(QGraphicsScene *scene);
    virtual ~LoggingOutput();
    QList<GraphicsWidget *> graphicsWidgets() const;

signals:

public slots:
    void outputMessage(const QString&);
private:
    TwoSidedGraphicsWidget *m_widget;
    QGraphicsScene *m_scene;

    LoggingModel* m_model;
    QTreeView* m_view1;
    CandleStickSetting* m_setting;

};

#endif // LOGGINGOUTPUT_H
