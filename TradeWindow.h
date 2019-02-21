#ifndef TRADEWINDOW_H
#define TRADEWINDOW_H

#include <QMainWindow>
#include <string.h>
#include "ThostFtdcTraderApi.h"
#include "ThostFtdcMdApi.h"

using namespace std;

namespace Ui {
class TradeWindow;
}

class TradeWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit TradeWindow(QWidget *parent = 0);
    ~TradeWindow();
    void setTrdApiPtr(CThostFtdcTraderApi *pTrdApi);
    void setMdApiPtr(CThostFtdcMdApi *pMdApi);
    void outputMsg(QString msg);
    void clearMsg();

private slots:
    void on_ArbgSellBtn_clicked();
    void on_ArbgBuyBtn_clicked();
    void on_ClearPosiBtn_clicked();
    void on_BuyInstLineEdit_editingFinished();
    void on_SellInstLineEdit_editingFinished();
    void on_BuyInstLotLineEdit_editingFinished();
    void on_SellInstLotLineEdit_editingFinished();

private:
    Ui::TradeWindow *ui;
    CThostFtdcTraderApi *m_pTrdApi;
    CThostFtdcMdApi *m_pMdApi;
};

#endif // TRADEWINDOW_H
