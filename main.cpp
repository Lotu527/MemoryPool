#include<iostream>
#include<cassert>
#include<ctime>
#include<vector>

#include"StackAlloc.h"
#include"MemoryPool.h"

#define ELEMS 1e6
#define REPS 100

int main()
{
    clock_t start;

    //use stl default allocator
    StackAlloc<int,std::allocator<int>> stackDefault;
    start = clock();
    for(int i = 0; i< REPS;i++)
    {
        //非空时会引起中断
        assert(stackDefault.empty());

        for(int j = 0;j<ELEMS;j++)
            stackDefault.push(j);
        for(int j = 0;j<ELEMS;j++)
            stackDefault.pop();
    }
    std::cout<<"Default Allocator Time:";
    std::cout<<(((double)clock()-start)/CLOCKS_PER_SEC)<<std::endl;

    //use stl vector
    std::vector<int> stackVector;
    start = clock();
    for(int i = 0; i< REPS;i++)
    {
        //非空时会引起中断
        assert(stackVector.empty());

        for(int j = 0;j<ELEMS;j++)
            stackVector.push_back(j);
        for(int j = 0;j<ELEMS;j++)
            stackVector.pop_back();
    }
    std::cout<<"Vector Allocator Time:";
    std::cout<<(((double)clock()-start)/CLOCKS_PER_SEC)<<std::endl;

    //use MemoryPool
    StackAlloc<int,MemoryPool<int>> stackPool;
    start = clock();
    for(int i = 0; i< REPS;i++)
    {
        //非空时会引起中断
        assert(stackPool.empty());

        for(int j = 0;j<ELEMS;j++)
            stackPool.push(j);
        for(int j = 0;j<ELEMS;j++)
            assert(stackPool.pop()==ELEMS-j-1);
    }
    std::cout<<"MemoryPool Allocator Time:";
    std::cout<<(((double)clock()-start)/CLOCKS_PER_SEC)<<std::endl;


    return 0;
}