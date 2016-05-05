#ifndef POWERSAVERSAVE_H
#define POWERSAVERSAVE_H

#include <QObject>


class PowerSaverStorageJson : public QObject
{
    Q_OBJECT

public:
    enum SaveFormat {
        Json, Binary
    };
    explicit PowerSaverStorageJson(SaveFormat format, QObject *parent = 0);
    bool updateStrategy(QMap<QString, QVariant> strategy);
    QMap<QString, QVariant> loadStrategy();
    bool isStrategyExist();
signals:

public slots:

private:
    SaveFormat m_format;
    bool m_jsonExist;
    void createJson();
};


#endif // POWERSAVERSAVE_H

