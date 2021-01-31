#include "Yaimp.hpp"

//#include <SDK/apiMessages.h>
//#include <SDK/apiOptions.h>

#include "Utility.hpp"
#include "PlayerHook.hpp"
#include "OptionsFrame.hpp"
#include "MenuManagement.hpp"
#include "AddUrlMenuAction.hpp"
#include "AddTrackedPlaylistAction.hpp"
#include "EventListeners.hpp"

using namespace std;
extern "C" [[maybe_unused]] HRESULT __declspec(dllexport) WINAPI AIMPPluginGetHeader(IAIMPPlugin **Header);
extern "C" [[maybe_unused]] HRESULT __declspec(dllexport) WINAPI AIMPPluginGetHeader(IAIMPPlugin **Header) {
	*Header = new Yaimp;
	return S_OK;
}

PWCHAR Yaimp::InfoGet(int Index) {
	switch (Index) {
		case AIMP_PLUGIN_INFO_NAME: return const_cast<PWCHAR>(L"Yaimp");
		case AIMP_PLUGIN_INFO_AUTHOR: return const_cast<PWCHAR>(L"Vedblade");
		case AIMP_PLUGIN_INFO_SHORT_DESCRIPTION: return const_cast<PWCHAR>(L"Yandex music integration");
		default: return nullptr;
	}
}

DWORD Yaimp::InfoGetCategories() {
	return AIMP_PLUGIN_CATEGORY_ADDONS;
}

HRESULT Yaimp::Initialize(IAIMPCore *Core) {
	Core->AddRef();
	RefCounter aimpCore{Core};
	try {
		const auto yandexAPI = make_shared<YandexAPI>(aimpCore);
		_playlistManager=make_shared<PlaylistManager>(cref(aimpCore),yandexAPI);

		const auto addUrlMenuItem = createMenu(Core, AIMP_MENUID_PLAYER_PLAYLIST_ADDING, L"AddUrl",
		                                       AddUrlMenuAction{aimpCore, yandexAPI,_playlistManager});
#ifdef YAIMP_UPDATABLE_PLAYLISTS
		const auto addTrackedPlaylistMenuItem=createMenu(Core, AIMP_MENUID_PLAYER_PLAYLIST_ADDING, L"AddPlaylist",
		                                                 AddTrackedPlaylistAction{aimpCore, yandexAPI,_playlistManager});
		const auto reloadTrackedMenuItem=createMenu(Core, AIMP_MENUID_PLAYER_PLAYLIST_MISCELLANEOUS, L"UpdateTracked",
		                                             [manager=_playlistManager]{manager->updateCurrentFromYandex();});
		checkResult(Core->RegisterExtension(IID_IAIMPServicePlaylistManager, makeRefCounter<TrackedPlaylistListener>(_playlistManager, reloadTrackedMenuItem)));
#endif
		checkResult(Core->RegisterExtension(IID_IAIMPServicePlaylistManager, makeRefCounter<EditablePlaylistListener>(addUrlMenuItem)));
		checkResult(Core->RegisterExtension(IID_IAIMPServiceOptionsDialog, makeRefCounter<OptionsFrame>(aimpCore,yandexAPI)));
		checkResult(Core->RegisterExtension(IID_IAIMPServicePlayer, makeRefCounter<PlayerHook>(aimpCore, yandexAPI)));
	} catch (const AimpQueryError&) {
		return E_ABORT;
	}
	return S_OK;
}

HRESULT Yaimp::Finalize() {return S_OK;}

void Yaimp::SystemNotification(int, IUnknown *) {}

const IID &Yaimp::thisIID() {
	return IID_IUnknown;
}
