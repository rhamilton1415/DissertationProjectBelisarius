#include "Connector.h"
using namespace std;
#define BUFSIZE 4096 

namespace Connectors
{
	//stolen from stackoverflow
	template <typename Map>
	bool map_compare(Map const &lhs, Map const &rhs)
	{
		// No predicate needed because there is operator== for pairs already.
		return lhs.size() == rhs.size() && std::equal(lhs.begin(), lhs.end(), rhs.begin());
	}

	void Connector::updateState(std::map<BWAPI::UnitType, int> inQueued, std::map<BWAPI::UnitType, int> inPlayerState)
	{
		if (!map_compare(queued, inQueued) || !map_compare(queued, inQueued))
		{
			queued = inQueued;
			playerState = inPlayerState;
			Connect(queued, playerState);
		}
	}


	int Connector::Connect(std::map<BWAPI::UnitType, int> queued, std::map<BWAPI::UnitType, int> playerState)
	{
		ofstream inputValues;
		SECURITY_ATTRIBUTES sa;
		sa.nLength = sizeof(sa);
		sa.lpSecurityDescriptor = NULL;
		sa.bInheritHandle = TRUE;
		HANDLE h = CreateFile(TEXT("in.txt"),
			FILE_APPEND_DATA,
			FILE_SHARE_WRITE | FILE_SHARE_READ,
			&sa,
			OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,
			NULL);
		inputValues.open("in.txt");
		inputValues << "Queued\n";
		for (std::map<BWAPI::UnitType, int>::const_iterator it = queued.begin(); it != queued.end(); ++it)
		{
			inputValues << (std::to_string(it->first.getID()) + "+" + std::to_string(queued.at(it->first))) + "\n";
		}
		inputValues << "PlayerState\n";
		for (std::map<BWAPI::UnitType, int>::const_iterator it = playerState.begin(); it != playerState.end(); ++it)
		{
			inputValues << (std::to_string(it->first.getID()) + "+" + std::to_string(playerState.at(it->first))) + "\n";
		}
		inputValues << "eof";
		inputValues.close();
		STARTUPINFO si;
		PROCESS_INFORMATION pi;

		// set the size of the structures
		ZeroMemory(&si, sizeof(si));
		ZeroMemory(&pi, sizeof(pi));
		si.cb = sizeof(si);
		si.hStdOutput = h;

		// start the program up
		CreateProcess(TEXT("Client.exe"),   // the path
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