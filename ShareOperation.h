#ifndef SHAREOPERATION_H
#define SHAREOPERATION_H

#include "ThostFtdcTraderApi.h"
#include "ThostFtdcMdApi.h"

void fillOrderField(CThostFtdcInputOrderField *pOrd,
                    TThostFtdcBrokerIDType broker,
                    TThostFtdcInvestorIDType user,
                    char *instID,
                    int ordref,
                    TThostFtdcDirectionType direction,
                    TThostFtdcOffsetFlagType opencls,
                    double price,
                    int volume);

void clearPosition(CThostFtdcTraderApi *pTrdApi, CThostFtdcMdApi *pMdApi);

#endif // SHAREOPERATION_H
