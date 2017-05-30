#include "dependlistnode.h"

DependListNode::DependListNode(const QList<QVariant> &data, DependListNode *parent)
{
    m_parentItem = parent;
    m_itemData = data;
}

DependListNode::~DependListNode()
{
    qDeleteAll(m_childItems);
}

void DependListNode::removeAllChilds(){
    m_childItems.clear();
}

void DependListNode::appendChild(DependListNode *item)
{
    m_childItems.append(item);
}

DependListNode *DependListNode::child(int row)
{
    return m_childItems.value(row);
}

int DependListNode::childCount() const
{
    return m_childItems.count();
}

int DependListNode::row() const
{
    if (m_parentItem)
        return m_parentItem->m_childItems.indexOf(const_cast<DependListNode*>(this));

    return 0;
}

int DependListNode::columnCount() const
{
    return m_itemData.count();
}

QVariant DependListNode::data(int column) const
{
    return m_itemData.value(column);
}

void DependListNode::setdata(int column, QVariant data)
{
    m_itemData.replace(column,data);
}

DependListNode *DependListNode::parentItem()
{
    return m_parentItem;
}
