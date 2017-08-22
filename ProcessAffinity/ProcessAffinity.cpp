// ThreadAffinity.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "Windows.h"
#include <iostream>
#include <thread>
#include <strsafe.h>

void func()
{
	int i = 0;
	for (;;)
	{
		i++;
	}
}

void ErrorExit(LPTSTR lpszFunction)
{
	// Retrieve the system error message for the last-error code

	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	DWORD dw = GetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);

	// Display the error message and exit the process

	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
		(lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
	StringCchPrintf((LPTSTR)lpDisplayBuf,
		LocalSize(lpDisplayBuf) / sizeof(TCHAR),
		TEXT("%s failed with error %d: %s"),
		lpszFunction, dw, lpMsgBuf);
	MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
	//ExitProcess(dw);
}

int main()
{
	HANDLE hProcess = GetCurrentProcess();
	unsigned long lProcessAffinityMask = 0;
	unsigned long lSystemAffinityMask = 0;
	int n = 0;
	int p = 0;
	std::thread t1(&func);
	std::thread t2(&func);

	fprintf(stderr, "please press key: s, g, p or x \n");
	fprintf(stderr, "s - SetProcessAffinityMask\n");
	fprintf(stderr, "g - GetProcessAffinityMask\n");
	fprintf(stderr, "p - SetPriorityClass\n");
	fprintf(stderr, "x - Exit\n");

	char key = std::getchar();

	while (key != 'x')
	{
		unsigned char mask = 1;
		DWORD dwPriClass;
		switch (key)
		{
		case 's':
			lProcessAffinityMask = mask << (n++>8?n=1:n);
			if (SetProcessAffinityMask(hProcess, lProcessAffinityMask))
			{
				fprintf(stderr, "SetProcessAffinityMask success %ld\n", lProcessAffinityMask);
			}
			else
			{
				fprintf(stderr, "SetProcessAffinityMask failed %ld\n", lProcessAffinityMask);
				ErrorExit(L"SetProcessAffinityMask");
			}
		
			break;
		case 'g':

			if (GetProcessAffinityMask(hProcess, &lProcessAffinityMask, &lSystemAffinityMask))
			{
				fprintf(stderr,"%ld,%ld ", lProcessAffinityMask, lSystemAffinityMask);
			}
			else
			{
				fprintf(stderr,"GetProcessAffinityMask failed\n");
				ErrorExit(L"GetProcessAffinityMask");
			}

			dwPriClass = GetPriorityClass(hProcess);

			_tprintf(TEXT("Current priority class is 0x%x %s\n"), dwPriClass, dwPriClass== NORMAL_PRIORITY_CLASS?L"NORMAL_PRIORITY_CLASS":L"HIGH_PRIORITY_CLASS");

			break;
		case 'p':
			p++;
			if (!SetPriorityClass(hProcess, p % 2 ? NORMAL_PRIORITY_CLASS : HIGH_PRIORITY_CLASS))
			{
				_tprintf(TEXT("Failed to end background mode (%d)\n"), GetLastError());
			}
			else
			{
				fprintf(stderr, "SetPriorityClass success %s\n", p % 2 ? "NORMAL_PRIORITY_CLASS" : "HIGH_PRIORITY_CLASS");
			}

			break;
		default:
			break;
		}

		key = std::getchar();
	}

	t1.detach();
	t2.detach();

    return 0;
}

