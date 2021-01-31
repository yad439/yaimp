//
// Created by YAD on 05.02.2020.
//

#ifndef YAIMP_EVENTLISTENERS_HPP
#define YAIMP_EVENTLISTENERS_HPP

#include <SDK/apiPlaylists.h>
#include <SDK/apiMenu.h>

#include <memory>

#include "IUnknownImpl.hpp"
#include "PlaylistManager.hpp"
#include "RefCounter.hpp"

class EditablePlaylistListener : public IUnknownImpl<IAIMPExtensionPlaylistManagerListener> {
public:
	explicit EditablePlaylistListener(RefCounter<IAIMPMenuItem> item);

	void WINAPI PlaylistActivated(IAIMPPlaylist *Playlist) override ;

	void WINAPI PlaylistAdded(IAIMPPlaylist *) override;

	void WINAPI PlaylistRemoved(IAIMPPlaylist *) override;

private:
	const IID &thisIID() override;

private:
	const RefCounter<IAIMPMenuItem> _item;
};
#ifdef YAIMP_UPDATABLE_PLAYLISTS
class TrackedPlaylistListener: public IUnknownImpl<IAIMPExtensionPlaylistManagerListener>{
public:
	TrackedPlaylistListener(std::shared_ptr<PlaylistManager> manager, RefCounter<IAIMPMenuItem> item);

	void WINAPI PlaylistActivated(IAIMPPlaylist *Playlist) override;

	void WINAPI PlaylistAdded(IAIMPPlaylist *Playlist) override;

	void WINAPI PlaylistRemoved(IAIMPPlaylist *Playlist) override;

private:
	const IID &thisIID() override;

	std::shared_ptr<PlaylistManager> _manager;
	const RefCounter<IAIMPMenuItem> _menuItem;
};
#endif
#endif //YAIMP_EVENTLISTENERS_HPP
