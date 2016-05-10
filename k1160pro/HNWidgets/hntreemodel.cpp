#include "hntreemodel.h"
#include <QStandardItem>

HNTreeModel::HNTreeModel(QObject *parent, HNFileSystem* fs) :
    QStandardItemModel(parent), m_fs(fs)
{
    connect(m_fs, SIGNAL(result()),
            this, SLOT(result()));
}

bool HNTreeModel::query(QString path)
{
    m_path = path;
    bool ret = m_fs->query(path);

    return ret;
}

bool HNTreeModel::removeRow(int arow, const QModelIndex &aparent)
{
    //fs
    QString prot, name;
    m_fs->parse(m_path, prot, name);


    QString path = data(index(aparent.row(), 1)).toString();
    QString file = data(index(arow, 0, aparent)).toString();

    QString sPath = QString("%1%2/%3").arg(prot).arg(path).arg(file);
    m_fs->del(sPath);
    return QStandardItemModel::removeRow(arow, aparent);
}

void HNTreeModel::result()
{


    QString prot, file;
    m_fs->parse(m_path, prot, file);

    pline() << m_path << prot << file;


    HNFilesInfo files = m_fs->record();

    if(prot != files.m_prot)
        return;

    QList<QStandardItem*> itemList = findItems(file, Qt::MatchExactly, FILE_FILEPATH);
    pline() << "找到文件夹数目" << itemList.size() << prot << file;

    if(itemList.size() == 0)
    {
        removeRows(0, rowCount());
        setColumnCount(FILE_MAX);
        setRowCount(0);
        int row = 0;
        QListIterator<HNFileInfo> itor(files.m_filelist);
        while(itor.hasNext())
        {
            HNFileInfo f = itor.next();
            pline() << f.m_path << f.m_fileName << f.m_filePath;
            insertRows(row, 1);
            setData(index(row, FILE_NAME), f.m_fileName);
            setData(index(row, FILE_PATH), f.m_path);
            setData(index(row, FILE_FILEPATH), f.m_filePath);
            row++;
        }
    }
    else
    {
        QStandardItem *dir = item(itemList.at(0)->row(), 0);
        dir->removeRows(0, dir->rowCount());
        dir->setColumnCount(FILE_MAX);
        dir->setRowCount(0);

        int row = 0;
        QListIterator<HNFileInfo> itor(files.m_filelist);
        while(itor.hasNext())
        {
            HNFileInfo f = itor.next();
            pline() << f.m_path << f.m_fileName << f.m_filePath;
            dir->insertRows(row, 1);
            setData(index(row, FILE_NAME, dir->index()), f.m_fileName);
            setData(index(row, FILE_PATH, dir->index()), f.m_path);
            setData(index(row, FILE_FILEPATH, dir->index()), f.m_filePath);
            row++;
        }
    }


    submit();
}