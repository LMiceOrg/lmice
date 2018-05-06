#include "twosidedgraphicswidget.h"
#include <QtWidgets>

TwoSidedGraphicsWidget::TwoSidedGraphicsWidget(QGraphicsScene *scene)
    : QObject(scene)
    , m_current(0)
    , m_angle(0)
    , m_delta(0)
{
    for (int i = 0; i < 2; ++i)
        m_proxyWidgets[i] = 0;

    m_filter = new DoublePressFilter(this);
}


TwoSidedGraphicsWidget::~TwoSidedGraphicsWidget()
{

}

void TwoSidedGraphicsWidget::setWidget(int index, QWidget *widget)
{
    if (index < 0 || index >= 2)
    {
        qWarning("TwoSidedGraphicsWidget::setWidget: Index out of bounds, index == %d", index);
        return;
    }


    widget->installEventFilter(m_filter);
    foreach(auto child, widget->children())
    {
        child->installEventFilter(m_filter);
        foreach(auto c, child->children())
        {
            c->installEventFilter(m_filter);
        }
    }

    GraphicsWidget *proxy = new GraphicsWidget;
    proxy->setWidget(widget);
    proxy->setWindowFlags(Qt::FramelessWindowHint);

    if (m_proxyWidgets[index])
    {
        qobject_cast<QGraphicsScene *>(parent())->removeItem(m_proxyWidgets[index]);
        m_proxyWidgets[index]->removeEventFilter(m_filter);
        delete m_proxyWidgets[index];
        m_proxyWidgets[index]=0;
    }
    m_proxyWidgets[index] = proxy;

    proxy->setCacheMode(QGraphicsItem::ItemCoordinateCache);
    proxy->setZValue(1e30); // Make sure the dialog is drawn on top of all other (OpenGL) items

    if (index != m_current)
        proxy->setVisible(false);

    qobject_cast<QGraphicsScene *>(parent())->addItem(proxy);
}

QWidget *TwoSidedGraphicsWidget::widget(int index)
{
    if (index < 0 || index >= 2)
    {
        qWarning("TwoSidedGraphicsWidget::widget: Index out of bounds, index == %d", index);
        return 0;
    }
    return m_proxyWidgets[index]->widget();
}

QObject *TwoSidedGraphicsWidget::filter()
{
    return m_filter;
}

QWidget *TwoSidedGraphicsWidget::currentWidget()
{
    return m_proxyWidgets[m_current]->widget();
}

QList<GraphicsWidget *> TwoSidedGraphicsWidget::proxyWidget() const
{
    QList<GraphicsWidget*> list;
    list<<m_proxyWidgets[0]<<m_proxyWidgets[1];
    return list;
}

void TwoSidedGraphicsWidget::flip()
{
    m_delta = (m_current == 0 ? 9 : -9);
    animateFlip();
}

void TwoSidedGraphicsWidget::clear()
{
    for (int i = 0; i < 2; ++i)
    {
        if (m_proxyWidgets[i])
        {
            qobject_cast<QGraphicsScene *>(parent())->removeItem(m_proxyWidgets[i]);
            m_proxyWidgets[i]->removeEventFilter(m_filter);
            delete m_proxyWidgets[i];
            m_proxyWidgets[i]=0;
        }
    }
}

void TwoSidedGraphicsWidget::animateFlip()
{
    m_angle += m_delta;
    if (m_angle == 90) {
        int old = m_current;
        m_current ^= 1;
        m_proxyWidgets[old]->setVisible(false);
        m_proxyWidgets[m_current]->setVisible(true);
        m_proxyWidgets[m_current]->setGeometry(m_proxyWidgets[old]->geometry());
    }

    QRectF r = m_proxyWidgets[m_current]->boundingRect();
    m_proxyWidgets[m_current]->setTransform(QTransform()
        .translate(r.width() / 2, r.height() / 2)
        .rotate(m_angle - 180 * m_current, Qt::YAxis)
        .translate(-r.width() / 2, -r.height() / 2));

    if ((m_current == 0 && m_angle > 0) || (m_current == 1 && m_angle < 180))
        QTimer::singleShot(25, this, SLOT(animateFlip()));

    if( (m_current == 1 && m_angle >= 180) || (m_current==0 && m_angle <= 0))
    {
        m_proxyWidgets[m_current]->setActive(true);
    }
}


DoublePressFilter::DoublePressFilter(TwoSidedGraphicsWidget *parent)
{
    m_parent=parent;
}

bool DoublePressFilter::eventFilter(QObject *obj, QEvent *event)
{
    QMouseEvent *evt = static_cast<QMouseEvent*>(event);
    if(event->type() == QEvent::MouseButtonDblClick)
    {
        if(evt->buttons() == Qt::RightButton)
        {
            m_parent->flip();
            return true;
        }
    }
    else if(event->type() == QEvent::MouseMove)
    {
        //qDebug()<<"moving...";
        QMouseEvent *evt = static_cast<QMouseEvent*>(event);
        QWidget* widget = m_parent->currentWidget();
        QPoint pnt = evt->pos() - m_offset;

        if(evt->buttons() == Qt::RightButton )
        {

            if(abs(pnt.x()) > 5 || abs(pnt.y()) > 5)
                widget->resize((m_size.width() + pnt.x())/5*5, (m_size.height()+pnt.y())/5*5);
        }
        else if(evt->buttons() == Qt::LeftButton)
        {

            QPoint step = widget->mapToParent(pnt);
            if(abs(step.x()) > 5 || abs(step.y()) > 5)
                widget->move( step.x()/5*5, step.y()/5*5);


        }

    }
    else if(event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent *evt = static_cast<QMouseEvent*>(event);

        if(evt->buttons() == Qt::LeftButton)
        {
            QWidget* widget = m_parent->currentWidget();
            m_offset = evt->pos();
            m_size = widget->size();
            //qDebug()<<"press "<<m_offset<<widget;
        }
        else if(evt->buttons() == Qt::RightButton)
        {
            QWidget* widget = m_parent->currentWidget();
            m_offset = evt->pos();
            m_size = widget->size();
        }
    }


    return QObject::eventFilter(obj, event);
}
