#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QThread>
#include <QDateTime>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/types.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>

class MyThread : public QThread
{
    Q_OBJECT
public:
    explicit MyThread(QObject *parent = 0);
    void setMessage(int &msg);
    void stop();

signals:

public slots:

protected:
    void run();

private:
    volatile bool m_Stopped;
    int m_Msg;

};

#endif // MYTHREAD_H
