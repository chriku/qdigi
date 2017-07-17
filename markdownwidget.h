#ifndef MARKDOWNWIDGET_H
#define MARKDOWNWIDGET_H

#include <QWidget>
#include <QPicture>

class HelpDialog;

struct word_t {
    QString text;
    bool bold;
    bool italic;
    int link;
    QString linkT;
    QPixmap rendered;
};
enum TYPE {
    TYPE_EMPTY,
    TYPE_TEXT,
    TYPE_IMAGE,
    TYPE_HEADLINE_1,
    TYPE_HEADLINE_2,
    TYPE_HEADLINE_3,
    TYPE_HEADLINE_4,
    TYPE_HEADLINE_5,
    TYPE_HEADLINE_6,
    TYPE_HR,
    TYPE_OL,
    TYPE_UL
};
struct line_t {
    TYPE type;
    QString content;
};

class MarkDownWidget : public QWidget
{
    Q_OBJECT
public:

HelpDialog* hd;
    explicit MarkDownWidget(QWidget *parent = 0);
    void setText(QString text);
    QString text;
    void paintEvent(QPaintEvent *event);
    QPixmap render();
    void resize(int w, int h);
    QPixmap renderText(QString text, int width, QList<QPair<QRect, QString> > *linkP,int fontSize);
    int getFontSize(TYPE what);
    QPixmap renderImage(QString line, int width);
    QPixmap renderUL(QStringList items,int width, QList<QPair<QRect, QString> > *linkP);
    QPixmap renderOL(QStringList items,int width, QList<QPair<QRect, QString> > *linkP);
    QPixmap renderLine(int width);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    QList<QPair<QRect,QString> > links;
    QPixmap content;
signals:
void linkClicked(QString);
public slots:
};

#endif // MARKDOWNWIDGET_H
