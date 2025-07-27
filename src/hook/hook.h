#ifndef CK_HOOK_H
#define CK_HOOK_H

#include "hook_it.h"

namespace ck
{

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
				if(_h && _need)
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
	template<auto _Adr, auto _Detour, bool _Immediately>
	class Hook : public HookBase
	{
		using _Fx = decltype(_Detour);
		static_assert(
			std::is_same_v<decltype(_Adr), intptr_t> || std::is_same_v<decltype(_Adr), _Fx>,
			"_Adr must is a intptr_t address or same type function with _Detour!"
			);
	public:
		Hook()
			: _fold(_Fx(_Adr))
		{
			_Fx fx = _Detour;
			_h = ck::make_hook((void*)_Adr, (void*)fx);
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
		_Fx _fold;
	};

#define DEF_HOOK(addr,fx,immediately,varname) \
inline static ck::Hook<addr,fx,immediately> varname = {};

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

	template<uintptr_t _Adr, auto _Detour, bool _Immediately>
	class HookMem : public HookBase
	{
		using _Fx = decltype(_Detour);
		using _Class = typename from_member<_Fx>::type;
	public:
		HookMem()
		{
			auto pdetour = _Detour;
			_h = ck::make_hook((void*)_Adr, *(void**)&pdetour);
			if(_Immediately)
				rehook();
		}

		template<typename... _Args>
		auto operator()(_Class* that, _Args... args)
		{
			guard g(_h.get());
			return (that->*_un.f)(std::forward<_Args>(args)...);
		}

		template<typename... _Args>
		auto operator()(const _Class* that, _Args... args)
		{
			guard g(_h.get());
			return (that->*_un.f)(std::forward<_Args>(args)...);
		}
	private:
		union
		{
			_Fx f;
			void* p = (void*)_Adr;
		} _un;
	};

#define DEF_HOOKMEM(addr,mem,immediately,varname) \
inline static ck::HookMem<addr,mem,immediately> varname = {};

template<typename Fx>
inline Fx memfunc(uintptr_t adr)
{
	union
	{
		Fx f;
		void* p = nullptr;
	} _un;
	_un.p = (void*)adr;
	return  _un.f;
}

}

#endif // !CK_HOOK_H
