#include "ToolWin.h"
#include "ui_ToolWin.h"

extern bool IsCanOpenPosi;

ToolWin::ToolWin(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ToolWin)
{
    ui->setupUi(this);
}

ToolWin::~ToolWin()
{
    delete ui;
}

void ToolWin::setTradeWinPtr(TradeWindow *pTradeWin)
{
    m_pTradeWin = pTradeWin;
}

void ToolWin::setRiskCtrlWinPtr(RiskCtrlWin *pRiskWin)
{
    m_pRiskWin = pRiskWin;
}

void ToolWin::on_GoodsTrdBtn_clicked()
{
    m_pTradeWin->show();
    m_pTradeWin->clearMsg();
}

void ToolWin::on_RiskCtrlBtn_clicked()
{
    m_pRiskWin->show();
}
