#include <QThread>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSql>
#include <QtSql/QSqlQuery>
#include <QMessageBox>
#include "LoginWindow.h"
#include "ui_LoginWindow.h"
#include "TypeDef.h"
#include "QueryPosiThread.h"

extern int g_TrdLogin;
extern int g_MktLogin;
extern int g_QryLogin;
extern int g_PosiLogin;
extern int g_OrdRef;
extern int g_AccRef;
extern Account g_Account;
//
LoginWindow::LoginWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::LoginWindow)
{
    ui->setupUi(this);
}

LoginWindow::~LoginWindow()
{
    m_pQryPosiThd->stop();
    m_pMoniPosiThd->stop();
    m_pUpdateRiskUiThd->stop();
    CThostFtdcUserLogoutField userlogout;
    strcpy(userlogout.BrokerID, g_Account.broker);
    strcpy(userlogout.UserID, g_Account.investor);
    m_pMdApi->ReqUserLogout(&userlogout, LOGOUT_REQ_MD);
    m_pTrdApi->ReqUserLogout(&userlogout, LOGOUT_REQ_TRD);
    m_pPosiTrdApi->ReqUserLogout(&userlogout, LOGOUT_REQ_POSI);
    m_pQryTrdApi->ReqUserLogout(&userlogout, LOGOUT_REQ_QRY);
    m_pMdApi->Release();
    m_pTrdApi->Release();
    m_pPosiTrdApi->Release();
    m_pQryTrdApi->Release();
    delete ui;
}

void LoginWindow::setTrdApiPtr(CThostFtdcTraderApi *pTrdApi)
{
    m_pTrdApi = pTrdApi;
}

void LoginWindow::setQryTrdApiPtr(CThostFtdcTraderApi *pQryTrdApi)
{
    m_pQryTrdApi = pQryTrdApi;
}

void LoginWindow::setPosiTrdApiPtr(CThostFtdcTraderApi *pPosiTrdApi)
{
    m_pPosiTrdApi = pPosiTrdApi;
}

void LoginWindow::setMdApiPtr(CThostFtdcMdApi *pMdApi)
{
    m_pMdApi = pMdApi;
}

void LoginWindow::setLogErrDiagPtr(LoginErrorDialog *pLogErrDlg)
{
    m_pLogErrDlg = pLogErrDlg;
}

void LoginWindow::setToolWinPtr(ToolWin *pToolWin)
{
    m_pToolWin = pToolWin;
}

void LoginWindow::setQryPosiThreadPtr(QueryPosiThread *pQryPosiThd)
{
    m_pQryPosiThd = pQryPosiThd;
}

void LoginWindow::setMoniPosiThdPtr(MonitorPosiThread *pMoniPosiThd)
{
    m_pMoniPosiThd = pMoniPosiThd;
}
void LoginWindow::setWatchDogPtr(WatchDog *pWtDog)
{
    m_pWtDog = pWtDog;
}

void LoginWindow::setUpdateRiskUiThreadPtr(UpdateRiskCtrlWinUI *pUpdateRiskUiThd)
{
    m_pUpdateRiskUiThd = pUpdateRiskUiThd;
}

void LoginWindow::on_ExitBtn_clicked()
{
    m_pTrdApi->Release();
    m_pQryTrdApi->Release();
    m_pPosiTrdApi->Release();
    m_pMdApi->Release();
    exit(1);
}

void LoginWindow::on_LoginBtn_clicked()
{
    if ((ui->AccountLineEdit->text() == "") || (ui->PwdLineEdit->text() == ""))
    {
        QMessageBox::warning(this,"警告","用户名和密码不能为空",QMessageBox::Ok);
        return;
    }
    else
    {
        // 建立数据库连接
        QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
        db.setHostName("192.168.10.189");
        db.setDatabaseName("ctpfutures");
        db.setUserName("Paddy");
        db.setPassword("123456");
        if (!db.open())
        {
            QMessageBox::warning(this,"Warning","Can not connect database.",QMessageBox::Ok);
            return;
        }
        // 查询名义帐号
        char sqlstr[200];
        sprintf(sqlstr,"select NominalPassword,RealAccount from nominalaccount where NominalAccount = \"%s\";",
                ui->AccountLineEdit->text().toLatin1().data());
        QString nomiPwd;
        QString realAcc;
        QSqlQuery query;
        if (query.exec(sqlstr))
        {
            while(query.next())
            {
                nomiPwd = query.value(0).toString();
                realAcc = query.value(1).toString();
            }
        }
        else
        {
            QMessageBox::warning(this,"Warning","Query database failed.",QMessageBox::Ok);
            return;
        }
        if (nomiPwd != ui->PwdLineEdit->text())
        {
            QMessageBox::warning(this,"Warning","User or password error.",QMessageBox::Ok);
            return;
        }
        strcpy(g_Account.investor,realAcc.toLatin1().data());
        // 查询真实帐号密码
        sprintf(sqlstr,"select Password,Broker from realaccount where Account = \"%s\";",g_Account.investor);
        if (query.exec(sqlstr))
        {
            while(query.next())
            {
                strcpy(g_Account.password,query.value(0).toString().toLatin1().data());
                strcpy(g_Account.broker, query.value(1).toString().toLatin1().data());
            }
        }
        else
        {
            QMessageBox::warning(this,"Warning","Query database failed.",QMessageBox::Ok);
            return;
        }
        // 查询前置机
        sprintf(sqlstr,"select FrontAddress, FrontType from broker where BrokerID = \"%s\";",g_Account.broker);
        if (query.exec(sqlstr))
        {
            int frtType = 0;
            char frtAddr[100];
            while(query.next())
            {
                frtType = query.value(1).toInt();
                strcpy(frtAddr, query.value(0).toString().toLatin1().data());
                if (1 == frtType)
                {
                    m_pTrdApi->RegisterFront(frtAddr);
                    m_pPosiTrdApi->RegisterFront(frtAddr);
                    m_pQryTrdApi->RegisterFront(frtAddr);
                }
                else if(2 == frtType)
                {
                    m_pMdApi->RegisterFront(frtAddr);
                }
            }
        }
        else
        {
            QMessageBox::warning(this,"Warning","Query database failed.",QMessageBox::Ok);
            return;
        }
        // Trade Front
        /*
        char frt[100];
        // 标准CTP交易前置机
        strcpy(frt, "tcp://180.168.146.187:10000");
        m_pTrdApi->RegisterFront(frt);
        strcpy(frt, "tcp://180.168.146.187:10001");
        m_pTrdApi->RegisterFront(frt);
        strcpy(frt, "tcp://218.202.237.33 :10002");
        m_pTrdApi->RegisterFront(frt);
        // CTPMini1交易前置机
        strcpy(frt, "tcp://180.168.146.187:10003");
        m_pTrdApi->RegisterFront(frt);
        // CTP开发交易前置机
        strcpy(frt, "tcp://180.168.146.187:10030");
        m_pTrdApi->RegisterFront(frt);
        //
        // Trade Front
        // 标准CTP交易前置机
        strcpy(frt, "tcp://218.202.237.33 :10002");
        m_pQryTrdApi->RegisterFront(frt);
        strcpy(frt, "tcp://180.168.146.187:10000");
        m_pQryTrdApi->RegisterFront(frt);
        strcpy(frt, "tcp://180.168.146.187:10001");
        m_pQryTrdApi->RegisterFront(frt);
        // CTPMini1交易前置机
        strcpy(frt, "tcp://180.168.146.187:10003");
        m_pQryTrdApi->RegisterFront(frt);
        // CTP开发交易前置机
        strcpy(frt, "tcp://180.168.146.187:10030");
        m_pQryTrdApi->RegisterFront(frt);
        //

        // Trade Front
        // 标准CTP交易前置机
        strcpy(frt, "tcp://218.202.237.33 :10002");
        m_pPosiTrdApi->RegisterFront(frt);
        strcpy(frt, "tcp://180.168.146.187:10000");
        m_pPosiTrdApi->RegisterFront(frt);
        strcpy(frt, "tcp://180.168.146.187:10001");
        m_pPosiTrdApi->RegisterFront(frt);
        // CTPMini1交易前置机
        strcpy(frt, "tcp://180.168.146.187:10003");
        m_pPosiTrdApi->RegisterFront(frt);
        // CTP开发交易前置机
        strcpy(frt, "tcp://180.168.146.187:10030");
        m_pPosiTrdApi->RegisterFront(frt);
        //
        // Market Front
        // 标准CTP行情前置机
        strcpy(frt, "tcp://180.168.146.187:10010");
        m_pMdApi->RegisterFront(frt);
        strcpy(frt, "tcp://180.168.146.187:10011");
        m_pMdApi->RegisterFront(frt);
        strcpy(frt, "tcp://218.202.237.33 :10012");
        m_pMdApi->RegisterFront(frt);
        // CTPMini行情前置机
        strcpy(frt, "tcp://180.168.146.187:10013");
        m_pMdApi->RegisterFront(frt);
        // CTP开发行情前置机
        strcpy(frt, "tcp://180.168.146.187:10031");
        m_pMdApi->RegisterFront(frt);
        //
        */
        m_pQryTrdApi->Init();
        m_pTrdApi->Init();
        m_pPosiTrdApi->Init();
        m_pMdApi->Init();
        QThread::sleep(1);
        // 登录
        CThostFtdcReqUserLoginField req;
        memset(&req, 0, sizeof(req));
        strcpy(g_Account.broker,"9999");
        strcpy(req.BrokerID, "9999");
        strcpy(req.UserID, g_Account.investor);
        strcpy(req.Password, g_Account.password);
        int trdret = m_pTrdApi->ReqUserLogin(&req, LOGIN_REQ_TRD);
        int trdqryret = m_pQryTrdApi->ReqUserLogin(&req, LOGIN_REQ_QRY);
        int trdposiret = m_pPosiTrdApi->ReqUserLogin(&req, LOGIN_REQ_POSI);
        int mktret = m_pMdApi->ReqUserLogin(&req, LOGIN_REQ_MD);
        //
        if ((trdret != 0) || (mktret != 0) || (trdqryret != 0) || (trdposiret != 0))
        {
            m_pLogErrDlg->show();
            return;
        }
        for(int i=0; i<10; i++)
        {
            QThread::msleep(500);
            if ((g_TrdLogin == 0) && (g_MktLogin == 0) && (g_QryLogin == 0) && (g_PosiLogin == 0))
            {
                break;
            }
        }
        if ((g_TrdLogin == 0) && (g_MktLogin == 0) && (g_QryLogin == 0) && (g_PosiLogin == 0))
        {
            // 查询当天是否已经确认结算结果
            CThostFtdcQrySettlementInfoConfirmField cfmField;
            memset(&cfmField, 0, sizeof(cfmField));
            strcpy(cfmField.BrokerID, g_Account.broker);
            strcpy(cfmField.InvestorID, g_Account.investor);
            m_pTrdApi->ReqQrySettlementInfoConfirm(&cfmField, QRY_SETTLEMENT);
            // 请求查询资金账户
            CThostFtdcQryTradingAccountField accField;
            memset(&accField,0,sizeof(accField));
            strcpy(accField.BrokerID, g_Account.broker);
            strcpy(accField.InvestorID, g_Account.investor);
            strcpy(accField.CurrencyID, "CNY");
            //m_pTrdApi->ReqQryTradingAccount(&accField, ++g_AccRef);
            // 启动监控仓位线程
            m_pQryPosiThd->start();
            // 启动更新风控界面线程
            m_pUpdateRiskUiThd->start();
            // 启动查询合约交易所线程
            m_pMoniPosiThd->start();
            // 启动Watchdog
            m_pWtDog->start();
            // 显示工具模块窗口
            m_pToolWin->show();
            // 关闭登录窗
            this->close();
        }
        else
        {
            m_pLogErrDlg->show();
        }
    }
}
