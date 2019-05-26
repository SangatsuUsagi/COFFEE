cmake_minimum_required(VERSION 3.1)

set(PROJECT_NAME sample_util)
project(${PROJECT_NAME})

# Common settings
include(common)
include(path_include)

if(APPLE)
	add_definitions( -DNO_TCMALLOC -DFULL_SAFE_BROWSING -DSAFE_BROWSING_CSD -DSAFE_BROWSING_DB_LOCAL -DCHROMIUM_BUILD -D_LIBCPP_HAS_NO_ALIGNED_ALLOCATION -DCR_XCODE_VERSION=1020 -DCR_CLANG_REVISION=\"352138-3\" -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -DCOMPONENT_BUILD -D__ASSERT_MACROS_DEFINE_VERSIONS_WITHOUT_UNDERSCORE=0 -D_DEBUG -DDYNAMIC_ANNOTATIONS_ENABLED=1 -DWTF_USE_DYNAMIC_ANNOTATIONS=1 -DANGLE_IS_64_BIT_CPU -DANGLE_EGL_LIBRARY_NAME=\"libEGL\" -DANGLE_GLESV2_LIBRARY_NAME=\"libGLESv2\" -DGL_GLES_PROTOTYPES=0 -DEGL_EGL_PROTOTYPES=0 -DANGLE_USE_UTIL_LOADER )
	set(CMAKE_C_FLAGS " -fno-strict-aliasing -fstack-protector-strong -fcolor-diagnostics -fmerge-all-constants -Xclang -mllvm -Xclang -instcombine-lower-dbg-declare=0 -arch x86_64 -Wno-builtin-macro-redefined -D__DATE__= -D__TIME__= -D__TIMESTAMP__= -no-canonical-prefixes -Wall -Werror -Wextra -Wimplicit-fallthrough -Wthread-safety -Wextra-semi -Wunguarded-availability -Wno-missing-field-initializers -Wno-unused-parameter -Wno-c++11-narrowing -Wno-unneeded-internal-declaration -Wno-undefined-var-template -Wno-ignored-pragma-optimize -mmacosx-version-min=10.10.0 -fvisibility=hidden -Wheader-hygiene -Wstring-conversion -Wtautological-overlap-compare -Wextra-semi -Winconsistent-missing-override -Wnon-virtual-dtor -Wunneeded-internal-declaration ")
	set(CMAKE_C_FLAGS_DEBUG   " -O0 -fno-omit-frame-pointer -g2 ")
	set(CMAKE_C_FLAGS_RELEASE " -O3 -fomit-frame-pointer ")
	set(CMAKE_CXX_FLAGS " -std=c++17 -Wno-undefined-bool-conversion -Wno-tautological-undefined-compare -stdlib=libc++ -fno-exceptions -fno-rtti -fvisibility-inlines-hidden ")

elseif(UNIX AND NOT APPLE)
	add_definitions( -DUSE_UDEV -DUSE_AURA=1 -DUSE_GLIB=1 -DUSE_NSS_CERTS=1 -DUSE_X11=1 -DFULL_SAFE_BROWSING -DSAFE_BROWSING_CSD -DSAFE_BROWSING_DB_LOCAL -DCHROMIUM_BUILD -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_GNU_SOURCE -DCR_CLANG_REVISION=\"352138-3\" -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -DCOMPONENT_BUILD -D_DEBUG -DDYNAMIC_ANNOTATIONS_ENABLED=1 -DWTF_USE_DYNAMIC_ANNOTATIONS=1 -DANGLE_IS_64_BIT_CPU -DANGLE_EGL_LIBRARY_NAME=\"libEGL\" -DANGLE_GLESV2_LIBRARY_NAME=\"libGLESv2\" -DGL_GLES_PROTOTYPES=0 -DEGL_EGL_PROTOTYPES=0 -DANGLE_USE_UTIL_LOADER )
	set(CMAKE_C_FLAGS " -fno-strict-aliasing --param=ssp-buffer-size=4 -fstack-protector -funwind-tables -fPIC -pthread -fcolor-diagnostics -fmerge-all-constants -Xclang -mllvm -Xclang -instcombine-lower-dbg-declare=0 -m64 -march=x86-64 -Wno-builtin-macro-redefined -D__DATE__= -D__TIME__= -D__TIMESTAMP__= -no-canonical-prefixes -Wall -Werror -Wextra -Wimplicit-fallthrough -Wthread-safety -Wextra-semi -Wno-missing-field-initializers -Wno-unused-parameter -Wno-c++11-narrowing -Wno-unneeded-internal-declaration -Wno-undefined-var-template -Wno-ignored-pragma-optimize -fvisibility=hidden -Wheader-hygiene -Wstring-conversion -Wtautological-overlap-compare -Wextra-semi -Winconsistent-missing-override -Wnon-virtual-dtor -Wunneeded-internal-declaration ")
	set(CMAKE_C_FLAGS_DEBUG   " -O0 -fno-omit-frame-pointer -g2 -gsplit-dwarf -ggnu-pubnames ")
	set(CMAKE_C_FLAGS_RELEASE " -O3 -fomit-frame-pointer ")
	set(CMAKE_CXX_FLAGS " -Wno-undefined-bool-conversion -Wno-tautological-undefined-compare -std=c++14 -fno-exceptions -fno-rtti -fvisibility-inlines-hidden")

elseif(MSVC OR MSYS OR MINGW)
	add_definitions( -DLIBANGLE_UTIL_IMPLEMENTATION -DUSE_AURA=1 -DNO_TCMALLOC -DFULL_SAFE_BROWSING -DSAFE_BROWSING_CSD -DSAFE_BROWSING_DB_LOCAL -DCHROMIUM_BUILD "-DCR_CLANG_REVISION=\"352138-3\"" -D_HAS_NODISCARD -D_HAS_EXCEPTIONS=0 -DCOMPONENT_BUILD -D__STD_C -D_CRT_RAND_S -D_CRT_SECURE_NO_DEPRECATE -D_SCL_SECURE_NO_DEPRECATE -D_ATL_NO_OPENGL -D_WINDOWS -DCERT_CHAIN_PARA_HAS_EXTRA_FIELDS -DPSAPI_VERSION=2 -DWIN32 -D_SECURE_ATL -D_USING_V110_SDK71_ -DWINAPI_FAMILY=WINAPI_FAMILY_DESKTOP_APP -DWIN32_LEAN_AND_MEAN -DNOMINMAX -D_UNICODE -DUNICODE -DNTDDI_VERSION=0x0A000003 -D_WIN32_WINNT=0x0A00 -DWINVER=0x0A00 -D_DEBUG -DDYNAMIC_ANNOTATIONS_ENABLED=1 -DWTF_USE_DYNAMIC_ANNOTATIONS=1 -D_HAS_ITERATOR_DEBUGGING=0 -DANGLE_IS_64_BIT_CPU "-DANGLE_EGL_LIBRARY_NAME=\"libEGL\"" "-DANGLE_GLESV2_LIBRARY_NAME=\"libGLESv2\"" -DGL_GLES_PROTOTYPES=0 -DEGL_EGL_PROTOTYPES=0 -DANGLE_USE_UTIL_LOADER )
	set(CMAKE_C_FLAGS " /utf-8 /X -fcolor-diagnostics -fmerge-all-constants -Xclang -mllvm -Xclang -instcombine-lower-dbg-declare=0 -fcomplete-member-pointers /Gy /FS /bigobj /d2FastFail /Zc$:sizedDealloc- -fmsc-version=1911 /Zc$:dllexportInlines- -m64 -fansi-escape-codes /Brepro -D__DATE__= -D__TIME__= -D__TIMESTAMP__= -Xclang -fdebug-compilation-dir -Xclang . -no-canonical-prefixes /W4 /WX /wd4091 /wd4127 /wd4251 /wd4275 /wd4312 /wd4324 /wd4351 /wd4355 /wd4503 /wd4589 /wd4611 /wd4100 /wd4121 /wd4244 /wd4505 /wd4510 /wd4512 /wd4610 /wd4838 /wd4995 /wd4996 /wd4456 /wd4457 /wd4458 /wd4459 /wd4200 /wd4201 /wd4204 /wd4221 /wd4245 /wd4267 /wd4305 /wd4389 /wd4702 /wd4701 /wd4703 /wd4661 /wd4706 /wd4715 /wd4702 /Od /Ob0 /GF /Z7 -fno-standalone-debug /MDd /we4244 /we4456 /we4458 /we4800 /we4838 ")
	set(CMAKE_CXX_FLAGS " /std:c++17 /TP /wd4577 /GR- ")
endif()
add_library(${PROJECT_NAME} ${LIB_TYPE} SampleApplication.cpp texture_utils.cpp tga_utils.cpp)
target_link_libraries(${PROJECT_NAME} angle_util)
