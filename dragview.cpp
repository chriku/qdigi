#include "dragview.h"
#include <QApplication>
#include "blocklist.h"
#include <QDrag>
#include <QMimeData>
#include <QDragLeaveEvent>

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
    int w=width();
    w-=QApplication::style()->pixelMetric(QStyle::PM_ScrollBarExtent);
        int rwidth=(w-2)/wid;
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

void DragView::startDrag(Qt::DropActions supportedActions)
{
    QDrag *drag = new QDrag(this);
    QPixmap map(1,1);
    map.fill(Qt::transparent);
    drag->setPixmap(map);
    const QModelIndexList indexes = selectedIndexes();
    drag->setMimeData( model()->mimeData( indexes ) );
    drag->exec(supportedActions);
}
