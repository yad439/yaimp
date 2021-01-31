//
// Created by YAD on 05.02.2020.
//

#include "EventListeners.hpp"

#include "Utility.hpp"

using namespace std;

EditablePlaylistListener::EditablePlaylistListener(RefCounter <IAIMPMenuItem> item) : _item(move(item)) {}

void WINAPI EditablePlaylistListener::PlaylistActivated(IAIMPPlaylist *Playlist) {
	int readonly{};
	const auto properties = queryObject(IAIMPPropertyList, Playlist);
	properties->GetValueAsInt32(AIMP_PLAYLIST_PROPID_READONLY, &readonly) | check;
	_item->SetValueAsInt32(AIMP_MENUITEM_PROPID_ENABLED, !readonly) | check;
}

void WINAPI EditablePlaylistListener::PlaylistAdded(IAIMPPlaylist *) {}

void WINAPI EditablePlaylistListener::PlaylistRemoved(IAIMPPlaylist *) {}

const IID &EditablePlaylistListener::thisIID() {
	return IID_IAIMPExtensionPlaylistManagerListener;
}
#ifdef YAIMP_UPDATABLE_PLAYLISTS
TrackedPlaylistListener::TrackedPlaylistListener(shared_ptr<PlaylistManager> manager, RefCounter<IAIMPMenuItem> item) : _manager{move(manager)}, _menuItem{move(item)} {}

void TrackedPlaylistListener::PlaylistActivated(IAIMPPlaylist* Playlist) {
	_menuItem->SetValueAsInt32(AIMP_MENUITEM_PROPID_ENABLED, _manager->isTracked(Playlist)) | check;
}

void TrackedPlaylistListener::PlaylistAdded(IAIMPPlaylist*) {}

void TrackedPlaylistListener::PlaylistRemoved(IAIMPPlaylist *Playlist) {
	if(_manager->isTracked(Playlist))_manager->removeTracking(Playlist);
}

const IID &TrackedPlaylistListener::thisIID() {
	return IID_IAIMPExtensionPlaylistManagerListener;
}

#endif
