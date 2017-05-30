#include "worker.h"


Worker::Worker(QStringList *FileNames, QString rpath)
{
    fileNames = FileNames;
    _rpath = rpath;
    ownrpaths = new QStringList();
}

Worker::~Worker() {

}

void Worker::process() {
    QStringList resultList;
    QString ret = "";
    QProcess *process = new QProcess(this);



    for(int fiInt=0; fiInt<fileNames->count(); fiInt++){

        emit progress(fiInt,fileNames->count());

        ownrpaths->clear();
        QString baseFileName = fileNames->at(fiInt);
        QString baseFolder = QFileInfo(baseFileName).absolutePath();
        QString baseF = QFileInfo(baseFileName).completeBaseName() + "." + QFileInfo(baseFileName).completeSuffix();

        QString baseID = "-";
        int mainprobs = 0;

        ret = "";
        QString checkFileName = baseFileName;
        checkFileName = redirectFile(checkFileName, baseFileName, baseFolder); // fix file if app or framework

        // His own rpath?
        checkownrpathsetting(checkFileName,process,ownrpaths);

        // Add Users rpath first, otherwise use current work folder
        if (_rpath.trimmed()!="" && QDir(_rpath).exists())
            ownrpaths->append(_rpath);
        else
            ownrpaths->append(baseFolder);

        process->start("otool", QStringList() << "-L" << checkFileName);
        if (process->waitForStarted(-1)) {
            while(process->waitForReadyRead(-1)) {
                ret += process->readAllStandardOutput();
            }
        }

        // Cleanup data
        ret = ret.replace("\t","");

        // Go to fix
        QStringList retListTemp = ret.split("\n");

        if (!retListTemp.isEmpty())
        {
            retListTemp.removeFirst();
            if (!retListTemp.empty())
            {
                if (baseFileName.right(4) != ".app"){
                    baseID = QString(retListTemp.first()).trimmed();
                    if (baseID.contains("(compat"))
                        baseID = baseID.left(baseID.indexOf("(compat")).trimmed();
                    retListTemp.removeFirst();
                } else {
                    baseID = "ApplicationBundle";
                }
            }
        }

        for(int i=0; i<retListTemp.count();i++){

            QString retEntry = retListTemp.at(i);

            if (retEntry.lastIndexOf("(") > 0){

                QString retFileName = retEntry.left(retEntry.lastIndexOf("(")).trimmed();
                QString orgFileName = retFileName;
                bool fileFound = QFile(orgFileName).exists();


                if (!fileFound)
                    fileFound = checkrpath(retFileName,baseFolder);

                if (!fileFound)
                    fileFound = checkloaderpath(retFileName,baseFolder);

                if (!fileFound)
                    fileFound = checkexecutablepath(retFileName,baseFolder);

                if (!fileFound){
                    QString lookForFileName = retFileName;
                    if (lookForFileName.contains("/"))
                        lookForFileName = lookForFileName.right(lookForFileName.length() - lookForFileName.lastIndexOf("/"));
                }

                retEntry = orgFileName + "\t" + retEntry.right(retEntry.length() - retEntry.lastIndexOf("("));

                // Begin to check file info
                int problems = 0;
                retEntry += "\t" + getFileInfos(retFileName,process,problems);
                mainprobs += problems;

                retEntry += "\t" + QString::number(problems)+"\t"+retFileName+"\t"+baseFileName;
                retListTemp.replace(i," " + retEntry);

                emit infomessage("File " + retFileName + " completed");
            }    
        }

        int nope = 0;
        QString mainRet = getFileInfos(fileNames->at(fiInt),process,nope);
        resultList.append(baseF + "\t"+baseID+"\t"+mainRet+"\t"+QString::number(mainprobs)+"\t"+baseFileName+"\t"+baseFileName);
        for(int fi=0; fi<retListTemp.count(); fi++)
            resultList.append(retListTemp.at(fi));

        ownrpaths->clear();
    }

    process->close();
    process->deleteLater();

    emit finished(resultList.join("\n"));
}

bool Worker::checkrpath(QString &fileName, QString fromBaseFolder)
{
    if (QFile(fileName).exists())
        return true;

    if (!fileName.contains("@rpath"))
        return false;

    for(int i=0; i<ownrpaths->count(); i++){

        QString checkFile = fileName;
        emit infomessage("Check rpath " + ownrpaths->at(i) + " for " + fileName);

        if (QFile(checkFile.replace("@rpath",ownrpaths->at(i))).exists()){
            fileName.replace("@rpath",ownrpaths->at(i));
            emit infomessage("Fix success with rpath to " + fileName);
            return true;
        }

        checkFile = fileName;

        if (QFile(checkFile.replace("@rpath",fromBaseFolder + "/" + ownrpaths->at(i))).exists()){
            fileName.replace("@rpath",fromBaseFolder + "/" + ownrpaths->at(i));
            emit infomessage("Fix success with rpath to " + fileName);
            return true;
        }
    }

    QString checkFile = fileName;
    qDebug() << "Look for " << QString(checkFile).replace("@rpath/",fromBaseFolder+"/");
    if (QFile(checkFile.replace("@rpath/",fromBaseFolder+"/")).exists()){
        return true;
    } else {
        qDebug() << "not found!!! " << QString(checkFile).replace("@rpath/",fromBaseFolder+"/");
        return false;
    }
}

bool Worker::checkloaderpath(QString &fileName, QString fromBaseFolder)
{
    if (!QFile(fileName).exists()){
        emit infomessage(fileName + " not found, looking for loader_path...");

        if (fileName.contains("@loader_path"))
        {
            fileName.replace("@loader_path",fromBaseFolder);
            emit infomessage("found at " + fileName);
        } else {
            emit infomessage("No loader_path definition found.");
        }

        return QFile(fileName).exists();
    }

    return true;
}

bool Worker::checkexecutablepath(QString &fileName, QString fromBaseFolder)
{
    if (!QFile(fileName).exists()){

        emit infomessage(fileName + " not found, looking for executable_path...");

        if (fileName.contains("@executable_path/"))
        {
            fileName.replace("@executable_path",fromBaseFolder);
            emit infomessage("executable_path resolved to " + fileName);
        } else {
            emit infomessage("No executable_path definition found.");
        }

        return QFile(fileName).exists();
    }

    return true;
}

QString Worker::getFileInfos(QString fileName, QProcess *currentProcess,int &probs)
{
    QString retEntry;
    currentProcess->start("file", QStringList() << fileName);
    if (currentProcess->waitForFinished(5000)){
        QStringList outData = QString(currentProcess->readAllStandardOutput()).split("\n");

        if (outData.count()>0){
            QString fileMesssage = QString(outData.at(0)).replace("\t","").replace("\n","");

            if (fileMesssage.indexOf(": ") > -1)
                fileMesssage = fileMesssage.right(fileMesssage.length() - fileMesssage.indexOf(": ")  - 2);

            if (fileMesssage.contains("cannot open"))
            {
                fileMesssage = "Not found";
                probs++;
            }

            if (fileMesssage.toLower().contains("universal"))
                fileMesssage = "Universal x86/x64 Bit";
            else if (fileMesssage.toLower().contains("x86_64"))
                fileMesssage = "64 Bit";
            else if (fileMesssage.toLower().contains("i386"))
                fileMesssage = "32 Bit";


            retEntry = fileMesssage;
        } else {
            retEntry = "Timeout";
            probs++;
        }
    } else {
        retEntry = "Timeout";
        probs++;
    }

    return retEntry;
}

// If file is app or framework, redirect to binary filename
QString Worker::redirectFile(QString fromFile, QString baseFile, QString &baseFolder)
{
    if (fromFile.right(10) == ".framework")
    {
        baseFolder = fromFile + "/Versions/Current";
        fromFile = fromFile + "/Versions/Current/"+QFileInfo(baseFile).completeBaseName();
        emit infomessage("Redirect to Framework executable /Versions/Current/"+QFileInfo(baseFile).completeBaseName());
    }

    if (fromFile.right(4) == ".app")
    {
        baseFolder = fromFile + "/Contents/MacOS";
        fromFile = fromFile + "/Contents/MacOS/"+QFileInfo(baseFile).completeBaseName();
        emit infomessage("Redirect to App executable /Contents/MacOS/"+QFileInfo(baseFile).completeBaseName());
    }

    return fromFile;
}

void Worker::checkownrpathsetting(QString fileName, QProcess *process, QStringList *pathList)
{
    QString result = "";
    process->start("otool", QStringList() << "-l" << fileName);
    if (process->waitForFinished(5000)){
        QStringList outData = QString(process->readAllStandardOutput()).split("\n");
        bool readpath = false;
        foreach(QString outstr, outData){
            outstr = outstr.trimmed();

            if (outstr.contains("cmd LC_RPATH"))
                readpath = true;

            if (readpath && outstr.length()>4 && outstr.left(4) == "path")
            {
                result = outstr.right(outstr.length()-4);

                if (result.contains("(offset"))
                    result = result.left(result.indexOf("(offset")).trimmed();

                if (result.toLower().contains("@executable_path/"))
                    result = result.replace("@executable_path/","");

                if (result.toLower().contains("@loader_path/"))
                    result = result.replace("@loader_path/","");

                readpath = false;

                if (result.trimmed()=="")
                    continue;

                pathList->append(result);
            }
        }
    }

    qDebug() << "Result: " << pathList->join("\r");
}
