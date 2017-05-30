#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QProcess>
#include <QFileInfo>
#include <QDebug>
#include <QDir>

class Worker : public QObject {
    Q_OBJECT

public:
    Worker(QStringList *FileNames, QString rpath);
    ~Worker();

public slots:
    void process();

signals:
    void finished(QString resultstring);
    void error(QString err);
    void progress(int current, int from);
    void infomessage(QString what);

private:
    QString _rpath;
    QStringList *ownrpaths;
    QStringList *fileNames;
    bool checkrpath(QString &fileName, QString fromBaseFolder);
    bool checkloaderpath(QString &fileName, QString fromBaseFolder);
    bool checkexecutablepath(QString &fileName, QString fromBaseFolder);
    QString getFileInfos(QString fileName, QProcess *currentProcess,int &probs);
    QString redirectFile(QString fromFile, QString baseFile, QString &baseFolder);
    void checkownrpathsetting(QString fileName, QProcess *process, QStringList *pathList);
};

#endif // WORKER_H
