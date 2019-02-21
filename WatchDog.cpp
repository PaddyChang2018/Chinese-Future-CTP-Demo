#include "WatchDog.h"
#include "TypeDef.h"

extern bool g_IsDisconnected;
extern Account g_Account;
extern int g_TrdLogin;
extern int g_QryLogin;
extern int g_PosiLogin;
extern int g_MktLogin;

void WatchDog::stop()
{
    m_StopThd = true;
}

void WatchDog::run()
{
    while(!m_StopThd)
    {
        if (g_IsDisconnected)
        {
            CThostFtdcReqUserLoginField req;
            memset(&req, 0, sizeof(req));
            strcpy(req.BrokerID, g_Account.broker);
            strcpy(req.UserID, g_Account.investor);
            strcpy(req.Password, g_Account.password);
            int trdret = m_pTrdApi->ReqUserLogin(&req, LOGIN_REQ_TRD);
            int qryret = m_pQryTrdApi->ReqUserLogin(&req, LOGIN_REQ_QRY);
            int posiret = m_pPosiTrdApi->ReqUserLogin(&req, LOGIN_REQ_POSI);
            int mdret = m_pMdApi->ReqUserLogin(&req, LOGIN_REQ_MD);
            //qDebug("Reconnecting...");
            sleep(5);
            if ((trdret == 0) && (qryret == 0) && (posiret == 0) && (mdret == 0))
            {
                if((g_TrdLogin == 0) && (g_QryLogin == 0) && (g_PosiLogin == 0) && (g_MktLogin == 0))
                {
                    g_IsDisconnected = false;
                    //qDebug("Reconnected!");
                }
            }
        }
        sleep(1);
    }
}
