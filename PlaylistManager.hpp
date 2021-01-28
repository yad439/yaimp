//
// Created by YAD on 12.06.2019.
//

#ifndef YAIMP_PLAYLISTMANAGER_HPP
#define YAIMP_PLAYLISTMANAGER_HPP

#include <SDK/apiPlaylists.h>
#include <SDK/apiCore.h>
#include <SDK/apiFileManager.h>
#include <SDK/apiMenu.h>

#include <map>
#include <memory>
#include <shared_mutex>
#include <filesystem>

#include "RefCounter.hpp"
#include "YandexAPI.hpp"

class PlaylistManager {
	struct YandexPlaylist{
		std::wstring owner;
		std::wstring playlistId;
	};
public:
	explicit PlaylistManager(const RefCounter<IAIMPCore>& aimpCore,std::shared_ptr<YandexAPI> yandexApi);

	void addToCurrent(IAIMPFileInfo *file) const;
	void addToCurrent(IAIMPObjectList *list) const;
	[[nodiscard]] RefCounter<IAIMPPlaylist> getCurrentPlaylist() const;
	RefCounter<IAIMPPlaylist> createNew(IAIMPString* title,IAIMPObjectList *list)const;

	void updateCurrentFromYandex();
	void updateAllFromYandex();

	void addTracking(IAIMPPlaylist* playlist,std::wstring_view owner,std::wstring_view yandexId);
	void removeTracking(IAIMPPlaylist* playlist);
	bool isTracked(IAIMPPlaylist* playlist);

	void activate(IAIMPPlaylist* playlist) const;

	virtual ~PlaylistManager();
private:
	void updateFromYandex(IAIMPPlaylist* playlist,const YandexPlaylist& yandexPlaylist) const;

	static std::wstring getPlaylistId(IAIMPPlaylist* playlist);

	const RefCounter<IAIMPCore> _aimpCore;
	const RefCounter<IAIMPServicePlaylistManager> _aimpManager;
	const std::shared_ptr<YandexAPI> _yandexApi;
	std::filesystem::path _configPath;
	std::map<std::wstring,YandexPlaylist> _updatablePlaylists;
	std::shared_mutex mapMutex;
};


#endif //YAIMP_PLAYLISTMANAGER_HPP
