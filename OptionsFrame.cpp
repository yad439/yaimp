//
// Created by YAD on 19.07.2019.
//

#include "OptionsFrame.hpp"

#include <SDK/apiMUI.h>

#include <gsl/gsl_util>

#include "Utility.hpp"
#include "LoginDialog.hpp"

using namespace std;

OptionsFrame::OptionsFrame(RefCounter<IAIMPCore> aimpCore,shared_ptr<YandexAPI> yandexAPI)
		: _aimpCore{move(aimpCore)},_yandexAPI{move(yandexAPI)} {}


HRESULT WINAPI OptionsFrame::GetName(IAIMPString **S) {
	auto tabName = createString(_aimpCore, L"Yaimp");
	*S = tabName;
	tabName->AddRef();
	return S_OK;
}

HWND WINAPI OptionsFrame::CreateFrame(HWND ParentWnd) {
	Assert(ParentWnd);
	auto localizationService=queryObject(IAIMPServiceMUI, _aimpCore);
	auto uiService = queryObject(IAIMPServiceUI, _aimpCore);
	uiService->CreateForm(ParentWnd, AIMPUI_SERVICE_CREATEFORM_FLAGS_CHILD, nullptr, nullptr, &_form);
	_form->SetPlacement({ualClient, {}, {}, {}}) | check;
	_form->SetValueAsInt32(AIMPUI_FORM_PROPID_BORDERSTYLE, AIMPUI_FLAGS_BORDERSTYLE_NONE) | check;
	_form->SetValueAsObject(AIMPUI_CONTROL_PROPID_NAME, createString(_aimpCore, L"OptionsFrame"));

	auto mainCategory = createChild(Category, _form, uiService);
	mainCategory->SetPlacement({ualClient,{},{},{}}) | check;
	mainCategory->SetValueAsObject(AIMPUI_CONTROL_PROPID_NAME, createString(_aimpCore, L"YaimpCategory"))
	| check;

	auto loginGroup=createChild(GroupBox,mainCategory,uiService);
	loginGroup->SetPlacement({ualTop, {8, 0, 0, 0}, {}, {0, 0, 0, 50}})|check;
	loginGroup->SetValueAsObject(AIMPUI_CONTROL_PROPID_NAME, createString(_aimpCore, L"LoginGroup")) | check;

	_statusLabel = createChild(Label, loginGroup, uiService);
	_statusLabel->SetPlacement({ualLeft, {0, 8, 0, 0}, {}, {0, 0, 200, 0}}) | check;

	auto buttonPanel=createChild(Panel,mainCategory,uiService);
	buttonPanel->SetPlacement({ualTop, {8, 0, 0, 0}, {}, {0, 0, 0, 20}})|check;
	buttonPanel->SetValueAsInt32(AIMPUI_PANEL_PROPID_BORDERS,0) | check;

	_loginButton=createChildWithEvent(Button,loginGroup,uiService,makeRefCounter<EventHandler>([this,localizationService]{
		LoginDialog dialog{_aimpCore};
		auto data=dialog.show();
		if(data){
			auto username=_yandexAPI->login(convertToString(data->first), convertToString(data->second));
			if(!username.empty()){
				IAIMPString* labelText;
				localizationService->GetValue(createString(_aimpCore, L"Yaimp\\LoggedAsLabel"), &labelText) | check;
				gsl::final_action _{[labelText]{labelText->Release();}};
				labelText->Add(createString(_aimpCore, username)) | check;
				_statusLabel->SetValueAsObject(AIMPUI_LABEL_PROPID_TEXT,labelText)|check;
				_loginButton->SetValueAsInt32(AIMPUI_CONTROL_PROPID_VISIBLE,false)|check;
				_logoutButton->SetValueAsInt32(AIMPUI_CONTROL_PROPID_VISIBLE,true)|check;
			}else{
				showMsgBox(_aimpCore, MB_ICONEXCLAMATION, L"Login error!", L"Incorrect login or password.");
			}
		}
	}));
	_loginButton->SetPlacement({ualRight, {0, 0, 0, 0}, {}, {0, 0, 60,0}})|check;
	_loginButton->SetValueAsObject(AIMPUI_CONTROL_PROPID_NAME, createString(_aimpCore, L"LoginButton")) | check;

	_logoutButton=createChildWithEvent(Button,loginGroup,uiService,makeRefCounter<EventHandler>([this,localizationService]{
		_yandexAPI->logout();
		IAIMPString* labelText;
		localizationService->GetValue(createString(_aimpCore, L"Yaimp\\NotLoggedLabel"), &labelText) | check;
		gsl::final_action _{[labelText]{labelText->Release();}};
		_statusLabel->SetValueAsObject(AIMPUI_LABEL_PROPID_TEXT,labelText)|check;
		_loginButton->SetValueAsInt32(AIMPUI_CONTROL_PROPID_VISIBLE,true)|check;
		_logoutButton->SetValueAsInt32(AIMPUI_CONTROL_PROPID_VISIBLE,false)|check;
	}));
	_logoutButton->SetPlacement({ualRight, {0, 0, 0, 0}, {}, {0, 0, 60, 0}})|check;
	_logoutButton->SetValueAsObject(AIMPUI_CONTROL_PROPID_NAME, createString(_aimpCore, L"LogoutButton")) | check;

	if(auto username=_yandexAPI->profileInfo();username.empty()){
		IAIMPString* labelText;
		localizationService->GetValue(createString(_aimpCore, L"Yaimp\\NotLoggedLabel"), &labelText) | check;
		gsl::final_action _{[labelText]{labelText->Release();}};
		_statusLabel->SetValueAsObject(AIMPUI_LABEL_PROPID_TEXT,labelText)|check;
		_loginButton->SetValueAsInt32(AIMPUI_CONTROL_PROPID_VISIBLE,true)|check;
		_logoutButton->SetValueAsInt32(AIMPUI_CONTROL_PROPID_VISIBLE,false)|check;
	} else{
		IAIMPString* labelText;
		localizationService->GetValue(createString(_aimpCore, L"Yaimp\\LoggedAsLabel"), &labelText) | check;
		gsl::final_action _{[labelText]{labelText->Release();}};
		labelText->Add(createString(_aimpCore, username)) | check;
		_statusLabel->SetValueAsObject(AIMPUI_LABEL_PROPID_TEXT,labelText)|check;
		_loginButton->SetValueAsInt32(AIMPUI_CONTROL_PROPID_VISIBLE,false)|check;
		_logoutButton->SetValueAsInt32(AIMPUI_CONTROL_PROPID_VISIBLE,true)|check;
	}

	return _form->GetHandle();
}

void WINAPI OptionsFrame::DestroyFrame() {
	_loginButton.reset();
	_logoutButton.reset();
	_statusLabel.reset();
	if (_form){
		_form->Release(false);
		static_cast<IUnknown*>(_form)->Release();
	}
}

void WINAPI OptionsFrame::Notification(int) {}

const IID &OptionsFrame::thisIID() {return IID_IAIMPOptionsDialogFrame;}

OptionsFrame::EventHandler::EventHandler(function<void()> handle) : _handle{move(handle)} {}

void WINAPI OptionsFrame::EventHandler::OnChanged(IUnknown *) {if(_handle)_handle();}

const IID &OptionsFrame::EventHandler::thisIID() {return IID_IAIMPUIChangeEvents;}
