/****************************************************************************
**
** Copyright (C) 2016 Michael Geschwill
** Im Kleinen Eschle 21
** 78054 Villingen-Schwenningen
** hpmacher@gmail.com
**
****************************************************************************/

#ifndef DEPENDLISTNODE_H
#define DEPENDLISTNODE_H

#include <QList>
#include <QVariant>

class DependListNode
{
public:
    explicit DependListNode(const QList<QVariant> &data, DependListNode *parentItem = 0);
    ~DependListNode();

    void appendChild(DependListNode *child);

    DependListNode *child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    void setdata(int column, QVariant data);
    int row() const;
    DependListNode *parentItem();
    void removeAllChilds();

private:
    QList<DependListNode*> m_childItems;
    QList<QVariant> m_itemData;
    DependListNode *m_parentItem;
};

#endif // DEPENDLISTNODE_H
