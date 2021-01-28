//
// Created by YAD on 17.07.2019.
//

#ifndef YAIMP_ADDURLDIALOG_HPP
#define YAIMP_ADDURLDIALOG_HPP

#include <SDK/apiGUI.h>
#include <SDK/apiCore.h>

#include <optional>

#include "RefCounter.hpp"
#include "DialogBase.hpp"

class AddUrlDialog: DialogBase {
public:
	explicit AddUrlDialog(IAIMPCore* core);

	[[nodiscard]] std::optional<RefCounter<IAIMPString>> show() const;

private:
	RefCounter<IAIMPUIEdit> _urlEdit;
};


#endif //YAIMP_ADDURLDIALOG_HPP
