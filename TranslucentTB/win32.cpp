﻿#include "win32.hpp"
#include "arch.h"
#include <optional>
#include <PathCch.h>
#include <processthreadsapi.h>
#include <shellapi.h>
#include <ShlObj.h>
#include <synchapi.h>
#include <utility>
#include <WinBase.h>
#include <winerror.h>
#include <winnt.h>
#include <winrt/base.h>

#include "autofree.hpp"
#include "../CPicker/ccolourpicker.hpp"
#include "clipboardcontext.hpp"
#include "common.hpp"
#include "ttberror.hpp"
#include "window.hpp"

const user32::pSetWindowCompositionAttribute user32::SetWindowCompositionAttribute =
	reinterpret_cast<pSetWindowCompositionAttribute>(
		GetProcAddress(GetModuleHandle(L"user32.dll"), "SetWindowCompositionAttribute")
	);

std::wstring win32::m_ExeLocation;
std::mutex win32::m_PickerThreadsLock;
std::vector<DWORD> win32::m_PickerThreads;

DWORD win32::PickerThreadProc(LPVOID data)
{
	const HRESULT hr = CColourPicker(*reinterpret_cast<uint32_t *>(data)).CreateColourPicker();
	const DWORD tid = GetCurrentThreadId();
	{
		std::lock_guard guard(m_PickerThreadsLock);
		for (DWORD &i_tid : m_PickerThreads)
		{
			if (i_tid == tid)
			{
				std::swap(i_tid, m_PickerThreads.back());
				m_PickerThreads.pop_back();
				break;
			}
		}
	}
	if (FAILED(hr))
	{
		ErrorHandle(hr, Error::Level::Error, L"Color picker 发生错误！");
	}

	return 0;
}

BOOL win32::EnumThreadWindowsProc(HWND hwnd, LPARAM lParam)
{
	Window wnd(hwnd);
	bool &needs_wait = *reinterpret_cast<bool *>(lParam);

	if (*wnd.title() == L"Color Picker")
	{
		// 1068 == IDB_CANCEL
		wnd.send_message(WM_COMMAND, MAKEWPARAM(1068, BN_CLICKED));

		needs_wait = true;
		return false;
	}

	return true;
}

const std::wstring &win32::GetExeLocation()
{
	if (m_ExeLocation.empty())
	{
		DWORD exeLocation_size = LONG_PATH;
		std::wstring exeLocation;
		exeLocation.resize(exeLocation_size);
		if (QueryFullProcessImageName(GetCurrentProcess(), 0, exeLocation.data(), &exeLocation_size))
		{
			exeLocation.resize(exeLocation_size);
			m_ExeLocation = std::move(exeLocation);
		}
		else
		{
			LastErrorHandle(Error::Level::Fatal, L"无法确定可执行文件的位置！");
		}
	}

	return m_ExeLocation;
}

bool win32::IsAtLeastBuild(const uint32_t &buildNumber)
{
	OSVERSIONINFOEX versionInfo = { sizeof(versionInfo), 10, 0, buildNumber };

	DWORDLONG mask = 0;
	VER_SET_CONDITION(mask, VER_MAJORVERSION, VER_GREATER_EQUAL);
	VER_SET_CONDITION(mask, VER_MINORVERSION, VER_GREATER_EQUAL);
	VER_SET_CONDITION(mask, VER_BUILDNUMBER, VER_GREATER_EQUAL);

	if (VerifyVersionInfo(&versionInfo, VER_MAJORVERSION | VER_MINORVERSION | VER_BUILDNUMBER, mask))
	{
		return true;
	}
	else
	{
		DWORD error = GetLastError();
		if (error != ERROR_OLD_WIN_VERSION)
		{
			ErrorHandle(HRESULT_FROM_WIN32(error), Error::Level::Log, L"获取版本信息时出错。");
		}

		return false;
	}
}

bool win32::IsSingleInstance()
{
	static winrt::handle mutex;

	if (!mutex)
	{
		mutex.attach(CreateMutex(NULL, FALSE, ID));
		LRESULT error = GetLastError();
		switch (error)
		{
		case ERROR_ALREADY_EXISTS:
			return false;
			break;

		case ERROR_SUCCESS:
			return true;
			break;

		default:
			ErrorHandle(HRESULT_FROM_WIN32(error), Error::Level::Error, L"无法打开应用互斥锁！");
			return true;
		}
	}
	else
	{
		return true;
	}
}

bool win32::IsDirectory(const std::wstring &directory)
{
	DWORD attributes = GetFileAttributes(directory.c_str());
	if (attributes == INVALID_FILE_ATTRIBUTES)
	{
		DWORD error = GetLastError();
		if (error != ERROR_FILE_NOT_FOUND && error != ERROR_PATH_NOT_FOUND)
		{
			// This function gets called during log initialization, so avoid potential recursivity
			ErrorHandle(HRESULT_FROM_WIN32(error), Error::Level::Debug, L"无法检查目录是否存在。");
		}
		return false;
	}
	else
	{
		return attributes & FILE_ATTRIBUTE_DIRECTORY;
	}
}

bool win32::FileExists(const std::wstring &file)
{
	DWORD attributes = GetFileAttributes(file.c_str());
	if (attributes == INVALID_FILE_ATTRIBUTES)
	{
		DWORD error = GetLastError();
		if (error != ERROR_FILE_NOT_FOUND && error != ERROR_PATH_NOT_FOUND)
		{
			ErrorHandle(HRESULT_FROM_WIN32(error), Error::Level::Log, L"无法检查文件是否存在。");
		}
		return false;
	}
	else
	{
		return !(attributes & FILE_ATTRIBUTE_DIRECTORY);
	}
}

void win32::CopyToClipboard(const std::wstring &text)
{
	ClipboardContext context;
	if (!context)
	{
		LastErrorHandle(Error::Level::Error, L"无法打开剪贴板");
		return;
	}

	if (!EmptyClipboard())
	{
		LastErrorHandle(Error::Level::Error, L"无法清空剪贴板。");
		return;
	}

	const size_t text_size = text.length() + 1;
	auto data = AutoFree::Global<wchar_t>::Alloc(text_size);
	if (!data)
	{
		LastErrorHandle(Error::Level::Error, L"无法为剪贴板分配内存。");
		return;
	}

	wcscpy_s(data.get(), text_size, text.c_str());

	if (!SetClipboardData(CF_UNICODETEXT, data.get()))
	{
		LastErrorHandle(Error::Level::Error, L"无法将数据复制到剪贴板。");
		return;
	}
}

void win32::EditFile(const std::wstring &file)
{
	SHELLEXECUTEINFO info = {
		sizeof(info),									// cbSize
		SEE_MASK_CLASSNAME | SEE_MASK_NOCLOSEPROCESS,	// fMask
		NULL,											// hwnd
		L"open",										// lpVerb
		file.c_str(),									// lpFile
		NULL,											// lpParameters
		NULL,											// lpDirectory
		SW_SHOW,										// nShow
		nullptr,										// hInstApp
		nullptr,										// lpIDList
		L"txtfile"										// lpClass
	};

	if (ShellExecuteEx(&info))
	{
		const winrt::handle hprocess(info.hProcess);

		if (WaitForSingleObject(hprocess.get(), INFINITE) == WAIT_FAILED)
		{
			LastErrorHandle(Error::Level::Log, L"无法等待文本编辑器关闭。");
		}
	}
	else
	{
		std::wstring boxbuffer =
			L"打开文件 \"" + file + L"\" 失败。" +
			L"\n\n" + Error::ExceptionFromHRESULT(HRESULT_FROM_WIN32(GetLastError())) +
			L"\n\n复制文件路径到剪贴板？";

		if (MessageBox(Window::NullWindow, boxbuffer.c_str(), NAME L" - Error", MB_ICONWARNING | MB_YESNO | MB_SETFOREGROUND) == IDYES)
		{
			CopyToClipboard(file);
		}
	}
}

void win32::OpenLink(const std::wstring &link)
{
	SHELLEXECUTEINFO info = {
		sizeof(info),							// cbSize
		SEE_MASK_CLASSNAME,						// fMask
		NULL,									// hwnd
		L"open",								// lpVerb
		link.c_str(),							// lpFile
		NULL,									// lpParameters
		NULL,									// lpDirectory
		SW_SHOW,								// nShow
		nullptr,								// hInstApp
		nullptr,								// lpIDList
		link[4] == L's' ? L"https" : L"http"	// lpClass
	};

	if (!ShellExecuteEx(&info))
	{
		std::wstring boxbuffer =
			L"打开链接 \"" + link + L"\" 失败。" +
			L"\n\n" + Error::ExceptionFromHRESULT(HRESULT_FROM_WIN32(GetLastError())) +
			L"\n\n复制链接到剪贴板？";

		if (MessageBox(Window::NullWindow, boxbuffer.c_str(), NAME L" - Error", MB_ICONWARNING | MB_YESNO | MB_SETFOREGROUND) == IDYES)
		{
			CopyToClipboard(link);
		}
	}
}

DWORD win32::PickColor(uint32_t &color)
{
	DWORD threadId;
	const winrt::handle hThread(CreateThread(nullptr, 0, PickerThreadProc, &color, CREATE_SUSPENDED, &threadId));

	if (hThread)
	{
		{
			std::lock_guard guard(m_PickerThreadsLock);
			m_PickerThreads.emplace_back(threadId);
		}

		ResumeThread(hThread.get());
		return threadId;
	}
	else
	{
		LastErrorHandle(Error::Level::Error, L"无法生成 Color picker 线程！");
		return 0;
	}
}

void win32::ClosePickers()
{
	std::unique_lock guard(m_PickerThreadsLock);
	while (m_PickerThreads.size() != 0)
	{
		const DWORD tid = *m_PickerThreads.begin();
		bool needs_wait = false;
		guard.unlock();
		EnumThreadWindows(tid, EnumThreadWindowsProc, reinterpret_cast<LPARAM>(&needs_wait));

		if (needs_wait)
		{
			const winrt::handle thread(OpenThread(SYNCHRONIZE, FALSE, tid));
			if (thread)
			{
				WaitForSingleObject(thread.get(), INFINITE);
			}
		}

		guard.lock();
	}
}

void win32::HardenProcess()
{
	PROCESS_MITIGATION_ASLR_POLICY aslr_policy;
	if (GetProcessMitigationPolicy(GetCurrentProcess(), ProcessASLRPolicy, &aslr_policy, sizeof(aslr_policy)))
	{
		aslr_policy.EnableForceRelocateImages = true;
		aslr_policy.DisallowStrippedImages = true;
		if (!SetProcessMitigationPolicy(ProcessASLRPolicy, &aslr_policy, sizeof(aslr_policy)))
		{
			LastErrorHandle(Error::Level::Log, L"无法删除图像。");
		}
	}
	else
	{
		LastErrorHandle(Error::Level::Log, L"无法获取当前的 ASLR 策略。");
	}

	PROCESS_MITIGATION_DYNAMIC_CODE_POLICY code_policy {};
	code_policy.ProhibitDynamicCode = true;
	code_policy.AllowThreadOptOut = false;
	code_policy.AllowRemoteDowngrade = false;
	if (!SetProcessMitigationPolicy(ProcessDynamicCodePolicy, &code_policy, sizeof(code_policy)))
	{
		LastErrorHandle(Error::Level::Log, L"无法禁用动态代码生成。");
	}

	PROCESS_MITIGATION_STRICT_HANDLE_CHECK_POLICY handle_policy {};
	handle_policy.RaiseExceptionOnInvalidHandleReference = true;
	handle_policy.HandleExceptionsPermanentlyEnabled = true;
	if (!SetProcessMitigationPolicy(ProcessStrictHandleCheckPolicy, &handle_policy, sizeof(handle_policy)))
	{
		LastErrorHandle(Error::Level::Log, L"无法启用严格的句柄检查。");
	}

	PROCESS_MITIGATION_EXTENSION_POINT_DISABLE_POLICY extension_policy {};
	extension_policy.DisableExtensionPoints = true;
	if (!SetProcessMitigationPolicy(ProcessExtensionPointDisablePolicy, &extension_policy, sizeof(extension_policy)))
	{
		LastErrorHandle(Error::Level::Log, L"无法禁用扩展点 DLL。");
	}

	PROCESS_MITIGATION_BINARY_SIGNATURE_POLICY signature_policy {};
	signature_policy.MitigationOptIn = true;
	if (!SetProcessMitigationPolicy(ProcessSignaturePolicy, &signature_policy, sizeof(signature_policy)))
	{
		LastErrorHandle(Error::Level::Log, L"无法启用图像强制签名。");
	}


	PROCESS_MITIGATION_IMAGE_LOAD_POLICY load_policy {};
	load_policy.NoLowMandatoryLabelImages = true;
	load_policy.PreferSystem32Images = true;

	// https://blogs.msdn.microsoft.com/oldnewthing/20160602-00/?p=93556
	std::vector<wchar_t> volumePath(LONG_PATH);
	if (GetVolumePathName(GetExeLocation().c_str(), volumePath.data(), LONG_PATH))
	{
		load_policy.NoRemoteImages = GetDriveType(volumePath.data()) != DRIVE_REMOTE;
	}
	else
	{
		LastErrorHandle(Error::Level::Log, L"无法获取卷路径名。");
	}

	if (!SetProcessMitigationPolicy(ProcessImageLoadPolicy, &load_policy, sizeof(load_policy)))
	{
		LastErrorHandle(Error::Level::Log, L"无法设置图片加载策略。");
	}
}

std::wstring win32::CharToWchar(const char *const str)
{
	const size_t strLength = std::char_traits<char>::length(str);
	std::wstring strW;
	strW.resize(strLength);
	int count = MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED | MB_ERR_INVALID_CHARS, str, strLength, strW.data(), strLength);
	if (count)
	{
		strW.resize(count);
	}
	else
	{
		strW.erase();
	}

	return strW;
}