#include <QHash>
#include "MonitorPosiThread.h"
#include "TypeDef.h"

extern int g_IF_ResOpenVol;
extern int g_IC_ResOpenVol;
extern int g_IH_ResOpenVol;
extern int g_TF_ResOpenVol;
extern int g_T_ResOpenVol;
extern int g_InstRef;
extern QHash<QString, Contract> g_InstHash;

MonitorPosiThread::MonitorPosiThread()
{
    m_StopThd = false;
}

MonitorPosiThread::~MonitorPosiThread()
{
    stop();
    quit();
    wait();
}

void MonitorPosiThread::stop()
{
    m_StopThd = false;
}

void MonitorPosiThread::setTrdApiPtr(CThostFtdcTraderApi *pTrdApi)
{
    m_pTrdApi = pTrdApi;
}

void MonitorPosiThread::setMdApiPtr(CThostFtdcMdApi *pMdApi)
{
    m_pMdApi = pMdApi;
}

void MonitorPosiThread::run()
{

    CThostFtdcQryInstrumentField qryInst;
    Contract cont;
    while (!m_StopThd)
    {
        int IF_Opened_Vol = 0;
        int IC_Opened_Vol = 0;
        int IH_Opened_Vol = 0;
        int TF_Opened_Vol = 0;
        int T_Opened_Vol  = 0;
        QHashIterator<QString,Contract> ci(g_InstHash);
        while(ci.hasNext())
        {
            bool needInsert = false;
            cont = ci.next().value();
            if ((cont.exchangID.isEmpty()) || (cont.exchangID.isNull()))
            {
                memset(&qryInst,0,sizeof(qryInst));
                strcpy(qryInst.InstrumentID, cont.id.toLatin1().data());
                m_pTrdApi->ReqQryInstrument(&qryInst, ++g_InstRef);
            }
            if ((cont.ask == 0) || (cont.bid == 0))
            {
                char * instset[] = {cont.id.toLatin1().data()};
                m_pMdApi->SubscribeMarketData(instset, 1);
            }
            //
            if (cont.id.left(2) == QString("IF"))
            {
                IF_Opened_Vol += (cont.long_open_vol + cont.short_open_vol);
                if (!cont.isCFFE)
                {
                    cont.isCFFE = true;
                    needInsert = true;
                }
            }
            else if (cont.id.left(2) == QString("IC"))
            {
                IC_Opened_Vol += (cont.long_open_vol + cont.short_open_vol);
                if (!cont.isCFFE)
                {
                    cont.isCFFE = true;
                    needInsert = true;
                }
            }
            else if (cont.id.left(2) == QString("IH"))
            {
                IH_Opened_Vol += (cont.long_open_vol + cont.short_open_vol);
                if (!cont.isCFFE)
                {
                    cont.isCFFE = true;
                    needInsert = true;
                }
            }
            else if (cont.id.left(2) == QString("TF"))
            {
                TF_Opened_Vol += (cont.long_open_vol + cont.short_open_vol);
                if (!cont.isCFFE)
                {
                    cont.isCFFE = true;
                    needInsert = true;
                }
            }
            else if (cont.id.left(1) == QString("T"))
            {
                T_Opened_Vol += (cont.long_open_vol + cont.short_open_vol);
                if (!cont.isCFFE)
                {
                    cont.isCFFE = true;
                    needInsert = true;
                }
            }
            //
            if (needInsert)
            {
                g_InstHash.insert(cont.id,cont);
            }
        }
        g_IF_ResOpenVol = IF_MAX_OPEN_VOL - IF_Opened_Vol;
        g_IC_ResOpenVol = IC_MAX_OPEN_VOL - IC_Opened_Vol;
        g_IH_ResOpenVol = IH_MAX_OPEN_VOL - IH_Opened_Vol;
        g_TF_ResOpenVol = TF_MAX_OPEN_VOL - TF_Opened_Vol;
        g_T_ResOpenVol  = T_MAX_OPEN_VOL  - T_Opened_Vol;
        msleep(10);
    }
}
