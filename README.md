# lmice
Lmice Message Interactive Computational Environment

A High-Dynamic Agile-like financial analysis system and IDE

# libforecaster

## 类型更新

1. instrument_id string --> char[8]
2. product_id    string --> char[8]
3. price         double --> float
4. scheme        single value --> variable table

## ChinaL1Msg

结构体8字节对齐；

## BUG OtherXX Feature
handleOtherMsg 更新了m_book, m_cur_other_xx 参数
handleSelfMsg 基于上述参数计算m_signal

如果信号顺序  ..., T0-OtherInstrument, T0-SelfInstrument, ... 则一切OK
如果信号顺序  ..., T0-SelfInstrument, T1-OtherInstrument, ... 则计算结果无法解释

## CMake command
### OSX clang
cmake -G "CodeBlocks - Ninja" -DCMAKE_BUILD_TYPE=Release -DCMAKE_MAKE_PROGRAM=/usr/local/bin/ninja -DCMAKE_C_COMPILER=/usr/bin/clang -DCMAKE_CXX_COMPILER=/usr/bin/clang++  ..

### OSX gcc-7

cmake -G "CodeBlocks - Ninja" -DCMAKE_BUILD_TYPE=Release -DCMAKE_MAKE_PROGRAM=/usr/local/bin/ninja -DCMAKE_C_COMPILER=/usr/local/bin/gcc-7 -DCMAKE_CXX_COMPILER=/usr/local/bin/g++-7  ..

## 交易系统执行流程

1. 启动交易平台[m]                  --lmiced

    1. 启动平台服务[m]                    --server
        1. 处理命令行、配置文件
        1. 启动服务                       --server_launch
        1. 清除上一次运行的状态[m]          --
        1. 初始化CS运行环境[m]             --udss_init
    1. 创建工作区域[m]             -- worker_board
        1. 创建强实时进程[w1]
            1. 初始化交易接口           -- trader_api
            1. 初始化投资组合模型        -- portfolio
            1. 初始化开仓模型           -- open_feature
            1. 初始化行情接收           -- quote
        1. 创建弱实时进程[w2]
            1. 平仓信号模型             -- close_feature
            1. 市场模型                 -- market_trend
            1. 风险模型                 -- risk
            1. 投资组合模型             --portfolio
            1. 交易反馈处理             -- trader_spi
			1. 开始循环
        1. 非实时进程:主进程[m]
            1. 创建日志服务             -- service_logging
            1. 创建终端服务             -- service_console
            1. 开始命令循环


2.  准备交易[w1]

    1. 开始循环                     --requsite_
        1. 接收行情
        1. 判断 是否满足交易需求
			1. 开仓模型就绪
			1. 平仓、风控、市场、投资组合模型就绪
    1. 结束循环

3. 开始交易[w1]

    1. 开始循环

        1. 接收行情
        1. 判断 行情 在订阅列表
        	1. 判断 行情 在交易合约列表
				1. 执行该行情对应的SELF子信号列表
	        	1. 计算开仓信号
	        	1. 判断 开仓
    	    	1. 发出报价
			1. 执行该行情对应的OTHER子信号列表
    1. 返回循环

4. 更新模型[w2]

5. 强制平仓[w1]

    1. 退出交易循环
    1. 停止接收行情
    1. 平仓
    1. 断开交易柜台
    1. 清除执行环境

6. 结束交易[m]

# SHIF

dates = "20180523"
case "kx":
datatxt = "http://www.shfe.com.cn/data/dailydata/kx/kx" + dates + ".dat";
dispkxdata(datatxt);
lastButtionId ="kx";
header If-Modified-Since 0


##IP length
/*
 * Worse Case: IPv4-Mapped IPv6 Address
 * 0000:0000:0000:0000:0000:FFFF:255.255.255.255
 */
#define MAXIPSIZE  46


# TODO

## 内核模式 LMiced服务

## 存储管理 VFS文件系统

使用共享内存实现类似文件系统的资源管理机制。
