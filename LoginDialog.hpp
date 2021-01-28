//
// Created by YAD on 30.08.2019.
//

#ifndef YAIMP_LOGINDIALOG_HPP
#define YAIMP_LOGINDIALOG_HPP

#include <optional>

#include "DialogBase.hpp"
#include "YandexAPI.hpp"

class LoginDialog: DialogBase {
public:
	explicit LoginDialog(IAIMPCore* core);

	[[nodiscard]] std::optional<std::pair<RefCounter<IAIMPString>,RefCounter<IAIMPString>>> show()const;
private:
	RefCounter<IAIMPUIEdit> _loginEdit;
	RefCounter<IAIMPUIEdit> _passwordEdit;
};


#endif //YAIMP_LOGINDIALOG_HPP
