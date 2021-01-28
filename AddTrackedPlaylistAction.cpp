//
// Created by YAD on 03.02.2020.
//

#include "AddTrackedPlaylistAction.hpp"
#include "AddUrlDialog.hpp"
#include "Utility.hpp"

using namespace std;

AddTrackedPlaylistAction::AddTrackedPlaylistAction(RefCounter<IAIMPCore> aimpCore,
                                                   std::shared_ptr<YandexAPI> yandexApi,
                                                   std::shared_ptr<PlaylistManager> playlistManager) :
		_aimpCore{move(aimpCore)}, _yandexAPI{move(yandexApi)}, _playlistManager{move(playlistManager)} {}

void AddTrackedPlaylistAction::operator()() const {
	try {
		AddUrlDialog dialog{_aimpCore};
		const auto url = dialog.show();
		if (!url)return;
		wcmatch urlMatch;
		if (regex_match(url.value()->GetData(), urlMatch, _playlistRegex)) {
			const auto owner = urlMatch.str(1);
			const auto id = urlMatch.str(2);
			auto playlist = _playlistManager->createNew(
					createString(_aimpCore, _yandexAPI->getPlaylistTitle(owner, id)),
					_yandexAPI->getPlaylist(owner, id));
			_playlistManager->addTracking(playlist, owner, id);
			_playlistManager->activate(playlist);
		} else {
			showMsgBox(_aimpCore, MB_ICONWARNING, L"Error.", L"Unsupported url type!");
		}
	} catch (const HttpError &he) {
		showException(_aimpCore, he);
	} catch (const nlohmann::json::parse_error &) {
		showMsgBox(_aimpCore, MB_ICONERROR, L"Exception!", L"Response parsing error!");
	} catch (const nlohmann::json::out_of_range &) {
		showMsgBox(_aimpCore, MB_ICONERROR, L"Exception!", L"Response parsing error!");
	}
}
