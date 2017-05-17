#ifndef HELPREPLY_H
#define HELPREPLY_H

#include <QtNetwork>

class HelpReply : public QNetworkReply
{
    Q_OBJECT
public:
    HelpReply(QNetworkAccessManager::Operation op, const QNetworkRequest &req, QIODevice *outgoingData,QObject* parent=NULL);
    void abort();
    qint64 bytesAvailable() const;
    bool isSequential() const;
    QString isValid(QString html);
signals:

public slots:
void timeout();
protected:
    qint64 readData(char *rdata, qint64 maxSize);

private:
QByteArray data;
};

#endif // HELPREPLY_H
