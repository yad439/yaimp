//
// Created by YAD on 17.05.2019.
//

#include "Utility.hpp"

#include <SDK/apiGUI.h>
#include <SDK/apiThreading.h>

#include <gsl/gsl_util>

#include <utf8.h>

#include <functional>
#include <string>

#include<windows.h>

#include "IUnknownImpl.hpp"

using namespace std;
using namespace gsl;

class Task:public IUnknownImpl<IAIMPTask>{
public:
	explicit Task(function<void(void)> action): _action{move(action)} {};

	void WINAPI Execute(IAIMPTaskOwner*) override{if(_action)_action();}

private:
	const IID &thisIID() override {return IID_IAIMPTask;}
private:
	function<void(void)> _action;
};

RefCounter<IAIMPString> createString(IAIMPCore *core, const wstring_view &content) {
	auto string=createObject(IAIMPString,core);
	string->SetData(const_cast<WCHAR *>(content.data()), narrow_cast<int>(content.length())) | check;
	return string;
}

RefCounter<IAIMPString> createString(IAIMPCore *core, const string_view &content) {
	return createString(core, convertToWstring(content));
}
#ifndef YAIMP_UTF_CONVERSION_NATIVE
wstring convertToWstring(string_view input) {
	wstring out;
#ifndef YAIMP_UTF_CONVERSION_SEMI_NATIVE
	out.reserve(input.length());
#else
	const auto wcharsCount=MultiByteToWideChar(CP_UTF8, 0, input.data(), narrow_cast<int>(input.length()), nullptr, 0);
	Assert(wcharsCount>0);
	out.reserve(wcharsCount);
#endif
	utf8::utf8to16(input.begin(),input.end(),back_inserter(out));
	return out;
}
#else
wstring convertToWstring(string_view input) {
	const auto wcharsCount=MultiByteToWideChar(CP_UTF8, 0, input.data(), narrow_cast<int>(input.length()), nullptr, 0);
	Assert(wcharsCount>0);
	const auto buffer = make_unique<wchar_t[]>(narrow_cast<size_t>(wcharsCount));
	const auto wcharsCount2=MultiByteToWideChar(CP_UTF8, 0, input.data(), narrow_cast<int>(input.length()), buffer.get(), wcharsCount);
	Assert(wcharsCount==wcharsCount2);
	return {buffer.get(),narrow_cast<wstring::size_type>(wcharsCount2)};
}
#endif
#ifndef YAIMP_UTF_CONVERSION_NATIVE
string convertToString(IAIMPString *input) {
	string out;
	const auto len = input->GetLength();
	const auto str=input->GetData();
	Assert(len>0);
#ifndef YAIMP_UTF_CONVERSION_SEMI_NATIVE
	out.reserve(narrow_cast<unsigned int>(len));
#else
	const auto charsCount=WideCharToMultiByte(CP_UTF8, 0, str, narrow_cast<int>(len), nullptr, 0,nullptr, nullptr);
	Assert(charsCount>0);
	out.reserve(charsCount);
#endif
	utf8::utf16to8(str,str+len,back_inserter(out));
	return out;
}
#else
string convertToString(IAIMPString *input) {
	const auto str=input->GetData();
	const auto len=narrow_cast<int>(input->GetLength());
	const auto charsCount=WideCharToMultiByte(CP_UTF8, 0,str ,len , nullptr, 0,nullptr, nullptr);
	Assert(charsCount>0);
	const auto buffer = make_unique<char[]>(narrow_cast<size_t>(charsCount));
	const auto charsCount2=WideCharToMultiByte(CP_UTF8, 0, str, len, buffer.get(), charsCount,nullptr, nullptr);
	Assert(charsCount==charsCount2);
	return {buffer.get(),narrow_cast<string::size_type>(charsCount2)};
}
#endif
/*void checkResult(HRESULT result) {
	if (result!=S_OK)throw AimpError{result};
}*/

void showException(const RefCounter<IAIMPCore> &core, const exception &error) {
	showMsgBox(core,MB_ICONERROR, L"Exception!", convertToWstring(error.what()));
}

//void operator|(HRESULT result, _Check_t) { checkResult(result); }

void showMsgBox(const RefCounter<IAIMPCore> &core, DWORD flags,wstring_view title,wstring_view message) {
	const auto executor=queryObject(IAIMPServiceThreads,core);
	const auto task=makeRefCounter<Task>([core,title=wstring(title),message=wstring(message),flags]{
		const auto msgBox = queryObject(IAIMPUIMessageDialog, core);
		msgBox->Execute(nullptr, createString(core, title),createString(core, message), flags);
	});
	executor->ExecuteInMainThread(task,0) | check;
}
/*#ifdef DEBUG
#define er(type) case type:debug(#type);break

void debugError(HRESULT error) {
	switch (error) {
		er(S_OK);
		er(E_FAIL);
		er(E_INVALIDARG);
		er(E_UNEXPECTED);
		er(E_ACCESSDENIED);
		er(E_NOTIMPL);
		default:debug("Unknown error");
	}
}

#endif //DEBUG*/
