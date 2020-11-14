#include "toolbox.h"
#include "ui_toolbox.h"

#include <QDesktopServices>
#include <QFile>
#include <QTextStream>
#include <QUrl>
#include <QSettings>
#include <QProgressDialog>
#include <QMessageBox>
#include <QtGlobal>
#include <QJsonDocument>
#include <QJsonObject>


Toolbox::Toolbox(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Toolbox)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/new/prefix1/images/cn1.png"));
    setWindowFlags(Qt::Dialog|Qt::WindowMinimizeButtonHint|Qt::WindowCloseButtonHint);

    QSettings settings("toolbox-config.ini", QSettings::IniFormat);
    settings.beginGroup("General");
    ui->cn1LingVers->setText("Version : "+settings.value("cn1linguist-version").toString());
    ui->cn1ExtVers->setText("Version : "+settings.value("cn1extensions-version").toString());
    settings.endGroup();

    ui->cn1LingUpdate->setText("No update available");
    ui->cn1ExtUpdate->setText("No update available");

    creerAppMenus();

    netManager=new QNetworkAccessManager(this);
}

Toolbox::~Toolbox()
{
    delete ui;
}

void Toolbox::on_action_Quit_triggered()
{
    qApp->quit();
}

inline void Toolbox::launchCN1Linguist()
{
    #if defined(Q_OS_WIN)
        QDesktopServices::openUrl(QUrl("cn1linguist.exe"));
    #elif defined(Q_OS_MACOS)
        QDesktopServices::openUrl(QUrl("cn1linguist"));
    #else
        QDesktopServices::openUrl(QUrl("cn1linguist"));
    #endif
}

inline void Toolbox::launchCN1Extensions()
{
    #if defined(Q_OS_WIN)
        QDesktopServices::openUrl(QUrl("cn1extensions.exe"));
    #elif defined(Q_OS_MACOS)
        QDesktopServices::openUrl(QUrl("cn1extensions"));
    #else
        QDesktopServices::openUrl(QUrl("cn1extensions"));
    #endif
}

void Toolbox::updateCN1Linguist()
{
    #if defined(Q_OS_WIN)
        updateApp(CN1LING_WINDOWS_BIN, "Linguist");
    #elif defined(Q_OS_MACOS)
        updateApp(CN1LING_MACOS_BIN, "Linguist");
    #else
        updateApp(CN1LING_LINUX_BIN, "Linguist");
    #endif   
}

void Toolbox::updateCN1Extensions()
{
    #if defined(Q_OS_WIN)
        updateApp(CN1EXT_WINDOWS_BIN, "Extensions");
    #elif defined(Q_OS_MACOS)
        updateApp(CN1EXT_MACOS_BIN, "Extensions");
    #else
        updateApp(CN1EXT_LINUX_BIN, "Extensions");
    #endif
}

void Toolbox::updateApp(const QString &url, const QString &nomApp)
{
    QNetworkRequest request;
    request.setUrl(QUrl(url));

    QNetworkReply *response=netManager->get(request);

    QProgressDialog *progress=new QProgressDialog("Updating "+nomApp+"...", nullptr, 0, 100, this);
    progress->setWindowModality(Qt::WindowModal);
    progress->show();

    connect(response, &QNetworkReply::downloadProgress, this, [=](qint64 received, qint64 total){
        if(received>0 && total>0){
            pourcentage=(received*100)/total;
            progress->setValue(int(pourcentage));
        } else {
            progress->cancel();
        }
    });

    connect(response,SIGNAL(error(QNetworkReply::NetworkError)),this,SLOT(erreurTelUpdates(QNetworkReply::NetworkError)));
    connect(response,&QNetworkReply::finished,this,[=](){

        if(response->bytesAvailable()){
            QFile binZipFile("updates.zip");
            binZipFile.open(QIODevice::WriteOnly);
            binZipFile.write(response->readAll());
            binZipFile.close();

            process=new QProcess(this);
            QStringList params={"updates.zip"};
            process->start("extract-updates",params);
            if(nomApp=="Linguist"){
                connect(process, SIGNAL(finished(int)), this, SLOT(updateLinguistFinished(int)));

            } else if(nomApp=="Extensions"){
                connect(process, SIGNAL(finished(int)), this, SLOT(updateExtensionsFinished(int)));
            }
            connect(process, SIGNAL(errorOccurred(QProcess::ProcessError)), this, SLOT(erreurUpdates(QProcess::ProcessError)));
        }

        response->close();
        response->deleteLater();
    });
}

void Toolbox::on_actionCheck_for_updates_triggered()
{
    QNetworkRequest request;
    request.setUrl(QUrl(APPS_VERSIONS));

    QNetworkReply *response=netManager->get(request);

    QProgressDialog *progress=new QProgressDialog("Searching update...", nullptr, 0, 100, this);
    progress->setWindowModality(Qt::WindowModal);
    progress->show();

    connect(response, &QNetworkReply::downloadProgress, this, [=](qint64 received, qint64 total){
        if(received>0 && total>0){
            pourcentage=(received*100)/total;
            progress->setValue(int(pourcentage));
        } else {
            progress->cancel();
        }
    });

    connect(response,SIGNAL(error(QNetworkReply::NetworkError)),this,SLOT(erreurVerifierUpdates(QNetworkReply::NetworkError)));
    connect(response,&QNetworkReply::finished,this,[=](){

        if(response->bytesAvailable()){

            QByteArray contenuJson=response->readAll();
            if(contenuJson.isEmpty()){
                return;
            }

            QJsonDocument doc=QJsonDocument::fromJson(contenuJson);
            QJsonObject obj=doc.object();

            QString cn1ling_vers=obj.value("cn1linguist-version").toString();
            QString cn1ext_vers=obj.value("cn1extensions-version").toString();

            QSettings settings("toolbox-config.ini", QSettings::IniFormat);
            settings.beginGroup("General");
            QString cn1ling_versLocal=settings.value("cn1linguist-version").toString();
            QString cn1ext_versLocal=settings.value("cn1extensions-version").toString();
            settings.endGroup();

            if(cn1ling_vers!=cn1ling_versLocal){
                cn1LingVersion=cn1ling_vers;
                ui->cn1LingUpdate->setText("<font color='#ff4a4a'><b>Update available</b></font>");
            } else {
                linguistUpdateAvailable=false;
            }

            if(cn1ext_vers!=cn1ext_versLocal){
                cn1ExtVersion=cn1ext_vers;
                ui->cn1ExtUpdate->setText("<font color='#ff4a4a'><b>Update available</b></font>");
            } else {
                extensionsUpdateAvailable=false;
            }

            if(linguistUpdateAvailable==false && extensionsUpdateAvailable==false){
                QMessageBox::information(this, tr("No Update"),tr("There is no update available"));
                linguistUpdateAvailable=true;
                extensionsUpdateAvailable=true;
            }
        }

        response->close();
        response->deleteLater();
    });
}

void Toolbox::erreurTelUpdates(QNetworkReply::NetworkError error)
{
    if(error==QNetworkReply::ConnectionRefusedError){
        QMessageBox::critical(this, tr("Error"), tr("An error has occured. Check your internet connexion."));
    } else {
        QMessageBox::critical(this,tr("Failed"),tr("Update failed! Please retry later"));
    }

    QFile updatesZipFile("updates.zip");
    if(updatesZipFile.exists()){
        updatesZipFile.remove();
    }
}

void Toolbox::erreurVerifierUpdates(QNetworkReply::NetworkError error)
{
    if(error==QNetworkReply::ConnectionRefusedError){
        QMessageBox::critical(this, tr("Error"), tr("An error has occured. Check your internet connexion."));
    } else {
        QMessageBox::critical(this,tr("Failed"),tr("Checking update failed! Please retry"));
    }
}

void Toolbox::updateLinguistFinished(int exitCode)
{
    if(process->readAllStandardOutput()=="Extraction finished"){
        QMessageBox::information(this,tr("Succeed"),tr("<b>Update completed</b> successfully"));

        QSettings settings("toolbox-config.ini", QSettings::IniFormat);
        settings.beginGroup("General");

        ui->cn1LingUpdate->setText("No update available");
        if(cn1LingVersion==""){
            ui->cn1LingVers->setText("Version : "+settings.value("cn1linguist-version").toString());
        } else {
            ui->cn1LingVers->setText("Version : "+cn1LingVersion);
            settings.setValue("cn1linguist-version", cn1LingVersion);
        }

        settings.endGroup();

    } else {
        QMessageBox::critical(this,tr("Failed"),tr("Update failed! Please retry"));
    }
}

void Toolbox::updateExtensionsFinished(int exitCode)
{
    if(process->readAllStandardOutput()=="Extraction finished"){
        QMessageBox::information(this,tr("Succeed"),tr("<b>Update completed</b> successfully"));

        QSettings settings("toolbox-config.ini", QSettings::IniFormat);
        settings.beginGroup("General");

        ui->cn1ExtUpdate->setText("No update available");
        if(cn1ExtVersion==""){
            ui->cn1ExtVers->setText("Version : "+settings.value("cn1extensions-version").toString());
        } else {
            ui->cn1ExtVers->setText("Version : "+cn1ExtVersion);
            settings.setValue("cn1extensions-version", cn1ExtVersion);
        }

        settings.endGroup();

    } else {
        QMessageBox::critical(this,tr("Failed"),tr("Update failed! Please retry"));
    }
}

void Toolbox::erreurUpdates(QProcess::ProcessError error)
{
    QMessageBox::critical(this,tr("Failed"),tr("Update failed! Please retry"));
}

void Toolbox::creerAppMenus()
{
    QAction *launchCN1Ling=new QAction("Launch Codename One Linguist", this);
    QAction *updateCN1Ling=new QAction("Download the update", this);
    QAction *launchCN1LingWebPage=new QAction("Open Web page", this);
    QAction *launchCN1LingGithubPage=new QAction("Open GitHub page", this);
    ui->cn1LinguistToolbutton->addAction(launchCN1Ling);
    ui->cn1LinguistToolbutton->addAction(updateCN1Ling);
    ui->cn1LinguistToolbutton->addAction(launchCN1LingWebPage);
    ui->cn1LinguistToolbutton->addAction(launchCN1LingGithubPage);

    QAction *launchCN1Ext=new QAction("Launch Codename One Extensions", this);
    QAction *updateCN1Ext=new QAction("Download the update", this);
    QAction *launchCN1ExtWebPage=new QAction("Open Web page", this);
    QAction *launchCN1ExtGithubPage=new QAction("Open GitHub page", this);
    ui->cn1ExtensionsToolbutton->addAction(launchCN1Ext);
    ui->cn1ExtensionsToolbutton->addAction(updateCN1Ext);
    ui->cn1ExtensionsToolbutton->addAction(launchCN1ExtWebPage);
    ui->cn1ExtensionsToolbutton->addAction(launchCN1ExtGithubPage);

    connect(launchCN1Ling, &QAction::triggered, this, [=](){
        launchCN1Linguist();
    });

    connect(launchCN1Ext, &QAction::triggered, this, [=](){
        launchCN1Extensions();
    });

    connect(launchCN1LingWebPage, &QAction::triggered, this, [=](){
        QDesktopServices::openUrl(QUrl(CN1LING_HOMEPAGE));
    });

    connect(launchCN1LingGithubPage, &QAction::triggered, this, [=](){
        QDesktopServices::openUrl(QUrl(CN1LING_GITHUBPAGE));
    });

    connect(updateCN1Ling, &QAction::triggered, this, [=](){
        updateCN1Linguist();
    });

    connect(updateCN1Ext, &QAction::triggered, this, [=](){
        updateCN1Extensions();
    });

    connect(launchCN1ExtWebPage, &QAction::triggered, this, [=](){
        QDesktopServices::openUrl(QUrl(CN1EXT_HOMEPAGE));
    });

    connect(launchCN1ExtGithubPage, &QAction::triggered, this, [=](){
        QDesktopServices::openUrl(QUrl(CN1EXT_GITHUBPAGE));
    });
}

