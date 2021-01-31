//
// Created by YAD on 12.06.2019.
//

#include "PlaylistManager.hpp"

#include <gsl/gsl_util>
#include <fstream>
#include <regex>

#include "Utility.hpp"

using namespace std;
using namespace gsl;

PlaylistManager::PlaylistManager(const RefCounter<IAIMPCore> &aimpCore, std::shared_ptr<YandexAPI> yandexApi)
		: _aimpCore{aimpCore}, _aimpManager{queryObject(IAIMPServicePlaylistManager, aimpCore)},
		  _yandexApi{move(yandexApi)} {
#ifdef YAIMP_UPDATABLE_PLAYLISTS
	IAIMPString *configDir{};
	aimpCore->GetPath(AIMP_CORE_PATH_PROFILE, &configDir) | check;
	Assert(configDir);
	final_action _{[configDir] { configDir->Release(); }};
	_configPath = convertToString(configDir);
	_configPath /= "YaimpPlaylists.ini";
	if (!exists(_configPath))return;
	wifstream config{_configPath};
	wregex playlistRegex{L"({[A-Z\\d\\-]+})=([\\w\\-]+)\\/([\\w-]+)\\n?"};
	for (wstring line; getline(config, line);) {
		if (line.empty())continue;
		wsmatch regMatch;
		const auto res=regex_match(line,regMatch,playlistRegex);
		if(!res)continue;
		_updatablePlaylists.emplace(regMatch.str(1), YandexPlaylist{regMatch.str(2), regMatch.str(3)});
	}
#endif
}

void PlaylistManager::addToCurrent(IAIMPFileInfo *file) const {
	Assert(file);
	getCurrentPlaylist()->Add(file, AIMP_PLAYLIST_ADD_FLAGS_FILEINFO, -1) | check;
}

void PlaylistManager::addToCurrent(IAIMPObjectList *list) const {
	Assert(list);
	getCurrentPlaylist()->AddList(list, AIMP_PLAYLIST_ADD_FLAGS_FILEINFO, -1) | check;
}

RefCounter<IAIMPPlaylist> PlaylistManager::getCurrentPlaylist() const {
	IAIMPPlaylist *currentPlaylist{};
	_aimpManager->GetActivePlaylist(&currentPlaylist) | check;
	Assert(currentPlaylist);
	return RefCounter{currentPlaylist};
}
#ifdef YAIMP_UPDATABLE_PLAYLISTS
RefCounter<IAIMPPlaylist> PlaylistManager::createNew(IAIMPString *title, IAIMPObjectList *list) const {
	IAIMPPlaylist *newPlaylist{};
	_aimpManager->CreatePlaylist(title, false, pointer_cast<IAIMPPlaylist **>(&newPlaylist)) | check;
	Assert(newPlaylist);
	RefCounter playlist{newPlaylist};
	newPlaylist->AddList(list, AIMP_PLAYLIST_ADD_FLAGS_FILEINFO, -1) | check;
	return playlist;
}

void PlaylistManager::updateCurrentFromYandex() {
	auto aimpPlaylist = getCurrentPlaylist();
	const auto idString = getPlaylistId(aimpPlaylist);
	shared_lock lock{mapMutex};
	const auto &yandexPlaylist = _updatablePlaylists.at(idString);
	updateFromYandex(aimpPlaylist, yandexPlaylist);
}

void PlaylistManager::updateAllFromYandex() {
	shared_lock lock{mapMutex};
	for (const auto&[aimpPlaylistId, yandexPlaylist]:_updatablePlaylists) {
		IAIMPPlaylist *aimpPlaylist{};
		_aimpManager->GetLoadedPlaylistByID(createString(_aimpCore, aimpPlaylistId), &aimpPlaylist) | check;
		final_action _{[aimpPlaylist] { aimpPlaylist->Release(); }};
		updateFromYandex(aimpPlaylist, yandexPlaylist);
	}
}

void PlaylistManager::addTracking(IAIMPPlaylist *playlist, std::wstring_view owner, std::wstring_view yandexId) {
	const auto aimpId = getPlaylistId(playlist);
	unique_lock lock{mapMutex};
	_updatablePlaylists.emplace(aimpId, YandexPlaylist{wstring{owner}, wstring{yandexId}});
}

void PlaylistManager::removeTracking(IAIMPPlaylist *playlist) {
	const auto aimpId = getPlaylistId(playlist);
	unique_lock lock{mapMutex};
	AssertSlow(_updatablePlaylists.find(aimpId) != _updatablePlaylists.end());
	_updatablePlaylists.erase(aimpId);
}

bool PlaylistManager::isTracked(IAIMPPlaylist *playlist) {
	const auto aimpId = getPlaylistId(playlist);
	shared_lock lock{mapMutex};
	return _updatablePlaylists.find(aimpId) != _updatablePlaylists.end();
}

void PlaylistManager::activate(IAIMPPlaylist *playlist) const {
	_aimpManager->SetActivePlaylist(playlist) | check;
}

void PlaylistManager::updateFromYandex(IAIMPPlaylist *playlist, const YandexPlaylist &yandexPlaylist) const {
	Assert(playlist);
	const auto files = _yandexApi->getPlaylist(yandexPlaylist.owner, yandexPlaylist.playlistId);
	{
		playlist->BeginUpdate() | check;
		final_action _{[playlist] { playlist->EndUpdate(); }};
		playlist->DeleteAll() | check;//NOLINTNEXTLINE(hicpp-signed-bitwise)
		playlist->AddList(files, AIMP_PLAYLIST_ADD_FLAGS_FILEINFO | AIMP_PLAYLIST_ADD_FLAGS_NOTHREADING, -1)
		| check;
	}
	AssertSlow(playlist->GetItemCount() == files->GetCount());
}

std::wstring PlaylistManager::getPlaylistId(IAIMPPlaylist *playlist) {
	const auto properties = queryObject(IAIMPPropertyList, playlist);
	IAIMPString *id{};
	properties->GetValueAsObject(AIMP_PLAYLIST_PROPID_ID, IID_IAIMPString, pointer_cast<void **>(&id)) | check;
	Assert(id);
	final_action _{[id] { id->Release(); }};
	return {id->GetData(), gsl::narrow_cast<wstring::size_type>(id->GetLength())};
}
PlaylistManager::~PlaylistManager() {
	wofstream config{_configPath};
	for (const auto&[aimpId, yandexPlaylist]:_updatablePlaylists)
		config << aimpId << '=' << yandexPlaylist.owner << '/' << yandexPlaylist.playlistId << '\n';
}
#endif