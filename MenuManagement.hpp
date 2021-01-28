//
// Created by YAD on 21.07.2019.
//

#ifndef YAIMP_MENUMANAGEMENT_HPP
#define YAIMP_MENUMANAGEMENT_HPP

#include <SDK/apiMenu.h>
#include <SDK/apiCore.h>

#include <string_view>
#include <functional>

#include "RefCounter.hpp"

RefCounter<IAIMPMenuItem> createMenu(IAIMPCore* core, IAIMPMenuItem *parent, const std::wstring_view &name, const std::function<void(void)>& action);
RefCounter<IAIMPMenuItem> createMenu(IAIMPCore* core, int                              parentId, const std::wstring_view &name, const std::function<void(void)>& action);

#endif //YAIMP_MENUMANAGEMENT_HPP
