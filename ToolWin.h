#ifndef TOOLWIN_H
#define TOOLWIN_H

#include <QWidget>
#include "TradeWindow.h"
#include "RiskCtrlWin.h"

namespace Ui {
class ToolWin;
}

class ToolWin : public QWidget
{
    Q_OBJECT

public:
    explicit ToolWin(QWidget *parent = 0);
    ~ToolWin();
    void setTradeWinPtr(TradeWindow *pTradeWin);
    void setRiskCtrlWinPtr(RiskCtrlWin *pRiskWin);

private slots:
    void on_GoodsTrdBtn_clicked();
    void on_RiskCtrlBtn_clicked();

private:
    Ui::ToolWin *ui;
    TradeWindow *m_pTradeWin;
    RiskCtrlWin *m_pRiskWin;
};

#endif // TOOLWIN_H
