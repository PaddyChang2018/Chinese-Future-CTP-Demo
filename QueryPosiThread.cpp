#include "QueryPosiThread.h"
#include "TypeDef.h"

extern Account g_Account;
extern int g_PosiRef;

void QueryPosiThread::setTrdApiPtr(CThostFtdcTraderApi *pTrdApi)
{
    m_pTrdApi = pTrdApi;
}

void QueryPosiThread::stop()
{
    m_StopThd = true;
}

void QueryPosiThread::run()
{
    CThostFtdcQryInvestorPositionField posi;
    memset(&posi,0,sizeof(posi));
    strcpy(posi.BrokerID, g_Account.broker);
    strcpy(posi.InvestorID, g_Account.investor);
    strcpy(posi.InstrumentID,"");
    //
    while(!m_StopThd)
    {
        // 查询仓位请求
        m_pTrdApi->ReqQryInvestorPosition(&posi, ++g_PosiRef);
        msleep(50);
    }
    m_StopThd = false;
}
