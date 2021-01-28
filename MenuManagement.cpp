//
// Created by YAD on 21.07.2019.
//

#include "MenuManagement.hpp"

#include <fmt/format.h>

#include "Utility.hpp"
#include "ActionManagement.hpp"

RefCounter<IAIMPMenuItem> createMenu(IAIMPCore *core, IAIMPMenuItem *parent,
                                     const std::wstring_view &name, const std::function<void(void)> &action) {
	const auto item = createObject(IAIMPMenuItem, core);

	item->SetValueAsObject(AIMP_MENUITEM_PROPID_PARENT, parent) | check;
	item->SetValueAsObject(AIMP_MENUITEM_PROPID_ID, createString(core, fmt::format(L"YaimpMenu{}", name)))
	| check;
	if (action)item->SetValueAsObject(AIMP_MENUITEM_PROPID_ACTION, createAction(core, name, action)) | check;

	checkResult(core->RegisterExtension(IID_IAIMPServiceMenuManager, item));
	return item;
}

RefCounter<IAIMPMenuItem> createMenu(IAIMPCore *core, int parentId,
                                     const std::wstring_view &name, const std::function<void(void)> &action) {
	auto menuService = queryObject(IAIMPServiceMenuManager, core);
	IAIMPMenuItem *parent{};
	menuService->GetBuiltIn(parentId, &parent) | check;
	Assert(parent);
	return createMenu(core, RefCounter{parent}, name, action);
}
