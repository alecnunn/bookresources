#include <cassert>
#include <cstdio>
#include <algorithm>

#include <conio.h>
#include <Winsock2.h>

#include "FileWatcher.h"

const int kServerPort = 34910;

namespace
{
	FileWatcher::StringVec s_changedFiles;

	void Report(const FileWatcher::StringVec& changedFiles)
	{
		for (FileWatcher::StringVec::const_iterator cur = changedFiles.begin(); 
			 cur != changedFiles.end(); 
			 ++cur)
		{
			if (std::find(s_changedFiles.begin(), s_changedFiles.end(), *cur) == s_changedFiles.end())
				s_changedFiles.push_back(*cur);
		}
	}

	SOCKET s_appSocket = INVALID_SOCKET;

	void BatchPending()
	{
		if (!s_changedFiles.empty())
		{
			printf("\n");

			for (FileWatcher::StringVec::const_iterator cur = s_changedFiles.begin(); 
				cur != s_changedFiles.end(); 
				++cur)
			{
				printf("%s changed, sending...\n", cur->c_str());

				char *toSend = new char[cur->size() + 1];
				strcpy(toSend, cur->c_str());
				toSend[cur->size()] = '\n';

				send(s_appSocket, toSend, static_cast< int >(cur->size() + 1), 0);
				delete [] toSend;
			}

			s_changedFiles.clear();
		}
	}

	bool ConnectToServer(const char* ipAddress)
	{
		WSADATA wsaData;
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		{
			printf("Couldn't startup WinSock!\n");
			return false;
		}

		s_appSocket = socket(AF_INET, SOCK_STREAM, 0);
		if (s_appSocket == INVALID_SOCKET)
		{
			printf("Couldn't create a listen socket!\n");
			return false;
		}

		struct hostent* server = gethostbyname(ipAddress);
		if (server == NULL)
		{
			printf("Couldn't resolve address for '%s'\n", ipAddress);
			closesocket(s_appSocket);
			return false;
		}

		struct sockaddr_in serverAddr;
		memset(&serverAddr, sizeof(serverAddr), 0);

		memcpy((char *)&serverAddr.sin_addr.s_addr, (const char *)server->h_addr, server->h_length);

		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(kServerPort);

		if (connect(s_appSocket, (const sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) 
		{
			printf("Couldn't connect to '%s', no server found!\n", ipAddress);
			closesocket(s_appSocket);
			return false;
		}

        printf("Connected to '%s'.\n", ipAddress);
		return true;
	}

	void ShutdownNetwork()
	{
		if (s_appSocket != INVALID_SOCKET)
			closesocket(s_appSocket);

		WSACleanup();
	}
}

struct ScopedNetwork
{
	ScopedNetwork(const char* ipAddress) { ok = ConnectToServer(ipAddress); }
	~ScopedNetwork() { ShutdownNetwork(); }
	bool ok;
};

int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		printf("Usage:  %s <directory to watch> <address to connect to>\n", argv[0]);
		return 0;
	}

	ScopedNetwork network(argv[2]);
	if (network.ok == false)
		return 1;

	FileWatcher fw(argv[1]);
	fw.InstallChangedFileSetCallback(&Report);
	printf("Monitoring '%s' for changes...\n", argv[1]);

	bool done = false;
	while (!done)
	{
		if (!fw.Update())
		{
			printf("Error updating filewatcher.\n");
			break;
		}

		BatchPending();

		Sleep(500);

		printf(".");
		fflush(stdout);

		if (kbhit() && _getch() == 27)
			done = true;
	}
}