//
// Created by YAD on 22.07.2019.
//

#ifndef YAIMP_EXCEPTIONS_HPP
#define YAIMP_EXCEPTIONS_HPP

#include <stdexcept>

#include <windows.h>

struct AimpError:public std::runtime_error {

	AimpError(const char *string, HRESULT result);

	explicit AimpError(HRESULT result);

//	[[nodiscard]] HRESULT getResult() const;

private:
	const HRESULT _result;
};

struct AimpNewObjectError:public AimpError{
	AimpNewObjectError(const char *string, const IID &iid, HRESULT result);

//	[[nodiscard]] const IID &getIid() const;
private:
	const IID _iid;
};

struct AimpCreationError:public AimpNewObjectError{
	AimpCreationError(const IID &iid, HRESULT creationResult);
};

struct AimpQueryError:public AimpNewObjectError{
	AimpQueryError(const IID &iid, HRESULT creationResult);
};

struct HttpError:public std::runtime_error{
	explicit HttpError(unsigned short code);
private:
	const unsigned short _code;

	static_assert(sizeof(_code)==2);
};

#endif //YAIMP_EXCEPTIONS_HPP
