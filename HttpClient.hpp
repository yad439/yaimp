//
// Created by YAD on 27.06.2019.
//

#ifndef YAIMP_HTTPCLIENT_HPP
#define YAIMP_HTTPCLIENT_HPP

#include <SDK/apiObjects.h>
#include <SDK/apiInternet.h>

#include <vector>
#include <span>
#include <string_view>
#include <regex>
#include <utility>
#include <unordered_map>
#include <sstream>

#include "RefCounter.hpp"
#include "IUnknownImpl.hpp"

class HttpClient {
public:
	struct Result {
	private:
		class EventHandler : public IAIMPHTTPClientEvents2, public IUnknownImpl<IAIMPHTTPClientEvents> {
		public:
			[[nodiscard]] unsigned short status() const;

			//[[nodiscard]] const std::unordered_map<std::wstring,std::wstring>& cookies() const;

			void WINAPI OnAccept(IAIMPString *ContentType,INT64 ContentSize, BOOL *Allow) override;

			void WINAPI OnComplete(IAIMPErrorInfo *ErrorInfo, BOOL Canceled) override;

			void WINAPI OnProgress(INT64 Downloaded, INT64 Total) override;

			void WINAPI OnAcceptHeaders(IAIMPString *Header, BOOL *Allow) override;

		ULONG WINAPI AddRef() override;

			ULONG WINAPI Release() override;

			HRESULT WINAPI QueryInterface(const IID &riid, LPVOID *ppvObj) override;

		private:
			const IID &thisIID() override;

		private:
			static const std::wregex _statusRegex;
			//static const std::wregex _cookieRegex;
			//std::unordered_map<std::wstring, std::wstring> _cookies;
			unsigned short _status{};

			static_assert(sizeof(_status) == 2);
		};

		class Stream : public IUnknownImpl<IAIMPStream> {
		public:
			[[nodiscard]] std::stringstream& data();

//			void reset();

			INT64 WINAPI GetSize() override;

			HRESULT WINAPI SetSize(INT64 Value) override;

			INT64 WINAPI GetPosition() override;

			HRESULT WINAPI Seek(INT64 Offset, int Mode) override;

			int WINAPI Read(unsigned char *Buffer, unsigned int Count) override;

			HRESULT WINAPI Write(unsigned char *Buffer, unsigned int Count, unsigned int *Written) override;

		private:
			const IID &thisIID() override;
		private:
			std::stringstream _data;
		};

	public:
		Result();

		[[nodiscard]] unsigned short status() const;

		[[nodiscard]] std::stringstream &content() const;

		//[[nodiscard]] const std::unordered_map<std::wstring, std::wstring> &cookies() const;

		[[nodiscard]] RefCounter<Stream> stream()const;

		[[nodiscard]] RefCounter<EventHandler> handler()const;

//		void reset() const;

	private:
		RefCounter<Stream> _stream;
		RefCounter<EventHandler> _handler;
	};

	explicit HttpClient(const RefCounter<IAIMPCore> &aimpCore);

	[[nodiscard]] Result get(std::wstring_view url) const;

	[[nodiscard]] Result post(std::wstring_view url, std::span<std::pair<std::string_view, std::string_view>> data) const;

private:
	const RefCounter<IAIMPCore> _aimpCore;
	const RefCounter<IAIMPServiceHTTPClient> _aimpClient;
};


#endif //YAIMP_HTTPCLIENT_HPP
