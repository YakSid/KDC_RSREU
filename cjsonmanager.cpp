#include "cjsonmanager.h"
#include <QFile>
#include <QJsonDocument>

CJSONManager::CJSONManager(QObject *parent) : QObject(parent) {}

void CJSONManager::saveJson(const QJsonDocument *jDoc, QString filename)
{
    QFile jFile(filename);
    jFile.open(QFile::WriteOnly);
    jFile.write(jDoc->toJson());
    jFile.close();
}

QJsonDocument CJSONManager::loadJson(QString filename)
{
    QFile jFile(filename);
    jFile.open(QFile::ReadOnly);
    auto result = QJsonDocument().fromJson(jFile.readAll());
    jFile.close();
    return result;
}
