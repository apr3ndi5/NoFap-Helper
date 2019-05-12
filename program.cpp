
#include <iostream>
#include <fstream>
#include <string>
#include <Windows.h>
#include <vector>
#include <msclr\marshal_cppstd.h>

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "ntdll.lib")

using namespace System;
typedef msclr::interop::marshal_context MCONTEXT;

#pragma region Functions

BOOL RegWriteAppToRegister() 
{
	try
	{
		MCONTEXT mc;
		Microsoft::Win32::Registry::LocalMachine->OpenSubKey(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", true)->SetValue(System::Windows::Forms::Application::ProductName, System::Windows::Forms::Application::ExecutablePath);
		return TRUE;
	}
	catch (const std::exception&)
	{
		return FALSE;
	}
}

std::vector<std::wstring> read_l_s(_In_ const wchar_t* filename, _In_ short lines)
{
	std::wifstream ifs(filename);

	std::vector<std::wstring> pwstr(lines);

	short i = 0;
	while (!ifs.eof())
	{
		ifs >> pwstr[i];
		i++;
	}

	return pwstr;
}

BOOL CALLBACK WindowsCallback(HWND hWnd, LPARAM lParam)
{
	WCHAR WindowTitle[USHRT_MAX];
	GetWindowTextW(hWnd, WindowTitle, USHRT_MAX);

	std::wstring stdWindowTitle = L"";

	stdWindowTitle = (&WindowTitle[0]);

	std::vector<std::wstring>& pTitles = *reinterpret_cast<std::vector<std::wstring>*>(lParam);

	pTitles.push_back(stdWindowTitle);

	return TRUE;
}

#pragma endregion

//default keys
std::vector<std::wstring> pBadWindowTitles = {L"xxx", L"porn", L"xvideo", "pornhub", "sexy", "boobs", "pussy", "porno"};

int start() 
{
	RegWriteAppToRegister();

	MCONTEXT mc;

	for (std::wstring TxtFileKeyword : read_l_s(L"keywords.txt", 1000))
	{
		if (TxtFileKeyword != L"")
		{
			pBadWindowTitles.push_back(TxtFileKeyword);
		}
	}

	std::vector<std::wstring> pWindowsTitles = {};
	DWORD cPID = GetCurrentProcessId();

	while (TRUE)
	{
		Sleep(2000);

		SYSTEMTIME st = {}; 
		GetSystemTime(&st);

		if (st.wSecond == 20 || st.wSecond == 40 || st.wSecond == 00)
		{
			System::Windows::Forms::Application::Restart();
		
			for each (System::Diagnostics::Process^ Proc in System::Diagnostics::Process::GetProcesses())
			{
				if (Proc->ProcessName == L"NoFap Helper")
				{
					if (Proc->Id == cPID)
					{
						Proc->Kill();
					}
				}
			}
		}

		EnumWindows(WindowsCallback, reinterpret_cast<LPARAM>(&pWindowsTitles));

		for (const std::wstring &WindowTitle : pWindowsTitles)
		{
			if (WindowTitle != L"")
			{
				HWND  hWnd = FindWindowW(nullptr, WindowTitle.c_str());
				DWORD PID = 0;
				GetWindowThreadProcessId(hWnd, &PID);

				for (std::wstring BadWindowTitle : pBadWindowTitles)
				{
					String^ BadWT = gcnew String(BadWindowTitle.c_str());
					String^ mWT = gcnew String(WindowTitle.c_str());

					BadWT = BadWT->ToLower();
					mWT = mWT->ToLower();

					if (mWT->Contains(BadWT))
					{
						if (PID != cPID)
						{
							HANDLE hBadProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
							TerminateProcess(hBadProcess, 0);
						}
					}
				}
			}
		}
	}

	return 0;
}
