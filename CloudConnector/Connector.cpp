#include "Connector.h"
using namespace std;
#define BUFSIZE 4096 

namespace Connectors
{
	int Connector::Connect()
	{
		HANDLE g_hChildStd_IN_Rd = NULL;
		HANDLE g_hChildStd_IN_Wr = NULL;
		HANDLE g_hChildStd_OUT_Rd = NULL;
		HANDLE g_hChildStd_OUT_Wr = NULL;
		// additional information
		SECURITY_ATTRIBUTES saAttr;
		saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
		saAttr.bInheritHandle = TRUE;
		saAttr.lpSecurityDescriptor = NULL;
		if (!CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0))
			return -1;
		// Ensure the read handle to the pipe for STDOUT is not inherited.
		if (!SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0))
			return -1;
		// Create a pipe for the child process's STDIN. 
		if (!CreatePipe(&g_hChildStd_IN_Rd, &g_hChildStd_IN_Wr, &saAttr, 0))
			return -1;
		// Ensure the write handle to the pipe for STDIN is not inherited. 
		if (!SetHandleInformation(g_hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0))
			return -1;
		STARTUPINFO si;
		PROCESS_INFORMATION pi;

		// set the size of the structures
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));

		// start the program up
		CreateProcess(TEXT("C:\\Users\\Robert\\Documents\\Visual Studio 2015\\Projects\\ConsoleApplication4\\Release\\ConsoleApplication4.exe"),   // the path
			NULL,        // Command line
			NULL,           // Process handle not inheritable
			NULL,           // Thread handle not inheritable
			FALSE,          // Set handle inheritance to FALSE
			0,              // No creation flags
			NULL,           // Use parent's environment block
			NULL,           // Use parent's starting directory 
			&si,            // Pointer to STARTUPINFO structure
			&pi             // Pointer to PROCESS_INFORMATION structure (removed extra parentheses)
		);
		// Close process and thread handles. 
		
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		return 0;
	}
}