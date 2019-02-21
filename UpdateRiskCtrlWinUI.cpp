#include "UpdateRiskCtrlWinUI.h"
#include "TypeDef.h"

extern Account g_Account;

void UpdateRiskCtrlWinUI::stop()
{
    m_StopThd = true;
}

void UpdateRiskCtrlWinUI::setRiskWinPtr(RiskCtrlWin *pRiskWin)
{
    m_pRiskWin = pRiskWin;
}

void UpdateRiskCtrlWinUI::run()
{
    while(!m_StopThd)
    {
        // 设置RiskWin中净资产label值
        m_pRiskWin->setRealNetAssetLabelText(QString::number(g_Account.balance,'f',2));
        msleep(700);
    }
    m_StopThd = false;
}
