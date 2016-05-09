#ifndef POWERSAVERSTORAGE
#define POWERSAVERSTORAGE

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>

class PowerSaverMangerStorage : public QObject
{
    Q_OBJECT

public:
    explicit PowerSaverMangerStorage(QObject *parent = 0);
    bool updateStrategy(const QMap<QString, QVariant> & strategy);
    QMap<QString, QVariant> loadStrategy();
    bool isTableExist();
signals:

public slots:

private:
    QSqlDatabase m_db;
    bool createTable();
    void initDB();
};

#endif // POWERSAVERSTORAGE

