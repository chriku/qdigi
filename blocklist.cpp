#include "blocklist.h"
#include <QDir>

QList<Block*> BlockList::blocks;

BlockList::BlockList(QObject *parent) : QObject(parent)
{
    if(blocks.isEmpty())
    {
        QDir rootDir("../newDigi/modules/");
        QStringList dirs=rootDir.entryList(QDir::Dirs|QDir::NoDotAndDotDot);
        if(dirs.contains("base"))
        {
            dirs.removeAll("base");
            dirs.append("base");
        }
        for(int i=0;i<dirs.length();i++)
        {
            QDir dir(rootDir.absoluteFilePath(dirs[i]));
            QStringList files=dir.entryList();
            for(int i=0;i<files.length();i++)
                if(files[i].endsWith(".lua"))
                {
                    Block*b=new Block;
                    b->load(dir.absoluteFilePath(files[i]));
                    blocks.append(b);
                }
        }
    }
}

Block* BlockList::newBlock(QString name)
{
    for(int i=0;i<blocks.length();i++)
        if(blocks[i]->name==name)
            return blocks[i]->clone();
}
