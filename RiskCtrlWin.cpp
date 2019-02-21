#include <QMessageBox>
#include <QHash>
#include "RiskCtrlWin.h"
#include "ui_RiskCtrlWin.h"
#include "TypeDef.h"
#include "ShareOperation.h"

extern bool g_IsRiskCtrl;
extern double g_BalanceLmt;
extern QHash<QString, Contract> g_InstHash;
extern Account g_Account;
extern int g_OrdRef;

RiskCtrlWin::RiskCtrlWin(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::RiskCtrlWin)
{
    ui->setupUi(this);
}

RiskCtrlWin::~RiskCtrlWin()
{
    delete ui;
    m_RiskThd.stop();
    m_RiskThd.quit();
    m_RiskThd.wait();
    m_pTrdApi->Release();
}

void RiskCtrlWin::setTrdApiPtr(CThostFtdcTraderApi *pTrdApi)
{
    m_pTrdApi = pTrdApi;
}

void RiskCtrlWin::setMdApiPtr(CThostFtdcMdApi *pMdApi)
{
    m_pMdApi = pMdApi;
}

void RiskCtrlWin::setRealNetAssetLabelText(QString &txt)
{
    ui->RealNetAssetLabel->setText(txt);
}

void RiskCtrlWin::on_RiskCtrlBtn_clicked()
{
    if (g_IsRiskCtrl == false)
    {
        // 获取净资产值
        QByteArray qbalance = ui->NetAssetLineEdit->text().toLatin1();
        g_BalanceLmt = atof(qbalance.data());
        if (g_BalanceLmt <= 0)
        {
            g_BalanceLmt = 0;
            QMessageBox::warning(this,"警告","净资产限值不能为空或负数",QMessageBox::Ok);
            return;
        }
        //
        g_IsRiskCtrl = true;
        ui->RiskCtrlBtn->setText("停止");
        // 开始监控, 启动监控线程
        m_RiskThd.setTrdApiPtr(m_pTrdApi);
        m_RiskThd.setMdApiPtr(m_pMdApi);
        m_RiskThd.start();
    }
    else
    {
        // 停止监控
        m_RiskThd.stop();
        g_IsRiskCtrl = false;
        ui->RiskCtrlBtn->setText("监控");
    }
}

void RiskCtrlWin::on_ClearPosiBtn_clicked()
{
    int msgRet = QMessageBox::warning(this,"警告","确定要平掉所有头寸吗?",QMessageBox::Yes,QMessageBox::Abort);
    if (msgRet == QMessageBox::Yes)
    {
        clearPosition(m_pTrdApi, m_pMdApi);
    }
}

void RiskCtrlWin::on_NetAssetLineEdit_editingFinished()
{
    QByteArray qbalance = ui->NetAssetLineEdit->text().toLatin1();
    g_BalanceLmt = atof(qbalance.data());
    if (g_BalanceLmt <= 0)
    {
        g_BalanceLmt = 0;
        QMessageBox::warning(this,"警告","净资产限值不能为空或负数",QMessageBox::Ok);
    }
}
