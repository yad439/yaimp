//
// Created by YAD on 22.07.2019.
//

#include "Exceptions.hpp"

#include <fmt/format.h>

AimpError::AimpError(const char *string, HRESULT result):std::runtime_error{string},_result{result} {}

AimpError::AimpError(HRESULT result):AimpError{"Aimp generic error.",result}{}

//HRESULT AimpError::getResult() const {return _result;}

AimpNewObjectError::AimpNewObjectError(const char *string, const IID &iid, HRESULT result)
: AimpError(string, result),_iid{iid}{}

//const IID &AimpNewObjectError::getIid() const {return _iid;}

AimpCreationError::AimpCreationError(const IID &iid, HRESULT creationResult)
: AimpNewObjectError("Aimp can't create object.",iid,creationResult) {}

AimpQueryError::AimpQueryError(const IID &iid, HRESULT creationResult)
		: AimpNewObjectError("Aimp can't query object.",iid,creationResult) {}

HttpError::HttpError(unsigned short code) : runtime_error(fmt::format("Server returned {}",code)), _code(code) {}
