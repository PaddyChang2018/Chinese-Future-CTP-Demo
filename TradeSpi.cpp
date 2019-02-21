#include <QHash>
#include <QQueue>
#include <QMessageBox>
#include "TradeSpi.h"
#include "TypeDef.h"

extern int g_TrdLogin;
extern int g_QryLogin;
extern int g_PosiLogin;
extern int g_MktLogin;
extern int g_OrdRef;
//
extern int g_TodayOpenIFPosi;
extern bool g_IsDisconnected;
//
extern double g_BalanceLmt;
extern QHash<QString, Contract> g_InstHash;
extern Account g_Account;

// 当客户端与交易后台通信连接断开时，该方法被调用。当发生这个情况后，API会自动重新连接，客户端可不做处理。
// @param nReason 错误原因
//        0x1001 网络读失败
//        0x1002 网络写失败
//        0x2001 接收心跳超时
//        0x2002 发送心跳失败
//        0x2003 收到错误报文
void TradeSpi::OnFrontDisconnected(int nReason)
{
    g_IsDisconnected = true;
    g_TrdLogin = -1;
    g_QryLogin = -1;
    g_PosiLogin = -1;
    g_MktLogin = -1;
}

// 设置行情对象MdApi指针
void TradeSpi::setMdApiPtr(CThostFtdcMdApi *pMdApi)
{
    m_pMdApi = pMdApi;
}

// 登录请求响应
void TradeSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
                              CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if ((pRspInfo->ErrorID == 0) && bIsLast && (pRspUserLogin->LoginTime != NULL))
    {
        if (nRequestID == LOGIN_REQ_TRD)
        {
            g_TrdLogin = 0;
        }
        else if(nRequestID == LOGIN_REQ_POSI)
        {
            g_PosiLogin = 0;
        }
        else if(nRequestID == LOGIN_REQ_QRY)
        {
            g_QryLogin = 0;
        }
    }
    else
    {
        if (nRequestID == LOGIN_REQ_TRD)
        {
            g_TrdLogin = -1;
        }
        else if(nRequestID == LOGIN_REQ_POSI)
        {
            g_PosiLogin = -1;
        }
        else if(nRequestID == LOGIN_REQ_QRY)
        {
            g_QryLogin = -1;
        }
    }
}

//请求查询结算信息确认响应
void TradeSpi::OnRspQrySettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm,
                                             CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pSettlementInfoConfirm != NULL)
    {
        if (0 == strcmp(pSettlementInfoConfirm->InvestorID, g_Account.investor))
        {
            return;
        }
    }
    // 请求确认结算单
    CThostFtdcSettlementInfoConfirmField cfmField;
    memset(&cfmField, 0 ,sizeof(cfmField));
    strcpy(cfmField.BrokerID, g_Account.broker);
    strcpy(cfmField.InvestorID, g_Account.investor);
    m_pTrdApi->ReqSettlementInfoConfirm(&cfmField, QRY_SETTLEMENT);
}

// 请求查询结算单响应
void TradeSpi::OnRspQrySettlementInfo(CThostFtdcSettlementInfoField *pSettlementInfo,
                                      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
}

// 投资者确认结算单响应
void TradeSpi::OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm,
                                          CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
}

// 请求查询投资者持仓响应
void TradeSpi::OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition,
                                        CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pInvestorPosition == NULL)
    {
        //没仓位则返回，否则程序会崩溃
        return;
    }
    //qDebug("inst:%s dir=%d posi=%d tdposi=%d ydposi=%d openvol=%d closevol=%d",
    //       pInvestorPosition->InstrumentID,pInvestorPosition->PosiDirection,pInvestorPosition->Position,pInvestorPosition->TodayPosition,
    //       pInvestorPosition->YdPosition,pInvestorPosition->OpenVolume,pInvestorPosition->CloseVolume);
    //记录昨仓，今仓
    Contract cont;
    QString instId(pInvestorPosition->InstrumentID);
    if (g_InstHash.contains(instId))
    {
        cont = g_InstHash.value(instId);
    }
    else
    {
        // 订阅行情数据
        char * instset[] = {pInvestorPosition->InstrumentID};
        m_pMdApi->SubscribeMarketData(instset,1);
        // 更新持仓数
        cont.id = QString(pInvestorPosition->InstrumentID);
    }
    //
    if (pInvestorPosition->PosiDirection == THOST_FTDC_PD_Long)
    {
        if (pInvestorPosition->YdPosition == 0)
        {
            cont.td_long_posi = pInvestorPosition->Position;
            if (cont.exchangID != QString("SHFE"))
            {
                cont.yd_long_posi = 0;
            }
        }
        else if (pInvestorPosition->YdPosition > 0)
        {
            if (pInvestorPosition->TodayPosition == 0)
            {
                cont.yd_long_posi = pInvestorPosition->Position;
                if (cont.exchangID != QString("SHFE"))
                {
                    cont.td_long_posi = 0;
                }
            }
            else if (pInvestorPosition->TodayPosition > 0)
            {
                cont.td_long_posi = pInvestorPosition->TodayPosition;
                int yd = pInvestorPosition->Position - pInvestorPosition->TodayPosition;
                cont.yd_long_posi =  (yd > 0) ? yd : 0;
            }
        }
        cont.long_open_vol = pInvestorPosition->OpenVolume;
    }
    else if (pInvestorPosition->PosiDirection == THOST_FTDC_PD_Short)
    {
        if (pInvestorPosition->YdPosition == 0)
        {
            cont.td_short_posi = pInvestorPosition->Position;
            if (cont.exchangID != QString("SHFE"))
            {
                cont.yd_short_posi = 0;
            }
        }
        else if (pInvestorPosition->YdPosition > 0)
        {
            if (pInvestorPosition->TodayPosition == 0)
            {
                cont.yd_short_posi = pInvestorPosition->Position;
                if (cont.exchangID != QString("SHFE"))
                {
                    cont.td_short_posi = 0;
                }
            }
            else if (pInvestorPosition->TodayPosition > 0)
            {
                cont.td_short_posi = pInvestorPosition->TodayPosition;
                int yd = pInvestorPosition->Position - pInvestorPosition->TodayPosition;
                cont.yd_short_posi = (yd > 0) ? yd : 0;
            }
        }
        cont.short_open_vol = pInvestorPosition->OpenVolume;
    }
    g_InstHash.insert(instId, cont);
    //qDebug("inst:%s td_long=%d yd_long=%d td_short=%d yd_short=%d long_openvol=%d short_openvol=%d",
    //       cont.id.toStdString(),cont.td_long_posi,cont.yd_long_posi,cont.td_short_posi,cont.yd_short_posi,
    //       cont.long_open_vol,cont.short_open_vol);
}

// 请求查询资金账户响应
void TradeSpi::OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount,
                                      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
    if (pTradingAccount != NULL)
    {
        g_Account.balance = pTradingAccount->Balance;
    }
}

// 成交通知
void TradeSpi::OnRtnTrade(CThostFtdcTradeField *pTrade)
{ 
}

// 请求查询合约响应
void TradeSpi::OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo,
                                  int nRequestID, bool bIsLast)
{
    if (pInstrument != NULL)
    {
        Contract cont;
        QString inst = QString(pInstrument->InstrumentID);
        if (g_InstHash.contains(inst))
        {
            cont = g_InstHash.value(inst);
            cont.exchangID = QString(pInstrument->ExchangeID);
            g_InstHash.insert(inst, cont);
        }
    }
}
