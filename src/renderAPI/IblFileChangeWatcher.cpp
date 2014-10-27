//------------------------------------------------------------------------------------//
//                                                                                    //
//    ._____________.____   __________         __                                     //
//    |   \______   \    |  \______   \_____  |  | __ ___________                     //
//    |   ||    |  _/    |   |    |  _/\__  \ |  |/ // __ \_  __ \                    //
//    |   ||    |   \    |___|    |   \ / __ \|    <\  ___/|  | \/                    //
//    |___||______  /_______ \______  /(____  /__|_ \\___  >__|                       //
//                \/        \/      \/      \/     \/    \/                           //
//                                                                                    //
//    IBLBaker is provided under the MIT License(MIT)                                 //
//    IBLBaker uses portions of other open source software.                           //
//    Please review the LICENSE file for further details.                             //
//                                                                                    //
//    Copyright(c) 2014 Matt Davidson                                                 //
//                                                                                    //
//    Permission is hereby granted, free of charge, to any person obtaining a copy    //
//    of this software and associated documentation files(the "Software"), to deal    //
//    in the Software without restriction, including without limitation the rights    //
//    to use, copy, modify, merge, publish, distribute, sublicense, and / or sell     //
//    copies of the Software, and to permit persons to whom the Software is           //
//    furnished to do so, subject to the following conditions :                       //
//                                                                                    //
//    1. Redistributions of source code must retain the above copyright notice,       //
//    this list of conditions and the following disclaimer.                           //
//    2. Redistributions in binary form must reproduce the above copyright notice,    //
//    this list of conditions and the following disclaimer in the                     //
//    documentation and / or other materials provided with the distribution.          //
//    3. Neither the name of the copyright holder nor the names of its                //
//    contributors may be used to endorse or promote products derived                 //
//    from this software without specific prior written permission.                   //
//                                                                                    //
//    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR      //
//    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,        //
//    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE      //
//    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER          //
//    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,   //
//    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN       //
//    THE SOFTWARE.                                                                   //
//                                                                                    //
//------------------------------------------------------------------------------------//
#include <IblFileChangeWatcher.h>
#include <IblLog.h>

namespace Ibl
{
#define MAX_BUFFER  4096
typedef struct DIRECTORY_INFO {   
HANDLE      hDir;   
TCHAR       lpszDirName[MAX_BUFFER];   
CHAR        lpBuffer[MAX_BUFFER];   
DWORD       dwBufLength;   
OVERLAPPED  Overlapped;   
}*PDIRECTORY_INFO, *LPDIRECTORY_INFO;

namespace
{
FileChangeWatcher*  fileChangeWatcher = nullptr;

void WINAPI CheckChangedFile( LPDIRECTORY_INFO lpdi,   
                              PFILE_NOTIFY_INFORMATION lpfni)   
{   
    TCHAR      szFullPathName[MAX_PATH];   

    lstrcpy( szFullPathName, lpdi->lpszDirName );   
    lstrcat( szFullPathName, L"/" );   

    lstrcat( szFullPathName, lpfni->FileName );

    std::wstring wPathName(szFullPathName);
    std::string filePathName(wPathName.begin(), wPathName.end());

    if (fileChangeWatcher)
    {
        fileChangeWatcher->addChange(filePathName);
    }
}

void WINAPI handleDirectoryChange(DWORD dwCompletionPort)
{
    DWORD numBytes;
    DWORD cbOffset;
    LPDIRECTORY_INFO di;
    LPOVERLAPPED lpOverlapped;
    PFILE_NOTIFY_INFORMATION fni;

    do
    {
        // Retrieve the directory info for this directory   
        // through the completion key   
        GetQueuedCompletionStatus( (HANDLE) dwCompletionPort,   
                                   &numBytes,   
                                   reinterpret_cast<PULONG_PTR>(&di),   
                                   &lpOverlapped,   
                                   INFINITE);   
        if ( di )   
        {   
   
            fni = (PFILE_NOTIFY_INFORMATION)di->lpBuffer;   
   
            do   
            {   
                cbOffset = fni->NextEntryOffset;   
   
                //if( fni->Action == FILE_ACTION_MODIFIED )   
                CheckChangedFile( di, fni );   
   
                fni = (PFILE_NOTIFY_INFORMATION)((LPBYTE) fni + cbOffset);   
   
            } while( cbOffset );   
   
            // Reissue the watch command   
            ReadDirectoryChangesW( di->hDir,di->lpBuffer,   
                                   MAX_BUFFER,   
                                   TRUE,   
                                   FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_ATTRIBUTES,   
                                   &di->dwBufLength,   
                                   &di->Overlapped,   
                                   NULL);   
        }   
   
    }
    while( di );
}
}

FileChangeWatcher::FileChangeWatcher() :
    _processHandle(nullptr),
    _processCompletionHandle(nullptr)
{
    if (!fileChangeWatcher)
    {
        fileChangeWatcher = this;
    }

    initialize();
}

FileChangeWatcher::~FileChangeWatcher()
{
    destroy();
}

bool
FileChangeWatcher::initialize()
{
    unsigned long tid = 0;
    // Max length of lpszDirName?
    wchar_t currentPath[4096];
    memset(currentPath, 0, sizeof(wchar_t)* 4096);
    _wgetcwd(currentPath, 4096);
    std::wstring pathName = currentPath + std::wstring(L"/data/shadersD3D11");

    _dirInfo = new DIRECTORY_INFO ();
    _dirInfo->hDir = CreateFile(pathName.c_str(),
                                FILE_LIST_DIRECTORY,   
                                FILE_SHARE_READ |   
                                FILE_SHARE_WRITE |   
                                FILE_SHARE_DELETE,   
                                NULL,   
                                OPEN_EXISTING,   
                                FILE_FLAG_BACKUP_SEMANTICS |   
                                FILE_FLAG_OVERLAPPED,   
                                NULL);   
    
    if( _dirInfo->hDir == INVALID_HANDLE_VALUE )   
    {   
        LOG( "Unable to open directory for file watching");
        return false;
    }
    
    memcpy(_dirInfo->lpszDirName, pathName.c_str(), sizeof(wchar_t) * pathName.length());
    _processCompletionHandle= CreateIoCompletionPort( _dirInfo->hDir,   
                                                     _processCompletionHandle,   
                                                    (DWORD) _dirInfo,   
                                                     0);   

    ReadDirectoryChangesW(_dirInfo->hDir,   
                          _dirInfo->lpBuffer,   
                          MAX_BUFFER,   
                          TRUE,   
                          FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_ATTRIBUTES,   
                          &_dirInfo->dwBufLength,&_dirInfo->Overlapped,   
                          NULL);  



    _processHandle = CreateThread(NULL, 0,   
                                  (LPTHREAD_START_ROUTINE) handleDirectoryChange,   
                                  _processCompletionHandle,   
                                  0,   
                                  &tid);

    return true;
}

bool
FileChangeWatcher::destroy()
{
    if(_processHandle!=nullptr)
    {
        // Post completion
        PostQueuedCompletionStatus( _processCompletionHandle, 0, 0, NULL );   

        // Wait for thread to stop
        if(_processHandle!=nullptr)
        {
            WaitForSingleObject(_processHandle,INFINITE);
            CloseHandle(_processHandle);
        }
        CloseHandle(_processCompletionHandle);
        // Close monitoring handle
        CloseHandle(_dirInfo->hDir);
        delete _dirInfo;
        return true;
    }
    return false;
}

void
FileChangeWatcher::addChange(const std::string& filename)
{
    {
        std::lock_guard<std::recursive_mutex> lock(_taskLock);
        LOG ("Added change " << filename);
        _changeList.insert (std::string(filename));
    }
}

bool
FileChangeWatcher::hasChanges() const
{
    {
        std::lock_guard<std::recursive_mutex> lock(_taskLock);
        return _changeList.size() > 0;
    }
}

std::set<std::string>
FileChangeWatcher::changeList() const
{
    {
        std::lock_guard<std::recursive_mutex> lock(_taskLock);
        std::set<std::string> changes = _changeList;
        _changeList.clear();
        return changes;
    }
}
}
