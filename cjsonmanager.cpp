#include "cjsonmanager.h"
#include <QFile>
#include <QJsonDocument>

CJSONManager::CJSONManager(QObject *parent) : QObject(parent) {}

void CJSONManager::saveJson(const QJsonDocument *jDoc, QString filename)
{
    QFile jFile(filename);
    jFile.open(QFile::WriteOnly);
    jFile.write(jDoc->toJson());
    // TODO: нужен close?
}

QJsonDocument CJSONManager::loadJson(QString filename)
{
    QFile jFile(filename);
    jFile.open(QFile::ReadOnly);
    return QJsonDocument().fromJson(jFile.readAll());
}
