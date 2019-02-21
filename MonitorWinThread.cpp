#include "ExitThread.h"

MonitorWinThread::MonitorWinThread(LoginWindow *pLogWin,LoginErrorDialog *pLogErrWin,ToolWin *pToolWin,
                             TradeWindow *pTrdWin,RiskCtrlWin *pRiskWin)
{
    m_StopThd = false;
    m_pLogWin = pLogWin;
    m_pLogErrWin = pLogErrWin;
    m_pToolWin = pToolWin;
    m_pTrdWin = pTrdWin;
    m_pRiskWin = pRiskWin;
}

void MonitorWinThread::stop()
{
    m_StopThd = true;
}

void MonitorWinThread::setAppPtr(QApplication *pQApp)
{
    m_pQApp = pQApp;
}

void MonitorWinThread::run()
{
    while (!m_StopThd)
    {
        sleep(1);
        if (m_pLogErrWin->isVisible() || m_pLogWin->isVisible() || m_pToolWin->isVisible() ||
            m_pTrdWin->isVisible() || m_pRiskWin->isVisible())
        {
            continue;
        }
        else
        {
            m_pLogErrWin->close();
            m_pLogWin->close();
            m_pToolWin->close();
            m_pRiskWin->close();
            m_pLogWin->close();
            delete m_pLogWin;
            delete m_pLogErrWin;
            delete m_pLogWin;
            delete m_pToolWin;
            delete m_pRiskWin;
            m_pQApp->exit(1);
        }
    }
}
