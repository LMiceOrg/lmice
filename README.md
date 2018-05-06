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
如果信号顺序  ..., T0-SelfInstrument, T1-OtherInstrument, ... 则计算结构无法解释

## CMake command
### OSX clang
cmake -G "CodeBlocks - Ninja" -DCMAKE_BUILD_TYPE=Release -DCMAKE_MAKE_PROGRAM=/usr/local/bin/ninja -DCMAKE_C_COMPILER=/usr/bin/clang -DCMAKE_CXX_COMPILER=/usr/bin/clang++  ..

### OSX gcc-7

cmake -G "CodeBlocks - Ninja" -DCMAKE_BUILD_TYPE=Release -DCMAKE_MAKE_PROGRAM=/usr/local/bin/ninja -DCMAKE_C_COMPILER=/usr/local/bin/gcc-7 -DCMAKE_CXX_COMPILER=/usr/local/bin/g++-7  ..
