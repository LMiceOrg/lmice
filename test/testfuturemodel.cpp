/** Copyright 2018 He Hao<hehaoslj@sina.com> */

#include <stdio.h>
#include "include/discrete_self_trade_imbalance_v2.h"
int main()
{
    lmice::fm_discrete_self_trade_imbalance_v2<double> v1;
    lmice::fm_discrete_self_trade_imbalance_v2<float> v2;
    printf("fsize %lu dsize %lu\n", sizeof(v2), sizeof(v1));
    return 0;
}