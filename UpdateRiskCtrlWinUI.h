#ifndef UPDATERISKCTRLWINUI_H
#define UPDATERISKCTRLWINUI_H

#include <QThread>
#include "RiskCtrlWin.h"

class UpdateRiskCtrlWinUI : public QThread
{
    Q_OBJECT
public:
    UpdateRiskCtrlWinUI()
    {
        m_StopThd = false;
    }
    ~UpdateRiskCtrlWinUI()
    {
        stop();
        quit();
        wait();
    }
    void stop();
    void setRiskWinPtr(RiskCtrlWin *pRiskWin);

protected:
    void run();

private:
    bool m_StopThd;
    RiskCtrlWin *m_pRiskWin;
};

#endif // UPDATERISKCTRLWINUI_H
