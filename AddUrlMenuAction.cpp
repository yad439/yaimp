//
// Created by YAD on 21.07.2019.
//

#include "AddUrlMenuAction.hpp"

#include "AddUrlDialog.hpp"
#include "Utility.hpp"

using namespace std;

AddUrlMenuAction::AddUrlMenuAction(RefCounter<IAIMPCore> aimpCore, std::shared_ptr<YandexAPI> yandexApi,std::shared_ptr<PlaylistManager> playlistManager)
		: _aimpCore(move(aimpCore)), _yandexAPI(move(yandexApi)), _playlistManager{move(playlistManager)} {}

void AddUrlMenuAction::operator()() const {
	try {
		//HttpClient cl{aimpCore,{}};
		//cl.post(L"https://passport.yandex.ru/auth",{{"login","yad439"},{"passwd","rn0Botn%3Fn0nYfqltn"}});
		AddUrlDialog dialog{_aimpCore};
		const auto url = dialog.show();
		if (!url)return;
		wcmatch urlMatch;
		if (regex_match(url.value()->GetData(), urlMatch, trackRegex))
			_playlistManager->addToCurrent(_yandexAPI->getTrack(urlMatch.str(1), urlMatch.str(2)));
		else if (regex_match(url.value()->GetData(), urlMatch, albumRegex))
			_playlistManager->addToCurrent(_yandexAPI->getAlbum(urlMatch.str(1)));
		else if (regex_match(url.value()->GetData(), urlMatch, playlistRegex))
			_playlistManager->addToCurrent(_yandexAPI->getPlaylist(urlMatch.str(1), urlMatch.str(2)));
		else {
			showMsgBox(_aimpCore, MB_ICONWARNING, L"Error.", L"Unsupported url type!");
		}
		//LoginDialog d{aimpCore};
	} catch (const HttpError &he) {
		showException(_aimpCore, he);
	} catch (const nlohmann::json::parse_error&) {
		showMsgBox(_aimpCore, MB_ICONERROR, L"Exception!", L"Response parsing error!");
	}catch (const nlohmann::json::out_of_range&) {
		showMsgBox(_aimpCore, MB_ICONERROR, L"Exception!", L"Response parsing error!");
	}
}
