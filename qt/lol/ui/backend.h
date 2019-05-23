#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include <QString>

class 1BackEnd : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString userName READ userName WRITE setUserName NOTIFY userNameChanged)
    Q_PROPERTY( location READ getNextLocation)

public:
    explicit BackEnd(QObject *parent = nullptr);

    QString userName();
    void setUserName(const QString &userName);
    QVector<double> getNextLocation();



private:
    QString m_userName;
    int currentIndex=0;


};



class BackEnd: public QObject{

    Q_OBJECT
    Q_PROPERTY()


public:
    explicit  BackEnd(QObject *parent = nullptr);

    


};

#endif // BACKEND_H