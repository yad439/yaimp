//
// Created by YAD on 17.05.2019.
//
#ifndef YAIMP_UTILITY_HPP
#define YAIMP_UTILITY_HPP

#include <SDK/apiCore.h>

#include <gsl/gsl_assert>

#include <guiddef.h>
#include <unknwnbase.h>

#include <string_view>
#include <memory>
#include <iostream>

#include "RefCounter.hpp"
#include "Exceptions.hpp"
#include "Assert.hpp"

template<class T>
[[nodiscard]] auto aimpQuery(IUnknown* source, REFIID riid){
	Assert(source);
	void* ref{nullptr};
	const auto result = source->QueryInterface(riid, &ref);
	if (GSL_UNLIKELY(FAILED(result)))[[unlikely]]throw AimpQueryError{riid, result};//todo c++20 unlikely
	Assert(ref);
	return RefCounter{static_cast<T *>(ref)};
}
#define queryObject(type,source) aimpQuery<type>(source,IID_##type)
template<class T>
[[nodiscard]] auto aimpCreate(IAIMPCore* core,REFIID iid){
	Assert(core);
	void* ref{nullptr};
	const auto result = core->CreateObject(iid, &ref);
	if (GSL_UNLIKELY(FAILED(result)))[[unlikely]]throw AimpCreationError{iid, result};
	Assert(ref);
	return RefCounter{static_cast<T *>(ref)};
}
#define createObject(type,core) aimpCreate<type>(core,IID_##type)

[[nodiscard]] RefCounter<IAIMPString> createString(IAIMPCore* core,const std::wstring_view& content);
[[nodiscard]] RefCounter<IAIMPString> createString(IAIMPCore* core,const std::string_view& content);

[[nodiscard]] std::wstring convertToWstring(std::string_view input);
[[nodiscard]] std::string convertToString(IAIMPString* input);

template <typename Target,typename Source>
[[nodiscard]] constexpr inline Target pointer_cast(Source* ptr){
	static_assert(std::is_pointer<Target>::value);
	static_assert(sizeof(Source)==sizeof(typename std::remove_pointer<Target>::type));
	static_assert(std::is_pointer<Source>::value==std::is_pointer<typename std::remove_pointer<Target>::type>::value);
	return static_cast<Target>(static_cast<void*>(ptr));
}

template <typename Target,typename Source>
[[nodiscard]] constexpr inline Target pointer_cast(const Source* ptr){
	static_assert(std::is_pointer<Target>::value,"Must be cast to a pointer.");
	static_assert(sizeof(Source)==sizeof(typename std::remove_pointer<Target>::type));
	static_assert(std::is_pointer<Source>::value==std::is_pointer<typename std::remove_pointer<Target>::type>::value);
	return static_cast<Target>(static_cast<const void*>(ptr));
}

constexpr inline void checkResult(HRESULT result){
	if (result!=S_OK)[[unlikely]] throw AimpError{result};
}

struct Check_t{};
constexpr Check_t check{};

constexpr inline void operator|(HRESULT result,Check_t){ checkResult(result); }

void showException(const RefCounter<IAIMPCore> &core, const std::exception &error);

void showMsgBox(const RefCounter<IAIMPCore> &core, DWORD flags,std::wstring_view title,std::wstring_view message);

/*#ifdef DEBUG
#define debug(m) std::cout<<(m)<<std::endl
#define debug2(m0,m1) std::cout<<(m0)<<' '<<(m1)<<std::endl
#define debug3(m0,m1,m2) std::cout<<(m0)<<' '<<(m1)<<' '<<(m2)<<std::endl
#define debugw(m) {char buf[512];wcstombs(buf, m, 512);std::cout<<buf;}
void debugError(HRESULT error);
#endif //DEBUG*/
#endif //YAIMP_UTILITY_HPP
