#ifndef RISKCTRLTHREAD_H
#define RISKCTRLTHREAD_H

#include <QThread>
#include "ThostFtdcTraderApi.h"
#include "ThostFtdcMdApi.h"

class RiskCtrlThread : public QThread
{
    Q_OBJECT
public:
    RiskCtrlThread()
    {
        m_StopThd = false;
    }
    ~RiskCtrlThread()
    {
        stop();
        quit();
        wait();
    }
    void stop();
    void setTrdApiPtr(CThostFtdcTraderApi *pTrdApi);
    void setMdApiPtr(CThostFtdcMdApi *pMdApi);

protected:
    void run();

private:
    bool m_StopThd;
    CThostFtdcTraderApi *m_pTrdApi;
    CThostFtdcMdApi *m_pMdApi;
};

#endif // RISKCTRLTHREAD_H
