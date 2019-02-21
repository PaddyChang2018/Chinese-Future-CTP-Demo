#include <QThread>
#include "TradeWindow.h"
#include "ui_TradeWindow.h"
#include "TypeDef.h"
#include "ShareOperation.h"

extern int g_OrdRef;
//
extern QString g_BuyInstID;
extern QString g_SellInstID;
//
extern QHash<QString, Contract> g_InstHash;
extern Account g_Account;
//
extern bool g_IsCanOpenPosi;
//
extern int g_IF_ResOpenVol;
extern int g_IC_ResOpenVol;
extern int g_IH_ResOpenVol;
extern int g_TF_ResOpenVol;
extern int g_T_ResOpenVol;
//
TradeWindow::TradeWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::TradeWindow)
{
    ui->setupUi(this);
}

TradeWindow::~TradeWindow()
{
    delete ui;
}

void TradeWindow::setTrdApiPtr(CThostFtdcTraderApi *pTrdApi)
{
    m_pTrdApi = pTrdApi;
}

void TradeWindow::setMdApiPtr(CThostFtdcMdApi *pMdApi)
{
    m_pMdApi = pMdApi;
}

void TradeWindow::outputMsg(QString msg)
{
    ui->OutputTextBrowser->append(msg);
}

void TradeWindow::clearMsg()
{
    ui->OutputTextBrowser->clear();
}

void TradeWindow::on_ArbgSellBtn_clicked()
{
    char msg[100];
    //
    if ((!g_InstHash.contains(g_BuyInstID)) || (!g_InstHash.contains(g_SellInstID)))
    {
        sprintf(msg,"Error! Can not find %s or %s contract in g_InstHash.",
                g_BuyInstID.toLatin1().data(), g_SellInstID.toLatin1().data());
        ui->OutputTextBrowser->append(msg);
        return;
    }
    // 判断风控
    if (!g_IsCanOpenPosi)
    {
        ui->OutputTextBrowser->append(QString("Warning : Risk control... do not open position!"));
        return;
    }
    //
    Contract buyCont = g_InstHash.value(g_BuyInstID);
    Contract sellCont = g_InstHash.value(g_SellInstID);
    //
    CThostFtdcInputOrderField buyOrderField;
    CThostFtdcInputOrderField sellOrderField;
    // 处理股指期货
    if (buyCont.isCFFE || sellCont.isCFFE)
    {
        if(!(buyCont.isCFFE) || !(sellCont.isCFFE))
        {
            sprintf(msg,"Error : Arbitrage instrument should be all CFFE contracts!");
            ui->OutputTextBrowser->append(msg);
            return;
        }
        // 找buy合约还可开仓数
        int buycont_res_vol = 0;
        if (buyCont.id.left(2) == QString("IF"))
        {
            buycont_res_vol = g_IF_ResOpenVol;
        }
        else if (buyCont.id.left(2) == QString("IC"))
        {
            buycont_res_vol = g_IC_ResOpenVol;
        }
        else if (buyCont.id.left(2) == QString("IH"))
        {
            buycont_res_vol = g_IH_ResOpenVol;
        }
        else if (buyCont.id.left(2) == QString("TF"))
        {
            buycont_res_vol = g_TF_ResOpenVol;
        }
        else if (buyCont.id.left(1) == QString("T"))
        {
            buycont_res_vol = g_T_ResOpenVol;
        }
        // 找sell合约还可开仓数
        int sellcont_res_vol = 0;
        if (sellCont.id.left(2) == QString("IF"))
        {
            sellcont_res_vol = g_IF_ResOpenVol;
        }
        else if (sellCont.id.left(2) == QString("IC"))
        {
            sellcont_res_vol = g_IC_ResOpenVol;
        }
        else if (sellCont.id.left(2) == QString("IH"))
        {
            sellcont_res_vol = g_IH_ResOpenVol;
        }
        else if (sellCont.id.left(2) == QString("TF"))
        {
            sellcont_res_vol = g_TF_ResOpenVol;
        }
        else if (sellCont.id.left(1) == QString("T"))
        {
            sellcont_res_vol = g_T_ResOpenVol;
        }
        // 找buy和sell可开仓数的最小值
        int min_res_vol = (buycont_res_vol>sellcont_res_vol)?sellcont_res_vol:buycont_res_vol;
        // 处理BuyInst合约
        if (buyCont.yd_long_posi > 0)            //处理昨仓
        {
            if (buyCont.td_long_posi <= 0)   //无多头今仓，则可平多头昨仓
            {
                int closevol = 0;
                if (buyCont.send_ord_lot <= buyCont.yd_long_posi)
                {
                    closevol = buyCont.send_ord_lot;
                    buyCont.send_ord_lot = 0;


                }
                else
                {
                    closevol =  buyCont.yd_long_posi;
                    buyCont.send_ord_lot -= buyCont.yd_long_posi;
                }
                // 卖平昨仓
                fillOrderField(&buyOrderField, g_Account.broker, g_Account.investor, g_BuyInstID.toLatin1().data(), ++g_OrdRef,
                               THOST_FTDC_D_Sell, THOST_FTDC_OF_CloseYesterday, buyCont.bid, closevol);
                m_pTrdApi->ReqOrderInsert(&buyOrderField, buyOrderField.RequestID);
            }
        }
        if (buyCont.send_ord_lot > 0)       //处理今仓
        {   
            int openvol = (buyCont.send_ord_lot+buyCont.td_long_posi <= min_res_vol)?(buyCont.send_ord_lot+buyCont.td_long_posi):min_res_vol;
            //开空
            fillOrderField(&buyOrderField, g_Account.broker, g_Account.investor, g_BuyInstID.toLatin1().data(), ++g_OrdRef,
                           THOST_FTDC_D_Sell, THOST_FTDC_OF_Open, buyCont.bid, openvol);
            m_pTrdApi->ReqOrderInsert(&buyOrderField, buyOrderField.RequestID);
        }
        // 处理SellInst合约
        if (sellCont.yd_short_posi > 0)          //处理昨仓
        {
            if (sellCont.td_short_posi <= 0)     //无空头今仓，则可平空头昨仓
            {
                int closevol = 0;
                if (sellCont.send_ord_lot <= sellCont.yd_short_posi)
                {
                    closevol = sellCont.send_ord_lot;
                    sellCont.send_ord_lot = 0;
                }
                else
                {
                    closevol = sellCont.yd_short_posi;
                    sellCont.send_ord_lot -= sellCont.yd_short_posi;
                }
                // 买平昨仓
                fillOrderField(&sellOrderField, g_Account.broker, g_Account.investor, g_SellInstID.toLatin1().data(), ++g_OrdRef,
                               THOST_FTDC_D_Buy, THOST_FTDC_OF_CloseYesterday, sellCont.ask, closevol);
                m_pTrdApi->ReqOrderInsert(&sellOrderField, sellOrderField.RequestID);
            }
        }
        if (sellCont.send_ord_lot > 0)           //处理今仓
        {   
            int openvol = (sellCont.send_ord_lot+sellCont.td_short_posi <= min_res_vol)?(sellCont.send_ord_lot+sellCont.td_short_posi):min_res_vol;
            //开多
            fillOrderField(&sellOrderField, g_Account.broker, g_Account.investor, g_SellInstID.toLatin1().data(), ++g_OrdRef,
                           THOST_FTDC_D_Buy, THOST_FTDC_OF_Open, sellCont.ask, openvol);
            m_pTrdApi->ReqOrderInsert(&sellOrderField, sellOrderField.RequestID);
        }
    }
    else   // 处理商品期货
    {
        //-----------------------------------------------------------------------------------------------------------
        // 平BuyInst多头
        //-----------------------------------------------------------------------------------------------------------
        // 平昨
        if ((buyCont.yd_long_posi > 0) && (buyCont.send_ord_lot > 0))
        {
            if (buyCont.send_ord_lot <= buyCont.yd_long_posi)
            {
                // 平昨buyCont.send_ord_lot
                fillOrderField(&buyOrderField, g_Account.broker, g_Account.investor, g_BuyInstID.toLatin1().data(), ++g_OrdRef,
                               THOST_FTDC_D_Sell, THOST_FTDC_OF_Close, buyCont.bid, buyCont.send_ord_lot);
                m_pTrdApi->ReqOrderInsert(&buyOrderField, buyOrderField.RequestID);
                sprintf(msg,"Close Sell %s %d Lots",g_BuyInstID.toLatin1().data(),buyCont.send_ord_lot);
                ui->OutputTextBrowser->append(msg);
                buyCont.send_ord_lot = 0;
            }
            else
            {
                // 平昨buyCont.yd_long_posi
                fillOrderField(&buyOrderField, g_Account.broker, g_Account.investor, g_BuyInstID.toLatin1().data(), ++g_OrdRef,
                               THOST_FTDC_D_Sell, THOST_FTDC_OF_Close, buyCont.bid, buyCont.yd_long_posi);
                m_pTrdApi->ReqOrderInsert(&buyOrderField, buyOrderField.RequestID);
                sprintf(msg,"Close Sell %s %d Lots",g_BuyInstID.toLatin1().data(),buyCont.yd_long_posi);
                ui->OutputTextBrowser->append(msg);
                buyCont.send_ord_lot -= buyCont.yd_long_posi;
            }
        }
        // 平今
        if ((buyCont.td_long_posi > 0) && (buyCont.send_ord_lot > 0))
        {
            if (buyCont.send_ord_lot <= buyCont.td_long_posi)
            {
                // 平今buyCont.send_ord_lot
                fillOrderField(&buyOrderField, g_Account.broker, g_Account.investor, g_BuyInstID.toLatin1().data(), ++g_OrdRef,
                               THOST_FTDC_D_Sell, THOST_FTDC_OF_CloseToday, buyCont.bid, buyCont.send_ord_lot);
                m_pTrdApi->ReqOrderInsert(&buyOrderField, buyOrderField.RequestID);
                sprintf(msg,"Close Sell %s %d Lots",g_BuyInstID.toLatin1().data(),buyCont.send_ord_lot);
                ui->OutputTextBrowser->append(msg);
                buyCont.send_ord_lot = 0;
            }
            else
            {
                // 平今buyCont.td_long_posi
                fillOrderField(&buyOrderField, g_Account.broker, g_Account.investor, g_BuyInstID.toLatin1().data(), ++g_OrdRef,
                               THOST_FTDC_D_Sell, THOST_FTDC_OF_CloseToday, buyCont.bid, buyCont.td_long_posi);
                m_pTrdApi->ReqOrderInsert(&buyOrderField, buyOrderField.RequestID);
                sprintf(msg,"Close Sell %s %d Lots",g_BuyInstID.toLatin1().data(),buyCont.td_long_posi);
                ui->OutputTextBrowser->append(msg);
                buyCont.send_ord_lot -= buyCont.td_long_posi;
            }
        }
        // 开BuyInst空头
        if (buyCont.send_ord_lot > 0)
        {
            fillOrderField(&buyOrderField, g_Account.broker, g_Account.investor, g_BuyInstID.toLatin1().data(), ++g_OrdRef,
                           THOST_FTDC_D_Sell, THOST_FTDC_OF_Open, buyCont.bid, buyCont.send_ord_lot);
            m_pTrdApi->ReqOrderInsert(&buyOrderField, buyOrderField.RequestID);
            sprintf(msg,"Open Sell %s %d Lots",g_BuyInstID.toLatin1().data(),buyCont.send_ord_lot);
            ui->OutputTextBrowser->append(msg);
        }
        //---------------------------------------------------------------------------------------------------------------------
        // 平SellInst空头
        //---------------------------------------------------------------------------------------------------------------------
        // 平昨
        if ((sellCont.yd_short_posi > 0) && (sellCont.send_ord_lot > 0))
        {
            if (sellCont.send_ord_lot <= sellCont.yd_short_posi)
            {
                // 平昨sellCont.send_ord_lot
                fillOrderField(&sellOrderField, g_Account.broker, g_Account.investor, g_SellInstID.toLatin1().data(), ++g_OrdRef,
                               THOST_FTDC_D_Buy, THOST_FTDC_OF_Close, sellCont.ask, sellCont.send_ord_lot);
                m_pTrdApi->ReqOrderInsert(&sellOrderField, sellOrderField.RequestID);
                sprintf(msg,"Close Buy %s %d Lots",g_SellInstID.toLatin1().data(),sellCont.send_ord_lot);
                ui->OutputTextBrowser->append(msg);
                sellCont.send_ord_lot = 0;
            }
            else
            {
                // 平昨sellCont.yd_short_posi
                fillOrderField(&sellOrderField, g_Account.broker, g_Account.investor, g_SellInstID.toLatin1().data(), ++g_OrdRef,
                               THOST_FTDC_D_Buy, THOST_FTDC_OF_Close, sellCont.ask, sellCont.yd_short_posi);
                m_pTrdApi->ReqOrderInsert(&sellOrderField, sellOrderField.RequestID);
                sprintf(msg,"Close Buy %s %d Lots",g_SellInstID.toLatin1().data(),sellCont.yd_short_posi);
                ui->OutputTextBrowser->append(msg);
                sellCont.send_ord_lot -= sellCont.yd_short_posi;
            }
        }
        // 平今
        if ((sellCont.td_short_posi > 0) && (sellCont.send_ord_lot >0))
        {
            if (sellCont.send_ord_lot <= sellCont.td_short_posi)
            {
                // 平今sellCont.send_ord_lot
                fillOrderField(&sellOrderField, g_Account.broker, g_Account.investor, g_SellInstID.toLatin1().data(), ++g_OrdRef,
                               THOST_FTDC_D_Buy, THOST_FTDC_OF_CloseToday, sellCont.ask, sellCont.send_ord_lot);
                m_pTrdApi->ReqOrderInsert(&sellOrderField, sellOrderField.RequestID);
                sprintf(msg,"Close Buy %s %d Lots",g_SellInstID.toLatin1().data(),sellCont.send_ord_lot);
                ui->OutputTextBrowser->append(msg);
                sellCont.send_ord_lot = 0;
            }
            else
            {
                // 平今sellCont.td_short_posi
                fillOrderField(&sellOrderField, g_Account.broker, g_Account.investor, g_SellInstID.toLatin1().data(), ++g_OrdRef,
                               THOST_FTDC_D_Buy, THOST_FTDC_OF_CloseToday, sellCont.ask, sellCont.td_short_posi);
                m_pTrdApi->ReqOrderInsert(&sellOrderField, sellOrderField.RequestID);
                sprintf(msg,"Close Buy %s %d Lots",g_SellInstID.toLatin1().data(),sellCont.td_short_posi);
                ui->OutputTextBrowser->append(msg);
                sellCont.send_ord_lot -= sellCont.td_short_posi;
            }
        }
        // 开SellInst多头
        if (sellCont.send_ord_lot > 0)
        {
            fillOrderField(&sellOrderField, g_Account.broker, g_Account.investor, g_SellInstID.toLatin1().data(), ++g_OrdRef,
                           THOST_FTDC_D_Buy, THOST_FTDC_OF_Open, sellCont.ask, sellCont.send_ord_lot);
            m_pTrdApi->ReqOrderInsert(&sellOrderField, sellOrderField.RequestID);
            sprintf(msg,"Open Buy %s %d Lots",g_SellInstID.toLatin1().data(),sellCont.send_ord_lot);
            ui->OutputTextBrowser->append(msg);
        }
    }
}

void TradeWindow::on_ArbgBuyBtn_clicked()
{
    char msg[100];
    //
    if ((!g_InstHash.contains(g_BuyInstID)) || (!g_InstHash.contains(g_SellInstID)))
    {
        sprintf(msg,"Error! Can not find %s or %s contract in g_InstHash.",
                g_BuyInstID.toLatin1().data(), g_SellInstID.toLatin1().data());
        ui->OutputTextBrowser->append(msg);
        return;
    }
    // 判断风控
    if (!g_IsCanOpenPosi)
    {
        ui->OutputTextBrowser->append(QString("Warning : Risk control... do not open position!"));
        return;
    }
    //
    Contract buyCont = g_InstHash.value(g_BuyInstID);
    Contract sellCont = g_InstHash.value(g_SellInstID);
    //
    CThostFtdcInputOrderField buyOrderField;
    CThostFtdcInputOrderField sellOrderField;
    if (buyCont.isCFFE || sellCont.isCFFE)
    {
        if(!(buyCont.isCFFE) || !(sellCont.isCFFE))
        {
            sprintf(msg,"Error : Arbitrage instrument should be all CFFE contracts!");
            ui->OutputTextBrowser->append(msg);
            return;
        }
        // 找buy合约还可开仓数
        int buycont_res_vol = 0;
        if (buyCont.id.left(2) == QString("IF"))
        {
            buycont_res_vol = g_IF_ResOpenVol;
        }
        else if (buyCont.id.left(2) == QString("IC"))
        {
            buycont_res_vol = g_IC_ResOpenVol;
        }
        else if (buyCont.id.left(2) == QString("IH"))
        {
            buycont_res_vol = g_IH_ResOpenVol;
        }
        else if (buyCont.id.left(2) == QString("TF"))
        {
            buycont_res_vol = g_TF_ResOpenVol;
        }
        else if (buyCont.id.left(1) == QString("T"))
        {
            buycont_res_vol = g_T_ResOpenVol;
        }
        // 找sell合约还可开仓数
        int sellcont_res_vol = 0;
        if (sellCont.id.left(2) == QString("IF"))
        {
            sellcont_res_vol = g_IF_ResOpenVol;
        }
        else if (sellCont.id.left(2) == QString("IC"))
        {
            sellcont_res_vol = g_IC_ResOpenVol;
        }
        else if (sellCont.id.left(2) == QString("IH"))
        {
            sellcont_res_vol = g_IH_ResOpenVol;
        }
        else if (sellCont.id.left(2) == QString("TF"))
        {
            sellcont_res_vol = g_TF_ResOpenVol;
        }
        else if (sellCont.id.left(1) == QString("T"))
        {
            sellcont_res_vol = g_T_ResOpenVol;
        }
        // 找buy和sell可开仓数的最小值
        int min_res_vol = (buycont_res_vol>sellcont_res_vol)?sellcont_res_vol:buycont_res_vol;
        // 处理BuyInst合约
        if (buyCont.yd_short_posi > 0)       //处理昨仓
        {
            if (buyCont.td_short_posi <= 0)  //无今空仓，则平昨空仓
            {
                int closevol = 0;
                if (buyCont.send_ord_lot <= buyCont.yd_short_posi)
                {
                    closevol = buyCont.send_ord_lot;
                    buyCont.send_ord_lot = 0;
                }
                else
                {
                    closevol = buyCont.yd_short_posi;
                    buyCont.send_ord_lot -= buyCont.yd_short_posi;
                }
                // 买平昨仓
                fillOrderField(&buyOrderField, g_Account.broker, g_Account.investor, g_BuyInstID.toLatin1().data(), ++g_OrdRef,
                               THOST_FTDC_D_Buy, THOST_FTDC_OF_Close, buyCont.ask, closevol);
                m_pTrdApi->ReqOrderInsert(&buyOrderField, buyOrderField.RequestID);
            }
        }
        if (buyCont.send_ord_lot > 0)              //处理今仓
        {
            int openvol = (buyCont.send_ord_lot+buyCont.td_short_posi <= min_res_vol)?buyCont.send_ord_lot+buyCont.td_short_posi:min_res_vol;
            fillOrderField(&buyOrderField, g_Account.broker, g_Account.investor, g_BuyInstID.toLatin1().data(), ++g_OrdRef,
                           THOST_FTDC_D_Buy, THOST_FTDC_OF_Open, buyCont.ask, openvol);
            m_pTrdApi->ReqOrderInsert(&buyOrderField, buyOrderField.RequestID);
        }
        // 处理SellInst合约
        if (sellCont.yd_long_posi > 0)            //处理昨仓
        {
            if (sellCont.td_long_posi <= 0)       //无多头今仓，则平多头昨仓
            {
                int closevol = 0;
                if (sellCont.send_ord_lot <= sellCont.yd_long_posi)
                {
                    closevol = sellCont.send_ord_lot;
                    sellCont.send_ord_lot = 0;
                }
                else
                {
                    closevol = sellCont.yd_long_posi;
                    sellCont.send_ord_lot -= sellCont.yd_long_posi;
                }
                fillOrderField(&sellOrderField, g_Account.broker, g_Account.investor, g_SellInstID.toLatin1().data(), ++g_OrdRef,
                               THOST_FTDC_D_Sell, THOST_FTDC_OF_Close, sellCont.bid, closevol);
                m_pTrdApi->ReqOrderInsert(&sellOrderField, sellOrderField.RequestID);
            }
        }
        if (sellCont.send_ord_lot > 0)                 // 处理今仓
        {
            int openvol = (sellCont.send_ord_lot+sellCont.td_long_posi <= min_res_vol)?sellCont.send_ord_lot+sellCont.td_long_posi:min_res_vol;
            //开空
            fillOrderField(&sellOrderField, g_Account.broker, g_Account.investor, g_SellInstID.toLatin1().data(), ++g_OrdRef,
                           THOST_FTDC_D_Sell, THOST_FTDC_OF_Open, sellCont.bid, openvol);
            m_pTrdApi->ReqOrderInsert(&sellOrderField, sellOrderField.RequestID);
        }
    }
    else   // 处理商品期货
    {
        //-----------------------------------------------------------------------------------------------------------
        // 平BuyInst空头
        //-----------------------------------------------------------------------------------------------------------
        // 平昨
        if ((buyCont.yd_short_posi > 0) && (buyCont.send_ord_lot > 0))
        {
            if (buyCont.send_ord_lot <= buyCont.yd_short_posi)
            {
                // 平昨buyCont.send_ord_lot
                fillOrderField(&buyOrderField, g_Account.broker, g_Account.investor, g_BuyInstID.toLatin1().data(), ++g_OrdRef,
                               THOST_FTDC_D_Buy, THOST_FTDC_OF_Close, buyCont.ask, buyCont.send_ord_lot);
                m_pTrdApi->ReqOrderInsert(&buyOrderField, buyOrderField.RequestID);
                sprintf(msg, "Close Buy %s %d Lots", g_BuyInstID.toLatin1().data(), buyCont.send_ord_lot);
                ui->OutputTextBrowser->append(msg);
                buyCont.send_ord_lot = 0;
            }
            else
            {
                // 平昨buyCont.yd_short_posi
                fillOrderField(&buyOrderField, g_Account.broker, g_Account.investor, g_BuyInstID.toLatin1().data(), ++g_OrdRef,
                               THOST_FTDC_D_Buy, THOST_FTDC_OF_Close, buyCont.ask, buyCont.yd_short_posi);
                m_pTrdApi->ReqOrderInsert(&buyOrderField, buyOrderField.RequestID);
                sprintf(msg, "Close Buy %s %d Lots", g_BuyInstID.toLatin1().data(), buyCont.yd_short_posi);
                ui->OutputTextBrowser->append(msg);
                buyCont.send_ord_lot -= buyCont.yd_short_posi;
            }
        }
        // 平今
        if ((buyCont.td_short_posi > 0) && (buyCont.send_ord_lot > 0))
        {
            if (buyCont.send_ord_lot <= buyCont.td_short_posi)
            {
                // 平今buyCont.send_ord_lot
                fillOrderField(&buyOrderField, g_Account.broker, g_Account.investor, g_BuyInstID.toLatin1().data(), ++g_OrdRef,
                               THOST_FTDC_D_Buy, THOST_FTDC_OF_CloseToday, buyCont.ask, buyCont.send_ord_lot);
                m_pTrdApi->ReqOrderInsert(&buyOrderField, buyOrderField.RequestID);
                sprintf(msg, "Close Buy %s %d Lots", g_BuyInstID.toLatin1().data(), buyCont.send_ord_lot);
                ui->OutputTextBrowser->append(msg);
                buyCont.send_ord_lot = 0;
            }
            else
            {
                // 平今buyCont.td_short_posi
                fillOrderField(&buyOrderField, g_Account.broker, g_Account.investor, g_BuyInstID.toLatin1().data(), ++g_OrdRef,
                               THOST_FTDC_D_Buy, THOST_FTDC_OF_CloseToday, buyCont.ask, buyCont.td_short_posi);
                m_pTrdApi->ReqOrderInsert(&buyOrderField, buyOrderField.RequestID);
                sprintf(msg, "Close Buy %s %d Lots", g_BuyInstID.toLatin1().data(), buyCont.td_short_posi);
                ui->OutputTextBrowser->append(msg);
                buyCont.send_ord_lot -= buyCont.td_short_posi;
            }
        }
        // 开BuyInst多头
        if (buyCont.send_ord_lot > 0)
        {
            fillOrderField(&buyOrderField, g_Account.broker, g_Account.investor, g_BuyInstID.toLatin1().data(), ++g_OrdRef,
                           THOST_FTDC_D_Buy, THOST_FTDC_OF_Open, buyCont.ask, buyCont.send_ord_lot);
            m_pTrdApi->ReqOrderInsert(&buyOrderField, buyOrderField.RequestID);
            sprintf(msg, "Open Buy %s %d Lots", g_BuyInstID.toLatin1().data(), buyCont.send_ord_lot);
            ui->OutputTextBrowser->append(msg);
        }
        //---------------------------------------------------------------------------------------------------------------------
        // 平SellInst多头
        //---------------------------------------------------------------------------------------------------------------------
        // 平昨
        if ((sellCont.yd_long_posi > 0) && (sellCont.send_ord_lot > 0))
        {
            if (sellCont.send_ord_lot <= sellCont.yd_long_posi)
            {
                // 平昨sellCont.send_ord_lot
                fillOrderField(&sellOrderField, g_Account.broker, g_Account.investor, g_SellInstID.toLatin1().data(), ++g_OrdRef,
                               THOST_FTDC_D_Sell, THOST_FTDC_OF_Close, sellCont.bid, sellCont.send_ord_lot);
                m_pTrdApi->ReqOrderInsert(&sellOrderField, sellOrderField.RequestID);
                sprintf(msg, "Close Sell %s %d Lots", g_SellInstID.toLatin1().data(), sellCont.send_ord_lot);
                ui->OutputTextBrowser->append(msg);
                sellCont.send_ord_lot = 0;
            }
            else
            {
                // 平昨sellCont.yd_long_posi
                fillOrderField(&sellOrderField, g_Account.broker, g_Account.investor, g_SellInstID.toLatin1().data(), ++g_OrdRef,
                               THOST_FTDC_D_Sell, THOST_FTDC_OF_Close, sellCont.bid, sellCont.yd_long_posi);
                m_pTrdApi->ReqOrderInsert(&sellOrderField, sellOrderField.RequestID);
                sprintf(msg, "Close Sell %s %d Lots", g_SellInstID.toLatin1().data(), sellCont.yd_long_posi);
                ui->OutputTextBrowser->append(msg);
                sellCont.send_ord_lot -= sellCont.yd_long_posi;
            }
        }
        // 平今
        if ((sellCont.td_long_posi > 0) && (sellCont.send_ord_lot >0))
        {
            if (sellCont.send_ord_lot <= sellCont.td_long_posi)
            {
                // 平今sellCont.send_ord_lot
                fillOrderField(&sellOrderField, g_Account.broker, g_Account.investor, g_SellInstID.toLatin1().data(), ++g_OrdRef,
                               THOST_FTDC_D_Sell, THOST_FTDC_OF_CloseToday, sellCont.bid, sellCont.send_ord_lot);
                m_pTrdApi->ReqOrderInsert(&sellOrderField, sellOrderField.RequestID);
                sprintf(msg, "Close Sell %s %d Lots", g_SellInstID.toLatin1().data(), sellCont.send_ord_lot);
                ui->OutputTextBrowser->append(msg);
                sellCont.send_ord_lot = 0;
            }
            else
            {
                // 平今sellCont.td_long_posi
                fillOrderField(&sellOrderField, g_Account.broker, g_Account.investor, g_SellInstID.toLatin1().data(), ++g_OrdRef,
                               THOST_FTDC_D_Sell, THOST_FTDC_OF_CloseToday, sellCont.bid, sellCont.td_long_posi);
                m_pTrdApi->ReqOrderInsert(&sellOrderField, sellOrderField.RequestID);
                sprintf(msg, "Close Sell %s %d Lots", g_SellInstID.toLatin1().data(), sellCont.td_long_posi);
                ui->OutputTextBrowser->append(msg);
                sellCont.send_ord_lot -= sellCont.td_long_posi;
            }
        }
        // 开SellInst空头
        if (sellCont.send_ord_lot > 0)
        {
            fillOrderField(&sellOrderField, g_Account.broker, g_Account.investor, g_SellInstID.toLatin1().data(), ++g_OrdRef,
                           THOST_FTDC_D_Sell, THOST_FTDC_OF_Open, sellCont.bid, sellCont.send_ord_lot);
            m_pTrdApi->ReqOrderInsert(&sellOrderField, sellOrderField.RequestID);
            sprintf(msg, "Open Sell %s %d Lots", g_SellInstID.toLatin1().data(), sellCont.send_ord_lot);
            ui->OutputTextBrowser->append(msg);
        }
    }
}

void TradeWindow::on_ClearPosiBtn_clicked()
{
    clearPosition(m_pTrdApi, m_pMdApi);
}

void TradeWindow::on_BuyInstLineEdit_editingFinished()
{
    g_BuyInstID = ui->BuyInstLineEdit->text();
    if (g_BuyInstID.isEmpty())
    {
        QString msg("Waring : Buy contract could not be blank!");
        ui->OutputTextBrowser->append(msg);
        return;
    }
    // 比较买入和卖出合约是否一样，一样则报错
    g_SellInstID = ui->SellInstLineEdit->text();
    if (g_BuyInstID == g_SellInstID)
    {
        QString msg("Waring : Buy and sell contract should be different!");
        ui->OutputTextBrowser->append(msg);
        return;
    }
    // 订阅行情数据
    char * instset[] = {g_BuyInstID.toLatin1().data()};
    m_pMdApi->SubscribeMarketData(instset, 1);
    // 获得买入合约手数
    QByteArray bl = ui->BuyInstLotLineEdit->text().toLatin1();
    int buylot = atoi(bl.data());
    //
    Contract cont;
    if (g_InstHash.contains(g_BuyInstID))
    {
        cont = g_InstHash.value(g_BuyInstID);
    }
    else
    {
        cont.id = g_BuyInstID;
    }
    cont.send_ord_lot = buylot;
    g_InstHash.insert(g_BuyInstID, cont);
}

void TradeWindow::on_SellInstLineEdit_editingFinished()
{
    g_SellInstID = ui->SellInstLineEdit->text();
    if (g_SellInstID.isEmpty())
    {
        QString msg("Waring : Sell contract could not be blank!");
        ui->OutputTextBrowser->append(msg);
        return;
    }
    // 比较买入和卖出合约是否一样，一样则报错
    g_BuyInstID = ui->BuyInstLineEdit->text();
    if (g_BuyInstID == g_SellInstID)
    {
        QString msg("Waring : Buy and sell contract should be different!");
        ui->OutputTextBrowser->append(msg);
        return;
    }
    // 订阅行情数据
    char * instset[] = {g_SellInstID.toLatin1().data()};
    m_pMdApi->SubscribeMarketData(instset, 1);
    // 获得买入合约手数
    QByteArray sl = ui->SellInstLotLineEdit->text().toLatin1();
    int selllot = atoi(sl.data());
    //
    Contract cont;
    if (g_InstHash.contains(g_SellInstID))
    {
        cont = g_InstHash.value(g_SellInstID);
    }
    else
    {
        cont.id = g_SellInstID;
    }
    cont.send_ord_lot = selllot;
    g_InstHash.insert(g_SellInstID, cont);
}

void TradeWindow::on_BuyInstLotLineEdit_editingFinished()
{
    QString buyinst = ui->BuyInstLineEdit->text();
    QByteArray bl = ui->BuyInstLotLineEdit->text().toLatin1();
    int buylot = atoi(bl.data());
    if (buylot <= 0)
    {
        ui->OutputTextBrowser->append(QString("Warning : Buy instrument lot should be positive!"));
        return;
    }
    Contract cont;
    if (g_InstHash.contains(buyinst))
    {
        cont = g_InstHash.value(buyinst);
    }
    else
    {
        cont.id = buyinst;
    }
    cont.send_ord_lot = buylot;
    g_InstHash.insert(buyinst, cont);
}

void TradeWindow::on_SellInstLotLineEdit_editingFinished()
{
    QString sellinst = ui->SellInstLineEdit->text();
    QByteArray sl = ui->SellInstLotLineEdit->text().toLatin1();
    int selllot = atoi(sl.data());
    if (selllot <= 0)
    {
        ui->OutputTextBrowser->append(QString("Warning : Sell instrument lot should be positive!"));
        return;
    }
    Contract cont;
    if (g_InstHash.contains(sellinst))
    {
        cont = g_InstHash.value(sellinst);
    }
    else
    {
        cont.id = sellinst;
    }
    cont.send_ord_lot = selllot;
    g_InstHash.insert(sellinst, cont);
}
