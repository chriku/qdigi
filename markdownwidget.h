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
    QPicture render();
    void resize(int w, int h);
signals:

public slots:
};

#endif // MARKDOWNWIDGET_H
