#pragma once

#include <QFileInfo>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>

class JsonAppConf
{
    QJsonObject json_;

public:
    JsonAppConf()
    {

    }

    bool openFile(QString & err)
    {

        QStringList args = QApplication::arguments();

        QString confFileName;

        if (1 == args.count())
        {
            confFileName = QApplication::applicationFilePath()+".conf";
        }
        else if (2 == args.count())
        {
            confFileName = args[1];
        }
        else
        {
            QString msg = QString("Возможно задание только одного аргумента при запуске, задано <%1>").arg(args.count());
            qFatal(msg.toStdString().data());
        }



        bool confFileExists = QFileInfo::exists(confFileName) && QFileInfo(confFileName).isFile();
        if (!confFileExists)
        {
            err = QString("Файл конфигурации не найден <%1>").arg(confFileName);
            return false;
        }

        QFile confFile(confFileName);
        bool opened = confFile.open(QIODevice::ReadOnly);
        if (!opened)
        {
            err = QString("Файл конфигурации не открыт <%1>").arg(confFileName);
            return false;
        }

        QByteArray ba = confFile.readAll();
        QString confJsonStr = QString::fromUtf8(ba);

        qDebug() << ba;
        qDebug() << confJsonStr;


        QJsonParseError jsonErr;

        QJsonDocument jsonDoc = QJsonDocument::fromJson(ba, &jsonErr);

        if (jsonDoc.isNull())
        {
            err = QString("Файл конфигурации содержит ошибку <%1>, позиция <%2>").arg(jsonErr.errorString()).arg(jsonErr.offset);
            return false;
        }

        if (!jsonDoc.isObject())
        {
            err = QString("Файл конфигурации должен быть объектом json");
            return false;
        }

        json_ = jsonDoc.object();

        return true;
    }

    QString getStringValue(const QString & name, const QString & default_value)
    {

        if (!json_.contains(name))
        {
            return default_value;
        }

        QJsonValue v = json_.value(name);

        if (!v.isString())
        {
            Q_ASSERT(0);
            return default_value;
        }

        return v.toString();
    }

};
