#ifndef POWERSAVERSAVE_H
#define POWERSAVERSAVE_H

#include <QObject>


class PowerSaverManagerStorageJson : public QObject
{
    Q_OBJECT

public:
    enum SaveFormat {
        Json, Binary
    };
    enum Savetype
    {
        Strategy, PrevSetting
    };
    explicit PowerSaverManagerStorageJson(SaveFormat format, QObject *parent = 0);
    bool updateStorage(const QMap<QString, QVariant> & value, Savetype type);
    QMap<QString, QVariant> loadStorage(Savetype type);
    bool deleteStorage(Savetype type);
    bool isStrategyExist();
    bool isSettingExist();
signals:

public slots:

private:
    SaveFormat m_format;
    bool m_strategyExist;
    bool m_settingExist;
    void createJson();
};


#endif // POWERSAVERSAVE_H

