#include "loggingmodel.h"

/**! Tree Item */

TreeItem::TreeItem(const QList<QVariant> &data, TreeItem *parentItem)
{
    m_parentItem = parentItem;
    m_itemData = data;
}

TreeItem::~TreeItem()
{
    qDeleteAll(m_childItems);
}

void TreeItem::appendChild(TreeItem *item)
{
    m_childItems.append(item);
}
TreeItem *TreeItem::child(int row)
{
    return m_childItems.value(row);
}

int TreeItem::childCount() const
{
    return m_childItems.count();
}

int TreeItem::columnCount() const
{
    return m_itemData.count();
}

QVariant TreeItem::data(int column) const
{
    return m_itemData.value(column);
}

int TreeItem::row() const
{
    if (m_parentItem)
        return m_parentItem->m_childItems.indexOf(const_cast<TreeItem*>(this));

    return 0;
}

TreeItem *TreeItem::parentItem()
{
    return m_parentItem;
}

/**! Logging model */

LoggingModel::LoggingModel(QObject *parent) : QAbstractItemModel(parent)
{
    QList<QVariant> rootData;
    rootData<<"Date"<<"Time"<<"Type"<<"Program"<<"Summary";
    rootItem = new TreeItem(rootData);
    //setupModelData(data.split(QString("\n")), rootItem);

}

LoggingModel::~LoggingModel()
{
    delete rootItem;
}

QVariant LoggingModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

    return item->data(index.column());
}

Qt::ItemFlags LoggingModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return QAbstractItemModel::flags(index);
}

QVariant LoggingModel::headerData(int section, Qt::Orientation orientation,
                                  int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem->data(section);

    return QVariant();
}

QModelIndex LoggingModel::index(int row, int column, const QModelIndex &parent)
const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TreeItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    TreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex LoggingModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
    TreeItem *parentItem = childItem->parentItem();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int LoggingModel::rowCount(const QModelIndex &parent) const
{
    TreeItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}

int LoggingModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
    else
        return rootItem->columnCount();
}

void LoggingModel::appendModelData(const QStringList &lines)
{
    TreeItem* p;
    TreeItem* parent = rootItem;
    QList<QVariant> data;
    for(int i=0; i<lines.size(); ++i)
    {
        data<<lines.at(i);
    }
    if(lines.at(0).isEmpty() && rootItem->childCount()>0)
    {
        parent = rootItem->child(rootItem->childCount()-1);
    }
    p = new TreeItem(data, parent);
    parent->appendChild(p);
}
