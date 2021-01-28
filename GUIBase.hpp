//
// Created by YAD on 19.07.2019.
//

#ifndef YAIMP_GUIBASE_HPP
#define YAIMP_GUIBASE_HPP

#include <SDK/apiGUI.h>
#include <SDK/apiCore.h>

#include <string_view>

class GUIBase {
protected:
public:
	GUIBase();

protected:

	GUIBase(IAIMPCore *core, LONG width, LONG height, const std::wstring_view &name);

	[[nodiscard]] void* createChildI(IAIMPServiceUI* service, IAIMPUIWinControl *parent, IAIMPString *name, IUnknown* eventsHandler, REFIID iid) const;

	IAIMPUIForm* _form{};
};

#define createChildWithEvent(type,parent,service,handler) RefCounter{static_cast<IAIMPUI##type*>(createChildI(service,parent,nullptr,handler,IID_IAIMPUI##type))}
#define createChild(type,parent,service) createChildWithEvent(type,parent,service,nullptr)

#endif //YAIMP_GUIBASE_HPP
