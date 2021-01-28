//
// Created by YAD on 21.07.2019.
//

#ifndef YAIMP_ACTIONMANAGEMENT_HPP
#define YAIMP_ACTIONMANAGEMENT_HPP

#include <SDK/apiActions.h>
#include <SDK/apiCore.h>

#include <string>
#include <functional>

#include "RefCounter.hpp"

RefCounter<IAIMPAction> createAction(IAIMPCore* core, const std::wstring_view& name, const std::function<void(void)> &handle);

#endif //YAIMP_ACTIONMANAGEMENT_HPP
