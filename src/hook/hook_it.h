#ifndef CK_HOOK_IT_H
#define CK_HOOK_IT_H

#include <memory>

namespace ck
{

    /* 基础Hook类 */
    struct HookIt;
    using HookItPtr = std::shared_ptr<HookIt>;
    struct HookIt
    {
        virtual ~HookIt() {}

        /**
         * @brief 钩取库函数,目标函数声明必须与原函数一致
         * @param dll_name 动态链接库名
         * @param old_func_name函数名
         * @param new_func_ptr 新函数指针
         */
        static HookItPtr $(const char* dll_name, const char* old_func_name, void* new_func_ptr);

        /**
         * @brief 钩取com接口函数,新函数添加一个参数1,即com对象指针
         * @param com_obj_ptr com对象
         * @param com_func_num 函数序号 函数表从1开始数
         * @param new_func_ptr 新函数指针
         */
        static HookItPtr $(void* com_obj_ptr, uint32_t com_func_num, void* new_func_ptr);

        /**
         * @brief 勾取普通函数或成员函数,目标函数声明必须与原函数一致
         * @param old_func_ptr 原函数指针
         * @param new_func_ptr 新函数指针
         */
        static HookItPtr $(void* old_func_ptr, void* new_func_ptr);

        virtual const bool hooked() const = 0;

        /* 还原被钩取函数 (若要调用原函数功能请先调用此函数) */
        virtual void unhook() = 0;

        /* 重新勾取函数 (若要调用自定义函数功能请先调用此函数) */
        virtual void hook() = 0;
    };

    inline HookItPtr make_hook(const char* dll_name, const char* old_func_name, void* new_func_ptr)
    {
        return HookIt::$(dll_name, old_func_name, new_func_ptr);
    }

    inline HookItPtr make_hook(void* com_obj_ptr, uint32_t com_func_num, void* new_func_ptr)
    {
        return HookIt::$(com_obj_ptr, com_func_num, new_func_ptr);
    }

    inline HookItPtr make_hook(void* old_func_ptr, void* new_func_ptr)
    {
        return HookIt::$(old_func_ptr, new_func_ptr);
    }

    template<typename T,typename F>
    inline T whatever_cast(F adr)
    {
        static_assert(sizeof(F) == sizeof(T), "The sizeof(F) must equal sizeof(T)!");
        return *reinterpret_cast<T*>(&adr);
    }

    // // 从地址转换为
    // template<typename T>
    // inline T from_adr(void* adr)
    // { return *reinterpret_cast<T*>(&adr); }

    // // 转换为地址
    // template<typename T>
    // inline void* to_adr(T t)
    // {
    //     static_assert(sizeof(t) >= sizeof(void*), "The size of type must geater than or equal a pointer size!");
    //     return *reinterpret_cast<void**>(&t);
    // }
}

#endif //CK_HOOK_IT_H
