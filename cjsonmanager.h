#ifndef CJSONMANAGER_H
#define CJSONMANAGER_H

#include <QObject>

class CJSONManager : public QObject
{
    Q_OBJECT
public:
    explicit CJSONManager(QObject *parent = nullptr);
    void saveJson(const QJsonDocument *jDoc, QString filename);
    QJsonDocument loadJson(QString filename);

private:
};

#endif // CJSONMANAGER_H
