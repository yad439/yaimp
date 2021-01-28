//
// Created by YAD on 21.07.2019.
//

#include "ActionManagement.hpp"

#include <fmt/format.h>

#include "IUnknownImpl.hpp"
#include "Utility.hpp"

using namespace std;

class ClickHandler : public IUnknownImpl<IAIMPActionEvent> {
public:
	explicit ClickHandler(function<void(void)> action) : _action{std::move(action)} {}

	void WINAPI OnExecute(IUnknown *) override { if (_action) _action(); }

private:
	const IID &thisIID() override { return IID_IAIMPActionEvent; }

private:
	const function<void(void)> _action;
};

RefCounter<IAIMPAction>
createAction(IAIMPCore *core, const std::wstring_view &name, const function<void(void)> &handle) {
	auto action = createObject(IAIMPAction, core);

	action->SetValueAsObject(AIMP_ACTION_PROPID_ID, createString(core, fmt::format(L"YaimpAction{}", name))) | check;
	action->SetValueAsObject(AIMP_ACTION_PROPID_GROUPNAME, createString(core, L"Yaimp")) | check;
	action->SetValueAsObject(AIMP_ACTION_PROPID_EVENT, makeRefCounter<ClickHandler>(handle)) | check;

	checkResult(core->RegisterExtension(IID_IAIMPServiceActionManager, action));
	return action;
}
