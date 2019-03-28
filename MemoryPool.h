#ifndef MEMORY_POOL_H
#define MEMORY_POOL_H

#include<climits>
#include<cstddef>

template <typename T,size_t BlockSize = 4096>
class MemoryPool
{
public:
    using pointer = T*;
    using uintptr_t = unsigned long int;
    template<typename U>
    struct rebind{
        using other = MemoryPool<U>;
    };

    MemoryPool()noexcept:_currentBlock(nullptr),_currentSlot(nullptr),_lastSlot(nullptr),_freeSlots(nullptr)
    { }
    ~MemoryPool()noexcept
    {
        _slot_pointer cur = _currentBlock;
        while(cur!=nullptr){
            _slot_pointer prev = cur->next;
            operator delete(reinterpret_cast<void*>(cur));
            cur = prev;
        }
        
    }

    //同一时间只能分配一个对象
    //TODO:n和hint会被忽略？
    pointer allocate(size_t n = 1,const pointer hint = 0)
    {
        if(_freeSlots != nullptr)
        {
            pointer res = reinterpret_cast<pointer>(_freeSlots);
            _freeSlots = _freeSlots->next;
            return res;
        }else
        {
            if(_currentSlot >= _lastSlot)
            {
                _data_pointer newBlock = reinterpret_cast<_data_pointer>(operator new(BlockSize));
                reinterpret_cast<_slot_pointer>(newBlock)->next = _currentBlock;
                _currentBlock = reinterpret_cast<_slot_pointer>(newBlock);
                //填补整个区块来满足元素内存区域对齐要求
                _data_pointer body = newBlock + sizeof(_slot_pointer);
                uintptr_t res = reinterpret_cast<uintptr_t>(body);
                size_t bodyPadding = (alignof(_slot_type) - res) % alignof(_slot_type);
                _currentSlot = reinterpret_cast<_slot_pointer>(body+bodyPadding);
                _lastSlot = reinterpret_cast<_slot_pointer>(newBlock+BlockSize-sizeof(_slot_type));
            }
            return reinterpret_cast<pointer>(_currentSlot++);
        }
        
    }
    //销毁指针指向的内存块
    void deallocate(pointer p,size_t n = 1)
    {
        if(p!=nullptr)
        {
            reinterpret_cast<_slot_pointer>(p)->next = _freeSlots;
            _freeSlots = reinterpret_cast<_slot_pointer>(p);
        }
    }
    
    //调用构造函数,使用std::forward转发变参模板
    template<typename U,typename... Args>
    void construct(U* p,Args&&...args)
    {
        //TODO:new (p)U什么意思
        new (p)U(std::forward<Args>(args)...);
    }

    //销毁对象，即调用对象析构函数
    template<typename U>
    void destroy(U* p)
    {
        p->~U();
    }

private:
    /**
     * 用于存储内存池中的对象槽
     * 要么被实例化为一个存放对象的槽
     * 要么被实例化为一个指向存放对象槽的槽指针
     */ 
    union _Slot
    {
        /* data */
        T element;
        _Slot* next;
    };

    //数据指针
        using _data_pointer = char*;
    //对象槽
    using _slot_type = _Slot;
    //对象槽指针
    using _slot_pointer = _Slot*;

    //指向当前内存区块
    _slot_pointer _currentBlock;
    //指向当前内存区块的一个对象槽
    _slot_pointer _currentSlot;
    //指向当前内存区块的最后一个对象槽
    _slot_pointer _lastSlot;
    //指向当前内存区块中的空闲对象槽
    _slot_pointer _freeSlots;

    //检测定义的内存池大小是否过小
    //静态断言，在编译器指出错误
    static_assert(BlockSize>=2 * sizeof(_slot_type),"BlockSize too small.");
};

#endif