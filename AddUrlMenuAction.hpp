//
// Created by YAD on 21.07.2019.
//

#ifndef YAIMP_ADDURLMENUACTION_HPP
#define YAIMP_ADDURLMENUACTION_HPP

#include <SDK/apiCore.h>

#include <memory>
#include <regex>

#include "RefCounter.hpp"
#include "PlaylistManager.hpp"
#include "YandexAPI.hpp"

class AddUrlMenuAction {
public:
	AddUrlMenuAction(RefCounter<IAIMPCore> aimpCore, std::shared_ptr<YandexAPI> yandexApi,std::shared_ptr<PlaylistManager> playlistManager);

	void operator()() const;
private:
	const RefCounter<IAIMPCore> _aimpCore;
	const std::shared_ptr<YandexAPI> _yandexAPI;
	const std::shared_ptr<PlaylistManager> _playlistManager;

	const std::wregex trackRegex{L"https://music\\.yandex\\.ru/album/([0-9]+)/track/([0-9]+)",std::regex::optimize};
	const std::wregex albumRegex{L"https://music\\.yandex\\.ru/album/([0-9]+)",std::regex::optimize};
	const std::wregex playlistRegex{L"https://music\\.yandex\\.ru/users/([\\w\\-]+)/playlists/([0-9]+)",std::regex::optimize};
};


#endif //YAIMP_ADDURLMENUACTION_HPP
