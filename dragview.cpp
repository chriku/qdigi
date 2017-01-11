#include "dragview.h"
#include "blocklist.h"

DragView::DragView(QWidget *parent) : QListWidget(parent)
{
setDragEnabled(true);
BlockList list;
for(int i=0;i<list.blocks.length();i++)
{
    QPixmap pix=list.blocks[i]->drawBlock();
    QListWidgetItem* item=new QListWidgetItem(QIcon(pix),list.blocks[i]->name);
    addItem(item);
}
}

void DragView::dropEvent(QDropEvent *event)
{

}
