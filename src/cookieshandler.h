#ifndef COOKIESHANDLER_H
#define COOKIESHANDLER_H
#include <QObject>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkCookieJar>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QDebug>
#include <QTextBrowser>

class CookiesHandler: public QObject{
    Q_OBJECT

public:
    CookiesHandler(QObject *parent = 0) : QObject(parent){
        mManager = new QNetworkAccessManager(this);
        mManager->setCookieJar(new QNetworkCookieJar(this));
        connect(mManager, SIGNAL(finished(QNetworkReply*)), SLOT(replyFinished(QNetworkReply*)));
    }

    QNetworkReply *sendPostRequest(const QUrl &url, const QByteArray &data){
        mUrl = url;
        QList<QNetworkCookie>  cookies = mManager->cookieJar()->cookiesForUrl(mUrl);
        QVariant var;
        var.setValue(cookies);

        QNetworkRequest r(mUrl);
        r.setHeader(QNetworkRequest::CookieHeader, var);
        return mManager->post(r, data);
    }

    QNetworkReply *sendGetRequest(const QUrl &url)
    {
        mUrl = url;
        QList<QNetworkCookie>  cookies = mManager->cookieJar()->cookiesForUrl(mUrl);
        QVariant var;
        var.setValue(cookies);

        QNetworkRequest r(mUrl);
        r.setHeader(QNetworkRequest::CookieHeader, var);
        return mManager->get(r);
    }

    virtual ~CookiesHandler(){}

private slots:
    void replyFinished(QNetworkReply *reply){
        if (reply->error() != QNetworkReply::NoError){
            qWarning() << "ERROR:" << reply->errorString();
            return;
        }
/*
        QTextBrowser *b = new QTextBrowser;
        b->setText(reply->readAll());
        if(reply->hasRawHeader("Location"))
        {
            b->setText(reply->header(QNetworkRequest::LocationHeader).toString());
        }
        b->show();*/

    }
public:
    QNetworkAccessManager *mManager;
    QUrl mUrl;
};


#endif // COOKIESHANDLER_H
