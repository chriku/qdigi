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
    int wid=floor(width()/100.0);
    setColumnCount(wid);
    setRowCount(ceil(double(blockList.length())/wid));
    for(int i=0;i<blockList.length();i++)
    {
        QPixmap pix=blockList[i]->drawBlock();
        QTableWidgetItem* item=new QTableWidgetItem(QIcon(pix),blockList[i]->name);
        int col=i%wid;
        int row=i/wid;
        setItem(row,col,item);
    }
    int rwidth=(width()-2)/wid;
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
