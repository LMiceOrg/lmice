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

    GraphicsWidget *proxy = new GraphicsWidget;
    proxy->setWidget(widget);

    if (m_proxyWidgets[index])
    {
        m_proxyWidgets[index]->removeEventFilter(m_filter);
        delete m_proxyWidgets[index];
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

    return QObject::eventFilter(obj, event);
}
