#ifndef _UDP_RECV_DCE_H_
#define _UDP_RECV_DCE_H_
  
  
/**** some declaration or so *****/  
  
   
#ifdef  __cplusplus  
extern "C"  { 
#endif  /* end of __cplusplus */ 

typedef struct DCEQuotDataTag
{
	char TradeDate[8];     // 交易日期
	char ContractID[80];   // 合约代码（将所有行情类别定义为一个结构体）	
	char UpdateTime[12];   // 更新时间
	int InitOpenInterest;  // 初始持仓量	
	double LastPrice;      // 最新价
	double HighPrice;      // 最高价
	double LowPrice;       // 最低价
	int LastMatchQty;      // 最新成交量
	int MatchTotQty;       // 成交量
	double Turnover;       // 成交金额
	double ClearPrice;     // 今结算价
	double LifeLow;        // 历史低价
	double LifeHigh;       // 历史高价
	double RiseLimit;      // 涨停价
	double FallLimit;      // 跌停价
	double LastClearPrice; // 昨结算价
	double LastClose;      // 昨收盘价
	double BidPrice;       // 申买价
	int BidQty;            // 申买量
	int BidImplyQty;       // 推导申买量
	double AskPrice;       // 申卖价
	int AskQty;            // 申卖量
	int AskImplyQty;       // 推导申卖量
	double AvgPrice;       // 当日平均价
	double OpenPrice;      // 开盘价
	double ClosePrice;     // 收盘价
	// 期权相关行情
	double Delta;          //
	double Gamma;          //
	double Rho;            //
	double Theta;          //
	double Vega;           //	
	int OpenInterest;      // 持仓量
	int InterestChg;       // 持仓改变量
} DCEQuotDataT;

typedef void (*MCSubscribeCallback)(DCEQuotDataT* pData);

int MCSubscribe(const char *if_addr, MCSubscribeCallback user_callback);
#ifdef __cplusplus
}
#endif
#endif
