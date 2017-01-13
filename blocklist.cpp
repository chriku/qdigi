#include "blocklist.h"
#include <QDir>
#include "settings.h"

QList<Block*> BlockList::blocks;

BlockList::BlockList(QObject *parent) : QObject(parent)
{
    if(blocks.isEmpty())
    {
        QDir dir(Settings::final()->applicationDir());
        dir.cd("modules");
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

Block* BlockList::newBlock(QString name)
{
    for(int i=0;i<blocks.length();i++)
        if(blocks[i]->name==name)
            return blocks[i]->clone();
    return NULL;
}
