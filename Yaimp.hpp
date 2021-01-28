#ifndef YAIMP_YAIMP_HPP
#define YAIMP_YAIMP_HPP

#include<SDK/apiPlugin.h>

#include<memory>

#include "IUnknownImpl.hpp"
#include "PlaylistManager.hpp"

class Yaimp: public IUnknownImpl<IAIMPPlugin>{
public:
	PWCHAR WINAPI InfoGet(int Index) override;

	DWORD WINAPI InfoGetCategories() override;

	void WINAPI SystemNotification(int NotifyID, IUnknown *Data) override;

	HRESULT WINAPI Initialize(IAIMPCore *Core) override;

	HRESULT WINAPI Finalize() override;
private:
	const IID &thisIID() override;

	std::shared_ptr<PlaylistManager> _playlistManager;
};

#endif //YAIMP_YAIMP_HPP