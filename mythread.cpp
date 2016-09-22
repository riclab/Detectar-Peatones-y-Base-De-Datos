#include "mythread.h"

mongocxx::instance inst{};
mongocxx::client conn{mongocxx::uri{"mongodb://root:toor@ds017432.mlab.com:17432/riclabdb"}};

MyThread::MyThread(QObject *parent) :
    QThread(parent),
    m_Stopped(false)
{
}

void MyThread::setMessage(int& msg)
{
    m_Msg = msg;
}

void MyThread::stop()
{
    m_Stopped = true;
}

void MyThread::run()
{
    if (m_Stopped == false) {

       bsoncxx::builder::stream::document document{};
       auto collection = conn["riclabdb"]["datos"];

       document  << "peatones" << m_Msg
                 << "fecha" << bsoncxx::types::b_date{QDateTime::currentMSecsSinceEpoch()};

       collection.insert_one(document.view());

    }
    m_Stopped = false;
}
