#ifndef YAIMP_IUNKNOWNIMPL_HPP
#define YAIMP_IUNKNOWNIMPL_HPP

#include <unknwn.h>
#include <type_traits>

template<class Base>
		requires std::is_base_of<IUnknown,Base>::value
class IUnknownImpl : public Base {
public:
	ULONG WINAPI AddRef() override;

	ULONG WINAPI Release() override;

	[[maybe_unused]] HRESULT WINAPI QueryInterface(REFIID riid, LPVOID *ppvObj) override;

	virtual ~IUnknownImpl();

private:
	virtual REFIID thisIID()=0;

	ULONG _refCounter{1};
};

#endif//YAIMP_IUNKNOWNIMPL_HPP