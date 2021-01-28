//
// Created by YAD on 30.08.2019.
//

#include "LoginDialog.hpp"

#include "Utility.hpp"

LoginDialog::LoginDialog(IAIMPCore* core):DialogBase{core,300,160,L"LoginForm"} {
	const auto uiService=queryObject(IAIMPServiceUI, core);

	const auto loginPanel=createChild(Panel, _form, uiService);
	loginPanel->SetPlacement({ualTop, {0, 10, 0, 0}, {}, {0, 0, 0, 40}}) | check;
	loginPanel->SetValueAsInt32(AIMPUI_PANEL_PROPID_BORDERS, 0) | check;

	const auto loginLabel=createChild(Label, loginPanel, uiService);
	loginLabel->SetPlacement({ualTop,{10,0,0,0},{},{0,0,0,20}}) | check;
	loginLabel->SetValueAsObject(AIMPUI_CONTROL_PROPID_NAME,createString(core,L"EnterLoginLabel")) | check;

	_loginEdit=createChild(Edit, loginPanel, uiService);
	_loginEdit->SetPlacement({ualTop, {10, 0, 10, 0}, {}, {0, 0, 0, 20}}) | check;

	const auto passwordPanel=createChild(Panel, _form, uiService);
	passwordPanel->SetPlacement({ualTop, {0, 0, 0, 0}, {}, {0, 0, 0, 40}}) | check;
	passwordPanel->SetValueAsInt32(AIMPUI_PANEL_PROPID_BORDERS, 0) | check;

	const auto passwordLabel=createChild(Label, passwordPanel, uiService);
	passwordLabel->SetPlacement({ualTop,{10,0,0,0},{},{0,0,0,20}}) | check;
	passwordLabel->SetValueAsObject(AIMPUI_CONTROL_PROPID_NAME,createString(core,L"EnterPasswordLabel")) | check;

	_passwordEdit=createChild(Edit, passwordPanel, uiService);
	_passwordEdit->SetPlacement({ualTop, {10, 0, 10, 0}, {}, {0, 0, 0, 20}}) | check;
	_passwordEdit->SetValueAsInt32(AIMPUI_EDIT_PROPID_PASSWORDCHAR,L'*')|check;
}

std::optional<std::pair<RefCounter<IAIMPString>, RefCounter<IAIMPString>>> LoginDialog::show() const {
	if(_form->ShowModal() != AIMPUI_FLAGS_MODALRESULT_OK)return std::nullopt;
	IAIMPString* login;
	IAIMPString* password;
	_loginEdit->GetValueAsObject(AIMPUI_BASEEDIT_PROPID_TEXT, IID_IAIMPString, pointer_cast<void **>(&login)) | check;
	_passwordEdit->GetValueAsObject(AIMPUI_BASEEDIT_PROPID_TEXT, IID_IAIMPString, pointer_cast<void **>(&password)) | check;
	Assert(login && password);
	return std::pair{RefCounter{login},RefCounter{password}};
}


