#ifndef SCHEMATIC_H
#define SCHEMATIC_H

#include <QObject>

class Schematic : public QObject
{
    Q_OBJECT
public:
    explicit Schematic(QObject *parent = 0);

signals:

public slots:
};

#endif // SCHEMATIC_H