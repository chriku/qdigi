#include "dragview.h"
#include "blocklist.h"

DragView::DragView(QWidget *parent) : QTableWidget(parent)
{
    setDragEnabled(true);
    makeCols();
}

void DragView::dropEvent(QDropEvent *event)
{

}

void DragView::makeCols()
{
    clear();
    BlockList list;
    int wid=floor(width()/100.0);
    setColumnCount(wid);
    setRowCount((list.blocks.length()/wid)+1);
    for(int i=0;i<list.blocks.length();i++)
    {
        QPixmap pix=list.blocks[i]->drawBlock();
        QTableWidgetItem* item=new QTableWidgetItem(QIcon(pix),list.blocks[i]->name);
        int col=i%wid;
        int row=i/wid;
        setItem(row,col,item);
    }
    int rwidth=width()/wid;
    rwidth-=3;
    for(int i=0;i<wid;i++)
    {
        setColumnWidth(i,rwidth);
    }
}

void DragView::resizeEvent(QResizeEvent *event)
{
    QTableWidget::resizeEvent(event);
    makeCols();
}
