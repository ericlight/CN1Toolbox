#ifndef TOOLBOX_H
#define TOOLBOX_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QProcess>

QT_BEGIN_NAMESPACE
namespace Ui { class Toolbox; }
QT_END_NAMESPACE

class Toolbox : public QMainWindow
{
    Q_OBJECT

public:
    Toolbox(QWidget *parent = nullptr);
    ~Toolbox();

private slots:
    void on_action_Quit_triggered();

    void on_actionCheck_for_updates_triggered();

    void erreurTelUpdates(QNetworkReply::NetworkError);

    void updateLinguistFinished(int exitCode);

    void updateExtensionsFinished(int exitCode);

    void erreurUpdates(QProcess::ProcessError);

    void erreurVerifierUpdates(QNetworkReply::NetworkError);

private:
    Ui::Toolbox *ui;
    QNetworkAccessManager *netManager;
    qint64 pourcentage=0;
    QProcess *process;
    QString cn1LingVersion;
    QString cn1ExtVersion;
    bool linguistUpdateAvailable=true;
    bool extensionsUpdateAvailable=true;

    const QString CN1LING_HOMEPAGE="https://ericlight.github.io/CN1Linguist";
    const QString CN1LING_GITHUBPAGE="https://github.com/ericlight/CN1Linguist";
    const QString CN1EXT_HOMEPAGE="https://ericlight.github.io/CN1Extensions";
    const QString CN1EXT_GITHUBPAGE="https://github.com/ericlight/CN1Extensions";

    const QString APPS_VERSIONS="http://localhost:8080/versions.json";
    const QString CN1LING_WINDOWS_BIN="http://localhost:8080/cn1ling-windows-bin.zip";
    const QString CN1LING_MACOS_BIN="http://localhost:8080/cn1ling-macos-bin.zip";
    const QString CN1LING_LINUX_BIN="http://localhost:8080/cn1ling-linux-bin.zip";
    const QString CN1EXT_WINDOWS_BIN="http://localhost:8080/cn1ext-windows-bin.zip";
    const QString CN1EXT_MACOS_BIN="http://localhost:8080/cn1ext-macos-bin.zip";
    const QString CN1EXT_LINUX_BIN="http://localhost:8080/cn1ext-linux-bin.zip";

//    const QString APPS_VERSIONS="https://raw.githubusercontent.com/ericlight/CN1Toolbox/master/apps-updates/versions.json";
//    const QString CN1LING_WINDOWS_BIN="https://github.com/ericlight/CN1Toolbox/blob/master/apps-updates/cn1ling-windows-bin.zip?raw=true";
//    const QString CN1LING_MACOS_BIN="https://github.com/ericlight/CN1Toolbox/blob/master/apps-updates/cn1ling-macos-bin.zip?raw=true";
//    const QString CN1LING_LINUX_BIN="https://github.com/ericlight/CN1Toolbox/blob/master/apps-updates/cn1ling-linux-bin.zip?raw=true";
//    const QString CN1EXT_WINDOWS_BIN="https://github.com/ericlight/CN1Toolbox/blob/master/apps-updates/cn1ext-windows-bin.zip?raw=true";
//    const QString CN1EXT_MACOS_BIN="https://github.com/ericlight/CN1Toolbox/blob/master/apps-updates/cn1ext-macos-bin.zip?raw=true";
//    const QString CN1EXT_LINUX_BIN="https://github.com/ericlight/CN1Toolbox/blob/master/apps-updates/cn1ext-linux-bin.zip?raw=true";


    inline void launchCN1Linguist();
    inline void launchCN1Extensions();
    void updateCN1Linguist();
    void updateCN1Extensions();
    void updateApp(const QString &url, const QString &nomApp);
    void creerAppMenus();
};
#endif // TOOLBOX_H
