//
// Created by YAD on 30.08.2019.
//
#include "IUnknownImpl.hpp"

#include <SDK/apiObjects.h>
#include <SDK/apiPlugin.h>
#include <SDK/apiPlayer.h>
#include <SDK/apiGUI.h>
#include <SDK/apiInternet.h>
#include <SDK/apiOptions.h>

template<class Base>
ULONG WINAPI IUnknownImpl<Base>::AddRef() { return ++_refCounter; }

template<class Base>
ULONG WINAPI IUnknownImpl<Base>::Release() {
	--_refCounter;
	if (_refCounter == 0)delete this;
	return _refCounter;
}

template<class Base>
HRESULT WINAPI IUnknownImpl<Base>::QueryInterface(REFIID riid, LPVOID *ppvObj) {
	if (!ppvObj) return E_POINTER;
	if (riid == thisIID()) {
		*ppvObj = this;
		++_refCounter;
		return S_OK;
	}
	return E_NOINTERFACE;
}

template<class Base>
IUnknownImpl<Base>::~IUnknownImpl() = default;

template
class IUnknownImpl<IAIMPPlugin>;

template
class IUnknownImpl<IAIMPStream>;

template
class IUnknownImpl<IAIMPExtensionPlayerHook>;

template
class IUnknownImpl<IAIMPExtensionPlaylistManagerListener>;

template
class IUnknownImpl<IAIMPHTTPClientEvents>;

template
class IUnknownImpl<IAIMPUIChangeEvents>;

template
class IUnknownImpl<IAIMPOptionsDialogFrame>;

template
class IUnknownImpl<IAIMPActionEvent>;

template
class IUnknownImpl<IAIMPTask>;