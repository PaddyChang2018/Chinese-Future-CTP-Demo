#include <QHash>
#include <QThread>
#include "ShareOperation.h"
#include "TypeDef.h"

extern int g_OrdRef;
extern Account g_Account;
extern QHash<QString, Contract> g_InstHash;

// 填合约域
void fillOrderField(CThostFtdcInputOrderField *pOrd,
                    TThostFtdcBrokerIDType broker,
                    TThostFtdcInvestorIDType user,
                    char *instID,
                    int ordref,
                    TThostFtdcDirectionType direction,
                    TThostFtdcOffsetFlagType opencls,
                    double price,
                    int volume)
{
    memset(pOrd, 0, sizeof(*pOrd));
    strcpy(pOrd->BrokerID, broker);
    strcpy(pOrd->InvestorID, user);
    strcpy(pOrd->InstrumentID, instID);
    sprintf(pOrd->OrderRef, "%d", ordref);
    pOrd->OrderPriceType = THOST_FTDC_OPT_LimitPrice;
    pOrd->Direction = direction;
    pOrd->CombOffsetFlag[0] = opencls;
    pOrd->CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
    pOrd->LimitPrice = price;
    pOrd->VolumeTotalOriginal = volume;
    pOrd->TimeCondition = THOST_FTDC_TC_IOC;
    pOrd->VolumeCondition = THOST_FTDC_VC_CV;
    pOrd->MinVolume = 1;
    pOrd->ContingentCondition = THOST_FTDC_CC_Immediately;
    pOrd->ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
    pOrd->IsAutoSuspend = 0;
}

// 清仓函数，依赖全局变量g_Account,g_InstHash,g_OrdRef
void clearPosition(CThostFtdcTraderApi *pTrdApi, CThostFtdcMdApi *pMdApi)
{
    CThostFtdcInputOrderField ordField;
    Contract cont;
    QHashIterator<QString,Contract> ci(g_InstHash);
    while(ci.hasNext())
    {  
        cont = ci.next().value();
        // 无价格数据重新订阅
        int cnt = 0;
        while (((cont.ask <= 0) || (cont.bid <= 0) || (cont.ask <= 0) || (cont.bid <= 0)) && (cnt < 5))
        {
            char * instset[] = {cont.id.toLatin1().data()};
            pMdApi->SubscribeMarketData(instset, 1);
            QThread::sleep(1);
            cont = ci.value();
            cnt++;
        }
        if (cnt >= 5)
        {
            continue;
        }
        // 平今多头，卖平
        if (cont.td_long_posi > 0)
        {
            fillOrderField(&ordField, g_Account.broker, g_Account.investor, cont.id.toLatin1().data(), ++g_OrdRef,
                           THOST_FTDC_D_Sell, THOST_FTDC_OF_CloseToday, cont.bid, cont.td_long_posi);
            pTrdApi->ReqOrderInsert(&ordField, ordField.RequestID);
        }
        // 平昨多头，卖平
        if (cont.yd_long_posi > 0)
        {
            fillOrderField(&ordField, g_Account.broker, g_Account.investor, cont.id.toLatin1().data(), ++g_OrdRef,
                           THOST_FTDC_D_Sell, THOST_FTDC_OF_CloseYesterday, cont.bid, cont.yd_long_posi);
            pTrdApi->ReqOrderInsert(&ordField, ordField.RequestID);
        }
        // 平今空头，买平
        if(cont.td_short_posi > 0)
        {
            fillOrderField(&ordField, g_Account.broker, g_Account.investor, cont.id.toLatin1().data(), ++g_OrdRef,
                           THOST_FTDC_D_Buy, THOST_FTDC_OF_CloseToday, cont.ask, cont.td_short_posi);
            pTrdApi->ReqOrderInsert(&ordField, ordField.RequestID);
        }
        // 平昨空头，买平
        if(cont.yd_short_posi > 0)
        {
            fillOrderField(&ordField, g_Account.broker, g_Account.investor, cont.id.toLatin1().data(), ++g_OrdRef,
                           THOST_FTDC_D_Buy, THOST_FTDC_OF_CloseYesterday, cont.ask, cont.yd_short_posi);
            pTrdApi->ReqOrderInsert(&ordField, ordField.RequestID);
        }
    }
}
