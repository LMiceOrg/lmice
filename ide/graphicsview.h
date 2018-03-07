#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include <QtWidgets>
#include <QObject>

class GraphicsView : public QGraphicsView
{

public:
    explicit GraphicsView();

signals:

public slots:

protected:
    void resizeEvent(QResizeEvent *event) override;
};

#endif // GRAPHICSVIEW_H
