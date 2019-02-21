#ifndef RISKCTRLWIN_H
#define RISKCTRLWIN_H

#include <QMainWindow>
#include "ThostFtdcTraderApi.h"
#include "ThostFtdcMdApi.h"
#include "RiskCtrlThread.h"

namespace Ui {
class RiskCtrlWin;
}

class RiskCtrlWin : public QMainWindow
{
    Q_OBJECT

public:
    explicit RiskCtrlWin(QWidget *parent = 0);
    ~RiskCtrlWin();
    void setTrdApiPtr(CThostFtdcTraderApi *pTrdApi);
    void setMdApiPtr(CThostFtdcMdApi *pMdApi);
    void setRealNetAssetLabelText(QString &txt);

private slots:
    void on_RiskCtrlBtn_clicked();
    void on_ClearPosiBtn_clicked();
    void on_NetAssetLineEdit_editingFinished();

private:
    Ui::RiskCtrlWin *ui;
    CThostFtdcTraderApi *m_pTrdApi;
    CThostFtdcMdApi *m_pMdApi;
    RiskCtrlThread m_RiskThd;
};

#endif // RISKCTRLWIN_H
