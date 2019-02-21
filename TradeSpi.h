#ifndef TRADESPI_H
#define TRADESPI_H

#include "ThostFtdcTraderApi.h"
#include "ThostFtdcMdApi.h"

class TradeSpi : public CThostFtdcTraderSpi
{
public:
    // Constructor
    TradeSpi(CThostFtdcTraderApi *pTrdApi):m_pTrdApi(pTrdApi){}
    // Deconstructor
    ~TradeSpi(){}
    // 当客户端与交易后台通信连接断开时，该方法被调用。当发生这个情况后，API会自动重新连接，客户端可不做处理。
    // @param nReason 错误原因
    //        0x1001 网络读失败
    //        0x1002 网络写失败
    //        0x2001 接收心跳超时
    //        0x2002 发送心跳失败
    //        0x2003 收到错误报文
    virtual void OnFrontDisconnected(int nReason);

    // 登录请求响应
    virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
                                CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);



    // 请求查询结算信息是否确认响应
    virtual void OnRspQrySettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm,
                                               CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    // 请求查询投资者结算单响应
    virtual void OnRspQrySettlementInfo(CThostFtdcSettlementInfoField *pSettlementInfo,
                                        CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    // 结算结果确认响应
    virtual void OnRspSettlementInfoConfirm(CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm,
                                            CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    // 请求查询投资者持仓响应
    virtual void OnRspQryInvestorPosition(CThostFtdcInvestorPositionField *pInvestorPosition,
                                          CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    // 请求查询资金账户响应
    virtual void OnRspQryTradingAccount(CThostFtdcTradingAccountField *pTradingAccount,
                                        CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);

    // 请求查询合约响应
    virtual void OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument, CThostFtdcRspInfoField *pRspInfo,
                                    int nRequestID, bool bIsLast);

    // 成交通知
    virtual void OnRtnTrade(CThostFtdcTradeField *pTrade);

    // 设置行情对象MdApi指针
    void setMdApiPtr(CThostFtdcMdApi *pMdApi);

private:
    CThostFtdcTraderApi *m_pTrdApi;
    CThostFtdcMdApi *m_pMdApi;
};

#endif // TRADESPI_H
