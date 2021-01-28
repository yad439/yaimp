#ifndef YAIMP_PLAYERHOOK_HPP
#define YAIMP_PLAYERHOOK_HPP

#include <SDK/apiPlayer.h>

#include <memory>
#include <regex>

#include "IUnknownImpl.hpp"
#include "YandexAPI.hpp"


class PlayerHook : public IUnknownImpl<IAIMPExtensionPlayerHook> {
public:
	explicit PlayerHook(RefCounter<IAIMPCore> aimpCore, std::shared_ptr<YandexAPI> downloader);

	HRESULT WINAPI OnCheckURL(IAIMPString *URL, BOOL *Handled) override;

private:
	const IID &thisIID() override;
private:
	const RefCounter<IAIMPCore> _aimpCore;
	const std::shared_ptr<YandexAPI> _downloader;
	const std::wregex yaRegex{L"yamusic://([0-9]+)\\.mp3",std::regex::optimize};
};

#endif //YAIMP_PLAYERHOOK_HPP