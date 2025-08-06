#ifndef CK_HOOK_H
#define CK_HOOK_H

#include "hook_it.h"

namespace ck
{
	// from_member 从成员获取类
	template <typename T>
	struct from_member
	{
		static_assert("You need to specify a member function!");
	};

	template <typename M, typename T>
	struct from_member< M T::* >
	{
		using type = T;
	};

	// normal_func 获得任意函数的普通函数形式
	template <typename T>
	struct normal_func
	{
		using type = void;
	};

	template <typename R, typename ...Args>
	struct normal_func< R(Args...) >
	{
		using type = R(*)(Args...);
	};

	template <typename R, typename C, typename ...Args>
	struct normal_func< R(C::*)(Args...) >
	{
		using type = R(*)(Args...);
	};

	class HookBase
	{
	protected:
		struct guard
		{
			inline guard(ck::HookIt* h) : _h(h)
			{
				if (!_h) return;
				_need = _h->hooked();
				_h->unhook();
			}
			inline ~guard()
			{
				if (_h && _need)
					_h->hook();
			}
		private:
			ck::HookIt* _h;
			bool _need = false;	// 记录是否需要rehook
		};
	public:
		inline void rehook()
		{
			if (!_h) return;
			_h->hook();
		}

		inline void unhook()
		{
			if (!_h) return;
			_h->unhook();
		}
	protected:
		ck::HookItPtr _h;
	};

	// @_Immediately  是否立即hook函数
	template<auto _Target, auto _Detour, bool _Immediately>
	class Hook : public HookBase
	{
		using _Tgt = decltype(_Target);
		using _Fn = decltype(_Detour);
		static_assert(
			std::is_same_v<_Tgt, intptr_t> || std::is_same_v<_Tgt, _Fn>,
			"_Target must is a intptr_t function address or same type function with _Detour!"
			);
	public:
		Hook()
			: _fold(_Fn(_Target))
		{
			_Fn fx = _Detour;
			_h = ck::make_hook((void*)_Target, (void*)fx);
			if (_Immediately)
				rehook();
		}

		template<typename... _Args>
		auto operator()(_Args... args)
		{
			guard g(_h.get());
			return _fold(std::forward<_Args>(args)...);
		}
	private:
		_Fn _fold;
	};

#define DEF_HOOK(addr,fx,immediately,varname) \
inline static ck::Hook<addr,fx,immediately> varname = {};

	template<auto _Target, auto _Detour, bool _Immediately>
	class HookMem : public HookBase
	{
		using _Tgt = decltype(_Target);
		using _Fn = decltype(_Detour);
		using _Class = typename from_member<_Fn>::type;
		template<typename T>
		using normal_func_t = typename normal_func<T>::type;
		static_assert(
			std::is_same_v<_Tgt, intptr_t> || std::is_same_v<normal_func_t<_Tgt>, normal_func_t<_Fn>>,
			"_Target must is a intptr_t function address or same declaration function with _Detour!"
			);
	public:
		HookMem() : f(whatever_cast<_Fn>(_Target))
		{
			auto pdetour = _Detour;
			_h = ck::make_hook(whatever_cast<void*>(_Target), *(void**)&pdetour);
			if (_Immediately)
				rehook();
		}

		template<typename... _Args>
		auto operator()(_Class* that, _Args... args)
		{
			guard g(_h.get());
			return (that->*f)(std::forward<_Args>(args)...);
		}

		template<typename... _Args>
		auto operator()(const _Class* that, _Args... args)
		{
			guard g(_h.get());
			return (that->*f)(std::forward<_Args>(args)...);
		}
	private:
		_Fn f;
	};

#define DEF_HOOKMEM(addr,mem,immediately,varname) \
inline static ck::HookMem<addr,mem,immediately> varname = {};

}

#endif // !CK_HOOK_H
