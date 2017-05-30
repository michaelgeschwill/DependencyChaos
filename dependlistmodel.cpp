/****************************************************************************
**
** Copyright (C) 2016 Michael Geschwill
** Im Kleinen Eschle 21
** 78054 Villingen-Schwenningen
** hpmacher@gmail.com
**
****************************************************************************/

#include "dependlistmodel.h"
#include <QColor>

DependListModel::DependListModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    QList<QVariant> rootData;
    rootData << "File" << "ID / Compatibility" << "Bits" << "Errors" << "Found here";
    rootItem = new DependListNode(rootData);
}

DependListModel::~DependListModel()
{
    delete rootItem;
}

QModelIndex DependListModel::index(int row, int column, const QModelIndex &parent)
            const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    DependListNode *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<DependListNode*>(parent.internalPointer());

    DependListNode *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}


QModelIndex DependListModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    DependListNode *childItem = static_cast<DependListNode*>(index.internalPointer());
    DependListNode *parentItem = childItem->parentItem();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int DependListModel::rowCount(const QModelIndex &parent) const
{
    DependListNode *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<DependListNode*>(parent.internalPointer());

    return parentItem->childCount();
}

int DependListModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<DependListNode*>(parent.internalPointer())->columnCount();
    else
        return rootItem->columnCount();
}

QVariant DependListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::ForegroundRole )
    {
        DependListNode *item = static_cast<DependListNode*>(index.internalPointer());
        if ( item->data(3).toInt()>0 )
        {
            return QVariant( QColor( Qt::red ) );
        }
        return QVariant( QColor( Qt::darkGreen ) );
    }

    if (role != Qt::DisplayRole)
        return QVariant();

    DependListNode *item = static_cast<DependListNode*>(index.internalPointer());
    return item->data(index.column());
}

Qt::ItemFlags DependListModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return QAbstractItemModel::flags(index);
}

QVariant DependListModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem->data(section);

    return QVariant();
}



void DependListModel::setupModelData(QStringList *lines, DependListNode *parent)
{
    rootItem->removeAllChilds();
    QList<DependListNode*> parents;
    QList<int> indentations;
    parents << rootItem;
    indentations << 0;

    int number = 0;

    while (number < lines->count()) {
        int position = 0;
        while (position < lines->at(number).length()) {
            if (lines->at(number).at(position) != ' ')
                break;
            position++;
        }

        QString lineData = lines->at(number).mid(position).trimmed();

        if (!lineData.isEmpty()) {
            // Read the column data from the rest of the line.
            QStringList columnStrings = lineData.split("\t"); //, QString::SkipEmptyParts);
            QList<QVariant> columnData;
            for (int column = 0; column < columnStrings.count(); ++column)
                columnData << columnStrings[column];

            if (position > indentations.last()) {
                // The last child of the current parent is now the new parent
                // unless the current parent has no children.

                if (parents.last()->childCount() > 0) {
                    parents << parents.last()->child(parents.last()->childCount()-1);
                    indentations << position;
                }
            } else {
                while (position < indentations.last() && parents.count() > 0) {
                    parents.pop_back();
                    indentations.pop_back();
                }
            }

            // Append a new item to the current parent's list of children.
            parents.last()->appendChild(new DependListNode(columnData, parents.last()));
        }

        ++number;
    }

}

QVariant DependListModel::getData(const QModelIndex &index, int column)
{
    if (!index.isValid())
        return QVariant();

    DependListNode *item = static_cast<DependListNode*>(index.internalPointer());

    return item->data(column);
}

void DependListModel::setMyData(const QModelIndex &index, int column, QVariant value)
{
    if (!index.isValid())
        return;

    DependListNode *item = static_cast<DependListNode*>(index.internalPointer());

    item->setdata(column,value);
}
