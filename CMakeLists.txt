cmake_minimum_required(VERSION 3.12)
project(yaimp VERSION 0.4.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

set(JSON_ImplicitConversions OFF)
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_library(yaimp MODULE)
set_target_properties(yaimp PROPERTIES PREFIX "")

target_sources(yaimp PRIVATE
               dllmain.cpp
               yaimp.def
               yaimp.rc
               Yaimp.cpp
               Utility.cpp
               MD5.cpp
               IUnknownImpl.cpp
               Exceptions.cpp
               EventListeners.cpp
               ActionManagement.cpp
               MenuManagement.cpp
               AddUrlMenuAction.cpp
               AddTrackedPlaylistAction.cpp
               PlayerHook.cpp
               PlaylistManager.cpp
               HttpClient.cpp
               YandexAPI.cpp
               GUIBase.cpp
               OptionsFrame.cpp
               DialogBase.cpp
               AddUrlDialog.cpp
               LoginDialog.cpp)

target_link_libraries(yaimp PRIVATE ${CONAN_LIBS})

target_include_directories(yaimp PRIVATE "include")

set(YAIMP_UNICODE_CONVERSION_TYPE "UTFlib" CACHE STRING "Unicode conversion method")
if ("${YAIMP_UNICODE_CONVERSION_TYPE}" STREQUAL "Native")
	target_compile_definitions(yaimp PRIVATE YAIMP_UTF_CONVERSION_NATIVE)
elseif("${YAIMP_UNICODE_CONVERSION_TYPE}" STREQUAL "SemiNative")
	target_compile_definitions(yaimp PRIVATE YAIMP_UTF_CONVERSION_SEMI_NATIVE)
endif ()

install(TARGETS yaimp DESTINATION "Plugins/yaimp")
install(DIRECTORY Langs DESTINATION "Plugins/yaimp")

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
    target_link_options(yaimp PRIVATE -Wl,--enable-stdcall-fixup)
	target_compile_options(yaimp PRIVATE -Wall -Wextra -Wpedantic -Wno-unused-variable
	                       -Wcast-align
	                       -Wcast-qual
	                       -Wconversion
	                       -Wenum-compare
	                       -Wfloat-equal
	                       -Wwrite-strings
	                       -Winit-self
	                       -Wpointer-arith
	                       -Wstrict-aliasing
	                       -Wformat=2
	                       -Wmissing-declarations
	                       -Wmissing-include-dirs
	                       -Wuninitialized
	                       -Wpointer-arith
	                       -Wstrict-overflow=5
	                       -Winline
	                       -Wundef
	                       -Wshadow
	                       -Wunreachable-code
	                       -Wlogical-op
	                       -Wstrict-aliasing=2
	                       -Wredundant-decls
	                       -Wduplicated-cond
	                       -Wduplicated-branches
	                       -Wrestrict
	                       -Wnull-dereference
	                       -Wdouble-promotion)
	if("${CMAKE_BUILD_TYPE}" MATCHES "Rel")
		target_link_options(yaimp PRIVATE -static)
	endif()
elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "MSVC")
	target_compile_options(yaimp PRIVATE /W4 /analyze)
endif ()

#target_compile_definitions(yaimp PRIVATE JSON_USE_IMPLICIT_CONVERSIONS=0)
if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
	target_compile_definitions(yaimp PRIVATE GSL_THROW_ON_CONTRACT_VIOLATION)
	target_compile_definitions(yaimp PRIVATE VB_ASSERT_THROW)
	target_compile_definitions(yaimp PRIVATE DEBUG)
elseif("${CMAKE_BUILD_TYPE}" MATCHES "Rel")
	target_compile_definitions(yaimp PRIVATE GSL_UNENFORCED_ON_CONTRACT_VIOLATION)
	target_compile_definitions(yaimp PRIVATE VB_ASSERT_IGNORE)
endif()
