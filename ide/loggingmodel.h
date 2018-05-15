#ifndef LOGGINGMODEL_H
#define LOGGINGMODEL_H

#include <QObject>
#include <QList>
#include <QVariant>
#include <QAbstractItemModel>

class TreeItem
 {
 public:
     explicit TreeItem(const QList<QVariant> &data, TreeItem *parentItem = 0);
     ~TreeItem();

     void appendChild(TreeItem *child);

     TreeItem *child(int row);
     int childCount() const;
     int columnCount() const;
     QVariant data(int column) const;
     int row() const;
     TreeItem *parentItem();

 private:
     QList<TreeItem*> m_childItems;
     QList<QVariant> m_itemData;
     TreeItem *m_parentItem;
 };


class LoggingModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit LoggingModel(QObject *parent = nullptr);
      ~LoggingModel();

      QVariant data(const QModelIndex &index, int role) const override;
      Qt::ItemFlags flags(const QModelIndex &index) const override;
      QVariant headerData(int section, Qt::Orientation orientation,
                          int role = Qt::DisplayRole) const override;
      QModelIndex index(int row, int column,
                        const QModelIndex &parent = QModelIndex()) const override;
      QModelIndex parent(const QModelIndex &index) const override;
      int rowCount(const QModelIndex &parent = QModelIndex()) const override;
      int columnCount(const QModelIndex &parent = QModelIndex()) const override;

      void appendModelData(const QStringList &lines);
  private:
      void setupModelData(const QStringList &lines, TreeItem *parent);

      TreeItem *rootItem;
};

#endif // LOGGINGMODEL_H
