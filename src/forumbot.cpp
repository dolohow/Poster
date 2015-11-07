#include "forumbot.h"
#include <iostream>
#include <QTextBrowser>

ForumBot::ForumBot()
{
    isLoggedIn = false;
    codec = QTextCodec::codecForName("ISO8859-2");
    connect(&mapper, SIGNAL(mapped(QString)), this, SLOT(addPostFinished(QString)));
}

void ForumBot::login(QString login, QString password, QString forumUrl, QString forumType)
{
    url = forumUrl;
    loginData = login;
    type = forumType;
    QUrl params;
    QString loginPath = "";
    if(forumType == "phpbb")
    {
        if(url.contains("darkwarez.pl"))
        {
            params.addQueryItem("usrname", login);
            params.addQueryItem("passwrd", password);
        }
        else
        {
            params.addQueryItem("username", login);
            params.addQueryItem("password", password);
        }
        params.addQueryItem("login", "");
        params.addQueryItem("redirect", "");
        params.addQueryItem("autologin", "");
        loginPath = "login.php";
    }
    else if(forumType == "vbulletin")
    {
        QCryptographicHash h(QCryptographicHash::Md5);
        QString encodedPass = QString(QCryptographicHash::hash(QByteArray(password.toStdString().c_str()),QCryptographicHash::Md5).toHex().constData());

        params.addQueryItem("vb_login_username", login);
        params.addQueryItem("cookieuser", "1");
        params.addQueryItem("do", "login");
        params.addQueryItem("vb_login_password", "");
        params.addQueryItem("vb_login_md5password", encodedPass);
        params.addQueryItem("vb_login_md5password_utf", encodedPass);
        params.addQueryItem("securitytoken", "guest");
        loginPath = "login.php?do=login";
    }
    QByteArray data;
    data.append(params.toString());
    data.remove(0,1);

    reply = handler.sendPostRequest(url+loginPath, data);
    connect(reply, SIGNAL(finished()), this, SLOT(loginFinished()));
}


void ForumBot::loginFinished()
{

    QString content(reply->readAll());

//    QTextEdit *b = new QTextEdit;
//    b ->setPlainText(content);
//    b->show();

//    qDebug() << reply->header(QNetworkRequest::LocationHeader).toString();

    if(content.contains("http-equiv=\"Refresh") || reply->header(QNetworkRequest::LocationHeader).toString() != "")
    {
        isLoggedIn = true;

        int i = 0;
        QList<QNetworkCookie> cookies = handler.mManager->cookieJar()->cookiesForUrl(QUrl(url));
        if(type == "phpbb")
        {
            while(i <= cookies.size())
            {
                qDebug() << cookies.at(i);
                if(cookies.at(i).name().contains("sid") || cookies.at(i).name().contains("SID")) break;
                i++;
            }
            sid = cookies.at(i).value();
        }
        emit loginSuccess();
    }
    else
    {
        emit loginFailure();
    }
}

void ForumBot::addPost(QString topicUrl, QString title, QString body)
{
    QRegExp r;
    QString topic_id;
    if(topicUrl.contains("darkwarez.pl"))
    {
        r.setPattern("\\/(\\d+)");
        r.indexIn(topicUrl);
        topic_id = r.cap(1);
    }
    else if(topicUrl.contains("peb.pl") || topicUrl.contains("precyl.pl"))
    {
        r.setPattern("\\/(\\d+)");
        r.indexIn(topicUrl);
        topic_id = r.cap(1);
    }
    else if(topicUrl.contains("mmocenter.pl"))
    {
        r.setPattern("(\\d+)\\.html");
        r.indexIn(topicUrl);
        topic_id = r.cap(1);
    }
    else
    {
        r.setPattern("t=(\\d+)");
        r.indexIn(topicUrl);
        topic_id = r.cap(1);

        if(topic_id == "")
        {
            r.setPattern("\\/(\\d+)");
            r.indexIn(topicUrl);
            topic_id = r.cap(1);
        }
    }
    if(!topicUrl.contains(url))
    {
        emit addPostFailure();
        return;
    }

    QUrl postPageUrl;
    QUrl params;

    if(type == "phpbb")
    {
        postPageUrl = url+"posting.php";
        postPageUrl.addQueryItem("mode", "reply");
        postPageUrl.addQueryItem("t", topic_id);

        params.addQueryItem("mode", "reply");
        params.addQueryItem("t", topic_id);
        params.addQueryItem("subject", "");//codec->fromUnicode(title).toPercentEncoding().data());
        params.addQueryItem("message", codec->fromUnicode(body).toPercentEncoding().data());
        params.addQueryItem("post", "Wyslij");
        params.addQueryItem("sid", sid);
        params.addQueryItem("attach_sig", "on");
    }
    else if(type == "vbulletin")
    {
        postPageUrl = url+"newreply.php";
        postPageUrl.addQueryItem("do", "postreply");
        postPageUrl.addQueryItem("t", topic_id);

        params.addQueryItem("do", "postreply");
        params.addQueryItem("emailupdate", "1");
        params.addQueryItem("iconid", "0");
        params.addQueryItem("loggedinuser", "postreply");
        params.addQueryItem("message", codec->fromUnicode(body).toPercentEncoding().data());
        params.addQueryItem("multiquoteempty", "");
        params.addQueryItem("p", "");
        params.addQueryItem("posthash", "");
        params.addQueryItem("poststarttime", "");
        params.addQueryItem("rating", "0");

        QNetworkReply *rr = handler.sendGetRequest(topicUrl);
        QEventLoop eventLoop;
        connect(rr, SIGNAL(finished()), &eventLoop, SLOT(quit()));
        eventLoop.exec();

        QString content(rr->readAll());
        r.setPattern("SECURITYTOKEN\\ =\\ \\\"([a-zA-Z0-9\\-]+)\\\"\;");
        r.indexIn(content);
        QString token = r.cap(1);

//        QTextBrowser *b = new QTextBrowser;
//        b ->setPlainText(content);
//        b->setWindowTitle(QString("PREPOST"));
//        b->show();

        qDebug() << "REG: " << "SECURITYTOKEN\\ =\\ \\\"([a-zA-Z0-9\\-]+)\\\"\;";
        qDebug() << "ID: " << topic_id;
        qDebug() << "TOKEN: " << token;


        params.addQueryItem("securitytoken", token);
        params.addQueryItem("specifiedpost", "");
        params.addQueryItem("t", topic_id);
        params.addQueryItem("title", "");//codec->fromUnicode(title).toPercentEncoding().data());
        params.addQueryItem("wysiwyg", "0");
    }

//    qDebug() << "ADDING POST";
//    qDebug() << handler.mManager->cookieJar()->cookiesForUrl(QUrl(topicUrl));
//    qDebug() << params;
    QByteArray data;
    data.append(params.toString());
    data.remove(0,1);

    qDebug() << data;
    qDebug() << topic_id;

    QNetworkReply *rr = handler.sendPostRequest(postPageUrl, data);
    connect(rr, SIGNAL(finished()), &mapper, SLOT(map()));
    mapper.setMapping(rr, topicUrl);

//DEBUG: REMOVE ASAP
/*
    QEventLoop eventLoop;
    connect(rr, SIGNAL(finished()), &eventLoop, SLOT(quit()));
    eventLoop.exec();

    QString content(rr->readAll());

    QTextBrowser *b = new QTextBrowser;
    b ->setText(content);
    b->show();
*/
}

void ForumBot::addPostFinished(QString topicUrl)
{
    QNetworkReply *r = handler.sendGetRequest(topicUrl);
    QEventLoop eventLoop;
    connect(r, SIGNAL(finished()), &eventLoop, SLOT(quit()));
    eventLoop.exec();

    QString content(r->readAll());

//    QTextBrowser *b = new QTextBrowser;
//    b ->setText(content);
//    b->show();

    if(content.contains(loginData))
    {
        emit addPostSuccess();
    }
    else emit addPostFailure();
}
