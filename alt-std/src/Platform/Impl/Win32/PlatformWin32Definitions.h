#pragma once

#ifndef __ALT_STD_PLATFORM_WIN32_DEFINITIONS__
#define __ALT_STD_PLATFORM_WIN32_DEFINITIONS__

#include "../../../Defines.h"

#ifdef ALT_OS_WINDOWS

#include "sdkddkver.h"

#include "PlatformWin32DefinitionsPreInclude.h"

//#include "windows.h"
//#include "ntdef.h"
//#include "winioctl.h"
//#include "winternl.h"

typedef const char* LPCSTR;
typedef struct HINSTANCE__* HINSTANCE;
typedef HINSTANCE HMODULE;
#if defined(_MINWINDEF_)
    /* minwindef.h defines FARPROC, and attempting to redefine it may conflict with -Wstrict-prototypes */
#elif defined(_WIN64)
    typedef __int64 (__stdcall* FARPROC)(void);
#else
    typedef int (__stdcall* FARPROC)(void);
#endif
typedef wchar_t             WCHAR;
typedef const WCHAR         *LPCWSTR, *PCWSTR;

namespace Alt {
namespace _PlatformDetails {

#define __Win32__DECLARE_HANDLE(name) struct name##__{int unused;}; typedef struct name##__ *name

__Win32__DECLARE_HANDLE            (HWND);

typedef unsigned long       DWORD;
typedef int                 BOOL;
typedef unsigned char       BYTE;
typedef unsigned short      WORD;
typedef float               FLOAT;
typedef FLOAT               *PFLOAT;
typedef BOOL                *PBOOL;
typedef BOOL                *LPBOOL;
typedef BYTE                *PBYTE;
typedef BYTE                *LPBYTE;
typedef int                 *PINT;
typedef int                 *LPINT;
typedef WORD                *PWORD;
typedef WORD                *LPWORD;
typedef long                *LPLONG;
typedef DWORD               *PDWORD;
typedef DWORD               *LPDWORD;
typedef void                *LPVOID;
typedef const void          *LPCVOID;

typedef int                 INT;
typedef unsigned int        UINT;
typedef unsigned int        *PUINT;

typedef unsigned long       ULONG;
typedef ULONG               *PULONG;
typedef unsigned short      USHORT;
typedef USHORT              *PUSHORT;
typedef unsigned char       UCHAR;
typedef UCHAR               *PUCHAR;
typedef char                *PSZ;

typedef WCHAR               *PWCHAR, *LPWCH, *PWCH;
typedef WCHAR               *NWPSTR, *LPWSTR, *PWSTR;

typedef WCHAR *PWCHAR, *LPWCH, *PWCH;
typedef const WCHAR *LPCWCH, *PCWCH;

typedef WCHAR *NWPSTR, *LPWSTR, *PWSTR;
typedef PWSTR *PZPWSTR;
typedef const PWSTR *PCZPWSTR;
typedef WCHAR *LPUWSTR, *PUWSTR;
typedef const WCHAR *LPCWSTR, *PCWSTR;
typedef PCWSTR *PZPCWSTR;
typedef const PCWSTR *PCZPCWSTR;
typedef const WCHAR *LPCUWSTR, *PCUWSTR;

typedef WCHAR *PZZWSTR;
typedef const WCHAR *PCZZWSTR;
typedef WCHAR *PUZZWSTR;
typedef const WCHAR *PCUZZWSTR;

typedef  WCHAR *PNZWCH;
typedef  const WCHAR *PCNZWCH;
typedef  WCHAR *PUNZWCH;
typedef  const WCHAR *PCUNZWCH;

typedef LPWCH LPTCH, PTCH;
typedef LPCWCH LPCTCH, PCTCH;
typedef LPWSTR PTSTR, LPTSTR;
typedef LPCWSTR PCTSTR, LPCTSTR;
typedef LPUWSTR PUTSTR, LPUTSTR;
typedef LPCUWSTR PCUTSTR, LPCUTSTR;
typedef LPWSTR LP;
typedef PZZWSTR PZZTSTR;
typedef PCZZWSTR PCZZTSTR;
typedef PUZZWSTR PUZZTSTR;
typedef PCUZZWSTR PCUZZTSTR;
typedef PZPWSTR PZPTSTR;
typedef PNZWCH PNZTCH;
typedef PCNZWCH PCNZTCH;
typedef PUNZWCH PUNZTCH;
typedef PCUNZWCH PCUNZTCH;

typedef __int64             LONGLONG;
typedef unsigned __int64    ULONGLONG;
typedef char                CHAR;
typedef short               SHORT;
typedef long                LONG;
typedef int                 INT;

typedef void                *HANDLE;

typedef HANDLE              *SPHANDLE;
typedef HANDLE              *LPHANDLE;
typedef HANDLE              HGLOBAL;
typedef HANDLE              HLOCAL;
typedef HANDLE              GLOBALHANDLE;
typedef HANDLE              LOCALHANDLE;

#define __Win32__FACILITY_WIN32      7

#define __Win32__SEVERITY_SUCCESS    0
#define __Win32__SEVERITY_ERROR      1

#define __Win32__SUCCEEDED(hr) (((Alt::_PlatformDetails::HRESULT)(hr)) >= 0)

#define __Win32__FAILED(hr) (((Alt::_PlatformDetails::HRESULT)(hr)) < 0)

#define __Win32__IS_ERROR(Status) (((unsigned long)(Status)) >> 31 == __Win32__SEVERITY_ERROR)

#define __Win32__HRESULT_CODE(hr)    ((hr) & 0xFFFF)
#define __Win32__SCODE_CODE(sc)      ((sc) & 0xFFFF)

#define __Win32__HRESULT_FACILITY(hr)  (((hr) >> 16) & 0x1fff)
#define __Win32__SCODE_FACILITY(sc)    (((sc) >> 16) & 0x1fff)

#define __Win32__HRESULT_SEVERITY(hr)  (((hr) >> 31) & 0x1)
#define __Win32__SCODE_SEVERITY(sc)    (((sc) >> 31) & 0x1)

#define __Win32__MAKE_HRESULT(sev,fac,code) \
    ((Alt::_PlatformDetails::HRESULT) (((unsigned long)(sev)<<31) | ((unsigned long)(fac)<<16) | ((unsigned long)(code))) )

#define __Win32__FACILITY_NT_BIT                 0x10000000

#define __Win32____HRESULT_FROM_WIN32(x) ((Alt::_PlatformDetails::HRESULT)(x) <= 0 ? ((Alt::_PlatformDetails::HRESULT)(x)) : ((Alt::_PlatformDetails::HRESULT) (((x) & 0x0000FFFF) | (__Win32__FACILITY_WIN32 << 16) | 0x80000000)))

/*
 * https://devblogs.microsoft.com/oldnewthing/20061103-07/?p=29133/
 */
#define __Win32__LASTERROR_FROM_HRESULT(hr) (hr) <= 0 ? (Alt::_PlatformDetails::DWORD)(hr) : (Alt::_PlatformDetails::DWORD)0

#if !defined(__Win32___HRESULT_DEFINED) && !defined(__midl)
#define __Win32___HRESULT_DEFINED
typedef long HRESULT;
#endif

#define __Win32__HRESULT_FROM_NT(x)      ((Alt::_PlatformDetails::HRESULT) ((x) | __Win32__FACILITY_NT_BIT))

#if !defined(__Win32__W64)
#if !defined(__midl) && (defined(_X86_) || defined(_M_IX86) || defined(_ARM_) || defined(_M_ARM)) && _MSC_VER >= 1300
#define __Win32__W64 __w64
#else
#define __Win32__W64
#endif
#endif

#if defined(_WIN64)
    typedef __int64 INT_PTR, *PINT_PTR;
    typedef unsigned __int64 UINT_PTR, *PUINT_PTR;

    typedef __int64 LONG_PTR, *PLONG_PTR;
    typedef unsigned __int64 ULONG_PTR, *PULONG_PTR;

    #define __Win32____int3264   __int64

#else
    typedef __Win32__W64 int INT_PTR, *PINT_PTR;
    typedef __Win32__W64 unsigned int UINT_PTR, *PUINT_PTR;

    typedef __Win32__W64 long LONG_PTR, *PLONG_PTR;
    typedef __Win32__W64 unsigned long ULONG_PTR, *PULONG_PTR;

    #define __Win32____int3264   __int32

#endif

#if !defined(_MAC) && (defined(_M_MRX000) || defined(_WIN64)) && (_MSC_VER >= 1100) && !(defined(MIDL_PASS) || defined(RC_INVOKED))
 #define __Win32__POINTER_64 __ptr64
 typedef unsigned __int64 __Win32__POINTER_64_INT;
 #if defined(_WIN64)
  #define __Win32__POINTER_32 __ptr32
 #else
  #define __Win32__POINTER_32
 #endif
#else
 #if defined(_MAC) && defined(_MAC_INT_64)
  #define __Win32__POINTER_64 __ptr64
  typedef unsigned __int64 __Win32__POINTER_64_INT;
 #else
  #if (_MSC_VER >= 1300) && !(defined(MIDL_PASS) || defined(RC_INVOKED))
   #define __Win32__POINTER_64 __ptr64
  #else
   #define __Win32__POINTER_64
  #endif
  typedef unsigned long __Win32__POINTER_64_INT;
 #endif
 #define __Win32__POINTER_32
#endif

#if defined(_M_MRX000) && !(defined(MIDL_PASS) || defined(RC_INVOKED)) && defined(ENABLE_RESTRICTED)
#define __Win32__RESTRICTED_POINTER __restrict
#else
#define __Win32__RESTRICTED_POINTER
#endif

typedef void *PVOID;
typedef void * __Win32__POINTER_64 PVOID64;

typedef BYTE                BOOLEAN;           
typedef BOOLEAN             *PBOOLEAN;       

typedef DWORD ACCESS_MASK;
typedef ACCESS_MASK *PACCESS_MASK;

typedef CHAR *PCHAR, *LPCH, *PCH;
typedef const CHAR *LPCCH, *PCCH;

typedef LONG NTSTATUS;

typedef ULONG_PTR SIZE_T, *PSIZE_T;
typedef LONG_PTR SSIZE_T, *PSSIZE_T;

typedef const char *PCSZ;

typedef ULONG_PTR DWORD_PTR, *PDWORD_PTR;

typedef __int64 LONG64, *PLONG64;

typedef unsigned __int64 ULONG64, *PULONG64;
typedef unsigned __int64 DWORD64, *PDWORD64;

typedef UINT_PTR            WPARAM;
typedef LONG_PTR            LPARAM;
typedef LONG_PTR            LRESULT;

typedef ULONG_PTR KAFFINITY;
typedef KAFFINITY *PKAFFINITY;

typedef UINT_PTR (__stdcall *LPOFNHOOKPROC) (HWND, UINT, WPARAM, LPARAM);

typedef DWORD (__stdcall *PTHREAD_START_ROUTINE)(LPVOID);
typedef PTHREAD_START_ROUTINE LPTHREAD_START_ROUTINE;

typedef struct _STRING {
    USHORT Length;
    USHORT MaximumLength;
    PCHAR Buffer;
} STRING;
typedef STRING *PSTRING;

typedef STRING ANSI_STRING;
typedef PSTRING PANSI_STRING;
typedef PSTRING PCANSI_STRING;

typedef STRING OEM_STRING;
typedef PSTRING POEM_STRING;
typedef const STRING* PCOEM_STRING;

typedef struct _UNICODE_STRING {
    USHORT Length;
    USHORT MaximumLength;
    PWSTR  Buffer;
} UNICODE_STRING;
typedef UNICODE_STRING *PUNICODE_STRING;
typedef const UNICODE_STRING *PCUNICODE_STRING;

typedef LONG KPRIORITY;

typedef struct _CLIENT_ID {
    HANDLE UniqueProcess;
    HANDLE UniqueThread;
} CLIENT_ID;

typedef union _LARGE_INTEGER {
    struct {
        DWORD LowPart;
        LONG HighPart;
    } s;
    struct {
        DWORD LowPart;
        LONG HighPart;
    } u;
    LONGLONG QuadPart;
} LARGE_INTEGER;

typedef LARGE_INTEGER *PLARGE_INTEGER;

typedef union _ULARGE_INTEGER {
    struct {
        DWORD LowPart;
        DWORD HighPart;
    } s;
    struct {
        DWORD LowPart;
        DWORD HighPart;
    } u;
    ULONGLONG QuadPart;
} ULARGE_INTEGER;

typedef ULARGE_INTEGER *PULARGE_INTEGER;

typedef struct _SYSTEM_INFO {
    union {
        DWORD dwOemId;          // Obsolete field...do not use
        struct {
            WORD wProcessorArchitecture;
            WORD wReserved;
        } s;
    } u;
    DWORD dwPageSize;
    LPVOID lpMinimumApplicationAddress;
    LPVOID lpMaximumApplicationAddress;
    DWORD_PTR dwActiveProcessorMask;
    DWORD dwNumberOfProcessors;
    DWORD dwProcessorType;
    DWORD dwAllocationGranularity;
    WORD wProcessorLevel;
    WORD wProcessorRevision;
} SYSTEM_INFO, *LPSYSTEM_INFO;

typedef enum _THREADINFOCLASS
{
    ThreadBasicInformation,                         // q: THREAD_BASIC_INFORMATION
    ThreadTimes,                                    // q: KERNEL_USER_TIMES // since VISTA
    ThreadPriority,                                 // s: KPRIORITY (requires SeIncreaseBasePriorityPrivilege)
    ThreadBasePriority,                             // s: KPRIORITY
    ThreadAffinityMask,                             // s: KAFFINITY
    ThreadImpersonationToken,                       // s: HANDLE
    ThreadDescriptorTableEntry,                     // q: DESCRIPTOR_TABLE_ENTRY (or WOW64_DESCRIPTOR_TABLE_ENTRY)
    ThreadEnableAlignmentFaultFixup,                // s: BOOLEAN
    ThreadEventPair,                                // q: Obsolete
    ThreadQuerySetWin32StartAddress,                // q: PVOID
    ThreadZeroTlsCell,                              // s: ULONG // TlsIndex // 10
    ThreadPerformanceCount,                         // q: LARGE_INTEGER
    ThreadAmILastThread,                            // q: ULONG
    ThreadIdealProcessor,                           // s: ULONG
    ThreadPriorityBoost,                            // qs: ULONG
    ThreadSetTlsArrayAddress,                       // s: ULONG_PTR
    ThreadIsIoPending,                              // q: ULONG
    ThreadHideFromDebugger,                         // qs: BOOLEAN
    ThreadBreakOnTermination,                       // qs: ULONG
    ThreadSwitchLegacyState,                        // s: void // NtCurrentThread // NPX/FPU
    ThreadIsTerminated,                             // q: ULONG // 20
    ThreadLastSystemCall,                           // q: THREAD_LAST_SYSCALL_INFORMATION
    ThreadIoPriority,                               // qs: IO_PRIORITY_HINT (s: requires SeIncreaseBasePriorityPrivilege)
    ThreadCycleTime,                                // q: THREAD_CYCLE_TIME_INFORMATION (requires THREAD_QUERY_LIMITED_INFORMATION)
    ThreadPagePriority,                             // qs: PAGE_PRIORITY_INFORMATION
    ThreadActualBasePriority,                       // q: LONG
    ThreadTebInformation,                           // q: THREAD_TEB_INFORMATION (requires THREAD_GET_CONTEXT + THREAD_SET_CONTEXT)
    ThreadCSwitchMon,                               // q: Obsolete
    ThreadCSwitchPmu,                               // q: Obsolete
    ThreadWow64Context,                             // qs: WOW64_CONTEXT, ARM_NT_CONTEXT since 20H1
    ThreadGroupInformation,                         // qs: GROUP_AFFINITY // 30
    ThreadUmsInformation,                           // q: THREAD_UMS_INFORMATION // Obsolete
    ThreadCounterProfiling,                         // qs: THREAD_PROFILING_INFORMATION
    ThreadIdealProcessorEx,                         // qs: PROCESSOR_NUMBER; s: previous PROCESSOR_NUMBER on return
    ThreadCpuAccountingInformation,                 // s: HANDLE // since WIN8
    ThreadSuspendCount,                             // q: ULONG // since WINBLUE
    ThreadHeterogeneousCpuPolicy,                   // qs: KHETERO_CPU_POLICY // since THRESHOLD
    ThreadContainerId,                              // q: GUID
    ThreadNameInformation,                          // qs: THREAD_NAME_INFORMATION (requires THREAD_SET_LIMITED_INFORMATION)
    ThreadSelectedCpuSets,                          // qs: ULONG[]
    ThreadSystemThreadInformation,                  // q: SYSTEM_THREAD_INFORMATION // 40
    ThreadActualGroupAffinity,                      // q: GROUP_AFFINITY // since THRESHOLD2
    ThreadDynamicCodePolicyInfo,                    // qs: ULONG // NtCurrentThread
    ThreadExplicitCaseSensitivity,                  // qs: ULONG; s: 0 disables, otherwise enables // (requires SeDebugPrivilege and PsProtectedSignerAntimalware)
    ThreadWorkOnBehalfTicket,                       // q: RTL_WORK_ON_BEHALF_TICKET_EX; s: ALPC_WORK_ON_BEHALF_TICKET // NtCurrentThread
    ThreadSubsystemInformation,                     // q: SUBSYSTEM_INFORMATION_TYPE // since REDSTONE2
    ThreadDbgkWerReportActive,                      // s: ULONG
    ThreadAttachContainer,                          // s: HANDLE (job object) // NtCurrentThread
    ThreadManageWritesToExecutableMemory,           // s: MANAGE_WRITES_TO_EXECUTABLE_MEMORY // since REDSTONE3
    ThreadPowerThrottlingState,                     // qs: POWER_THROTTLING_THREAD_STATE // since REDSTONE3 (set), WIN11 22H2 (query)
    ThreadWorkloadClass,                            // qs: THREAD_WORKLOAD_CLASS // since REDSTONE5 // 50
    ThreadCreateStateChange,                        // s: Obsolete // since WIN11
    ThreadApplyStateChange,                         // s: Obsolete
    ThreadStrongerBadHandleChecks,                  // qs: ULONG // NtCurrentThread // since 22H1
    ThreadEffectiveIoPriority,                      // q: IO_PRIORITY_HINT
    ThreadEffectivePagePriority,                    // q: ULONG
    ThreadUpdateLockOwnership,                      // s: THREAD_LOCK_OWNERSHIP // since 24H2
    ThreadSchedulerSharedDataSlot,                  // qs: THREAD_SCHEDULER_SHARED_DATA_SLOT_INFORMATION
    ThreadTebInformationAtomic,                     // q: THREAD_TEB_INFORMATION (requires THREAD_GET_CONTEXT + THREAD_QUERY_INFORMATION)
    ThreadIndexInformation,                         // q: THREAD_INDEX_INFORMATION
    MaxThreadInfoClass
} THREADINFOCLASS;

typedef struct _LIST_ENTRY {
   struct _LIST_ENTRY *Flink;
   struct _LIST_ENTRY *Blink;
} LIST_ENTRY, *PLIST_ENTRY, *__Win32__RESTRICTED_POINTER PRLIST_ENTRY;

typedef struct _PEB_LDR_DATA {
    BYTE Reserved1[8];
    PVOID Reserved2[3];
    LIST_ENTRY InMemoryOrderModuleList;
} PEB_LDR_DATA, *PPEB_LDR_DATA;

typedef void (__stdcall *PPS_POST_PROCESS_INIT_ROUTINE)();

typedef struct _RTL_USER_PROCESS_PARAMETERS {
    BYTE Reserved1[16];
    PVOID Reserved2[10];
    UNICODE_STRING ImagePathName;
    UNICODE_STRING CommandLine;
} RTL_USER_PROCESS_PARAMETERS, *PRTL_USER_PROCESS_PARAMETERS;

typedef struct _PEB {
    BYTE Reserved1[2];
    BYTE BeingDebugged;
    BYTE Reserved2[1];
    PVOID Reserved3[2];
    PPEB_LDR_DATA Ldr;
    PRTL_USER_PROCESS_PARAMETERS ProcessParameters;
    PVOID Reserved4[3];
    PVOID AtlThunkSListPtr;
    PVOID Reserved5;
    ULONG Reserved6;
    PVOID Reserved7;
    ULONG Reserved8;
    ULONG AtlThunkSListPtr32;
    PVOID Reserved9[45];
    BYTE Reserved10[96];
    PPS_POST_PROCESS_INIT_ROUTINE PostProcessInitRoutine;
    BYTE Reserved11[128];
    PVOID Reserved12[1];
    ULONG SessionId;
} PEB, *PPEB;

typedef struct _TEB {
    PVOID Reserved1[12];
    PPEB ProcessEnvironmentBlock;
    PVOID Reserved2[399];
    BYTE Reserved3[1952];
    PVOID TlsSlots[64];
    BYTE Reserved4[8];
    PVOID Reserved5[26];
    PVOID ReservedForOle;  // Windows 2000 only
    PVOID Reserved6[4];
    PVOID TlsExpansionSlots;
} TEB, *PTEB;

typedef struct _THREAD_BASIC_INFORMATION
{
    NTSTATUS ExitStatus;        // The exit status of the thread or STATUS_PENDING when the thread has not terminated. (GetExitCodeThread)
    PTEB TebBaseAddress;        // The base address of the memory region containing the TEB structure. (NtCurrentTeb)
    CLIENT_ID ClientId;         // The process and thread identifier of the thread.
    KAFFINITY AffinityMask;     // The affinity mask of the thread. (deprecated) (SetThreadAffinityMask)
    KPRIORITY Priority;         // The current priority of the thread. (GetThreadPriority)
    KPRIORITY BasePriority;     // The current base priority of the thread determined by the thread priority and process priority class.
} THREAD_BASIC_INFORMATION, *PTHREAD_BASIC_INFORMATION;

typedef struct _RTL_CRITICAL_SECTION_DEBUG {
    WORD   Type;
    WORD   CreatorBackTraceIndex;
    struct _RTL_CRITICAL_SECTION *CriticalSection;
    LIST_ENTRY ProcessLocksList;
    DWORD EntryCount;
    DWORD ContentionCount;
    DWORD Flags;
    WORD   CreatorBackTraceIndexHigh;
    WORD   Identifier;
} RTL_CRITICAL_SECTION_DEBUG, *PRTL_CRITICAL_SECTION_DEBUG, RTL_RESOURCE_DEBUG, *PRTL_RESOURCE_DEBUG;

#define __Win32__RTL_CRITICAL_SECTION_DEBUG_FLAG_STATIC_INIT     0x00000001

#pragma pack(push, 8)

typedef struct _RTL_CRITICAL_SECTION {
    PRTL_CRITICAL_SECTION_DEBUG DebugInfo;

    //
    //  The following three fields control entering and exiting the critical
    //  section for the resource
    //

    LONG LockCount;
    LONG RecursionCount;
    HANDLE OwningThread;        // from the thread's ClientId->UniqueThread
    HANDLE LockSemaphore;
    ULONG_PTR SpinCount;        // force size on 64-bit systems when packed
} RTL_CRITICAL_SECTION, *PRTL_CRITICAL_SECTION;

#pragma pack(pop)

typedef struct _RTL_SRWLOCK {
    PVOID Ptr;
} RTL_SRWLOCK, *PRTL_SRWLOCK;
#define __Win32__RTL_SRWLOCK_INIT {0}

#define __Win32__SRWLOCK_INIT __Win32__RTL_SRWLOCK_INIT

typedef RTL_SRWLOCK SRWLOCK, *PSRWLOCK;

typedef RTL_CRITICAL_SECTION CRITICAL_SECTION;
typedef PRTL_CRITICAL_SECTION PCRITICAL_SECTION;
typedef PRTL_CRITICAL_SECTION LPCRITICAL_SECTION;

typedef RTL_CRITICAL_SECTION_DEBUG CRITICAL_SECTION_DEBUG;
typedef PRTL_CRITICAL_SECTION_DEBUG PCRITICAL_SECTION_DEBUG;
typedef PRTL_CRITICAL_SECTION_DEBUG LPCRITICAL_SECTION_DEBUG;

typedef struct _OVERLAPPED {
    ULONG_PTR Internal;
    ULONG_PTR InternalHigh;
    union {
        struct {
            DWORD Offset;
            DWORD OffsetHigh;
        } s;
        PVOID Pointer;
    } u;

    HANDLE  hEvent;
} OVERLAPPED, *LPOVERLAPPED;

typedef struct _SECURITY_ATTRIBUTES {
    DWORD nLength;
    LPVOID lpSecurityDescriptor;
    BOOL bInheritHandle;
} SECURITY_ATTRIBUTES, *PSECURITY_ATTRIBUTES, *LPSECURITY_ATTRIBUTES;

typedef struct _FILE_NOTIFY_INFORMATION {
    DWORD NextEntryOffset;
    DWORD Action;
    DWORD FileNameLength;
    WCHAR FileName[1];
} FILE_NOTIFY_INFORMATION, *PFILE_NOTIFY_INFORMATION;

#define __Win32__FILE_INVALID_FILE_ID               ((LONGLONG)-1LL) 
typedef struct _FILE_ID_128 {                               
    BYTE  Identifier[16];                                   
} FILE_ID_128, *PFILE_ID_128;                               

typedef struct _FILETIME {
    DWORD dwLowDateTime;
    DWORD dwHighDateTime;
} FILETIME, *PFILETIME, *LPFILETIME;

typedef struct _BY_HANDLE_FILE_INFORMATION {
    DWORD dwFileAttributes;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    DWORD dwVolumeSerialNumber;
    DWORD nFileSizeHigh;
    DWORD nFileSizeLow;
    DWORD nNumberOfLinks;
    DWORD nFileIndexHigh;
    DWORD nFileIndexLow;
} BY_HANDLE_FILE_INFORMATION, *PBY_HANDLE_FILE_INFORMATION, *LPBY_HANDLE_FILE_INFORMATION;

// https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/ns-ntifs-_reparse_data_buffer
typedef struct _REPARSE_DATA_BUFFER {
  ULONG  ReparseTag;
  USHORT ReparseDataLength;
  USHORT Reserved;
  union {
    struct {
      USHORT SubstituteNameOffset;
      USHORT SubstituteNameLength;
      USHORT PrintNameOffset;
      USHORT PrintNameLength;
      ULONG  Flags;
      WCHAR  PathBuffer[1];
    } SymbolicLinkReparseBuffer;
    struct {
      USHORT SubstituteNameOffset;
      USHORT SubstituteNameLength;
      USHORT PrintNameOffset;
      USHORT PrintNameLength;
      WCHAR  PathBuffer[1];
    } MountPointReparseBuffer;
    struct {
      UCHAR DataBuffer[1];
    } GenericReparseBuffer;
  } u;
} REPARSE_DATA_BUFFER, *PREPARSE_DATA_BUFFER;

// Nt kernel stuff
typedef struct _IO_STATUS_BLOCK {
#pragma warning(push)
#pragma warning(disable: 4201) // we'll always use the Microsoft compiler
    union {
        NTSTATUS Status;
        PVOID Pointer;
    } u;
#pragma warning(pop)

    ULONG_PTR Information;
} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;

typedef void (__stdcall *PIO_APC_ROUTINE)(PVOID, PIO_STATUS_BLOCK, ULONG);

typedef struct _FILE_ATTRIBUTE_TAG_INFORMATION {
  ULONG FileAttributes;
  ULONG ReparseTag;
} FILE_ATTRIBUTE_TAG_INFORMATION, *PFILE_ATTRIBUTE_TAG_INFORMATION;

typedef struct _FILE_ID_INFORMATION {
  ULONGLONG   VolumeSerialNumber;
  FILE_ID_128 FileId;
} FILE_ID_INFORMATION, *PFILE_ID_INFORMATION;

typedef struct _FILE_ID_INFO {
  ULONGLONG   VolumeSerialNumber;
  FILE_ID_128 FileId;
} FILE_ID_INFO, *PFILE_ID_INFO;

typedef struct _OBJECT_ATTRIBUTES {
    ULONG Length;
    HANDLE RootDirectory;
    PUNICODE_STRING ObjectName;
    ULONG Attributes;
    PVOID SecurityDescriptor;
    PVOID SecurityQualityOfService;
} OBJECT_ATTRIBUTES;
typedef OBJECT_ATTRIBUTES *POBJECT_ATTRIBUTES;

typedef struct _FILE_DIRECTORY_INFORMATION {
  ULONG         NextEntryOffset;
  ULONG         FileIndex;
  LARGE_INTEGER CreationTime;
  LARGE_INTEGER LastAccessTime;
  LARGE_INTEGER LastWriteTime;
  LARGE_INTEGER ChangeTime;
  LARGE_INTEGER EndOfFile;
  LARGE_INTEGER AllocationSize;
  ULONG         FileAttributes;
  ULONG         FileNameLength;
  WCHAR         FileName[1];
} FILE_DIRECTORY_INFORMATION, *PFILE_DIRECTORY_INFORMATION;

typedef enum _FILE_INFORMATION_CLASS {
  FileDirectoryInformation = 1,
  FileFullDirectoryInformation = 2,
  FileBothDirectoryInformation = 3,
  FileBasicInformation = 4,
  FileStandardInformation = 5,
  FileInternalInformation = 6,
  FileEaInformation = 7,
  FileAccessInformation = 8,
  FileNameInformation = 9,
  FileRenameInformation = 10,
  FileLinkInformation = 11,
  FileNamesInformation = 12,
  FileDispositionInformation = 13,
  FilePositionInformation = 14,
  FileFullEaInformation = 15,
  FileModeInformation = 16,
  FileAlignmentInformation = 17,
  FileAllInformation = 18,
  FileAllocationInformation = 19,
  FileEndOfFileInformation = 20,
  FileAlternateNameInformation = 21,
  FileStreamInformation = 22,
  FilePipeInformation = 23,
  FilePipeLocalInformation = 24,
  FilePipeRemoteInformation = 25,
  FileMailslotQueryInformation = 26,
  FileMailslotSetInformation = 27,
  FileCompressionInformation = 28,
  FileObjectIdInformation = 29,
  FileCompletionInformation = 30,
  FileMoveClusterInformation = 31,
  FileQuotaInformation = 32,
  FileReparsePointInformation = 33,
  FileNetworkOpenInformation = 34,
  FileAttributeTagInformation = 35,
  FileTrackingInformation = 36,
  FileIdBothDirectoryInformation = 37,
  FileIdFullDirectoryInformation = 38,
  FileValidDataLengthInformation = 39,
  FileShortNameInformation = 40,
  FileIoCompletionNotificationInformation = 41,
  FileIoStatusBlockRangeInformation = 42,
  FileIoPriorityHintInformation = 43,
  FileSfioReserveInformation = 44,
  FileSfioVolumeInformation = 45,
  FileHardLinkInformation = 46,
  FileProcessIdsUsingFileInformation = 47,
  FileNormalizedNameInformation = 48,
  FileNetworkPhysicalNameInformation = 49,
  FileIdGlobalTxDirectoryInformation = 50,
  FileIsRemoteDeviceInformation = 51,
  FileUnusedInformation = 52,
  FileNumaNodeInformation = 53,
  FileStandardLinkInformation = 54,
  FileRemoteProtocolInformation = 55,
  FileRenameInformationBypassAccessCheck = 56,
  FileLinkInformationBypassAccessCheck = 57,
  FileVolumeNameInformation = 58,
  FileIdInformation = 59,
  FileIdExtdDirectoryInformation = 60,
  FileReplaceCompletionInformation = 61,
  FileHardLinkFullIdInformation = 62,
  FileIdExtdBothDirectoryInformation = 63,
  FileDispositionInformationEx = 64,
  FileRenameInformationEx = 65,
  FileRenameInformationExBypassAccessCheck = 66,
  FileDesiredStorageClassInformation = 67,
  FileStatInformation = 68,
  FileMemoryPartitionInformation = 69,
  FileStatLxInformation = 70,
  FileCaseSensitiveInformation = 71,
  FileLinkInformationEx = 72,
  FileLinkInformationExBypassAccessCheck = 73,
  FileStorageReserveIdInformation = 74,
  FileCaseSensitiveInformationForceAccessCheck = 75,
  FileKnownFolderInformation = 76,
  FileStatBasicInformation = 77,
  FileId64ExtdDirectoryInformation = 78,
  FileId64ExtdBothDirectoryInformation = 79,
  FileIdAllExtdDirectoryInformation = 80,
  FileIdAllExtdBothDirectoryInformation = 81,
  FileStreamReservationInformation,
  FileMupProviderInfo,
  FileMaximumInformation
} FILE_INFORMATION_CLASS, *PFILE_INFORMATION_CLASS;

typedef struct _SYSTEMTIME {
    WORD wYear;
    WORD wMonth;
    WORD wDayOfWeek;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMilliseconds;
} SYSTEMTIME, *PSYSTEMTIME, *LPSYSTEMTIME;

typedef struct _TIME_ZONE_INFORMATION {
    LONG Bias;
    WCHAR StandardName[ 32 ];
    SYSTEMTIME StandardDate;
    LONG StandardBias;
    WCHAR DaylightName[ 32 ];
    SYSTEMTIME DaylightDate;
    LONG DaylightBias;
} TIME_ZONE_INFORMATION, *PTIME_ZONE_INFORMATION, *LPTIME_ZONE_INFORMATION;

typedef struct _COORD {
    SHORT X;
    SHORT Y;
} COORD, *PCOORD;

typedef struct _SMALL_RECT {
    SHORT Left;
    SHORT Top;
    SHORT Right;
    SHORT Bottom;
} SMALL_RECT, *PSMALL_RECT;

typedef struct _CONSOLE_SCREEN_BUFFER_INFO {
    COORD dwSize;
    COORD dwCursorPosition;
    WORD  wAttributes;
    SMALL_RECT srWindow;
    COORD dwMaximumWindowSize;
} CONSOLE_SCREEN_BUFFER_INFO, *PCONSOLE_SCREEN_BUFFER_INFO;

typedef struct tagOFNW {
   DWORD        lStructSize;
   HWND         hwndOwner;
   HINSTANCE    hInstance;
   LPCWSTR      lpstrFilter;
   LPWSTR       lpstrCustomFilter;
   DWORD        nMaxCustFilter;
   DWORD        nFilterIndex;
   LPWSTR       lpstrFile;
   DWORD        nMaxFile;
   LPWSTR       lpstrFileTitle;
   DWORD        nMaxFileTitle;
   LPCWSTR      lpstrInitialDir;
   LPCWSTR      lpstrTitle;
   DWORD        Flags;
   WORD         nFileOffset;
   WORD         nFileExtension;
   LPCWSTR      lpstrDefExt;
   LPARAM       lCustData;
   LPOFNHOOKPROC lpfnHook;
   LPCWSTR      lpTemplateName;
#ifdef _MAC
   LPEDITMENU   lpEditInfo;
   LPCSTR       lpstrPrompt;
#endif
#if (_WIN32_WINNT >= 0x0500)
   void *        pvReserved;
   DWORD        dwReserved;
   DWORD        FlagsEx;
#endif // (_WIN32_WINNT >= 0x0500)
} OPENFILENAMEW, *LPOPENFILENAMEW;

#define __Win32__MAXULONG64  ((ULONG64)~((ULONG64)0))
#define __Win32__MAXLONG64   ((LONG64)(__Win32__MAXULONG64 >> 1))
#define __Win32__MINLONG64   ((LONG64)~__Win32__MAXLONG64)

#define __Win32__MINCHAR     0x80        
#define __Win32__MAXCHAR     0x7f        
#define __Win32__MINSHORT    0x8000      
#define __Win32__MAXSHORT    0x7fff      
#define __Win32__MINLONG     0x80000000  
#define __Win32__MAXLONG     0x7fffffff  
#define __Win32__MAXBYTE     0xff        
#define __Win32__MAXWORD     0xffff      
#define __Win32__MAXDWORD    0xffffffff  

//
// Low order two bits of a handle are ignored by the system and available
// for use by application code as tag bits.  The remaining bits are opaque
// and used to store a serial number and table index.
//

#define __Win32__OBJ_HANDLE_TAGBITS  0x00000003L

#define __Win32__ERROR_SUCCESS                    0L
#define __Win32__ERROR_FILE_EXISTS                80L
#define __Win32__ERROR_ALREADY_EXISTS             183L
#define __Win32__ERROR_FILE_NOT_FOUND             2L
#define __Win32__ERROR_PATH_NOT_FOUND             3L
#define __Win32__ERROR_INVALID_NAME               123L    // dderror
#define __Win32__ERROR_BAD_NETPATH                53L
#define __Win32__ERROR_ACCESS_DENIED              5L
#define __Win32__ERROR_TIMEOUT                    1460L
#define __Win32__WAIT_TIMEOUT                     258L    // dderror

#define __Win32__FAST_FAIL_LEGACY_GS_VIOLATION               0
#define __Win32__FAST_FAIL_VTGUARD_CHECK_FAILURE             1
#define __Win32__FAST_FAIL_STACK_COOKIE_CHECK_FAILURE        2
#define __Win32__FAST_FAIL_CORRUPT_LIST_ENTRY                3
#define __Win32__FAST_FAIL_INCORRECT_STACK                   4
#define __Win32__FAST_FAIL_INVALID_ARG                       5
#define __Win32__FAST_FAIL_GS_COOKIE_INIT                    6
#define __Win32__FAST_FAIL_FATAL_APP_EXIT                    7
#define __Win32__FAST_FAIL_RANGE_CHECK_FAILURE               8
#define __Win32__FAST_FAIL_UNSAFE_REGISTRY_ACCESS            9
#define __Win32__FAST_FAIL_GUARD_ICALL_CHECK_FAILURE         10
#define __Win32__FAST_FAIL_GUARD_WRITE_CHECK_FAILURE         11
#define __Win32__FAST_FAIL_INVALID_FIBER_SWITCH              12
#define __Win32__FAST_FAIL_INVALID_SET_OF_CONTEXT            13
#define __Win32__FAST_FAIL_INVALID_REFERENCE_COUNT           14
#define __Win32__FAST_FAIL_INVALID_JUMP_BUFFER               18
#define __Win32__FAST_FAIL_MRDATA_MODIFIED                   19
#define __Win32__FAST_FAIL_CERTIFICATION_FAILURE             20
#define __Win32__FAST_FAIL_INVALID_EXCEPTION_CHAIN           21
#define __Win32__FAST_FAIL_CRYPTO_LIBRARY                    22
#define __Win32__FAST_FAIL_INVALID_CALL_IN_DLL_CALLOUT       23
#define __Win32__FAST_FAIL_INVALID_IMAGE_BASE                24
#define __Win32__FAST_FAIL_DLOAD_PROTECTION_FAILURE          25
#define __Win32__FAST_FAIL_UNSAFE_EXTENSION_CALL             26
#define __Win32__FAST_FAIL_DEPRECATED_SERVICE_INVOKED        27
#define __Win32__FAST_FAIL_INVALID_BUFFER_ACCESS             28
#define __Win32__FAST_FAIL_INVALID_BALANCED_TREE             29
#define __Win32__FAST_FAIL_INVALID_NEXT_THREAD               30
#define __Win32__FAST_FAIL_GUARD_ICALL_CHECK_SUPPRESSED      31         // Telemetry, nonfatal
#define __Win32__FAST_FAIL_APCS_DISABLED                     32
#define __Win32__FAST_FAIL_INVALID_IDLE_STATE                33
#define __Win32__FAST_FAIL_MRDATA_PROTECTION_FAILURE         34
#define __Win32__FAST_FAIL_UNEXPECTED_HEAP_EXCEPTION         35
#define __Win32__FAST_FAIL_INVALID_LOCK_STATE                36
#define __Win32__FAST_FAIL_GUARD_JUMPTABLE                   37         // Known to compiler, must retain value 37
#define __Win32__FAST_FAIL_INVALID_LONGJUMP_TARGET           38
#define __Win32__FAST_FAIL_INVALID_DISPATCH_CONTEXT          39
#define __Win32__FAST_FAIL_INVALID_THREAD                    40
#define __Win32__FAST_FAIL_INVALID_SYSCALL_NUMBER            41         // Telemetry, nonfatal
#define __Win32__FAST_FAIL_INVALID_FILE_OPERATION            42         // Telemetry, nonfatal
#define __Win32__FAST_FAIL_LPAC_ACCESS_DENIED                43         // Telemetry, nonfatal
#define __Win32__FAST_FAIL_GUARD_SS_FAILURE                  44
#define __Win32__FAST_FAIL_LOADER_CONTINUITY_FAILURE         45         // Telemetry, nonfatal
#define __Win32__FAST_FAIL_GUARD_EXPORT_SUPPRESSION_FAILURE  46
#define __Win32__FAST_FAIL_INVALID_CONTROL_STACK             47
#define __Win32__FAST_FAIL_SET_CONTEXT_DENIED                48
#define __Win32__FAST_FAIL_INVALID_IAT                       49
#define __Win32__FAST_FAIL_HEAP_METADATA_CORRUPTION          50
#define __Win32__FAST_FAIL_PAYLOAD_RESTRICTION_VIOLATION     51
#define __Win32__FAST_FAIL_LOW_LABEL_ACCESS_DENIED           52         // Telemetry, nonfatal
#define __Win32__FAST_FAIL_ENCLAVE_CALL_FAILURE              53
#define __Win32__FAST_FAIL_UNHANDLED_LSS_EXCEPTON            54
#define __Win32__FAST_FAIL_ADMINLESS_ACCESS_DENIED           55         // Telemetry, nonfatal
#define __Win32__FAST_FAIL_UNEXPECTED_CALL                   56
#define __Win32__FAST_FAIL_CONTROL_INVALID_RETURN_ADDRESS    57
#define __Win32__FAST_FAIL_UNEXPECTED_HOST_BEHAVIOR          58
#define __Win32__FAST_FAIL_FLAGS_CORRUPTION                  59
#define __Win32__FAST_FAIL_VEH_CORRUPTION                    60
#define __Win32__FAST_FAIL_ETW_CORRUPTION                    61
#define __Win32__FAST_FAIL_RIO_ABORT                         62
#define __Win32__FAST_FAIL_INVALID_PFN                       63
#define __Win32__FAST_FAIL_GUARD_ICALL_CHECK_FAILURE_XFG     64
#define __Win32__FAST_FAIL_CAST_GUARD                        65         // Known to compiler, must retain value 65
#define __Win32__FAST_FAIL_HOST_VISIBILITY_CHANGE            66
#define __Win32__FAST_FAIL_KERNEL_CET_SHADOW_STACK_ASSIST    67
#define __Win32__FAST_FAIL_PATCH_CALLBACK_FAILED             68
#define __Win32__FAST_FAIL_NTDLL_PATCH_FAILED                69
#define __Win32__FAST_FAIL_INVALID_FLS_DATA                  70
#define __Win32__FAST_FAIL_ASAN_ERROR                        71         // Known to Asan, must retain value 71
#define __Win32__FAST_FAIL_CLR_EXCEPTION_AOT                 72
#define __Win32__FAST_FAIL_POINTER_AUTH_INVALID_RETURN_ADDRESS 73
#define __Win32__FAST_FAIL_INVALID_THREAD_STATE              74
#define __Win32__FAST_FAIL_CORRUPT_WOW64_STATE               75
#define __Win32__FAST_FAIL_INVALID_EXTENDED_STATE            76
#define __Win32__FAST_FAIL_KERNEL_POINTER_EXPECTED           77
#define __Win32__FAST_FAIL_INVALID_FAST_FAIL_CODE            0xFFFFFFFF

#define __Win32__THREAD_DYNAMIC_CODE_ALLOW   1     // Opt-out of dynamic code generation.

#define __Win32__THREAD_BASE_PRIORITY_LOWRT  15  // value that gets a thread to LowRealtime-1
#define __Win32__THREAD_BASE_PRIORITY_MAX    2   // maximum thread base priority boost
#define __Win32__THREAD_BASE_PRIORITY_MIN    (-2)  // minimum thread base priority boost
#define __Win32__THREAD_BASE_PRIORITY_IDLE   (-15) // value that gets a thread to idle

#define __Win32__STACK_SIZE_PARAM_IS_A_RESERVATION   0x00010000    // Threads only

#define __Win32__THREAD_PRIORITY_LOWEST          __Win32__THREAD_BASE_PRIORITY_MIN
#define __Win32__THREAD_PRIORITY_BELOW_NORMAL    (__Win32__THREAD_PRIORITY_LOWEST+1)
#define __Win32__THREAD_PRIORITY_NORMAL          0
#define __Win32__THREAD_PRIORITY_HIGHEST         __Win32__THREAD_BASE_PRIORITY_MAX
#define __Win32__THREAD_PRIORITY_ABOVE_NORMAL    (__Win32__THREAD_PRIORITY_HIGHEST-1)
#define __Win32__THREAD_PRIORITY_ERROR_RETURN    (__Win32__MAXLONG)

#define __Win32__THREAD_PRIORITY_TIME_CRITICAL   __Win32__THREAD_BASE_PRIORITY_LOWRT
#define __Win32__THREAD_PRIORITY_IDLE            __Win32__THREAD_BASE_PRIORITY_IDLE

#define __Win32__THREAD_MODE_BACKGROUND_BEGIN    0x00010000
#define __Win32__THREAD_MODE_BACKGROUND_END      0x00020000

#define __Win32__CDERR_DIALOGFAILURE   0xFFFF

#define __Win32__CDERR_GENERALCODES     0x0000
#define __Win32__CDERR_STRUCTSIZE       0x0001
#define __Win32__CDERR_INITIALIZATION   0x0002
#define __Win32__CDERR_NOTEMPLATE       0x0003
#define __Win32__CDERR_NOHINSTANCE      0x0004
#define __Win32__CDERR_LOADSTRFAILURE   0x0005
#define __Win32__CDERR_FINDRESFAILURE   0x0006
#define __Win32__CDERR_LOADRESFAILURE   0x0007
#define __Win32__CDERR_LOCKRESFAILURE   0x0008
#define __Win32__CDERR_MEMALLOCFAILURE  0x0009
#define __Win32__CDERR_MEMLOCKFAILURE   0x000A
#define __Win32__CDERR_NOHOOK           0x000B
#define __Win32__CDERR_REGISTERMSGFAIL  0x000C

#define __Win32__PDERR_PRINTERCODES     0x1000
#define __Win32__PDERR_SETUPFAILURE     0x1001
#define __Win32__PDERR_PARSEFAILURE     0x1002
#define __Win32__PDERR_RETDEFFAILURE    0x1003
#define __Win32__PDERR_LOADDRVFAILURE   0x1004
#define __Win32__PDERR_GETDEVMODEFAIL   0x1005
#define __Win32__PDERR_INITFAILURE      0x1006
#define __Win32__PDERR_NODEVICES        0x1007
#define __Win32__PDERR_NODEFAULTPRN     0x1008
#define __Win32__PDERR_DNDMMISMATCH     0x1009
#define __Win32__PDERR_CREATEICFAILURE  0x100A
#define __Win32__PDERR_PRINTERNOTFOUND  0x100B
#define __Win32__PDERR_DEFAULTDIFFERENT 0x100C

#define __Win32__CFERR_CHOOSEFONTCODES  0x2000
#define __Win32__CFERR_NOFONTS          0x2001
#define __Win32__CFERR_MAXLESSTHANMIN   0x2002

#define __Win32__FNERR_FILENAMECODES    0x3000
#define __Win32__FNERR_SUBCLASSFAILURE  0x3001
#define __Win32__FNERR_INVALIDFILENAME  0x3002
#define __Win32__FNERR_BUFFERTOOSMALL   0x3003

#define __Win32__FRERR_FINDREPLACECODES 0x4000
#define __Win32__FRERR_BUFFERLENGTHZERO 0x4001

#define __Win32__CCERR_CHOOSECOLORCODES 0x5000

#define __Win32__OFN_READONLY                 0x00000001
#define __Win32__OFN_OVERWRITEPROMPT          0x00000002
#define __Win32__OFN_HIDEREADONLY             0x00000004
#define __Win32__OFN_NOCHANGEDIR              0x00000008
#define __Win32__OFN_SHOWHELP                 0x00000010
#define __Win32__OFN_ENABLEHOOK               0x00000020
#define __Win32__OFN_ENABLETEMPLATE           0x00000040
#define __Win32__OFN_ENABLETEMPLATEHANDLE     0x00000080
#define __Win32__OFN_NOVALIDATE               0x00000100
#define __Win32__OFN_ALLOWMULTISELECT         0x00000200
#define __Win32__OFN_EXTENSIONDIFFERENT       0x00000400
#define __Win32__OFN_PATHMUSTEXIST            0x00000800
#define __Win32__OFN_FILEMUSTEXIST            0x00001000
#define __Win32__OFN_CREATEPROMPT             0x00002000
#define __Win32__OFN_SHAREAWARE               0x00004000
#define __Win32__OFN_NOREADONLYRETURN         0x00008000
#define __Win32__OFN_NOTESTFILECREATE         0x00010000
#define __Win32__OFN_NONETWORKBUTTON          0x00020000
#define __Win32__OFN_NOLONGNAMES              0x00040000     // force no long names for 4.x modules
#if(WINVER >= 0x0400)
#define __Win32__OFN_EXPLORER                 0x00080000     // new look commdlg
#define __Win32__OFN_NODEREFERENCELINKS       0x00100000
#define __Win32__OFN_LONGNAMES                0x00200000     // force long names for 3.x modules

#define __Win32__OFN_ENABLEINCLUDENOTIFY      0x00400000     // send include message to callback
#define __Win32__OFN_ENABLESIZING             0x00800000
#endif /* WINVER >= 0x0400 */
#if (_WIN32_WINNT >= 0x0500)
#define __Win32__OFN_DONTADDTORECENT          0x02000000
#define __Win32__OFN_FORCESHOWHIDDEN          0x10000000    // Show All files including System and hidden files
#endif // (_WIN32_WINNT >= 0x0500)

//FlagsEx Values
#if (_WIN32_WINNT >= 0x0500)
#define  OFN_EX_NOPLACESBAR         0x00000001
#endif // (_WIN32_WINNT >= 0x0500)

#define __Win32__OFN_SHAREFALLTHROUGH     2
#define __Win32__OFN_SHARENOWARN          1
#define __Win32__OFN_SHAREWARN            0

#define __Win32__OBJ_INHERIT                         0x00000002L
#define __Win32__OBJ_PERMANENT                       0x00000010L
#define __Win32__OBJ_EXCLUSIVE                       0x00000020L
#define __Win32__OBJ_CASE_INSENSITIVE                0x00000040L
#define __Win32__OBJ_OPENIF                          0x00000080L
#define __Win32__OBJ_OPENLINK                        0x00000100L
#define __Win32__OBJ_KERNEL_HANDLE                   0x00000200L
#define __Win32__OBJ_FORCE_ACCESS_CHECK              0x00000400L
#define __Win32__OBJ_IGNORE_IMPERSONATED_DEVICEMAP   0x00000800L
#define __Win32__OBJ_DONT_REPARSE                    0x00001000L
#define __Win32__OBJ_VALID_ATTRIBUTES                0x00001FF2L

#define __Win32__STATUS_NAME_TOO_LONG ((NTSTATUS)0xC0000106L)
#define __Win32__STATUS_BUFFER_OVERFLOW ((NTSTATUS)0x80000005L)
#define __Win32__STATUS_NO_MORE_FILES ((NTSTATUS)0x80000006L)

#define __Win32__DEVICE_TYPE DWORD

//#define __Win32__PAGE_NOACCESS           0x01    
//#define __Win32__PAGE_READONLY           0x02    
//#define __Win32__PAGE_READWRITE          0x04    
//#define __Win32__PAGE_WRITECOPY          0x08    
#define __Win32__PAGE_EXECUTE            0x10    
#define __Win32__PAGE_EXECUTE_READ       0x20    
#define __Win32__PAGE_EXECUTE_READWRITE  0x40    
#define __Win32__PAGE_EXECUTE_WRITECOPY  0x80    
#define __Win32__PAGE_GUARD             0x100    
#define __Win32__PAGE_NOCACHE           0x200    
#define __Win32__PAGE_WRITECOMBINE      0x400    
#define __Win32__PAGE_GRAPHICS_NOACCESS           0x0800    
#define __Win32__PAGE_GRAPHICS_READONLY           0x1000    
#define __Win32__PAGE_GRAPHICS_READWRITE          0x2000    
#define __Win32__PAGE_GRAPHICS_EXECUTE            0x4000    
#define __Win32__PAGE_GRAPHICS_EXECUTE_READ       0x8000    
#define __Win32__PAGE_GRAPHICS_EXECUTE_READWRITE 0x10000    
#define __Win32__PAGE_GRAPHICS_COHERENT          0x20000    
#define __Win32__PAGE_GRAPHICS_NOCACHE           0x40000    
#define __Win32__PAGE_ENCLAVE_THREAD_CONTROL 0x80000000  
#define __Win32__PAGE_REVERT_TO_FILE_MAP     0x80000000  
#define __Win32__PAGE_TARGETS_NO_UPDATE      0x40000000  
#define __Win32__PAGE_TARGETS_INVALID        0x40000000  
#define __Win32__PAGE_ENCLAVE_UNVALIDATED    0x20000000  
#define __Win32__PAGE_ENCLAVE_MASK           0x10000000  
#define __Win32__PAGE_ENCLAVE_DECOMMIT       (__Win32__PAGE_ENCLAVE_MASK | 0) 
#define __Win32__PAGE_ENCLAVE_SS_FIRST       (__Win32__PAGE_ENCLAVE_MASK | 1) 
#define __Win32__PAGE_ENCLAVE_SS_REST        (__Win32__PAGE_ENCLAVE_MASK | 2) 
//#define __Win32__MEM_COMMIT                      0x00001000  
//#define __Win32__MEM_RESERVE                     0x00002000  
#define __Win32__MEM_REPLACE_PLACEHOLDER         0x00004000  
#define __Win32__MEM_RESERVE_PLACEHOLDER         0x00040000  
//#define __Win32__MEM_RESET                       0x00080000  
#define __Win32__MEM_TOP_DOWN                    0x00100000  
#define __Win32__MEM_WRITE_WATCH                 0x00200000  
#define __Win32__MEM_PHYSICAL                    0x00400000  
#define __Win32__MEM_ROTATE                      0x00800000  
#define __Win32__MEM_DIFFERENT_IMAGE_BASE_OK     0x00800000  
#define __Win32__MEM_RESET_UNDO                  0x01000000  
#define __Win32__MEM_LARGE_PAGES                 0x20000000  
#define __Win32__MEM_4MB_PAGES                   0x80000000  
#define __Win32__MEM_64K_PAGES                   (__Win32__MEM_LARGE_PAGES | __Win32__MEM_PHYSICAL)  
#define __Win32__MEM_UNMAP_WITH_TRANSIENT_BOOST  0x00000001  
#define __Win32__MEM_COALESCE_PLACEHOLDERS       0x00000001  
#define __Win32__MEM_PRESERVE_PLACEHOLDER        0x00000002  
//#define __Win32__MEM_DECOMMIT                    0x00004000  
//#define __Win32__MEM_RELEASE                     0x00008000  
#define __Win32__MEM_FREE                        0x00010000  
#define __Win32__MEM_EXTENDED_PARAMETER_GRAPHICS                 0x00000001  
#define __Win32__MEM_EXTENDED_PARAMETER_NONPAGED                 0x00000002  
#define __Win32__MEM_EXTENDED_PARAMETER_ZERO_PAGES_OPTIONAL      0x00000004  
#define __Win32__MEM_EXTENDED_PARAMETER_NONPAGED_LARGE           0x00000008  
#define __Win32__MEM_EXTENDED_PARAMETER_NONPAGED_HUGE            0x00000010  
#define __Win32__MEM_EXTENDED_PARAMETER_SOFT_FAULT_PAGES         0x00000020  
#define __Win32__MEM_EXTENDED_PARAMETER_EC_CODE                  0x00000040  
#define __Win32__MEM_EXTENDED_PARAMETER_NUMA_NODE_MANDATORY      __Win32__MINLONG64	  

#define __Win32__SECTION_QUERY                0x0001
#define __Win32__SECTION_MAP_WRITE            0x0002
#define __Win32__SECTION_MAP_READ             0x0004
#define __Win32__SECTION_MAP_EXECUTE          0x0008
#define __Win32__SECTION_EXTEND_SIZE          0x0010
#define __Win32__SECTION_MAP_EXECUTE_EXPLICIT 0x0020 // not included in SECTION_ALL_ACCESS

#define __Win32__DELETE                           (0x00010000L)
#define __Win32__READ_CONTROL                     (0x00020000L)
#define __Win32__WRITE_DAC                        (0x00040000L)
#define __Win32__WRITE_OWNER                      (0x00080000L)
#define __Win32__SYNCHRONIZE                      (0x00100000L)

#define __Win32__STANDARD_RIGHTS_REQUIRED         (0x000F0000L)

#define __Win32__STANDARD_RIGHTS_READ             (__Win32__READ_CONTROL)
#define __Win32__STANDARD_RIGHTS_WRITE            (__Win32__READ_CONTROL)
#define __Win32__STANDARD_RIGHTS_EXECUTE          (__Win32__READ_CONTROL)

#define __Win32__STANDARD_RIGHTS_ALL              (0x001F0000L)

#define __Win32__SPECIFIC_RIGHTS_ALL              (0x0000FFFFL)

#define __Win32__SECTION_ALL_ACCESS (__Win32__STANDARD_RIGHTS_REQUIRED|__Win32__SECTION_QUERY|\
                            __Win32__SECTION_MAP_WRITE |      \
                            __Win32__SECTION_MAP_READ |       \
                            __Win32__SECTION_MAP_EXECUTE |    \
                            __Win32__SECTION_EXTEND_SIZE)

#define __Win32__FILE_MAP_WRITE            __Win32__SECTION_MAP_WRITE
#define __Win32__FILE_MAP_READ             __Win32__SECTION_MAP_READ
#define __Win32__FILE_MAP_ALL_ACCESS       __Win32__SECTION_ALL_ACCESS

#define __Win32__FILE_MAP_EXECUTE          __Win32__SECTION_MAP_EXECUTE_EXPLICIT  // not included in FILE_MAP_ALL_ACCESS

#define __Win32__FILE_MAP_COPY             0x00000001

#define __Win32__FILE_MAP_RESERVE          0x80000000
#define __Win32__FILE_MAP_TARGETS_INVALID  0x40000000
#define __Win32__FILE_MAP_LARGE_PAGES      0x20000000

#define __Win32__FILE_DEVICE_BEEP                0x00000001
#define __Win32__FILE_DEVICE_CD_ROM              0x00000002
#define __Win32__FILE_DEVICE_CD_ROM_FILE_SYSTEM  0x00000003
#define __Win32__FILE_DEVICE_CONTROLLER          0x00000004
#define __Win32__FILE_DEVICE_DATALINK            0x00000005
#define __Win32__FILE_DEVICE_DFS                 0x00000006
#define __Win32__FILE_DEVICE_DISK                0x00000007
#define __Win32__FILE_DEVICE_DISK_FILE_SYSTEM    0x00000008
#define __Win32__FILE_DEVICE_FILE_SYSTEM         0x00000009
#define __Win32__FILE_DEVICE_INPORT_PORT         0x0000000a
#define __Win32__FILE_DEVICE_KEYBOARD            0x0000000b
#define __Win32__FILE_DEVICE_MAILSLOT            0x0000000c
#define __Win32__FILE_DEVICE_MIDI_IN             0x0000000d
#define __Win32__FILE_DEVICE_MIDI_OUT            0x0000000e
#define __Win32__FILE_DEVICE_MOUSE               0x0000000f
#define __Win32__FILE_DEVICE_MULTI_UNC_PROVIDER  0x00000010
#define __Win32__FILE_DEVICE_NAMED_PIPE          0x00000011
#define __Win32__FILE_DEVICE_NETWORK             0x00000012
#define __Win32__FILE_DEVICE_NETWORK_BROWSER     0x00000013
#define __Win32__FILE_DEVICE_NETWORK_FILE_SYSTEM 0x00000014
#define __Win32__FILE_DEVICE_NULL                0x00000015
#define __Win32__FILE_DEVICE_PARALLEL_PORT       0x00000016
#define __Win32__FILE_DEVICE_PHYSICAL_NETCARD    0x00000017
#define __Win32__FILE_DEVICE_PRINTER             0x00000018
#define __Win32__FILE_DEVICE_SCANNER             0x00000019
#define __Win32__FILE_DEVICE_SERIAL_MOUSE_PORT   0x0000001a
#define __Win32__FILE_DEVICE_SERIAL_PORT         0x0000001b
#define __Win32__FILE_DEVICE_SCREEN              0x0000001c
#define __Win32__FILE_DEVICE_SOUND               0x0000001d
#define __Win32__FILE_DEVICE_STREAMS             0x0000001e
#define __Win32__FILE_DEVICE_TAPE                0x0000001f
#define __Win32__FILE_DEVICE_TAPE_FILE_SYSTEM    0x00000020
#define __Win32__FILE_DEVICE_TRANSPORT           0x00000021
#define __Win32__FILE_DEVICE_UNKNOWN             0x00000022
#define __Win32__FILE_DEVICE_VIDEO               0x00000023
#define __Win32__FILE_DEVICE_VIRTUAL_DISK        0x00000024
#define __Win32__FILE_DEVICE_WAVE_IN             0x00000025
#define __Win32__FILE_DEVICE_WAVE_OUT            0x00000026
#define __Win32__FILE_DEVICE_8042_PORT           0x00000027
#define __Win32__FILE_DEVICE_NETWORK_REDIRECTOR  0x00000028
#define __Win32__FILE_DEVICE_BATTERY             0x00000029
#define __Win32__FILE_DEVICE_BUS_EXTENDER        0x0000002a
#define __Win32__FILE_DEVICE_MODEM               0x0000002b
#define __Win32__FILE_DEVICE_VDM                 0x0000002c
#define __Win32__FILE_DEVICE_MASS_STORAGE        0x0000002d
#define __Win32__FILE_DEVICE_SMB                 0x0000002e
#define __Win32__FILE_DEVICE_KS                  0x0000002f
#define __Win32__FILE_DEVICE_CHANGER             0x00000030
#define __Win32__FILE_DEVICE_SMARTCARD           0x00000031
#define __Win32__FILE_DEVICE_ACPI                0x00000032
#define __Win32__FILE_DEVICE_DVD                 0x00000033
#define __Win32__FILE_DEVICE_FULLSCREEN_VIDEO    0x00000034
#define __Win32__FILE_DEVICE_DFS_FILE_SYSTEM     0x00000035
#define __Win32__FILE_DEVICE_DFS_VOLUME          0x00000036
#define __Win32__FILE_DEVICE_SERENUM             0x00000037
#define __Win32__FILE_DEVICE_TERMSRV             0x00000038
#define __Win32__FILE_DEVICE_KSEC                0x00000039
#define __Win32__FILE_DEVICE_FIPS                0x0000003A
#define __Win32__FILE_DEVICE_INFINIBAND          0x0000003B
#define __Win32__FILE_DEVICE_VMBUS               0x0000003E
#define __Win32__FILE_DEVICE_CRYPT_PROVIDER      0x0000003F
#define __Win32__FILE_DEVICE_WPD                 0x00000040
#define __Win32__FILE_DEVICE_BLUETOOTH           0x00000041
#define __Win32__FILE_DEVICE_MT_COMPOSITE        0x00000042
#define __Win32__FILE_DEVICE_MT_TRANSPORT        0x00000043
#define __Win32__FILE_DEVICE_BIOMETRIC           0x00000044
#define __Win32__FILE_DEVICE_PMI                 0x00000045
#define __Win32__FILE_DEVICE_EHSTOR              0x00000046
#define __Win32__FILE_DEVICE_DEVAPI              0x00000047
#define __Win32__FILE_DEVICE_GPIO                0x00000048
#define __Win32__FILE_DEVICE_USBEX               0x00000049
#define __Win32__FILE_DEVICE_CONSOLE             0x00000050
#define __Win32__FILE_DEVICE_NFP                 0x00000051
#define __Win32__FILE_DEVICE_SYSENV              0x00000052
#define __Win32__FILE_DEVICE_VIRTUAL_BLOCK       0x00000053
#define __Win32__FILE_DEVICE_POINT_OF_SERVICE    0x00000054
#define __Win32__FILE_DEVICE_STORAGE_REPLICATION 0x00000055
#define __Win32__FILE_DEVICE_TRUST_ENV           0x00000056
#define __Win32__FILE_DEVICE_UCM                 0x00000057
#define __Win32__FILE_DEVICE_UCMTCPCI            0x00000058
#define __Win32__FILE_DEVICE_PERSISTENT_MEMORY   0x00000059
#define __Win32__FILE_DEVICE_NVDIMM              0x0000005a
#define __Win32__FILE_DEVICE_HOLOGRAPHIC         0x0000005b
#define __Win32__FILE_DEVICE_SDFXHCI             0x0000005c
#define __Win32__FILE_DEVICE_UCMUCSI             0x0000005d
#define __Win32__FILE_DEVICE_PRM                 0x0000005e
#define __Win32__FILE_DEVICE_EVENT_COLLECTOR     0x0000005f
#define __Win32__FILE_DEVICE_USB4                0x00000060
#define __Win32__FILE_DEVICE_SOUNDWIRE           0x00000061
#define __Win32__FILE_DEVICE_FABRIC_NVME         0x00000062
#define __Win32__FILE_DEVICE_SVM                 0x00000063
#define __Win32__FILE_DEVICE_HARDWARE_ACCELERATOR 0x00000064
#define __Win32__FILE_DEVICE_I3C                 0x00000065

#define __Win32__METHOD_BUFFERED                 0
#define __Win32__METHOD_IN_DIRECT                1
#define __Win32__METHOD_OUT_DIRECT               2
#define __Win32__METHOD_NEITHER                  3

#define __Win32__FILE_ANY_ACCESS                 0
#define __Win32__FILE_SPECIAL_ACCESS    (__Win32__FILE_ANY_ACCESS)
#define __Win32__FILE_READ_ACCESS          ( 0x0001 )    // file & pipe
#define __Win32__FILE_WRITE_ACCESS         ( 0x0002 )    // file & pipe

#define __Win32__CTL_CODE( DeviceType, Function, Method, Access ) (                 \
    ((DeviceType) << 16) | ((Access) << 14) | ((Function) << 2) | (Method) \
)

#define __Win32__FSCTL_SET_REPARSE_POINT         __Win32__CTL_CODE(__Win32__FILE_DEVICE_FILE_SYSTEM, 41, __Win32__METHOD_BUFFERED, __Win32__FILE_SPECIAL_ACCESS) // REPARSE_DATA_BUFFER,
#define __Win32__FSCTL_GET_REPARSE_POINT         __Win32__CTL_CODE(__Win32__FILE_DEVICE_FILE_SYSTEM, 42, __Win32__METHOD_BUFFERED, __Win32__FILE_ANY_ACCESS) // REPARSE_DATA_BUFFER
#define __Win32__FSCTL_DELETE_REPARSE_POINT      __Win32__CTL_CODE(__Win32__FILE_DEVICE_FILE_SYSTEM, 43, __Win32__METHOD_BUFFERED, __Win32__FILE_SPECIAL_ACCESS) // REPARSE_DATA_BUFFER,

#define __Win32__MAXIMUM_REPARSE_DATA_BUFFER_SIZE      ( 16 * 1024 )

//#if (NTDDI_VERSION >= NTDDI_LONGHORN)
typedef enum _FILE_INFO_BY_HANDLE_CLASS {
    FileBasicInfo,
    FileStandardInfo,
    FileNameInfo,
    FileRenameInfo,
    FileDispositionInfo,
    FileAllocationInfo,
    FileEndOfFileInfo,
    FileStreamInfo,
    FileCompressionInfo,
    FileAttributeTagInfo,
    FileIdBothDirectoryInfo,
    FileIdBothDirectoryRestartInfo,
    FileIoPriorityHintInfo,
    FileRemoteProtocolInfo,
    FileFullDirectoryInfo,
    FileFullDirectoryRestartInfo,
//#if (NTDDI_VERSION >= NTDDI_WIN8)
    FileStorageInfo,
    FileAlignmentInfo,
    FileIdInfo,
    FileIdExtdDirectoryInfo,
    FileIdExtdDirectoryRestartInfo,
//#endif
//#if (NTDDI_VERSION >= NTDDI_WIN10_RS1)
    FileDispositionInfoEx,
    FileRenameInfoEx,
//#endif
//#if (NTDDI_VERSION >= NTDDI_WIN10_19H1)
    FileCaseSensitiveInfo,
    FileNormalizedNameInfo,
//#endif
    MaximumFileInfoByHandleClass
} FILE_INFO_BY_HANDLE_CLASS, *PFILE_INFO_BY_HANDLE_CLASS;

//#endif // (NTDDI_VERSION >= NTDDI_LONGHORN)

//#if (NTDDI_VERSION >= NTDDI_WIN11_ZN)

typedef struct _FILE_STAT_BASIC_INFORMATION {
    LARGE_INTEGER FileId;
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER AllocationSize;
    LARGE_INTEGER EndOfFile;
    DWORD FileAttributes;
    DWORD ReparseTag;
    DWORD NumberOfLinks;
    DWORD DeviceType;
    DWORD DeviceCharacteristics;
    DWORD Reserved;
    LARGE_INTEGER VolumeSerialNumber;
    FILE_ID_128 FileId128;
} FILE_STAT_BASIC_INFORMATION, *PFILE_STAT_BASIC_INFORMATION;

typedef enum _FILE_INFO_BY_NAME_CLASS {
    FileStatByNameInfo,
    FileStatLxByNameInfo,
    FileCaseSensitiveByNameInfo,
    FileStatBasicByNameInfo,
    MaximumFileInfoByNameClass
} FILE_INFO_BY_NAME_CLASS, *PFILE_INFO_BY_NAME_CLASS;

//#endif // (NTDDI_VERSION >= NTDDI_WIN11_ZN)

//#if (_WIN32_WINNT >= 0x0600)

typedef struct _FILE_DISPOSITION_INFO {
    BOOLEAN DeleteFile;
} FILE_DISPOSITION_INFO, *PFILE_DISPOSITION_INFO;

typedef struct _FILE_ATTRIBUTE_TAG_INFO {
    DWORD FileAttributes;
    DWORD ReparseTag;
} FILE_ATTRIBUTE_TAG_INFO, *PFILE_ATTRIBUTE_TAG_INFO;

//#endif // (_WIN32_WINNT >= 0x0600)

typedef struct _FILE_STAT_INFORMATION {
    LARGE_INTEGER FileId;
    LARGE_INTEGER CreationTime;
    LARGE_INTEGER LastAccessTime;
    LARGE_INTEGER LastWriteTime;
    LARGE_INTEGER ChangeTime;
    LARGE_INTEGER AllocationSize;
    LARGE_INTEGER EndOfFile;
    DWORD FileAttributes;
    DWORD ReparseTag;
    DWORD NumberOfLinks;
    ACCESS_MASK EffectiveAccess;
} FILE_STAT_INFORMATION, *PFILE_STAT_INFORMATION;

typedef void (__stdcall *LPOVERLAPPED_COMPLETION_ROUTINE)(DWORD, DWORD, LPOVERLAPPED);

#define __Win32__STD_INPUT_HANDLE    ((_PlatformDetails::DWORD)-10)
#define __Win32__STD_OUTPUT_HANDLE   ((_PlatformDetails::DWORD)-11)
#define __Win32__STD_ERROR_HANDLE    ((_PlatformDetails::DWORD)-12)

#define __Win32__ENABLE_PROCESSED_INPUT              0x0001
#define __Win32__ENABLE_LINE_INPUT                   0x0002
#define __Win32__ENABLE_ECHO_INPUT                   0x0004
#define __Win32__ENABLE_WINDOW_INPUT                 0x0008
#define __Win32__ENABLE_MOUSE_INPUT                  0x0010
#define __Win32__ENABLE_INSERT_MODE                  0x0020
#define __Win32__ENABLE_QUICK_EDIT_MODE              0x0040
#define __Win32__ENABLE_EXTENDED_FLAGS               0x0080
#define __Win32__ENABLE_AUTO_POSITION                0x0100
#define __Win32__ENABLE_VIRTUAL_TERMINAL_INPUT       0x0200

#define __Win32__ENABLE_PROCESSED_OUTPUT             0x0001
#define __Win32__ENABLE_WRAP_AT_EOL_OUTPUT           0x0002
#define __Win32__ENABLE_VIRTUAL_TERMINAL_PROCESSING  0x0004
#define __Win32__DISABLE_NEWLINE_AUTO_RETURN         0x0008
#define __Win32__ENABLE_LVB_GRID_WORLDWIDE           0x0010

#define __Win32__MAX_PATH 260

#define __Win32__SYMBOLIC_LINK_FLAG_DIRECTORY                    (0x1)
#define __Win32__SYMBOLIC_LINK_FLAG_ALLOW_UNPRIVILEGED_CREATE    (0x2)

#define __Win32__VOLUME_NAME_DOS  0x0      //default
#define __Win32__VOLUME_NAME_GUID 0x1
#define __Win32__VOLUME_NAME_NT   0x2
#define __Win32__VOLUME_NAME_NONE 0x4

#define __Win32__FILE_NAME_NORMALIZED 0x0  //default
#define __Win32__FILE_NAME_OPENED     0x8

#define __Win32__STATUS_WAIT_0                           ((_PlatformDetails::DWORD   )0x00000000L) 
#define __Win32__STATUS_ABANDONED_WAIT_0          ((_PlatformDetails::DWORD   )0x00000080L)    
#define __Win32__STATUS_USER_APC                  ((_PlatformDetails::DWORD   )0x000000C0L)    

#define __Win32__FILE_BEGIN           0
#define __Win32__FILE_CURRENT         1
#define __Win32__FILE_END             2

#define __Win32__INFINITE            0xFFFFFFFF  // Infinite timeout

#define __Win32__WAIT_FAILED ((_PlatformDetails::DWORD)0xFFFFFFFF)
#define __Win32__WAIT_OBJECT_0       ((__Win32__STATUS_WAIT_0 ) + 0 )

#define __Win32__WAIT_ABANDONED         ((__Win32__STATUS_ABANDONED_WAIT_0 ) + 0 )
#define __Win32__WAIT_ABANDONED_0       ((__Win32__STATUS_ABANDONED_WAIT_0 ) + 0 )

#define __Win32__WAIT_IO_COMPLETION                  __Win32__STATUS_USER_APC

#define __Win32__INVALID_HANDLE_VALUE ((_PlatformDetails::HANDLE)(_PlatformDetails::LONG_PTR)-1)

#define __Win32__FILE_READ_DATA            ( 0x0001 )    // file & pipe
#define __Win32__FILE_LIST_DIRECTORY       ( 0x0001 )    // directory

#define __Win32__FILE_WRITE_DATA           ( 0x0002 )    // file & pipe
#define __Win32__FILE_ADD_FILE             ( 0x0002 )    // directory

#define __Win32__FILE_APPEND_DATA          ( 0x0004 )    // file
#define __Win32__FILE_ADD_SUBDIRECTORY     ( 0x0004 )    // directory
#define __Win32__FILE_CREATE_PIPE_INSTANCE ( 0x0004 )    // named pipe


#define __Win32__FILE_READ_EA              ( 0x0008 )    // file & directory

#define __Win32__FILE_WRITE_EA             ( 0x0010 )    // file & directory

#define __Win32__FILE_EXECUTE              ( 0x0020 )    // file
#define __Win32__FILE_TRAVERSE             ( 0x0020 )    // directory

#define __Win32__FILE_DELETE_CHILD         ( 0x0040 )    // directory

#define __Win32__FILE_READ_ATTRIBUTES      ( 0x0080 )    // all

#define __Win32__FILE_WRITE_ATTRIBUTES     ( 0x0100 )    // all

#define __Win32__FILE_ALL_ACCESS (__Win32__STANDARD_RIGHTS_REQUIRED | __Win32__SYNCHRONIZE | 0x1FF)

#define __Win32__FILE_GENERIC_READ         (__Win32__STANDARD_RIGHTS_READ     |\
                                   __Win32__FILE_READ_DATA           |\
                                   __Win32__FILE_READ_ATTRIBUTES     |\
                                   __Win32__FILE_READ_EA             |\
                                   __Win32__SYNCHRONIZE)


#define __Win32__FILE_GENERIC_WRITE        (__Win32__STANDARD_RIGHTS_WRITE    |\
                                   __Win32__FILE_WRITE_DATA          |\
                                   __Win32__FILE_WRITE_ATTRIBUTES    |\
                                   __Win32__FILE_WRITE_EA            |\
                                   __Win32__FILE_APPEND_DATA         |\
                                   __Win32__SYNCHRONIZE)


#define __Win32__FILE_GENERIC_EXECUTE      (__Win32__STANDARD_RIGHTS_EXECUTE  |\
                                   __Win32__FILE_READ_ATTRIBUTES     |\
                                   __Win32__FILE_EXECUTE             |\
                                   __Win32__SYNCHRONIZE)

#define __Win32__FILE_SHARE_READ                 0x00000001  
#define __Win32__FILE_SHARE_WRITE                0x00000002  
#define __Win32__FILE_SHARE_DELETE               0x00000004  
#define __Win32__FILE_ATTRIBUTE_READONLY             0x00000001  
#define __Win32__FILE_ATTRIBUTE_HIDDEN               0x00000002  
#define __Win32__FILE_ATTRIBUTE_SYSTEM               0x00000004  
#define __Win32__FILE_ATTRIBUTE_DIRECTORY            0x00000010  
#define __Win32__FILE_ATTRIBUTE_ARCHIVE              0x00000020  
#define __Win32__FILE_ATTRIBUTE_DEVICE               0x00000040  
#define __Win32__FILE_ATTRIBUTE_NORMAL               0x00000080  
#define __Win32__FILE_ATTRIBUTE_TEMPORARY            0x00000100  
#define __Win32__FILE_ATTRIBUTE_SPARSE_FILE          0x00000200  
#define __Win32__FILE_ATTRIBUTE_REPARSE_POINT        0x00000400  
#define __Win32__FILE_ATTRIBUTE_COMPRESSED           0x00000800  
#define __Win32__FILE_ATTRIBUTE_OFFLINE              0x00001000  
#define __Win32__FILE_ATTRIBUTE_NOT_CONTENT_INDEXED  0x00002000  
#define __Win32__FILE_ATTRIBUTE_ENCRYPTED            0x00004000  
#define __Win32__FILE_ATTRIBUTE_INTEGRITY_STREAM     0x00008000  
#define __Win32__FILE_ATTRIBUTE_VIRTUAL              0x00010000  
#define __Win32__FILE_ATTRIBUTE_NO_SCRUB_DATA        0x00020000  
#define __Win32__FILE_ATTRIBUTE_EA                   0x00040000  
#define __Win32__FILE_ATTRIBUTE_PINNED               0x00080000  
#define __Win32__FILE_ATTRIBUTE_UNPINNED             0x00100000  
#define __Win32__FILE_ATTRIBUTE_RECALL_ON_OPEN       0x00040000  
#define __Win32__FILE_ATTRIBUTE_RECALL_ON_DATA_ACCESS 0x00400000 
#define __Win32__TREE_CONNECT_ATTRIBUTE_PRIVACY      0x00004000  
#define __Win32__TREE_CONNECT_ATTRIBUTE_INTEGRITY    0x00008000  
#define __Win32__TREE_CONNECT_ATTRIBUTE_GLOBAL       0x00000004  
#define __Win32__TREE_CONNECT_ATTRIBUTE_PINNED       0x00000002  
#define __Win32__FILE_ATTRIBUTE_STRICTLY_SEQUENTIAL  0x20000000  
#define __Win32__FILE_NOTIFY_CHANGE_FILE_NAME    0x00000001   
#define __Win32__FILE_NOTIFY_CHANGE_DIR_NAME     0x00000002   
#define __Win32__FILE_NOTIFY_CHANGE_ATTRIBUTES   0x00000004   
#define __Win32__FILE_NOTIFY_CHANGE_SIZE         0x00000008   
#define __Win32__FILE_NOTIFY_CHANGE_LAST_WRITE   0x00000010   
#define __Win32__FILE_NOTIFY_CHANGE_LAST_ACCESS  0x00000020   
#define __Win32__FILE_NOTIFY_CHANGE_CREATION     0x00000040   
#define __Win32__FILE_NOTIFY_CHANGE_SECURITY     0x00000100   
#define __Win32__FILE_ACTION_ADDED                   0x00000001   
#define __Win32__FILE_ACTION_REMOVED                 0x00000002   
#define __Win32__FILE_ACTION_MODIFIED                0x00000003   
#define __Win32__FILE_ACTION_RENAMED_OLD_NAME        0x00000004   
#define __Win32__FILE_ACTION_RENAMED_NEW_NAME        0x00000005   
#define __Win32__MAILSLOT_NO_MESSAGE             ((_PlatformDetails::DWORD)-1) 
#define __Win32__MAILSLOT_WAIT_FOREVER           ((_PlatformDetails::DWORD)-1) 
#define __Win32__FILE_CASE_SENSITIVE_SEARCH          0x00000001  
#define __Win32__FILE_CASE_PRESERVED_NAMES           0x00000002  
#define __Win32__FILE_UNICODE_ON_DISK                0x00000004  
#define __Win32__FILE_PERSISTENT_ACLS                0x00000008  
#define __Win32__FILE_FILE_COMPRESSION               0x00000010  
#define __Win32__FILE_VOLUME_QUOTAS                  0x00000020  
#define __Win32__FILE_SUPPORTS_SPARSE_FILES          0x00000040  
#define __Win32__FILE_SUPPORTS_REPARSE_POINTS        0x00000080  
#define __Win32__FILE_SUPPORTS_REMOTE_STORAGE        0x00000100  
#define __Win32__FILE_RETURNS_CLEANUP_RESULT_INFO    0x00000200  
#define __Win32__FILE_SUPPORTS_POSIX_UNLINK_RENAME   0x00000400  
#define __Win32__FILE_SUPPORTS_BYPASS_IO             0x00000800  
#define __Win32__FILE_SUPPORTS_STREAM_SNAPSHOTS      0x00001000  
#define __Win32__FILE_SUPPORTS_CASE_SENSITIVE_DIRS   0x00002000  

#define __Win32__FILE_VOLUME_IS_COMPRESSED           0x00008000  
#define __Win32__FILE_SUPPORTS_OBJECT_IDS            0x00010000  
#define __Win32__FILE_SUPPORTS_ENCRYPTION            0x00020000  
#define __Win32__FILE_NAMED_STREAMS                  0x00040000  
#define __Win32__FILE_READ_ONLY_VOLUME               0x00080000  
#define __Win32__FILE_SEQUENTIAL_WRITE_ONCE          0x00100000  
#define __Win32__FILE_SUPPORTS_TRANSACTIONS          0x00200000  
#define __Win32__FILE_SUPPORTS_HARD_LINKS            0x00400000  
#define __Win32__FILE_SUPPORTS_EXTENDED_ATTRIBUTES   0x00800000  
#define __Win32__FILE_SUPPORTS_OPEN_BY_FILE_ID       0x01000000  
#define __Win32__FILE_SUPPORTS_USN_JOURNAL           0x02000000  
#define __Win32__FILE_SUPPORTS_INTEGRITY_STREAMS     0x04000000  
#define __Win32__FILE_SUPPORTS_BLOCK_REFCOUNTING     0x08000000  
#define __Win32__FILE_SUPPORTS_SPARSE_VDL            0x10000000  
#define __Win32__FILE_DAX_VOLUME                     0x20000000  
#define __Win32__FILE_SUPPORTS_GHOSTING              0x40000000  

#define __Win32__CREATE_NEW          1
#define __Win32__CREATE_ALWAYS       2
#define __Win32__OPEN_EXISTING       3
#define __Win32__OPEN_ALWAYS         4
#define __Win32__TRUNCATE_EXISTING   5

#define __Win32__INVALID_FILE_SIZE ((_PlatformDetails::DWORD)0xFFFFFFFF)
#define __Win32__INVALID_SET_FILE_POINTER ((_PlatformDetails::DWORD)-1)
#define __Win32__INVALID_FILE_ATTRIBUTES ((_PlatformDetails::DWORD)-1)

#define __Win32__FILE_FLAG_WRITE_THROUGH         0x80000000
#define __Win32__FILE_FLAG_OVERLAPPED            0x40000000
#define __Win32__FILE_FLAG_NO_BUFFERING          0x20000000
#define __Win32__FILE_FLAG_RANDOM_ACCESS         0x10000000
#define __Win32__FILE_FLAG_SEQUENTIAL_SCAN       0x08000000
#define __Win32__FILE_FLAG_DELETE_ON_CLOSE       0x04000000
#define __Win32__FILE_FLAG_BACKUP_SEMANTICS      0x02000000
#define __Win32__FILE_FLAG_POSIX_SEMANTICS       0x01000000
#define __Win32__FILE_FLAG_SESSION_AWARE         0x00800000
#define __Win32__FILE_FLAG_OPEN_REPARSE_POINT    0x00200000
#define __Win32__FILE_FLAG_OPEN_NO_RECALL        0x00100000
#define __Win32__FILE_FLAG_FIRST_PIPE_INSTANCE   0x00080000

#define __Win32__DELETE                           (0x00010000L)
#define __Win32__READ_CONTROL                     (0x00020000L)
#define __Win32__WRITE_DAC                        (0x00040000L)
#define __Win32__WRITE_OWNER                      (0x00080000L)
#define __Win32__SYNCHRONIZE                      (0x00100000L)

#define __Win32__STANDARD_RIGHTS_REQUIRED         (0x000F0000L)

#define __Win32__STANDARD_RIGHTS_READ             (__Win32__READ_CONTROL)
#define __Win32__STANDARD_RIGHTS_WRITE            (__Win32__READ_CONTROL)
#define __Win32__STANDARD_RIGHTS_EXECUTE          (__Win32__READ_CONTROL)

#define __Win32__STANDARD_RIGHTS_ALL              (0x001F0000L)

#define __Win32__SPECIFIC_RIGHTS_ALL              (0x0000FFFFL)

#define __Win32__ACCESS_SYSTEM_SECURITY           (0x01000000L)

#define __Win32__MAXIMUM_ALLOWED                  (0x02000000L)

#define __Win32__GENERIC_READ                     (0x80000000L)
#define __Win32__GENERIC_WRITE                    (0x40000000L)
#define __Win32__GENERIC_EXECUTE                  (0x20000000L)
#define __Win32__GENERIC_ALL                      (0x10000000L)

#define __Win32__FORMAT_MESSAGE_IGNORE_INSERTS  0x00000200
#define __Win32__FORMAT_MESSAGE_FROM_STRING     0x00000400
#define __Win32__FORMAT_MESSAGE_FROM_HMODULE    0x00000800
#define __Win32__FORMAT_MESSAGE_FROM_SYSTEM     0x00001000

#define __Win32__MAKELANGID(p, s)       ((((WORD  )(s)) << 10) | (WORD  )(p))
#define __Win32__LANG_NEUTRAL                     0x00
#define __Win32__SUBLANG_DEFAULT                             0x01    // user default

#define __Win32__FILE_TYPE_UNKNOWN   0x0000
#define __Win32__FILE_TYPE_DISK      0x0001
#define __Win32__FILE_TYPE_CHAR      0x0002
#define __Win32__FILE_TYPE_PIPE      0x0003
#define __Win32__FILE_TYPE_REMOTE    0x8000

#define __Win32__IO_REPARSE_TAG_RESERVED_INVALID         (0xC0008000L)
#define __Win32__IO_REPARSE_TAG_MOUNT_POINT              (0xA0000003L)
#define __Win32__IO_REPARSE_TAG_HSM                      (0xC0000004L)
#define __Win32__IO_REPARSE_TAG_HSM2                     (0x80000006L)
#define __Win32__IO_REPARSE_TAG_SIS                      (0x80000007L)
#define __Win32__IO_REPARSE_TAG_WIM                      (0x80000008L)
#define __Win32__IO_REPARSE_TAG_CSV                      (0x80000009L)
#define __Win32__IO_REPARSE_TAG_DFS                      (0x8000000AL)
#define __Win32__IO_REPARSE_TAG_SYMLINK                  (0xA000000CL)
#define __Win32__IO_REPARSE_TAG_DFSR                     (0x80000012L)
#define __Win32__IO_REPARSE_TAG_DEDUP                    (0x80000013L)
#define __Win32__IO_REPARSE_TAG_NFS                      (0x80000014L)
#define __Win32__IO_REPARSE_TAG_FILE_PLACEHOLDER         (0x80000015L)
#define __Win32__IO_REPARSE_TAG_WOF                      (0x80000017L)
#define __Win32__IO_REPARSE_TAG_WCI                      (0x80000018L)
#define __Win32__IO_REPARSE_TAG_WCI_1                    (0x90001018L)
#define __Win32__IO_REPARSE_TAG_GLOBAL_REPARSE           (0xA0000019L)
#define __Win32__IO_REPARSE_TAG_CLOUD                    (0x9000001AL)
#define __Win32__IO_REPARSE_TAG_CLOUD_1                  (0x9000101AL)
#define __Win32__IO_REPARSE_TAG_CLOUD_2                  (0x9000201AL)
#define __Win32__IO_REPARSE_TAG_CLOUD_3                  (0x9000301AL)
#define __Win32__IO_REPARSE_TAG_CLOUD_4                  (0x9000401AL)
#define __Win32__IO_REPARSE_TAG_CLOUD_5                  (0x9000501AL)
#define __Win32__IO_REPARSE_TAG_CLOUD_6                  (0x9000601AL)
#define __Win32__IO_REPARSE_TAG_CLOUD_7                  (0x9000701AL)
#define __Win32__IO_REPARSE_TAG_CLOUD_8                  (0x9000801AL)
#define __Win32__IO_REPARSE_TAG_CLOUD_9                  (0x9000901AL)
#define __Win32__IO_REPARSE_TAG_CLOUD_A                  (0x9000A01AL)
#define __Win32__IO_REPARSE_TAG_CLOUD_B                  (0x9000B01AL)
#define __Win32__IO_REPARSE_TAG_CLOUD_C                  (0x9000C01AL)
#define __Win32__IO_REPARSE_TAG_CLOUD_D                  (0x9000D01AL)
#define __Win32__IO_REPARSE_TAG_CLOUD_E                  (0x9000E01AL)
#define __Win32__IO_REPARSE_TAG_CLOUD_F                  (0x9000F01AL)
#define __Win32__IO_REPARSE_TAG_CLOUD_MASK               (0x0000F000L)
#define __Win32__IO_REPARSE_TAG_APPEXECLINK              (0x8000001BL)
#define __Win32__IO_REPARSE_TAG_PROJFS                   (0x9000001CL)
#define __Win32__IO_REPARSE_TAG_STORAGE_SYNC             (0x8000001EL)
#define __Win32__IO_REPARSE_TAG_WCI_TOMBSTONE            (0xA000001FL)
#define __Win32__IO_REPARSE_TAG_UNHANDLED                (0x80000020L)
#define __Win32__IO_REPARSE_TAG_ONEDRIVE                 (0x80000021L)
#define __Win32__IO_REPARSE_TAG_PROJFS_TOMBSTONE         (0xA0000022L)
#define __Win32__IO_REPARSE_TAG_AF_UNIX                  (0x80000023L)
#define __Win32__IO_REPARSE_TAG_STORAGE_SYNC_FOLDER      (0x90000027L)
#define __Win32__IO_REPARSE_TAG_WCI_LINK                 (0xA0000027L)
#define __Win32__IO_REPARSE_TAG_WCI_LINK_1               (0xA0001027L)
#define __Win32__IO_REPARSE_TAG_DATALESS_CIM             (0xA0000028L)

#define __Win32__IO_REPARSE_TAG_LX_SYMLINK               (0xA000001DL)

} // namespace _PlatformDetails
} // namespace Alt

#endif // ALT_OS_WINDOWS

#endif // __ALT_STD_PLATFORM_WIN32_DEFINITIONS__