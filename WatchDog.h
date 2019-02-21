#ifndef WATCHDOG_H
#define WATCHDOG_H

#include <QThread>
#include "ThostFtdcTraderApi.h"
#include "ThostFtdcMdApi.h"

class WatchDog : public QThread
{
    Q_OBJECT
public:
    WatchDog(CThostFtdcTraderApi *pTrd, CThostFtdcTraderApi *pPosi, CThostFtdcTraderApi *pQry,CThostFtdcMdApi *pMd) :
    m_pTrdApi(pTrd),m_pPosiTrdApi(pPosi),m_pQryTrdApi(pQry),m_pMdApi(pMd)
    {
        m_StopThd = false;
    }
    ~WatchDog()
    {
        stop();
        quit();
        wait();
    }
    void stop();

protected:
    void run();

private:
    bool m_StopThd;
    CThostFtdcTraderApi *m_pTrdApi;
    CThostFtdcTraderApi *m_pPosiTrdApi;
    CThostFtdcTraderApi *m_pQryTrdApi;
    CThostFtdcMdApi *m_pMdApi;
};

#endif // WATCHDOG_H
