#include<memory>
template<typename T>
struct _StackNode
{
    T data;
    _StackNode* prev;
};
template<typename T,typename Alloc = std::allocator<T>>
class StackAlloc
{
public:
    using Node = _StackNode<T>;
    using allocator = typename Alloc::template rebind<Node>::other;

    StackAlloc():_head(nullptr){}
    ~StackAlloc(){clear();}

    bool empty(){return (_head == nullptr);}
    void clear()
    {
        Node* cur = _head;
        while(cur != nullptr){
            Node* tmp = cur->prev;
            //先析构然后回收内存
            _allocator.destroy(cur);
            _allocator.deallocate(cur,1);
            cur = tmp;
        }
        _head = nullptr;
    }
    void push(T element)
    {
        //申请一个节点内存
        Node* newNode = _allocator.allocate(1);
        //调用节点构造函数
        _allocator.construct(newNode,Node());

        newNode->data = element;
        newNode->prev = _head;
        _head = newNode;
    }
    T pop()
    {
        T res = _head->data;

        Node* tmp = _head->prev;
        //先析构然后释放内存
        _allocator.destroy(_head);
        _allocator.deallocate(_head,1);
        _head = tmp;

        return res;
    }
    T top(){return (_head->data);}
private:
    allocator _allocator;
    Node* _head;
};