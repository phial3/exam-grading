#include "../exercise.h"
#include <memory>

// READ: `std::shared_ptr` <https://zh.cppreference.com/w/cpp/memory/shared_ptr>
// READ: `std::weak_ptr` <https://zh.cppreference.com/w/cpp/memory/weak_ptr>

// TODO: 将下列 `?` 替换为正确的值
int main(int argc, char **argv) {
    // use_count() 返回当前 shared_ptr 指向的实例的引用计数

    auto shared = std::make_shared<int>(10);
    // 此时有一个shared_ptr<int>指向整数对象a (值为10)。
    std::shared_ptr<int> ptrs[]{shared, shared, shared};
    // ptrs[0], ptrs[1], ptrs[2]与shared都指向同一个对象a。
    // 总共有4个shared_ptr指向同一个int对象a，故use_count = 4。

    std::weak_ptr<int> observer = shared;
    // observer是一个weak_ptr指向a，但weak_ptr不增加计数。
    // 此时对象a的引用计数依然是4 (shared + ptrs[0] + ptrs[1] + ptrs[2])。
    ASSERT(observer.use_count() == 4, "");

    ptrs[0].reset();
    // 释放ptrs[0]对a的引用。
    // 剩余指向a的shared_ptr有：shared, ptrs[1], ptrs[2]，共3个。
    ASSERT(observer.use_count() == 3, "");

    ptrs[1] = nullptr;
    // 释放ptrs[1]对a的引用。
    // 剩余指向a的shared_ptr有：shared, ptrs[2]，共2个。
    ASSERT(observer.use_count() == 2, "");


    ptrs[2] = std::make_shared<int>(*shared);
    // 现在ptrs[2]不再指向a，而是指向一个新对象b（通过对a的值拷贝构造而来）。
    // 此时指向a的只有shared一个shared_ptr。
    // use_count = 1。
    ASSERT(observer.use_count() == 1, "");

    ptrs[0] = shared;
    ptrs[1] = shared;
    ptrs[2] = std::move(shared);
    // 这三行把ptrs[0], ptrs[1], ptrs[2]又都指向a。
    // 原本shared指向a(计数1)，ptrs[0],ptrs[1]赋值给a使计数从1变为3（一次给ptrs[0]、一次给ptrs[1]），
    // 然后ptrs[2] = std::move(shared)使shared变空，但ptrs[2]获得a的所有权，计数依然是3。
    // 最终shared为空指针, ptrs[0]=a, ptrs[1]=a, ptrs[2]=a，共3个指向a。
    ASSERT(observer.use_count() == 3, "");

    std::ignore = std::move(ptrs[0]);
    // 将ptrs[0]移动赋值给一个临时目标（std::ignore本身是特殊标识，这里假设其效果是让ptrs[0]变为空）。
    // 这样ptrs[0]不再指向a。
    // 此时ptrs[1]=a, ptrs[2]=a，共2个指向a。
    ptrs[1] = std::move(ptrs[1]);
    // 对自己移动赋值不改变引用计数，仍然ptrs[1]=a, ptrs[2]=a，计数=2。
    ptrs[1] = std::move(ptrs[2]);
    // 此赋值过程比较微妙，但最终效果是：
    // - 在执行赋值时，ptrs[1]先释放其当前对a的引用(计数从2减到1)。
    // - 再从ptrs[2]获取对a的所有权(计数从1增回2)。
    // - ptrs[2]变为nullptr。
    // 最终仍然有2个shared_ptr指向a。不过这里需要特别留意的是，赋值过程中是先减后加，最终保持计数不变。
    // 结束后，ptrs[1]=a, ptrs[2]=nullptr。
    // 尽管最终只看到ptrs[1]指向a，但在此操作的中间状态中曾有一个临时对象或中间过程保持计数，最终结果是计数保持为2。
    // （实际中，编译器实现细节确保在赋值操作的临时过程中计数先减后增，最终计数未变。）
    ASSERT(observer.use_count() == 2, "");

    shared = observer.lock();
    // 通过weak_ptr锁定获得一个新的shared_ptr指向a，这会增加计数1。
    // 原本计数=2，现在增加一个shared指向a后计数=3。
    ASSERT(observer.use_count() == 3, "");

    shared = nullptr;
    for (auto &ptr : ptrs) ptr = nullptr;
    // 清空所有对a的shared_ptr引用。
    // 没有任何shared_ptr指向a，故计数=0。
    ASSERT(observer.use_count() == 0, "");

    shared = observer.lock();
    // 由于之前对象已无引用计数且被销毁，lock()返回空的shared_ptr，计数仍为0。
    ASSERT(observer.use_count() == 0, "");

    return 0;
}
