#ifndef MONITORWINTHREAD_H
#define MONITORWINTHREAD_H

#include <QThread>
#include <QApplication>
#include "TradeWindow.h"
#include "ToolWin.h"
#include "LoginErrorDialog.h"
#include "LoginWindow.h"
#include "RiskCtrlWin.h"

class MonitorWinThread : public QThread
{
    Q_OBJECT
public:
    MonitorWinThread(LoginWindow *pLogWin,LoginErrorDialog *pLogErrWin,ToolWin *pToolWin,
                  TradeWindow *pTrdWin,RiskCtrlWin *pRiskWin);
    ~MonitorWinThread()
    {
        stop();
        quit();
        wait();
    }
    void stop();
    void setAppPtr(QApplication *pQApp);

protected:
    void run();

private:
    bool m_StopThd;
    TradeWindow *m_pTrdWin;
    ToolWin *m_pToolWin;
    LoginErrorDialog *m_pLogErrWin;
    LoginWindow *m_pLogWin;
    RiskCtrlWin *m_pRiskWin;
    QApplication *m_pQApp;
};

#endif // MONITORWINTHREAD_H
