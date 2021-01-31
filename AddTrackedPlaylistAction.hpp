//
// Created by YAD on 03.02.2020.
//
#ifdef YAIMP_UPDATABLE_PLAYLISTS
#ifndef YAIMP_ADDTRACKEDPLAYLISTACTION_HPP
#define YAIMP_ADDTRACKEDPLAYLISTACTION_HPP

#include <SDK/apiCore.h>

#include <memory>

#include "YandexAPI.hpp"
#include "PlaylistManager.hpp"

class AddTrackedPlaylistAction {
public:
	AddTrackedPlaylistAction(RefCounter<IAIMPCore> aimpCore,
	                         std::shared_ptr<YandexAPI> yandexApi,
	                         std::shared_ptr<PlaylistManager> playlistManager);

	void operator()() const;

private:
	const RefCounter<IAIMPCore> _aimpCore;
	const std::shared_ptr<YandexAPI> _yandexAPI;
	const std::shared_ptr<PlaylistManager> _playlistManager;

	const std::wregex _playlistRegex{L"https://music\\.yandex\\.ru/users/([\\w\\-]+)/playlists/([0-9]+)", std::regex::optimize};
};

#endif //YAIMP_ADDTRACKEDPLAYLISTACTION_HPP
#endif