//
// Created by YAD on 19.07.2019.
//

#ifndef YAIMP_OPTIONSFRAME_HPP
#define YAIMP_OPTIONSFRAME_HPP

#include <SDK/apiOptions.h>
#include <SDK/apiCore.h>
#include <SDK/apiGUI.h>

#include <functional>
#include <memory>

#include "IUnknownImpl.hpp"
#include "RefCounter.hpp"
#include "GUIBase.hpp"
#include "YandexAPI.hpp"

class OptionsFrame : GUIBase, public IUnknownImpl<IAIMPOptionsDialogFrame> {
	class EventHandler:public IUnknownImpl<IAIMPUIChangeEvents>{
	public:
		explicit EventHandler(std::function<void()> handle);

		void WINAPI OnChanged(IUnknown *Sender) override;

	private:
		const IID &thisIID() override;

	private:
		std::function<void()> _handle;
	};

public:
	OptionsFrame(RefCounter<IAIMPCore> aimpCore,std::shared_ptr<YandexAPI> yandexAPI);

	HRESULT WINAPI GetName(IAIMPString **S) override;

	HWND WINAPI CreateFrame(HWND ParentWnd) override;

	void WINAPI DestroyFrame() override;

	void WINAPI Notification(int ID) override;

private:
	const IID &thisIID() override;

private:
	const RefCounter<IAIMPCore> _aimpCore;
	const std::shared_ptr<YandexAPI> _yandexAPI;
	RefCounter<IAIMPUIButton> _loginButton;
	RefCounter<IAIMPUIButton> _logoutButton;
	RefCounter<IAIMPUILabel> _statusLabel;
};


#endif //YAIMP_OPTIONSFRAME_HPP
