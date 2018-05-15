# lmice
Message Interactive Computational Environment

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

1. 初始化执行环境

    1. 清除上一次运行的状态
    1. 初始化执行环境
		1. 创建共享存储区域
        1. 创建强实时模型：主进程
            1. 初始化投资组合模型        -- portfolio
            1. 初始化开仓模型           -- open_feature
        1. 创建弱实时模型进程        --
			1. 平仓信号模型
			1. 市场模型
			1. 风险模型
			1. 投资组合模型
			1. 开始循环
		1. 创建日志进程
			1. 创建日志队列
			1. 开始循环
    1. 连接交易柜台
        1. 初始化交易接口
        1. 绑定交易线程-CPU亲缘性
    1. 初始化行情接收	

2.  准备交易

    1. 开始循环
        1. 接收行情
        1. 判断 是否满足交易需求
			1. 开仓模型就绪
			1. 平仓、风控、市场、投资组合模型就绪
    1. 结束循环

3. 开始交易

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

4. 结束交易 (强制平仓)

    1. 退出交易循环
    1. 停止接收行情
    1. 平仓
    1. 断开交易柜台
    1. 清除执行环境
