#ifndef MDSPI_H
#define MDSPI_H

#include "ThostFtdcMdApi.h"

class MdSpi : public CThostFtdcMdSpi
{
public:
    //MdSpi(CThostFtdcMdApi *pMdApi):m_pMdApi(pMdApi){}
    MdSpi(){}
    ~MdSpi(){}
    // 登录请求响应
    virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
                                CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast);
    // 深度行情通知
    virtual void OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData);

private:
    //CThostFtdcMdApi *m_pMdApi;
};

#endif // MDSPI_H
