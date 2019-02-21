#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QMainWindow>
#include "LoginErrorDialog.h"
#include "TradeWindow.h"
#include "ToolWin.h"
#include "QueryPosiThread.h"
#include "UpdateRiskCtrlWinUI.h"
#include "MonitorPosiThread.h"
#include "WatchDog.h"

namespace Ui {
class LoginWindow;
}

class LoginWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit LoginWindow(QWidget *parent = 0);
    ~LoginWindow();
    void setTrdApiPtr(CThostFtdcTraderApi *pTrdApi);
    void setQryTrdApiPtr(CThostFtdcTraderApi *pQryTrdApi);
    void setMdApiPtr(CThostFtdcMdApi *pMdApi);
    void setLogErrDiagPtr(LoginErrorDialog *pLogErrDlg);
    void setToolWinPtr(ToolWin *pToolWin);
    void setQryPosiThreadPtr(QueryPosiThread *pQryPosiThd);
    void setUpdateRiskUiThreadPtr(UpdateRiskCtrlWinUI *pUpdateRiskUiThd);
    void setMoniPosiThdPtr(MonitorPosiThread *pMoniPosiThd);
    void setPosiTrdApiPtr(CThostFtdcTraderApi *pPosiTrdApi);
    void setWatchDogPtr(WatchDog *pWtDog);

private slots:
    void on_LoginBtn_clicked();
    void on_ExitBtn_clicked();

private:
    Ui::LoginWindow *ui;
    LoginErrorDialog *m_pLogErrDlg;
    ToolWin *m_pToolWin;
    CThostFtdcTraderApi *m_pTrdApi;
    CThostFtdcTraderApi *m_pQryTrdApi;
    CThostFtdcTraderApi *m_pPosiTrdApi;
    CThostFtdcMdApi *m_pMdApi;
    QueryPosiThread *m_pQryPosiThd;
    UpdateRiskCtrlWinUI *m_pUpdateRiskUiThd;
    MonitorPosiThread *m_pMoniPosiThd;
    WatchDog *m_pWtDog;
};

#endif // LOGINWINDOW_H
