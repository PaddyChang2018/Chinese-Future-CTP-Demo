#include "RiskCtrlThread.h"
#include "TypeDef.h"
#include "ShareOperation.h"

extern double g_BalanceLmt;
extern Account g_Account;
extern bool g_IsCanOpenPosi;
extern int g_OrdRef;
extern int g_AccRef;

void RiskCtrlThread::stop()
{
    m_StopThd = true;
}

void RiskCtrlThread::setTrdApiPtr(CThostFtdcTraderApi *pTrdApi)
{
    m_pTrdApi = pTrdApi;
}

void RiskCtrlThread::setMdApiPtr(CThostFtdcMdApi *pMdApi)
{
    m_pMdApi = pMdApi;
}

void RiskCtrlThread::run()
{
    CThostFtdcQryTradingAccountField accField;
    memset(&accField, 0, sizeof(accField));
    strcpy(accField.BrokerID, g_Account.broker);
    strcpy(accField.InvestorID, g_Account.investor);
    //strcpy(accField.CurrencyID, "CNY");
    while (!m_StopThd)
    {
        // 查询资金帐户
        m_pTrdApi->ReqQryTradingAccount(&accField, ++g_AccRef);
        sleep(1);
        if ((g_Account.balance < g_BalanceLmt) && (g_Account.balance > 0) && (g_IsCanOpenPosi))
        {
            // 清仓
            clearPosition(m_pTrdApi, m_pMdApi);
            g_IsCanOpenPosi = false;
        }
        //qDebug("BalanceLmt=%0.2f RealBalance=%0.2f Risk Control ...", g_BalanceLmt, g_Account.balance);
    }
    m_StopThd = false;
    g_IsCanOpenPosi = true;
    g_Account.balance = 0;
}
