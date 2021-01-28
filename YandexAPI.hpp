//
// Created by YAD on 28.06.2019.
//

#ifndef YAIMP_YANDEXAPI_HPP
#define YAIMP_YANDEXAPI_HPP

#include <SDK/apiFileManager.h>

#include <nlohmann/json.hpp>

#include <string_view>

#include "RefCounter.hpp"
#include "HttpClient.hpp"

class YandexAPI {
public:
	explicit YandexAPI(const RefCounter<IAIMPCore> &aimpCore);

	[[nodiscard]] std::string getDownloadUrl(const std::wstring_view& trackId) const;
	[[nodiscard]] RefCounter<IAIMPFileInfo> getTrack(const std::wstring_view& albumId, const std::wstring_view& trackId) const;
	[[nodiscard]] RefCounter<IAIMPObjectList> getAlbum(const std::wstring_view& albumId)const;
	[[nodiscard]] RefCounter<IAIMPObjectList> getPlaylist(const std::wstring_view& owner,const std::wstring_view& playlistId)const;
	[[nodiscard]] std::string getPlaylistTitle(std::wstring_view owner,std::wstring_view playlistId) const;

	[[nodiscard]] std::string login(std::string_view login,std::string_view password)const;
	void logout()const;
	[[nodiscard]] std::string profileInfo()const;
private:
	[[nodiscard]] nlohmann::json getJson(std::wstring_view url) const;

	[[nodiscard]] RefCounter<IAIMPFileInfo> trackFromJson(const nlohmann::json& track) const;

private:
	static inline constexpr unsigned int MAX_RETRIES = 50;

	const RefCounter<IAIMPCore> _aimpCore;
	const HttpClient _client;
};


#endif //YAIMP_YANDEXAPI_HPP
