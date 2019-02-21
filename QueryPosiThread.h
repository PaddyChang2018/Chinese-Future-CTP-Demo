#ifndef QUERYPOSITHREAD_H
#define QUERYPOSITHREAD_H

#include <QThread>
#include "ThostFtdcTraderApi.h"

class QueryPosiThread : public QThread
{
    Q_OBJECT
public:
    QueryPosiThread()
    {
        m_StopThd = false;
    }
    ~QueryPosiThread()
    {
        stop();
        quit();
        wait();
    }
    void setTrdApiPtr(CThostFtdcTraderApi *pTrdApi);
    void stop();

protected:
    void run();

private:
    bool m_StopThd;
    CThostFtdcTraderApi *m_pTrdApi;
};

#endif // QUERYPOSITHREAD_H
