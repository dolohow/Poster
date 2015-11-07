#ifndef FORUMBOT_H
#define FORUMBOT_H

#include <QString>
#include <QtNetwork>
#include <QCryptographicHash>
#include "cookieshandler.h"

class ForumBot : public QObject
{
    Q_OBJECT

public:
    ForumBot();
    void login(QString login, QString password, QString url, QString forumType);
    void addPost(QString topicUrl, QString title, QString body);

    bool isLoggedIn;

private slots:
    void loginFinished();
    void addPostFinished(QString);
private:
    /*
     * Use it ONLY for sign in process
     */
    QNetworkReply *reply;
    CookiesHandler handler;
    QTextCodec *codec;
    QString sid, loginData, url, type;
    QSignalMapper mapper;
signals:
    void loginSuccess();
    void loginFailure();
    void addPostSuccess();
    void addPostFailure();
};

#endif // FORUMBOT_H
