#ifndef DEPENDLISTMODEL_H
#define DEPENDLISTMODEL_H

#include <QObject>
#include <QAbstractItemModel>
#include <QModelIndex>
#include <QStringList>
#include "dependlistnode.h"

class DependListModel : public QAbstractItemModel
{
public:
    explicit DependListModel(QObject *parent = 0);
    ~DependListModel();

    QVariant data(const QModelIndex &index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    void setupModelData(QStringList *lines, DependListNode *parent = NULL);
    QVariant getData(const QModelIndex &index, int column);
    void setMyData(const QModelIndex &index, int column, QVariant value);

private:
    DependListNode *rootItem;
};

#endif // DEPENDLISTMODEL_H
