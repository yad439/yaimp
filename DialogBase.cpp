//
// Created by YAD on 30.08.2019.
//

#include "DialogBase.hpp"

#include "Utility.hpp"

DialogBase::DialogBase(IAIMPCore* core,LONG width, LONG height, const std::wstring_view &name):GUIBase{core,width,height,name}  {
	const auto uiService=queryObject(IAIMPServiceUI, core);

	const auto buttonPanel=createChild(Panel, _form, uiService);
	buttonPanel->SetPlacement({ualBottom,{0,0,0,8},{}, {0,0,0,20}}) | check;
	buttonPanel->SetValueAsInt32(AIMPUI_PANEL_PROPID_BORDERS,0) | check;

	const auto okButton=createChild(Button, buttonPanel,uiService);
	okButton->SetPlacement({ualRight,{0,0,8,0},{},{0,0,60,0}}) | check;
	okButton->SetValueAsObject(AIMPUI_CONTROL_PROPID_NAME,createString(core,L"OkButton")) | check;
	okButton->SetValueAsInt32(AIMPUI_BUTTON_PROPID_MODALRESULT,AIMPUI_FLAGS_MODALRESULT_OK) | check;

	const auto cancelButton=createChild(Button, buttonPanel,uiService);
	cancelButton->SetPlacement({ualRight,{0,0,8,0},{},{0,0,60,0}}) | check;
	cancelButton->SetValueAsObject(AIMPUI_CONTROL_PROPID_NAME,createString(core,L"CancelButton")) | check;
	cancelButton->SetValueAsInt32(AIMPUI_BUTTON_PROPID_MODALRESULT,AIMPUI_FLAGS_MODALRESULT_CANCEL) | check;
}

DialogBase::~DialogBase() noexcept {
	if(GSL_LIKELY(_form)){
		_form->Release(false);
		static_cast<IUnknown*>(_form)->Release();
	}
}