#include<QHash>
#include "MdSpi.h"
#include "TypeDef.h"

extern int g_MktLogin;
//
extern QHash<QString, Contract> g_InstHash;

// 登录请求响应
void MdSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
                           CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    // nRequestID始终为0
    if ((pRspInfo->ErrorID == 0) && bIsLast && (pRspUserLogin->LoginTime != NULL))
    {
        g_MktLogin = 0;
    }
    else
    {
        g_MktLogin = -1;
    }
}

// 深度行情通知
void MdSpi::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
{
    QString instId(pDepthMarketData->InstrumentID);
    Contract cont;
    if (g_InstHash.contains(instId))
    {
         cont = g_InstHash.value(instId);
         cont.ask = pDepthMarketData->AskPrice1+10;
         cont.bid = pDepthMarketData->BidPrice1-10;
    }
    else
    {
        cont.id = instId;
        cont.ask = pDepthMarketData->AskPrice1+10;
        cont.bid = pDepthMarketData->BidPrice1-10;
    }
    g_InstHash.insert(instId, cont);
    //qDebug("Market Data : %s ask=%0.2f bid=%0.2f",pDepthMarketData->InstrumentID,pDepthMarketData->AskPrice1,pDepthMarketData->BidPrice1);
}


