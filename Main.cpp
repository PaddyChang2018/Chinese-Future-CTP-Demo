#include <stdio.h>
#include <string.h>
#include <QApplication>
#include <QHash>
#include <QQueue>
#include "TradeSpi.h"
#include "MdSpi.h"
#include "LoginWindow.h"
#include "TradeWindow.h"
#include "ToolWin.h"
#include "RiskCtrlWin.h"
#include "TypeDef.h"
#include "QueryPosiThread.h"
#include "UpdateRiskCtrlWinUI.h"
#include "MonitorPosiThread.h"
#include "ExitThread.h"
#include "WatchDog.h"

//登录用
int g_TrdLogin = -1;
int g_QryLogin = -1;
int g_PosiLogin = -1;
int g_MktLogin = -1;
//
int g_OrdRef = 0;
int g_PosiRef = 0;
int g_AccRef = 0;
int g_InstRef = 0;

// Control
bool g_IsRiskCtrl = false;
bool g_IsCanOpenPosi = true;
bool g_IsDisconnected = false;
// CFFE
int g_IF_ResOpenVol = 0;
int g_IC_ResOpenVol = 0;
int g_IH_ResOpenVol = 0;
int g_TF_ResOpenVol = 0;
int g_T_ResOpenVol  = 0;
//
double g_BalanceLmt = 0;

// 套利合约名称
QString g_BuyInstID;
QString g_SellInstID;

// 合约Hash表
QHash<QString, Contract> g_InstHash;

// 账户信息
Account g_Account;
//
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LoginWindow loginWin;
    LoginErrorDialog logErrDiag;
    TradeWindow tradeWin;
    ToolWin toolWin;
    RiskCtrlWin riskWin;
    QueryPosiThread qryPosiThd;
    UpdateRiskCtrlWinUI updateRiskUiThd;
    MonitorPosiThread moniPosiThd;
    MonitorWinThread moniWinThd(&loginWin,&logErrDiag,&toolWin,&tradeWin,&riskWin);
    moniWinThd.setAppPtr(&a);
    //
    tradeWin.setWindowFlags(Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);
    toolWin.setWindowFlags(Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);
    loginWin.setWindowFlags(Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);
    riskWin.setWindowFlags(Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);
    logErrDiag.setWindowFlags(Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint);
    // 初始化交易类对象
    CThostFtdcTraderApi *pTrdApi= CThostFtdcTraderApi::CreateFtdcTraderApi();			// 创建UserApi
    TradeSpi trdspi(pTrdApi);
    pTrdApi->RegisterSpi(&trdspi);
    pTrdApi->SubscribePublicTopic(THOST_TERT_QUICK);
    pTrdApi->SubscribePrivateTopic(THOST_TERT_QUICK);
    //-----------------------------------------------------------------------
    // 初始化仅供查询用交易类对象
    CThostFtdcTraderApi *pQryTrdApi= CThostFtdcTraderApi::CreateFtdcTraderApi();
    TradeSpi qrytrdspi(pQryTrdApi);
    //
    pQryTrdApi->RegisterSpi(&qrytrdspi);
    pQryTrdApi->SubscribePublicTopic(THOST_TERT_QUICK);
    pQryTrdApi->SubscribePrivateTopic(THOST_TERT_QUICK);
    //-----------------------------------------------------------------------
    // 初始化仅供查询用交易类对象
    CThostFtdcTraderApi *pPosiTrdApi= CThostFtdcTraderApi::CreateFtdcTraderApi();			// 创建UserApi
    TradeSpi positrdspi(pPosiTrdApi);
    pPosiTrdApi->RegisterSpi(&positrdspi);
    pPosiTrdApi->SubscribePublicTopic(THOST_TERT_QUICK);
    pPosiTrdApi->SubscribePrivateTopic(THOST_TERT_QUICK);
    //-----------------------------------------------------------------------
    // 初始化行情类对象
    CThostFtdcMdApi *pMdApi = CThostFtdcMdApi::CreateFtdcMdApi();
    MdSpi mdspi;
    pMdApi->RegisterSpi(&mdspi);
    //
    trdspi.setMdApiPtr(pMdApi);
    qrytrdspi.setMdApiPtr(pMdApi);
    positrdspi.setMdApiPtr(pMdApi);
    //
    WatchDog wtdog(pTrdApi,pPosiTrdApi,pQryTrdApi,pMdApi);
    //
    tradeWin.setTrdApiPtr(pTrdApi);
    tradeWin.setMdApiPtr(pMdApi);
    //
    loginWin.setTrdApiPtr(pTrdApi);
    loginWin.setQryTrdApiPtr(pQryTrdApi);
    loginWin.setPosiTrdApiPtr(pPosiTrdApi);
    loginWin.setMdApiPtr(pMdApi);
    loginWin.setLogErrDiagPtr(&logErrDiag);
    loginWin.setToolWinPtr(&toolWin);
    loginWin.setWatchDogPtr(&wtdog);
    //
    toolWin.setTradeWinPtr(&tradeWin);
    toolWin.setRiskCtrlWinPtr(&riskWin);
    //
    riskWin.setTrdApiPtr(pQryTrdApi);
    riskWin.setMdApiPtr(pMdApi);
    //
    qryPosiThd.setTrdApiPtr(pPosiTrdApi);
    //
    updateRiskUiThd.setRiskWinPtr(&riskWin);
    //
    moniPosiThd.setTrdApiPtr(pQryTrdApi);
    moniPosiThd.setMdApiPtr(pMdApi);
    //
    loginWin.setQryPosiThreadPtr(&qryPosiThd);
    loginWin.setUpdateRiskUiThreadPtr(&updateRiskUiThd);
    loginWin.setMoniPosiThdPtr(&moniPosiThd);
    //
    loginWin.show();
    moniWinThd.start();
    //
    return a.exec();
}
