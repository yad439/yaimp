//
// Created by YAD on 27.06.2019.
//

#include "HttpClient.hpp"

#include <fmt/format.h>

#include <gsl/gsl_util>

//#include <algorithm>
//#include <format>

#include "Utility.hpp"

using namespace std;
using namespace gsl;
using fmt::format;

HttpClient::HttpClient(const RefCounter<IAIMPCore> &aimpCore)
		: _aimpCore{aimpCore},  _aimpClient{queryObject(IAIMPServiceHTTPClient, aimpCore)}{}

HttpClient::Result HttpClient::get(wstring_view url) const {
	void *id{};
	Result result;
	_aimpClient->Get(createString(_aimpCore, url), AIMP_SERVICE_HTTPCLIENT_FLAGS_WAITFOR, result.stream(),
	                 result.handler(), nullptr, &id) | check;
	return result;
}

HttpClient::Result HttpClient::post(wstring_view url, span<pair<string_view, string_view>> data) const {
	const auto inputStream = aimpCreate<IAIMPStream>(_aimpCore, IID_IAIMPMemoryStream);
	for (const auto&[param, value]:data) {
		const auto str = format("{}={}&", param, value);
		inputStream->Write(const_cast<unsigned char *>(pointer_cast<const unsigned char *>(str.c_str())), str.length(),
		                   nullptr) | check;
	}
	void *id;
	Result result;
	_aimpClient->Post(createString(_aimpCore, url), AIMP_SERVICE_HTTPCLIENT_FLAGS_WAITFOR, result.stream(), inputStream,
	                  result.handler(), nullptr, &id) | check;
	return result;
}

HttpClient::Result::Result() : _stream{makeRefCounter<Stream>()}, _handler{makeRefCounter<EventHandler>()} {}

unsigned short HttpClient::Result::status() const { return _handler->status(); }

stringstream &HttpClient::Result::content() const { return _stream->data(); }

/*const unordered_map<wstring, wstring> &HttpClient::Result::cookies() const {
	return _handler->cookies();
}*/

RefCounter<HttpClient::Result::Stream> HttpClient::Result::stream() const { return _stream; }

RefCounter<HttpClient::Result::EventHandler> HttpClient::Result::handler() const { return _handler; }

//void HttpClient::Result::reset() const { _stream->reset(); }

unsigned short HttpClient::Result::EventHandler::status() const { return _status; }

//const unordered_map<wstring, wstring> &HttpClient::Result::EventHandler::cookies() const { return _cookies; }

HRESULT HttpClient::Result::EventHandler::QueryInterface(const IID &riid, LPVOID *ppvObj) {
	if (!ppvObj) return E_POINTER;
	if (riid == IID_IAIMPHTTPClientEvents || riid == IID_IAIMPHTTPClientEvents2) {
		*ppvObj = static_cast<IAIMPHTTPClientEvents2 *>(this);
		AddRef();
		return S_OK;
	}
	return E_NOINTERFACE;
}

void WINAPI HttpClient::Result::EventHandler::OnAccept(IAIMPString *, const INT64, BOOL *) {}

void WINAPI HttpClient::Result::EventHandler::OnComplete(IAIMPErrorInfo *ErrorInfo, BOOL Canceled) {
	if (GSL_UNLIKELY(Canceled))_status = 0;
	if (GSL_UNLIKELY(ErrorInfo)) {
		int code;
		IAIMPString *msg;
		IAIMPString *det;
		ErrorInfo->GetInfo(&code, &msg, &det) | check;
		_status = narrow<decltype(_status)>(code);
		msg->Release();
		det->Release();
	}
}

void WINAPI HttpClient::Result::EventHandler::OnProgress(const INT64, const INT64) {}

void WINAPI HttpClient::Result::EventHandler::OnAcceptHeaders(IAIMPString *Header, BOOL *) {
	const wchar_t *headerData = Header->GetData();
	const auto headerLength = Header->GetLength();
	wcmatch statusMatch;
	const auto res = regex_search(headerData, headerData + headerLength, statusMatch, _statusRegex);
	Assert(res);
	AssertSlow(statusMatch.ready());
	_status = narrow<decltype(_status)>(stoi(statusMatch.str(1)));
	/*for_each(regex_iterator{headerData, headerData + headerLength + 1, _cookieRegex},
	         regex_iterator<decltype(headerData)>{}, [this](const auto &match) {
				_cookies.emplace(match.str(1), match.str(2));
			});*/
}

ULONG WINAPI HttpClient::Result::EventHandler::AddRef() {
	return IUnknownImpl<IAIMPHTTPClientEvents>::AddRef();
}

ULONG WINAPI HttpClient::Result::EventHandler::Release() {
	return IUnknownImpl<IAIMPHTTPClientEvents>::Release();
}

const IID &HttpClient::Result::EventHandler::thisIID() {
	return IID_IUnknown;
}

const wregex HttpClient::Result::EventHandler::_statusRegex // NOLINT(cert-err58-cpp)
		{L"^HTTP/\\d\\.\\d ([\\d]{3}) ", wregex::optimize};
/*const wregex HttpClient::Result::EventHandler::_cookieRegex // NOLINT(cert-err58-cpp)
		{L"Set-Cookie: (\\w+)=(\\w+)(?:\\0|;)", wregex::optimize};*/

stringstream &HttpClient::Result::Stream::data() { return _data; }

/*void HttpClient::Result::Stream::reset() {
	_data.str({});
	_data.clear();
}*/

INT64 WINAPI HttpClient::Result::Stream::GetSize() { return 0; }

HRESULT WINAPI HttpClient::Result::Stream::SetSize(const INT64) { return S_OK; }

INT64 WINAPI HttpClient::Result::Stream::GetPosition() { return narrow_cast<INT64>(_data.tellp()); }

HRESULT WINAPI HttpClient::Result::Stream::Seek(const INT64 Offset, int Mode) {
	switch (Mode) {
		case AIMP_STREAM_SEEKMODE_FROM_BEGINNING:
			_data.seekp(narrow_cast<stringstream::off_type>(Offset), stringstream::beg);
			break;
		case AIMP_STREAM_SEEKMODE_FROM_CURRENT: _data.seekp(narrow_cast<stringstream::off_type>(Offset), stringstream::cur);
			break;
		case AIMP_STREAM_SEEKMODE_FROM_END: _data.seekp(narrow_cast<stringstream::off_type>(Offset), stringstream::end);
			break;
		default: Assert(false);
	}
	return S_OK;
}

int WINAPI HttpClient::Result::Stream::Read(unsigned char *, unsigned int) {
	Assert(false);
	return 0;
}

HRESULT WINAPI HttpClient::Result::Stream::Write(unsigned char *Buffer, unsigned int Count, unsigned int *Written) {
	_data.write(pointer_cast<stringstream::char_type *>(Buffer), narrow_cast<streamsize>(Count));
	if (Written)*Written = Count;
	return S_OK;
}

const IID &HttpClient::Result::Stream::thisIID() {
	return IID_IAIMPStream;
}
