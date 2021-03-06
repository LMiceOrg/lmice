#ifndef TWOSIDEDGRAPHICSWIDGET_H
#define TWOSIDEDGRAPHICSWIDGET_H

#include <QObject>
#include <QList>

#include <QWidget>


#include "graphicswidget.h"

class TwoSidedGraphicsWidget;

class DoublePressFilter : public QObject
 {
     Q_OBJECT
public:
    DoublePressFilter(TwoSidedGraphicsWidget *parent);
protected:
     bool eventFilter(QObject *obj, QEvent *event);
private:
     TwoSidedGraphicsWidget* m_parent;
     QPoint m_offset;
     QSize m_size;
 };


class TwoSidedGraphicsWidget : public QObject
{
    Q_OBJECT
public:
    TwoSidedGraphicsWidget(QGraphicsScene *scene);
    void setWidget(int index, QWidget *widget);
    QWidget *widget(int index);
    QObject* filter();
    QWidget * currentWidget();

    QList<GraphicsWidget*> proxyWidget() const;

    virtual ~TwoSidedGraphicsWidget();
public slots:
    void flip();
    void clear();
protected slots:
    void animateFlip();
private:
    DoublePressFilter* m_filter;
    GraphicsWidget *m_proxyWidgets[2];
    int m_current;
    int m_angle; // angle in degrees
    int m_delta;
};




#endif // TWOSIDEDGRAPHICSWIDGET_H
