#ifndef HELPMANAGER_H
#define HELPMANAGER_H

#include <QNetworkAccessManager>

class HelpManager : public QNetworkAccessManager
{
public:
    HelpManager();
    QNetworkReply* createRequest(Operation op, const QNetworkRequest &req, QIODevice *outgoingData);
};

#endif // HELPMANAGER_H
