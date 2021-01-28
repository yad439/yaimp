#include "PlayerHook.hpp"

#include <gsl/gsl_util>

#include "Utility.hpp"

using namespace std;

PlayerHook::PlayerHook(RefCounter<IAIMPCore> aimpCore, shared_ptr<YandexAPI> downloader)
		: _aimpCore{move(aimpCore)}, _downloader{move(downloader)} {}

HRESULT WINAPI PlayerHook::OnCheckURL(IAIMPString *URL, BOOL *Handled) {
	if(wcsncmp(URL->GetData(),L"yamusic://",10)!=0)return E_FAIL;
	try {
		wcmatch idMatch;
		if (!regex_match(URL->GetData(), idMatch, yaRegex))return E_FAIL;
		const auto newUrl = _downloader->getDownloadUrl(idMatch.str(1));
		const auto newAimpUrl=convertToWstring(newUrl);
		URL->SetData(const_cast<WCHAR *>(newAimpUrl.c_str()), gsl::narrow_cast<int>(newAimpUrl.length())) | check;
		*Handled = true;
		return S_OK;
	} catch (const HttpError &he) {
		showException(_aimpCore, he);
		return E_FAIL;
	} catch (const nlohmann::json::parse_error&) {
		showMsgBox(_aimpCore, MB_ICONERROR, L"Exception!", L"Response parsing error!");
		return E_FAIL;
	}catch (const nlohmann::json::out_of_range&) {
		showMsgBox(_aimpCore, MB_ICONERROR, L"Exception!", L"Response parsing error!");
		return E_FAIL;
	}
}

const IID &PlayerHook::thisIID() {
	return IID_IAIMPExtensionPlayerHook;
}