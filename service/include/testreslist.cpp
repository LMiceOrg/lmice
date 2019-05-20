#include "res_list.h"

#include <stdio.h>
int main() {
    typedef reslist<int> tp;
    typedef tp::iterator it_tp;
    reslist<int> *v;
    v = v->create(16);
    v->next = v->create(16);
    printf("size = %d\n", v->size() );
    for(int i=0; i<16; ++i)
        (*v)[i]=i;

    for(int i=0; i<5; ++i)
        (*v)[i+16] = i+16;

    it_tp ite = v->begin();
    for(;ite != v->end(); ++ite){
        printf("v = %d\n", *ite);
    }

    return 0;
}