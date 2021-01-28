//
// Created by YAD on 30.08.2019.
//

#ifndef YAIMP_DIALOGBASE_HPP
#define YAIMP_DIALOGBASE_HPP

#include "GUIBase.hpp"

class DialogBase:protected GUIBase {
protected:
	DialogBase(IAIMPCore* core,LONG width, LONG height, const std::wstring_view &name);

	~DialogBase() noexcept;
};


#endif //YAIMP_DIALOGBASE_HPP
