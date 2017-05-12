#include "markdownwidget.h"
#include <QDebug>
#include <QPicture>
#include <QPainter>

MarkDownWidget::MarkDownWidget(QWidget *parent) : QWidget(parent)
{

}

void MarkDownWidget::setText(QString text)
{
    this->text=text;
    render();
}

void MarkDownWidget::resize(int w, int h)
{
    //render();
}

QPicture MarkDownWidget::render()
{
    QStringList lines=text.split('\n');
    qDebug()<<lines;
}

void MarkDownWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.drawPicture(0,0,render());
}
