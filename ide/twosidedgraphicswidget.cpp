#include "twosidedgraphicswidget.h"

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
    for (int i = 0; i < 2; ++i)
    {
        m_proxyWidgets[i] = 0;
    }
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
    }

    GraphicsWidget *proxy = new GraphicsWidget;
    proxy->setWidget(widget);

    if (m_proxyWidgets[index])
    {
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

void TwoSidedGraphicsWidget::flip()
{
    m_delta = (m_current == 0 ? 9 : -9);
    animateFlip();
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
    if(event->type() == QEvent::MouseButtonDblClick)
    {

        m_parent->flip();
        return true;
    }
    else if(event->type() == QEvent::MouseMove)
    {
        QMouseEvent *evt = static_cast<QMouseEvent*>(event);
        if(evt->buttons() == Qt::LeftButton)
        {
            QWidget* widget = m_parent->currentWidget();
            QPoint step = widget->mapToParent(evt->pos() - m_offset);
            if(step.x() > 5 || step.y()>5)
                widget->move( step );
            //qDebug()<<"move "<<evt->pos();
        }

    }
    else if(event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent *evt = static_cast<QMouseEvent*>(event);
        if(evt->buttons() == Qt::LeftButton)
        {
            m_offset = evt->pos();
            qDebug()<<"press "<<m_offset;
        }
    }

    return QObject::eventFilter(obj, event);
}
