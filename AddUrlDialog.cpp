//
// Created by YAD on 17.07.2019.
//

#include "AddUrlDialog.hpp"

#include "Utility.hpp"

AddUrlDialog::AddUrlDialog(IAIMPCore* core):DialogBase{core,300,140,L"AddUrlForm"} {
	const auto uiService=queryObject(IAIMPServiceUI, core);

	const auto mainPanel=createChild(Panel, _form, uiService);
	mainPanel->SetPlacement({ualTop,{0,10,0,0}, {},{0,0,0,40}}) | check;
	mainPanel->SetValueAsInt32(AIMPUI_PANEL_PROPID_BORDERS,0) | check;

	_urlEdit=createChild(Edit, mainPanel, uiService);
	_urlEdit->SetPlacement({ualBottom, {10, 0, 10, 0}, {}, {0, 0, 0, 20}}) | check;

	const auto label=createChild(Label,mainPanel,uiService);
	label->SetPlacement({ualTop,{10,0,0,0},{},{0,0,0,20}}) | check;
	label->SetValueAsObject(AIMPUI_CONTROL_PROPID_NAME,createString(core,L"EnterUrlLabel")) | check;
}

std::optional<RefCounter<IAIMPString>> AddUrlDialog::show() const {
	if(_form->ShowModal() != AIMPUI_FLAGS_MODALRESULT_OK)return std::nullopt;
	IAIMPString* url{};
	_urlEdit->GetValueAsObject(AIMPUI_BASEEDIT_PROPID_TEXT, IID_IAIMPString, pointer_cast<void **>(&url)) | check;
	Assert(url);
	return RefCounter{url};
}
