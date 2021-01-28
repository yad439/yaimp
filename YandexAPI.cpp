//
// Created by YAD on 28.06.2019.
//

#include "YandexAPI.hpp"

#include <fmt/format.h>

#include <sstream>
#include <string>
#include <array>
//#include <format>

#include "MD5.hpp"
#include "Utility.hpp"

using namespace std;
using fmt::format;

YandexAPI::YandexAPI(const RefCounter<IAIMPCore> &aimpCore)
		: _aimpCore{aimpCore}, _client{aimpCore} {}

string YandexAPI::getDownloadUrl(const std::wstring_view &trackId) const {
	const auto infoUrl =
			getJson(format(L"https://music.yandex.ru/api/v2.1/handlers/track/{}/track/download/m?hq=1", trackId))
					.at("src").get<string>();
	const auto downloadInfo = getJson(format(L"{}&format=json", convertToWstring(infoUrl)));
	MD5 md5;
	md5.update("XGRlBW9FXlekgbPrRHuSiA"sv);
	md5.update(downloadInfo.at("path").get<string>().substr(1));
	md5.update(downloadInfo.at("s").get<string>());
	md5.finalize();
	const auto digest = md5.hexdigest();
	return format("https://{}/get-mp3/{}/{}{}",
	              downloadInfo.at("host").get<string>(),
	              digest,
	              downloadInfo.at("ts").get<string>(),
	              downloadInfo.at("path").get<string>());
}

RefCounter<IAIMPFileInfo> YandexAPI::getTrack(const std::wstring_view &albumId,
                                              const std::wstring_view &trackId) const {
	const auto track =
			getJson(format(L"https://music.yandex.ru/handlers/track.jsx?track={}%3A{}", trackId, albumId))
					.at("track");
	return trackFromJson(track);
}

RefCounter<IAIMPObjectList> YandexAPI::getAlbum(const std::wstring_view &albumId) const {
	const auto album = getJson(format(L"https://music.yandex.ru/handlers/album.jsx?album={}", albumId));
	const auto volumes = album.at("volumes");
	auto trackList = createObject(IAIMPObjectList, _aimpCore);
	for (const auto &volume:volumes)
		for (const auto &track:volume)trackList->Add(trackFromJson(track)) | check;
	return trackList;
}

RefCounter<IAIMPObjectList> YandexAPI::getPlaylist(const std::wstring_view &owner,
                                                   const std::wstring_view &playlistId) const {
	const auto playlist =
			getJson(format(L"https://music.yandex.ru/handlers/playlist.jsx?owner={}&kinds={}", owner, playlistId))
					.at("playlist");
	const auto tracks = playlist.at("tracks");
	auto trackList = createObject(IAIMPObjectList, _aimpCore);
	for (const auto &track:tracks)trackList->Add(trackFromJson(track)) | check;
	return trackList;
}

std::string YandexAPI::getPlaylistTitle(const wstring_view owner, const wstring_view playlistId) const {
	return getJson(format(L"https://music.yandex.ru/handlers/playlist.jsx?owner={}&kinds={}", owner, playlistId))
			.at("playlist")
			.at("title")
			.get<string>();
}

string YandexAPI::login(std::string_view login, std::string_view password) const {
	array<pair<string_view, string_view>,2> data{pair{"login"sv,  login},
	                                                      pair{"passwd"sv, password}};
	auto res = _client.post(L"https://passport.yandex.ru/auth"sv, data);
	return profileInfo();
}

void YandexAPI::logout() const {
	auto profile = getJson(L"https://music.yandex.ru/api/v2.1/handlers/auth"sv);
	auto res = _client.get(convertToWstring(
			format("https://passport.yandex.ru/passport?mode=embeddedauth&action=logout&retpath=https%3A%2F%2Fmusic.yandex.ru%2Fusers%2Fyad439%2Fplaylists&yu={}&uid={}",
			       profile.at("yandexuid").get<string>(),
			       profile.at("uid").get<string>())));
	AssertSlow(res.status() == 200);
}

RefCounter<IAIMPFileInfo> YandexAPI::trackFromJson(const nlohmann::json &track) const {
	ostringstream joinedArtists;
	const auto artists = track.at("artists");
	if (!artists.empty()) {
		auto it = artists.begin();
		joinedArtists << it->at("name").get<string>();
		++it;
		for (; it != artists.end(); ++it)joinedArtists << '/' << it->at("name").get<string>();
	}
	auto trackInfo = createObject(IAIMPFileInfo, _aimpCore);

	trackInfo->SetValueAsObject(AIMP_FILEINFO_PROPID_FILENAME, createString(_aimpCore,
	                                                                        format("yamusic://{}.mp3",
	                                                                               track.at("id").get<string>())))
	| check;
	trackInfo->SetValueAsObject(AIMP_FILEINFO_PROPID_TITLE,
	                            createString(_aimpCore, track.at("title").get<string>())) | check;
	trackInfo->SetValueAsObject(AIMP_FILEINFO_PROPID_ARTIST, createString(_aimpCore, joinedArtists.str())) | check;
	return trackInfo;
}

nlohmann::json YandexAPI::getJson(wstring_view url) const {
	auto aimpUrl = format(L"{}\r\nX-Retpath-Y: https%3A%2F%2Fmusic.yandex.ru", url);

	unsigned counter = 0;
	HttpClient::Result result;
	do {
		result = _client.get(aimpUrl);
		++counter;
	} while (result.status() != 200 && counter < MAX_RETRIES);
	if (result.status() != 200)throw HttpError{result.status()};

	return nlohmann::json::parse(result.content());
}

string YandexAPI::profileInfo() const {
	const auto json = getJson(L"https://music.yandex.ru/api/v2.1/handlers/auth"sv);
	return json.contains("login") ? json.at("login").get<string>() : string{};
}
