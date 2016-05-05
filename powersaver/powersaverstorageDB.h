#ifndef POWERSAVERSTORAGE
#define POWERSAVERSTORAGE

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>

class PowerSaverStorage : public QObject
{
    Q_OBJECT

public:
    explicit PowerSaverStorage(QObject *parent = 0);
    bool updateStrategy(QMap<QString, QVariant> strategy);
    QMap<QString, QVariant> selectStrategy();
    bool isTableExist();
signals:

public slots:

private:
    QSqlDatabase m_db;
    bool createTable();
    void initDB();
};

#endif // POWERSAVERSTORAGE

