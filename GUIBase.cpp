//
// Created by YAD on 19.07.2019.
//

#include "GUIBase.hpp"

#include <SDK/apiMessages.h>

#include "Utility.hpp"

void *GUIBase::createChildI(IAIMPServiceUI* service, IAIMPUIWinControl *parent, IAIMPString *name, IUnknown* eventsHandler, REFIID iid) const{
	Assert(_form);
	void* element{};
	service->CreateControl(_form, parent, name, eventsHandler, iid, &element) | check;
	Assert(element);
	return element;
}

GUIBase::GUIBase(IAIMPCore *core, LONG width, LONG height, const std::wstring_view &name) {
	const auto uiService=queryObject(IAIMPServiceUI, core);
	checkResult(uiService->CreateForm(nullptr, 0, nullptr, nullptr, &_form));
	_form->SetValueAsObject(AIMPUI_CONTROL_PROPID_NAME, createString(core, name)) | check;
	const auto messageDispatcher= queryObject(IAIMPServiceMessageDispatcher, core);
	HWND handle;
	messageDispatcher->Send(static_cast<DWORD>(AIMP_MSG_PROPERTY_HWND), AIMP_MPH_MAINFORM, &handle) | check;
	RECT windowRect;
	GetWindowRect(handle,&windowRect);
	const auto horMid=(windowRect.left+windowRect.right)/2;
	const auto verMid=(windowRect.top+windowRect.bottom)/2;
	_form->SetPlacement({ualNone, {}, {}, {horMid - width / 2, verMid - height / 2, horMid + width / 2, verMid + height / 2}}) | check;
	_form->SetValueAsInt32(AIMPUI_FORM_PROPID_BORDERSTYLE, AIMPUI_FLAGS_BORDERSTYLE_DIALOG) | check;
}

GUIBase::GUIBase()=default;
