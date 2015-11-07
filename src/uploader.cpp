#include "uploader.h"
#include "ui_uploader.h"
#include <QTableView>
#include <QInputDialog>

Uploader::Uploader(QWidget *parent) : QMainWindow(parent), ui(new Ui::Uploader)
{
    connect(this, SIGNAL(enableParent(bool)), this, SLOT(setEnabled(bool)));
    connect(this, SIGNAL(sendingFinished()), this, SLOT(displayReport()));
    connect(&progressDialog, SIGNAL(breakUpload()), this, SLOT(displayReport()));

    setWindowIcon(QIcon("uploader.ico"));

    /*
     * Database and models setup
     */
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("db.sqlite");
    if(!db.open())
    {
        QMessageBox::information(0, "Blad", QString("Wystapil blad podczas połączenia z baza danych: ")+db.lastError().text());
    }

    /*
     * Compability check
     */
    QString query1 = QString::fromUtf8("CREATE TABLE IF NOT EXISTS [settings] (id integer NOT NULL,key varchar NOT NULL,value text,type varchar,label varchar NOT NULL,description text,PRIMARY KEY (id));");
    QString query2 = QString::fromUtf8("INSERT INTO settings ( id,key,value,type,label,description ) VALUES ( '1','licence_key','TESTOWY','varchar','Klucz licencji','Klucz ktĂłry posĹ‚uĹĽy do autoryzacji twojego programu. Wpisz tutaj ciÄ…g znakĂłw przekazany Ci przez sprzedawcÄ™.' );");
    QString query3 = QString::fromUtf8("INSERT INTO settings ( id,key,value,type,label,description ) VALUES ( '2','default_url','http://albitos.eu/~albi/phpbb/strefa/','varchar','DomyĹ›lny adres','Adres forum do ktĂłrego domyĹ›lnie program bÄ™dzie siÄ™ logowaĹ‚.' );");
    QString query4 = QString::fromUtf8("INSERT INTO settings ( id,key,value,type,label,description ) VALUES ( '3','default_login','albi','varchar','Domyślny login',NULL );");
    QString query5 = QString::fromUtf8("INSERT INTO settings ( id,key,value,type,label,description ) VALUES ( '4','default_password',NULL,'password','Domyślne hasło',NULL );");
    QString query6 = QString::fromUtf8("INSERT INTO settings ( id,key,value,type,label,description ) VALUES ( '5','default_timeout','1','spinbox','Domyślny czas oczekiwania',NULL );");
    QString query7 = QString::fromUtf8("INSERT INTO settings ( id,key,value,type,label,description,ordering ) VALUES ( '100','maximum_timeout','1','spinbox','Czas losowego oczekiwania (domyslny+losowy)',400);");
    QSqlQuery q;
    q.exec(query1);
    q.exec(query2);
    q.exec(query3);
    q.exec(query4);
    q.exec(query5);
    q.exec(query6);
    q.exec(query7);


    filesModel = new QSqlRelationalTableModel;
    filesModel->setTable("files");
    filesModel->setRelation(6, QSqlRelation("templates", "id", "name"));
    filesModel->select();

    mirrorsModel = new QSqlRelationalTableModel;
    mirrorsModel->setTable("mirrors");
    mirrorsModel->setRelation(1, QSqlRelation("files", "id", "name"));
    mirrorsModel->select();

    templatesModel = new QSqlTableModel;
    templatesModel->setTable("templates");
    templatesModel->select();

    /*
     * Config library setup
     */
    config = new WConfig();

    /*
     * Login dialog specific code
     */
    loginDialog = new LoginDialog(config, 0);
    connect(loginDialog, SIGNAL(dataSend(QString, QString, QString, QString)), this, SLOT(afterLoginForm(QString, QString, QString, QString)));
    connect(loginDialog, SIGNAL(enableParent(bool)), this, SLOT(setEnabled(bool)));

    /*
     * Files list window specific code
     */
    filesListWindow.setup(filesModel, fileDialog);

    /*
     * Templates list window specific code
     */
    templatesListWindow.setup(templatesModel, templateDialog);

    /*
     * Mirrors list window specific code
     */
    mirrorsListWindow.setup(mirrorsModel);

    /*
     * Settings window specific code
     */
    settingsWindow = new SettingsWindow(config);

    /*
     * Forum bot setup
     */
    connect(&forumBot, SIGNAL(loginSuccess()), this, SLOT(prepareData()));
    connect(&forumBot, SIGNAL(loginFailure()), this, SLOT(onLoginFailure()));
    connect(&forumBot, SIGNAL(addPostSuccess()), this, SLOT(sendNextSuccess()));
    connect(&forumBot, SIGNAL(addPostFailure()), this, SLOT(sendNextFailure()));
    connect(&timer, SIGNAL(timeout()), this, SLOT(sendNext()));

    ui->setupUi(this);
    ui->spinBox->setValue((*config)["default_timeout"]["value"].toInt());
    ui->randomSpin->setValue((*config)["random_timeout"]["value"].toInt());
    authServers.append("http://albitos.eu/~albi/dwuploader/index.php?r=site/update");
}

Uploader::~Uploader()
{
    delete ui;
}
//TODO: REPLACE THOSE WITH SLOT CALLS
void Uploader::on_actionAddFile_triggered()
{
    fileDialog = new AddFileDialog;
    fileDialog->setup();
    fileDialog->show();
    connect(fileDialog, SIGNAL(changed()), &filesListWindow, SLOT(refresh()));
}


void Uploader::on_actionEditFile_triggered()
{
    filesListWindow.show();
}


void Uploader::on_actionClose_triggered()
{
    QApplication::exit(0);
}

void Uploader::on_pushButton_pressed()
{
    if((*config)["generator_type"]["value"] == "simple")
    {
        prepareData();
        getSimple();
    }
    else if(!forumBot.isLoggedIn)
    {
        loginDialog->show();
        this->setDisabled(true);
    }
    else
    {
        prepareData();
        if(totalCount > 0)
        {
            this->setDisabled(true);
            addCount = 0; errorCount = 0;
            time=ui->spinBox->value();
            randomTime=ui->randomSpin->value();
            progressDialog.ui->timerLabel->setText(QString::number((time+randomTime)*totalCount)+" sekund");
            progressDialog.open();
            sendNext();
        }
    }
}

void Uploader::afterLoginForm(QString login, QString password, QString url, QString forumType)
{
    CookiesHandler h;
    bool check = false;
    for(int i = 0; i < authServers.count(); i++)
    {
        QString server = authServers.at(i);

        QUrl params;
        params.addQueryItem("key", (*config)["licence_key"]["value"]);
        params.addQueryItem("url", url);
        params.addQueryItem("login", login);

        QByteArray data;
        data.append(params.toString());
        data.remove(0,1);

        QNetworkReply *r = h.sendPostRequest(server, data);

        QEventLoop eventLoop;
        connect(r, SIGNAL(finished()), &eventLoop, SLOT(quit()));
        eventLoop.exec();

        QString content(r->readAll());
        if(content.contains("YES")) check = true;
    }
    check = true;
    if(check)
    {
        forumBot.login(login, password, url, forumType);
        qDebug() << "LICENCE APPROVED";
    }
    else
    {
        QMessageBox::critical(this, "Blad licencji", "Niepoprawny klucz licencyjny lub licencja wygasla. Skontaktuj sie ze sprzedawca.");
        exit(1);
    }
}

void Uploader::prepareData()
{
    entries.clear();
qDebug() << "PREPARE DATA";
    WSqlQuery q;
    q.exec("SELECT * FROM files LEFT JOIN templates ON templates.id = files.template_id");

    linksLeft = ui->linksListEdit->toPlainText();
    while(q.next())
    {
        QRegExp reg(QString("((?:https?|ftp)://(www\\.){0,1}(\\S+)\\.\\S*")+q.value(2).toString()+"\\S*)");
        reg.setCaseSensitivity(Qt::CaseInsensitive);

        int pos = 0;
        QMap<QString, QStringList> mirrors;
        while((pos = reg.indexIn(ui->linksListEdit->toPlainText()+"\n", pos)) != -1)
        {
            //reg.cap(3) - domena linka
            //reg.cap(0) - caly link
            //q.value(x) - x-ta kolumna rozpatrywanego pliku

            WSqlQuery q2;
            q2.prepare("SELECT * FROM mirrors WHERE file_id = ?");
            q2.addBindValue(q.value(0).toInt());
            q2.exec();

            bool mirrored = false;
            while(q2.next())
            {
                QRegExp reg2(QString("*")+q2.value(3).toString()+"*");
                reg2.setPatternSyntax(QRegExp::WildcardUnix);
                reg2.setCaseSensitivity(Qt::CaseInsensitive);
                if(reg2.exactMatch(reg.cap(0)))
                {
                    if(mirrors.contains(reg.cap(2))) mirrors.insert(reg.cap(2), QStringList());
                    mirrors[q2.value(2).toString()] << reg.cap(0);
                    mirrored = true;
                }
            }
            if(!mirrored) mirrors[q.value(1).toString()] << reg.cap(0);
            linksLeft.replace(reg.cap(0), "");
            pos += reg.matchedLength();
        }

        QString linksString;
        QMapIterator<QString, QStringList> i(mirrors);
        while (i.hasNext()) {
            i.next();
            //linksString += QString(i.key())+"\n[code]";
            linksString += "[code]";
            for(int j = 0; j < i.value().size(); j++)
            {
                linksString += i.value().at(j)+"\n";
            }

            linksString += "[/code]\n";
        }

        QString imagesString = q.value(5).toString();
        imagesString.replace(QRegExp("((?:https?|ftp)://\\S+)"), "[img]\\1[/img]");

        QString title = q.value(1).toString();
        QString body = q.value(9).toString();
        body.replace("[NAZWA]", q.value(1).toString());
        body.replace("[OPIS]", q.value(4).toString());
        body.replace("[OBRAZKI]", imagesString);
        body.replace("[LINKI]", linksString);

        if(!mirrors.empty())
        {
            AddEntry e;
            e.body = body;
            e.title = title;
            e.topicUrl = q.value(3).toString();
            entries.push_back(e);
        }
    }
    uploadBreak = false;
    linksLeft.replace(QRegExp("\\n\\n"), "");
    totalCount = entries.size();

/*
    qDebug() << "TOTAL: " << totalCount;
    for(int i = 0; i <entries.size(); i++)
    {
        qDebug() << entries.at(i).topicUrl << entries.at(i).title;
    }
*/
}


void Uploader::sendNext()
{
    qDebug() << "SENDING NEXT";
    timer.stop();
    qDebug() << entries.at(0).topicUrl << entries.at(0).title << entries.at(0).body;
    forumBot.addPost(entries.at(0).topicUrl, entries.at(0).title, entries.at(0).body);

    //TODO: Check for success, dont do it blindly
    entries.removeAt(0);
    if(!entries.empty() && !uploadBreak)
    {
        QTime tim = QTime::currentTime();
        /* initialize random seed: */
        srand ( (int)tim.msec() );

        /* generate secret number: */
        int r = rand() % randomTime + 1;

        timer.start(time*1000+r*1000);
    }
}

void Uploader::sendNextSuccess()
{
    if(!uploadBreak)
    {
        progressDialog.ui->progressBar->setValue(((++addCount)+errorCount)*100/totalCount);
        progressDialog.ui->timerLabel->setText(QString::number(time*(totalCount-addCount-errorCount))+" sekund");
    }
    qDebug() << "SUCCESS";
    if(addCount+errorCount >= totalCount)
    {
        emit sendingFinished();
        emit enableParent(true);
    }
}

void Uploader::sendNextFailure()
{
    progressDialog.ui->progressBar->setValue((int)((addCount+(++errorCount))*100/totalCount));
    progressDialog.ui->timerLabel->setText(QString::number(time*(totalCount-addCount-errorCount))+" sekund");
    //linksLeft +=
    //TODO
    //unfinished.append(e);
    if(addCount+errorCount >= totalCount)
    {
        emit sendingFinished();
        emit enableParent(true);
    }
        qDebug() << "FAIL";
}

void Uploader::getSimple()
{
    QString message = "";
    QString url = QInputDialog::getText(this, "Podaj adres forum", "Podaj adres forum");
    while(!entries.empty())
    {
        QRegExp r;
        QString topic_id;
        QString topicUrl = entries.at(0).topicUrl;

        if(topicUrl.contains("darkwarez.pl"))
        {
            r.setPattern("\\/(\\d+)");
            r.indexIn(topicUrl);
            topic_id = r.cap(1);
        }
        else
        {
            qDebug() << "WTF";
            r.setPattern("t=(\\d+)");
            r.indexIn(topicUrl);
            topic_id = r.cap(1);
            qDebug() << topic_id;
            if(topic_id == "")
            {
                r.setPattern("\\/(\\d+)");
                r.indexIn(topicUrl);
                topic_id = r.cap(1);
            }
        }
        QUrl postPageUrl;
        postPageUrl = url+"posting.php";
        postPageUrl.addQueryItem("mode", "reply");
        postPageUrl.addQueryItem("t", topic_id);

        message += QString("<a href=\"")+postPageUrl.toString()+"\">"+postPageUrl.toString()+"</a><br><br>";
        QString body = entries.at(0).body;
        body = body.replace(" ", "&nbsp;");
        body = body.replace("\n", "<br>\n");
        message += body+"<br><br><br><br><br><br>";
        entries.removeAt(0);
    }
    QTextBrowser *b = new QTextBrowser();
    b->setText(message);
    b->setOpenExternalLinks(true);
    b->showMaximized();
}

void Uploader::closeEvent(QCloseEvent *event)
{
    QApplication::exit(0);
}

void Uploader::on_actionAddTemplate_triggered()
{
    templateDialog = new AddTemplateDialog;
    templateDialog->setup();
    templateDialog->open();
    connect(templateDialog, SIGNAL(changed()), &templatesListWindow, SLOT(refresh()));
}

void Uploader::on_actionEditTemplate_triggered()
{
    templatesListWindow.show();
}

void Uploader::on_actionEditMirror_triggered()
{
    mirrorsListWindow.show();
}

void Uploader::displayReport()
{
    QString report;

    uploadBreak = true;

    report = "Dodanych zostalo ";
    report += QString('0'+addCount) + " postow z " + QString('0'+totalCount)+ ".\n";
    report += "Nie udalo sie dodac ";
    report += QString('0'+errorCount) + " postow. Nastepujace linki nalezy sprawdzic i wyslac ponownie:\n";

    this->ui->linksListEdit->setPlainText(linksLeft);

    reportDialog.ui->linksEdit->setPlainText(linksLeft);
    reportDialog.ui->infoLabel->setText(report);
    reportDialog.show();
    progressDialog.hide();
}

void Uploader::onLoginFailure()
{
    loginDialog->show();
    QMessageBox::information(this, "Błąd logowania", "Nie udało się połączyć z forum lub dane logowania są niepoprawne. Spróbuj ponownie.");
}

void Uploader::sendData()
{

}

void Uploader::on_actionSettings_triggered()
{
    settingsWindow->show();
}
