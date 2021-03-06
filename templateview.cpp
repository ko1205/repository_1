#include "templateview.h"
#include <QMessageBox>
#include <QShortcut>
#include "preview.h"
#include "common.h"

TemplateView::TemplateView(QWidget *parent)
    :QTreeView(parent)
{

    templateModel = new QStandardItemModel();

    QStringList headerLabels;
    headerLabels<<tr("TemplateView");
    templateModel->setHorizontalHeaderLabels(headerLabels);


    createFolderIcon();
    createRootFolder();

    setModel(templateModel);

    expandAll();
    setSortingEnabled(1);
    sortByColumn(0,Qt::AscendingOrder);
    setDragDropMode(QAbstractItemView::InternalMove);
    setEditTriggers(QAbstractItemView::DoubleClicked);;


//    setDragEnabled(1);
//    setDropIndicatorShown(1);
//    this->viewport()->acceptDrops();
//    setAcceptDrops(true);

    setSelectionMode(QAbstractItemView::ExtendedSelection);

    QShortcut *deleteKey = new QShortcut(QKeySequence(Qt::Key_Delete), this);
//    installEventFilter(this);

    popupMenu = new QMenu(this);
    insertAct = new QAction("CreateFolder",this);
    deleteAct = new QAction("DeleteFolder",this);
    expandAllAct = new QAction("Expand All",this);
    collapseAllAct = new QAction("Collapse All",this);

    popupMenu->addAction(insertAct);
    popupMenu->addAction(deleteAct);
    popupMenu->addSeparator();
    popupMenu->addAction(expandAllAct);
    popupMenu->addAction(collapseAllAct);


//    connect(this,SIGNAL(activated(QModelIndex)),this,SLOT(storOldName(QModelIndex)));
    connect(this,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(storOldName(QModelIndex)));
    connect(templateModel,SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),this,SLOT(checkRename(QModelIndex)));
    connect(insertAct,SIGNAL(triggered(bool)),this,SLOT(insertFolder()));
    connect(deleteAct,SIGNAL(triggered(bool)),this,SLOT(deleteFolder()));
    connect(expandAllAct,SIGNAL(triggered(bool)),this,SLOT(expandAll()));
    connect(collapseAllAct,SIGNAL(triggered(bool)),this,SLOT(collapseAllNRoot()));
    connect(deleteKey,SIGNAL(activated()),this,SLOT(deleteFolder()));
    connect(templateModel,SIGNAL(rowsInserted(QModelIndex,int,int)),this,SLOT(activeStor(QModelIndex,int ,int)));
    connect(this,SIGNAL(clicked(QModelIndex)),this,SLOT(itemClicked(QModelIndex)));
    connect(templateModel,SIGNAL(rowsRemoved(QModelIndex,int,int)),this,SLOT(rowRemovedModel()));
}

TemplateView::~TemplateView()
{

}

void TemplateView::createFolderIcon()
{
    folderIcon.addPixmap(style()->standardPixmap(QStyle::SP_DirClosedIcon),
                         QIcon::Normal, QIcon::Off);
    folderIcon.addPixmap(style()->standardPixmap(QStyle::SP_DirOpenIcon),
                         QIcon::Normal, QIcon::On);
}

void TemplateView::createRootFolder()
{
    rootItem = new QStandardItem(tr("/"));
    rootItem->setIcon(folderIcon);
    templateModel->appendRow(rootItem);
    rootItem->setEditable(false);
}

void TemplateView::showIndex()
{
    int row = this->currentIndex().row();
    QString data =this->currentIndex().data().toString();
    QMessageBox::information(this,"test",QString::number(row)+" "+data,QMessageBox::Yes);
}

void TemplateView::contextMenuEvent(QContextMenuEvent *event)
{
    popupMenu->exec(event->globalPos());
}

void TemplateView::connectPreView(PreView *preview)
{
    previewIns = preview;
}

QStandardItem *TemplateView::root()
{
    return rootItem;
}

QIcon TemplateView::icon()
{
    return folderIcon;
}

void TemplateView::previewUpdate()
{
    previewIns->updatePreVew();
}

void TemplateView::insertFolder()
{
    NewFolderName = QString(tr("NewFolder"));
    QStandardItem *newFolder = new QStandardItem(NewFolderName);
    newFolder->setIcon(folderIcon);
    newFolder->setData(false,Qt::UserRole);
    QModelIndex index= currentIndex();
    if(index==rootIndex() || !index.isValid())
    {
        index = templateModel->indexFromItem(rootItem);
    }

    if(templateModel->hasChildren(index))
    {
        QString autoReName = autoRename(NewFolderName,index);
        newFolder->setData(autoReName,Qt::DisplayRole);
    }
    if(index==rootIndex())
    {
        templateModel->appendRow(newFolder);
    }else{
        templateModel->itemFromIndex(index)->appendRow(newFolder);
    }
    previewIns->updatePreVew();
}

void TemplateView::deleteFolder()
{
    bool hasParent;
    QModelIndexList list = selectedIndexes();
    QList<QStandardItem*> items;
    items.clear();
    foreach (QModelIndex index, list)
    {
        hasParent = false;
        if(rootItem!=templateModel->itemFromIndex(index))
        {
            for(int i=0;i<list.size();i++)
            {
                if(list[i]!=templateModel->indexFromItem(rootItem))
                {
                    if(list[i]==templateModel->parent(index))
                    {
                        hasParent=true;
                    }
                }
            }
            if(!hasParent)
            {
                items.append(templateModel->itemFromIndex(index));
            }
        }
    }
    foreach (QStandardItem *item, items) {
        QModelIndex deleteIndex= templateModel->indexFromItem(item);
        QModelIndex parent = templateModel->parent(deleteIndex);
        int row = deleteIndex.row();
        templateModel->removeRow(row,parent);
    }
    emit itemDeleted();
//    previewIns->updatePreVew();

}

void TemplateView::collapseAllNRoot()
{
    collapseAll();
    expand(root()->index());
}

void TemplateView::setRootFolderName(const QString &rootName)
{
    rootItem->setData(rootName,Qt::DisplayRole);
    previewIns->updatePreVew();

}

bool TemplateView::hasSameName(QString folderName,QModelIndex &parent)
{
    bool isSameNam=false;
    int count = templateModel->rowCount(parent);
    QModelIndex childIndex;
    QString data;
    for(int i=0;i<count;i++)
    {
        childIndex = templateModel->index(i,0,parent);
        data = templateModel->data(childIndex,Qt::DisplayRole).toString();
        if((isSequencName(folderName).count()!=0) && (isSequencName(data).count()!=0))
        {
            if((isSequencName(folderName)[0].toUpper()==isSequencName(data)[0].toUpper()) &&\
                (isSequencName(folderName)[2].toUpper()==isSequencName(data)[2].toUpper()))
            {
                isSameNam = true;
            }
        }else if(folderName.toUpper() == data.toUpper())
        {
            isSameNam = true;
        }
    }
    return isSameNam;
}

bool TemplateView::hasSameName(const QModelIndex &index,QString folderName,QModelIndex &parent)
{
    bool isSameNam=false;
    int count = templateModel->rowCount(parent);
    QModelIndex childIndex;
    QString data;

    for(int i=0;i<count;i++)
    {
        childIndex = templateModel->index(i,0,parent);
        QString data = templateModel->data(childIndex,Qt::DisplayRole).toString();
        if((isSequencName(folderName).count()!=0) && (isSequencName(data).count()!=0))
        {
            if((isSequencName(folderName)[0].toUpper()==isSequencName(data)[0].toUpper()) &&\
                (isSequencName(folderName)[2].toUpper()==isSequencName(data)[2].toUpper()))
            {
                if(index.row()!=i)
                {
                    isSameNam = true;
                }
            }

        }else if(folderName.toUpper() == data.toUpper())
        {
            if(index.row()!=i)
            {
                isSameNam = true;
            }
        }
    }
    return isSameNam;
}

QString TemplateView::autoRename(QString folderName,QModelIndex &parent,int addNum)
{
    if(hasSameName(folderName,parent))
    {
        QString autoFolderName;
        addNum++;
        autoFolderName = NewFolderName+QString::number(addNum);
        folderName = autoRename(autoFolderName,parent,addNum);
    }
    return folderName;
}

void TemplateView::storOldName(const QModelIndex &index)
{
    oldName = templateModel->data(index,Qt::DisplayRole).toString();
}

void TemplateView::checkRename(const QModelIndex &index)
{
    QModelIndex parent = templateModel->parent(index);
    QString newName = templateModel->data(index,Qt::DisplayRole).toString();
    QRegExp rx = QRegExp("[\\\\ \\/ \\: \? \" \\* \\< \\> \\|]");
    QStringList sequenceNames;

    disconnect(templateModel,SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),this,SLOT(checkRename(QModelIndex)));

    if(hasSameName(index,newName,parent))
    {
        templateModel->setData(index,oldName,Qt::DisplayRole);
        QMessageBox::information(this,tr("Information"),tr("already have the same name"),QMessageBox::Yes);
    }else if(newName.contains(rx))
    {
        if(parent!=rootIndex())
       {
            templateModel->setData(index,oldName,Qt::DisplayRole);
            QMessageBox::information(this,"","\\ / : * ? \" < > 는 사용할 수 없습니다.",QMessageBox::Yes);
        }
    }else
    {
        oldName = newName;
    }
    sequenceNames = isSequencName(newName,true);
    if(sequenceNames.count()!=0)
    {
        if(!(templateModel->data(index,Qt::UserRole).toBool()))
        {
            templateModel->setData(index,true,Qt::UserRole);
            templateModel->setData(index,0,Qt::UserRole+1);
            templateModel->setData(index,1,Qt::UserRole+2);
            templateModel->setData(index,1,Qt::UserRole+3);
        }
    }else{
        templateModel->setData(index,false,Qt::UserRole);
        templateModel->setData(index,0,Qt::UserRole+1);
        templateModel->setData(index,0,Qt::UserRole+2);
        templateModel->setData(index,0,Qt::UserRole+3);
    }

    previewIns->updatePreVew();
    if(parent!=rootIndex())
    {
        emit itemClickedView(templateModel->itemFromIndex(index));
    }
    connect(templateModel,SIGNAL(dataChanged(QModelIndex,QModelIndex,QVector<int>)),this,SLOT(checkRename(QModelIndex)));

//    QMessageBox::information(this,"","testABCDEFG",QMessageBox::Yes);
}


void TemplateView::itemClicked(const QModelIndex &index)
{
    QStandardItem *item;

    item = templateModel->itemFromIndex(index);
    if(item!=rootItem)
    {
        emit itemClickedView(item);
    }
}

void TemplateView::rowRemovedModel()
{
    previewIns->updatePreVew();
//    QMessageBox::information(this,"","test",QMessageBox::Yes);
}

void TemplateView::activeStor(const QModelIndex &index,int start,int end)
{
    QString data = templateModel->data(index,Qt::DisplayRole).toString();
//    QMessageBox::information(this,"TEST",data+" "+QString::number(start)+" "+QString::number(end),QMessageBox::Yes);
}




void TemplateView::dropEvent(QDropEvent *event)
{
    QModelIndexList selectList = selectedIndexes();
    QString compare;
    QString compareTo;

    //////////////////////////////////////////////////////////////////
    /// \brief index
    /// private Class 의 DropOn 함수 기능 제정의
    ///////////////////////////////////////////////////////////////

    QModelIndex parent;
        // rootIndex() (i.e. the viewport) might be a valid index

    if (viewport()->rect().contains(event->pos())) {
        parent = indexAt(event->pos());
        if (!parent.isValid() || !visualRect(parent).contains(event->pos()))
            parent = rootIndex();
    }

    if(parent != rootIndex())
    {


        foreach(QModelIndex index ,selectList)
        {
            for(int i=0;i<selectList.size();i++)
            {
                if(index!=selectList[i])
                {
                    compare = templateModel->data(index,Qt::DisplayRole).toString().toUpper();
                    compareTo = templateModel->data(selectList[i],Qt::DisplayRole).toString().toUpper();
                    if((isSequencName(compare).count()!=0) && (isSequencName(compareTo).count()!=0))
                    {
                        if((isSequencName(compare)[0].toUpper()==isSequencName(compareTo)[0].toUpper()) &&\
                            (isSequencName(compare)[2].toUpper()==isSequencName(compareTo)[2].toUpper()))
                        {
                            QMessageBox::information(this,tr("Information"),tr("selected Folder have same name"),QMessageBox::Yes);
                            return;
                        }

                    }else if(compare==compareTo)
                    {
                        QMessageBox::information(this,tr("Information"),tr("selected Folder have same name"),QMessageBox::Yes);
                        return;
                    }
                }
            }
            QString selectedItem = templateModel->data(index,Qt::DisplayRole).toString();
            if(hasSameName(selectedItem,parent))
            {
                QMessageBox::information(this,tr("Information"),tr("already have the same name"),QMessageBox::Yes);
                return;
            }
        }
        QTreeView::dropEvent(event);
    }
    return;
}

QVariant TemplateView::data(const QModelIndex &index, int Role) const
{
    return templateModel->data(index,Role);
}
