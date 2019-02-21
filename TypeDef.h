#ifndef TYPEDEF_H
#define TYPEDEF_H

#include<QString>
#include "ThostFtdcUserApiDataType.h"

#define LOGIN_REQ_TRD                1000
#define LOGIN_REQ_POSI               1100
#define LOGIN_REQ_QRY                1110
#define LOGIN_REQ_MD                 1111
#define LOGOUT_REQ_TRD               2000
#define LOGOUT_REQ_POSI              2200
#define LOGOUT_REQ_QRY               2220
#define LOGOUT_REQ_MD                2222
#define QRY_SETTLEMENT               3000
#define IF_MAX_OPEN_VOL              18
#define IC_MAX_OPEN_VOL              18
#define IH_MAX_OPEN_VOL              18
#define TF_MAX_OPEN_VOL              48
#define T_MAX_OPEN_VOL               48

typedef struct ContractStruct
{
    QString id = "";
    QString exchangID = "";
    int send_ord_lot = 0;
    int td_long_posi = 0;
    int yd_long_posi = 0;
    int td_short_posi = 0;
    int yd_short_posi = 0;
    int long_open_vol = 0;
    int short_open_vol = 0;
    double ask = 0;
    double bid = 0;
    bool isCFFE = false;
}Contract;

typedef struct AccountStruct
{
    TThostFtdcBrokerIDType   broker;
    TThostFtdcInvestorIDType investor;
    TThostFtdcPasswordType   password;
    double balance = 0;
}Account;

#endif // TYPEDEF_H
