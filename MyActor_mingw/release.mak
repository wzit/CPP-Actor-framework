#Generated by VisualGDB (http://visualgdb.com)
#DO NOT EDIT THIS FILE MANUALLY UNLESS YOU ABSOLUTELY NEED TO
#USE VISUALGDB PROJECT PROPERTIES DIALOG INSTEAD

BINARYDIR := ..\x64\Release

#Toolchain
CC := C:/mingw64/x86_64-4.9.3-release-posix-seh-rt_v4-rev1/mingw64/bin/gcc.exe
CXX := C:/mingw64/x86_64-4.9.3-release-posix-seh-rt_v4-rev1/mingw64/bin/g++.exe
LD := $(CXX)
AR := C:/mingw64/x86_64-4.9.3-release-posix-seh-rt_v4-rev1/mingw64/bin/ar.exe
OBJCOPY := 

#Additional flags
PREPROCESSOR_MACROS := NDEBUG RELEASE WIN32 _WIN64 _WIN32_WINNT=0x0600 ENABLE_NEXT_TICK ENABLE_CHECK_LOST DISABLE_BOOST_TIMER
INCLUDE_DIRS := E:\cpplib\boost\mingw64
LIBRARY_DIRS := E:\cpplib\boost\mingw64\lib_posix_seh_x64
LIBRARY_NAMES := Mswsock Winmm ws2_32 boost_thread-mgw49-mt-s-1_59 boost_system-mgw49-mt-s-1_59 boost_chrono-mgw49-mt-s-1_59
ADDITIONAL_LINKER_INPUTS := 
MACOS_FRAMEWORKS := 
LINUX_PACKAGES := 

CFLAGS := -ffunction-sections -O3
CXXFLAGS := -ffunction-sections -O3 -std=c++11
ASFLAGS := 
LDFLAGS := -Wl,-gc-sections
COMMONFLAGS := 

START_GROUP := -Wl,--start-group
END_GROUP := -Wl,--end-group

#Additional options detected from testing the toolchain
