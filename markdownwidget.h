#ifndef MARKDOWNWIDGET_H
#define MARKDOWNWIDGET_H

#include <QWidget>
#include <QPicture>

class MarkDownWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MarkDownWidget(QWidget *parent = 0);
    void setText(QString text);
    QString text;
    void paintEvent(QPaintEvent *event);
    QPixmap render();
    void resize(int w, int h);
    QPixmap renderText(QString text, int width, QList<QPair<QRect, QString> > *linkP);
    QPixmap renderHeadline(QString text,int width,int size);
    QPixmap renderUL(QStringList items,int width, QList<QPair<QRect, QString> > *linkP);
    QPixmap renderOL(QStringList items,int width, QList<QPair<QRect, QString> > *linkP);
    QPixmap renderLine(int width);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    QList<QPair<QRect,QString> > links;
signals:
void linkClicked(QString);
public slots:
};

#endif // MARKDOWNWIDGET_H
