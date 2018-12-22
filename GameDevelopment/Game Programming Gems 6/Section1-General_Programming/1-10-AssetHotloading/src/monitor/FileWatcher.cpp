#include "FileWatcher.h"
#include <cassert>
#include <algorithm>

const int kResultBufferSize = 4096;

FileWatcher::FileWatcher(const std::string& directoryToWatch) 
	: m_directoryHandle(NULL)
	, m_notifyEvent(NULL)
	, m_notifyResults(NULL)
	, m_timeOfFirstPendingChange(0)
	, m_timing(false)
	, m_cb(NULL)
{
	m_directoryHandle = CreateFile(directoryToWatch.c_str(),
								   FILE_LIST_DIRECTORY, 
								   FILE_SHARE_READ | FILE_SHARE_DELETE | FILE_SHARE_WRITE,
								   NULL,
								   OPEN_EXISTING,
								   FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
								   NULL);

	m_notifyEvent = CreateEvent(NULL, FALSE, FALSE, "FileWriteEvent");
	assert(m_notifyEvent);

	m_overlappedIo.hEvent = m_notifyEvent;

	m_notifyResults = new char[kResultBufferSize];

	Listen();
}

FileWatcher::~FileWatcher()
{
	if (m_directoryHandle)
	{
		CloseHandle(m_directoryHandle);
	}

	delete [] m_notifyResults;
}

bool FileWatcher::Listen()
{
	DWORD unused;
	BOOL rv = ReadDirectoryChangesW(m_directoryHandle, 
									m_notifyResults, 
									kResultBufferSize,
									TRUE,
									FILE_NOTIFY_CHANGE_LAST_WRITE,
									&unused,
									&m_overlappedIo,
									NULL);

	if (rv == 0)
	{
		LPVOID buffer;

		if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
						  NULL,
						  GetLastError(),
						  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
						  (LPTSTR)&buffer,
						  0,
						  NULL))
			printf("ReadDirectoryChangesW failed with '%s'\n", (const char*)buffer);
		else
			printf("Couldn't format error msg for ReadDirectoryChangesW failure.\n");

		LocalFree(buffer);
	}

	return (rv != 0);
}

bool FileWatcher::Update()
{
	UpdateReportTiming();

	DWORD bytesWritten;
	BOOL rv = GetOverlappedResult(m_notifyEvent, &m_overlappedIo, &bytesWritten, FALSE);

	if (rv == FALSE)
	{
		assert(GetLastError() == ERROR_IO_INCOMPLETE);
		return true;
	}

	const char *curEntry = m_notifyResults;

	bool done = false;
	while (!done)
	{
		const FILE_NOTIFY_INFORMATION *fileNotifyInfo = 
			reinterpret_cast< const FILE_NOTIFY_INFORMATION * >(curEntry);

		if (fileNotifyInfo->Action == FILE_ACTION_MODIFIED)
		{
			char filename[256];
			wcstombs(filename, fileNotifyInfo->FileName, 256);
			filename[fileNotifyInfo->FileNameLength / 2] = 0;

			ReportChangedFile(filename);
		}

		done = (fileNotifyInfo->NextEntryOffset == 0);
		curEntry += fileNotifyInfo->NextEntryOffset;
	}

	return Listen();
}

void FileWatcher::ReportChangedFile(const std::string& filename)
{
	if (std::find(m_pendingChangedFiles.begin(), 
				  m_pendingChangedFiles.end(), 
				  filename) == m_pendingChangedFiles.end())
	{
		m_pendingChangedFiles.push_back(filename);

		if (!m_timing)
		{
			m_timing = true;
			_time64(&m_timeOfFirstPendingChange);
		}
	}
}

void FileWatcher::UpdateReportTiming()
{
	if (m_timing)
	{
		__time64_t now;
		_time64(&now);

		if (now - m_timeOfFirstPendingChange > 1)
		{
			if (m_cb)
				m_cb(m_pendingChangedFiles);

			m_pendingChangedFiles.clear();
			m_timing = false;
		}
	}
}