#ifndef GRAPHICSWIDGET_H
#define GRAPHICSWIDGET_H


#include <QWidget>
#include <QGraphicsProxyWidget>
#include <QVariant>

class GraphicsWidget : public QGraphicsProxyWidget
{
public:
    GraphicsWidget();
protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;
    void resizeEvent(QGraphicsSceneResizeEvent *event) override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
};

#endif // GRAPHICSWIDGET_H
