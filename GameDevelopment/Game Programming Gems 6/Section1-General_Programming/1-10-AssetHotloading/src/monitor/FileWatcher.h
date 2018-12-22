#pragma once

#include <string>
#include <vector>

#define _WIN32_WINNT 0x0501
#include <windows.h>

#include <time.h>

class FileWatcher
{
public:
	FileWatcher(const std::string& directoryToWatch);
	~FileWatcher();

	bool Update();

	typedef std::vector< std::string > StringVec;
	typedef void (*ChangedFileSetCallback)(const StringVec &);
	void InstallChangedFileSetCallback(ChangedFileSetCallback cb) { m_cb = cb; }

private:
	bool Listen();
	void UpdateReportTiming();
	void ReportChangedFile(const std::string& filename);

	HANDLE m_directoryHandle;
	HANDLE m_notifyEvent;
	OVERLAPPED m_overlappedIo;
	char *m_notifyResults;

	StringVec m_pendingChangedFiles;
	__time64_t m_timeOfFirstPendingChange;
	bool m_timing;

	ChangedFileSetCallback m_cb;

private:
	FileWatcher(const FileWatcher&);
	FileWatcher& operator =(const FileWatcher&);
};