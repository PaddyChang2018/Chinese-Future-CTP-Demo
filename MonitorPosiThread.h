#ifndef MONITORPOSITHREAD_H
#define MONITORPOSITHREAD_H

#include <QThread>
#include "ThostFtdcTraderApi.h"
#include "ThostFtdcMdApi.h"

class MonitorPosiThread : public QThread
{
    Q_OBJECT
public:
    MonitorPosiThread();
    ~MonitorPosiThread();
    void setTrdApiPtr(CThostFtdcTraderApi *pTrdApi);
    void setMdApiPtr(CThostFtdcMdApi *pMdApi);
    void stop();

protected:
    void run();

private:
    bool m_StopThd;
    CThostFtdcTraderApi *m_pTrdApi;
    CThostFtdcMdApi *m_pMdApi;
};

#endif // MONITORPOSITHREAD_H
