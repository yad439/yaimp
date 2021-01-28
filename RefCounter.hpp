//
// Created by YAD on 14.05.2019.
//

#ifndef YAIMP_REFCOUNTER_HPP
#define YAIMP_REFCOUNTER_HPP

#include <utility>
#include <type_traits>

#include <unknwnbase.h>

#include "Assert.hpp"

template <class T>
class RefCounter {
	static_assert(std::is_base_of<IUnknown,T>::value);
public:
	inline RefCounter() :_ptr{nullptr}{}

	explicit inline RefCounter(T *ptr) : _ptr{ptr} {}
	inline RefCounter(RefCounter<T> &&other) noexcept : _ptr{other._ptr} {other._ptr= nullptr;}
	inline RefCounter(const RefCounter<T> &other):_ptr(other._ptr){if(VB_LIKELY(_ptr))[[likely]]_ptr->AddRef();}
	RefCounter&operator=(const RefCounter<T>& other){
		Assert(&other!=this);
//		if(&other==this)[[unlikely]]std::terminate();
		if(_ptr)_ptr->Release();
		_ptr=other._ptr;
		if(_ptr)_ptr->AddRef();
		return *this;
	}
	RefCounter&operator=(RefCounter<T>&& other) noexcept{
		//Assert(&other!=this);
#ifndef NDEBUG
		if(&other==this)[[unlikely]]std::terminate();
#endif
		if(_ptr)_ptr->Release();
		_ptr=other._ptr;
		other._ptr= nullptr;
		return *this;
	}
	inline void reset(){
		if(VB_LIKELY(_ptr))[[likely]]_ptr->Release();
		_ptr= nullptr;
	}

	virtual inline ~RefCounter() {
		if(VB_LIKELY(_ptr))[[likely]]_ptr->Release();
	}

	inline T* operator->() const{
		Assert(_ptr);
		return _ptr;
	}

	inline operator T*()const{
		Assert(_ptr);
		return _ptr;
	}

	[[nodiscard]] inline T* get() const{
		Assert(_ptr);
		return _ptr;
	}
private:
	T* _ptr;
};

template<class T, typename... Args>
inline auto makeRefCounter(Args&&... args){
	return RefCounter{new T(std::forward<Args>(args)...)};
}

#endif //YAIMP_REFCOUNTER_HPP
