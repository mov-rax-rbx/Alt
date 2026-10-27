#pragma once

#ifndef __ALT_STD_PLATFORM_WIN32__
#define __ALT_STD_PLATFORM_WIN32__

#include "../../../Defines.h"

#ifdef ALT_OS_WINDOWS

#include "PlatformWin32Definitions.h"

#include "../../../Platform/Impl/PlatformPreInclude.h"

#include "../../../Platform/Impl/Win32/PlatformWin32AtomicsCore.h"
#include "../../../Platform/Impl/Win32/PlatformWin32Atomics8.h"
#include "../../../Platform/Impl/Win32/PlatformWin32Atomics16.h"
#include "../../../Platform/Impl/Win32/PlatformWin32Atomics32.h"
#include "../../../Platform/Impl/Win32/PlatformWin32Atomics64.h"
#include "../../../Platform/Impl/Win32/PlatformWin32Atomics128.h"

#include "../../../String/String.h"
#include "../../../String/StringUTF16.h"
#include "../../../Collections/Vector.h"
#include "../../../Collections/StaticVector.h"
#include "../../../Iterators/Iterator.h"

#include "../../../Utils/Func.h"
#include "../../../Utils/Time.h"
#include "../../../Utils/NonCopyableMovable.h"

#ifdef ALT_COMPILER_CLANG
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-function-type-mismatch"
#endif // ALT_COMPILER_CLANG

extern "C" {

// Minimum supported client	Windows XP [desktop apps only]
// Minimum supported server	Windows Server 2003 [desktop apps only]
__declspec(dllimport) HMODULE __stdcall GetModuleHandleW(LPCWSTR);

// Minimum supported client	Windows XP [desktop apps | UWP apps]
// Minimum supported server	Windows Server 2003 [desktop apps | UWP apps]
__declspec(dllimport) FARPROC __stdcall GetProcAddress(HMODULE, LPCSTR);
}

namespace Alt {

#define ALT_WRITE_BUFFERED_CONSOLE(...) \
    Alt::_PlatformDetails::GetString<Alt::_PlatformDetails::StdIo::out>().FormatAppend(__VA_ARGS__);

#define ALT_FLUSH_BUFFERED_CONSOLE() \
do { \
    ALT_MAYBE_UNUSED Alt::Result<void, Alt::Platform::Error> __ALT_STD_CONCAT_MACROS__(_err_, __COUNTER__) = \
        Alt::Platform::ConsoleWrite(Alt::_PlatformDetails::StdIo::out, Alt::_PlatformDetails::GetString<Alt::_PlatformDetails::StdIo::out>()); \
    Alt::_PlatformDetails::GetString<Alt::_PlatformDetails::StdIo::out>().Clear(); \
} while(0)

#define ALT_EWRITE_BUFFERED_CONSOLE(...) \
    Alt::_PlatformDetails::GetString<Alt::_PlatformDetails::StdIo::err>().FormatAppend(__VA_ARGS__);

#define ALT_EFLUSH_BUFFERED_CONSOLE() \
do { \
    ALT_MAYBE_UNUSED Alt::Result<void, Alt::Platform::Error> __ALT_STD_CONCAT_MACROS__(_err_, __COUNTER__) = \
        Alt::Platform::ConsoleWrite(Alt::_PlatformDetails::StdIo::err, Alt::_PlatformDetails::GetString<Alt::_PlatformDetails::StdIo::err>()); \
    Alt::_PlatformDetails::GetString<Alt::_PlatformDetails::StdIo::err>().Clear(); \
} while(0)

#define ALT_PRINT(...) Alt::_PlatformDetails::Print_(__VA_ARGS__);
#define ALT_PRINTLN(...) Alt::_PlatformDetails::Println_(__VA_ARGS__);

#define ALT_EPRINT(...) Alt::_PlatformDetails::EPrint_(__VA_ARGS__);
#define ALT_EPRINTLN(...) Alt::_PlatformDetails::EPrintln_(__VA_ARGS__);

#ifdef ALT_DEV
    #define ALT_PANIC(...) \
    do { \
        ALT_EWRITE_BUFFERED_CONSOLE("{}:{}: ", __FILE__, (Alt::usize)__LINE__); \
        ALT_EWRITE_BUFFERED_CONSOLE(__VA_ARGS__); \
        ALT_EFLUSH_BUFFERED_CONSOLE(); \
        ALT_PANIC_SILENT(); \
    } while(0)
#else
    #define ALT_PANIC(...) \
    do { \
        ALT_EPRINT(__VA_ARGS__); \
        ALT_PANIC_SILENT(); \
    } while(0)
#endif // ALT_DEV

#define ALT_COMPILER_BARRIER _ReadWriteBarrier()

#define ALT_TIME_BENCH(name, iterations, codeBlock) \
do { \
    Alt::_PlatformDetails::FuncNoInline([&]() { \
        iferr_scope; \
        Alt::Platform::Error err; \
        const f64 start = Alt::Platform::GetTicksNs(); \
        const Alt::Platform::SysTime sysTimeStart = Alt::Platform::PerformanceGetCurrentProcessSysTime() iferr_panic_with(err, "{}", err); \
        const Alt::Optional<u64> cyclesStart = Alt::Platform::PerformanceGetCurrentProcessCycles().Map([](const u64 result) -> Alt::Optional<u64> { return Alt::MakeOptional(result); }).GetOkOr(Alt::Nullopt); \
        for (usize i = 0; i < iterations; ++i) { codeBlock } \
        const Alt::Optional<u64> cyclesEnd = Alt::Platform::PerformanceGetCurrentProcessCycles().Map([](const u64 result) -> Alt::Optional<u64> { return Alt::MakeOptional(result); }).GetOkOr(Alt::Nullopt); \
        const Alt::Platform::SysTime sysTimeEnd = Alt::Platform::PerformanceGetCurrentProcessSysTime() iferr_panic_with(err, "{}", err); \
        const f64 end = Alt::Platform::GetTicksNs(); \
        ALT_WRITE_BUFFERED_CONSOLE("[ALT BENCH] {}:\n", name); \
        ALT_WRITE_BUFFERED_CONSOLE("            elapsed - {}ms\n", (end - start) / 1'000'000.0); \
        ALT_WRITE_BUFFERED_CONSOLE("            elapsed - {}ns\n", end - start); \
        ALT_WRITE_BUFFERED_CONSOLE("        avg elapsed - {}ms\n", (end - start) / 1'000'000.0 / static_cast<f32>(iterations)); \
        ALT_WRITE_BUFFERED_CONSOLE("        avg elapsed - {}ns\n", (end - start) / static_cast<f32>(iterations)); \
        if (cyclesStart) ALT_WRITE_BUFFERED_CONSOLE("             cycles - {}\n", *cyclesEnd - *cyclesStart); \
        if (cyclesStart) ALT_WRITE_BUFFERED_CONSOLE("         avg cycles - {}\n", static_cast<f32>(*cyclesEnd - *cyclesStart) / static_cast<f32>(iterations)); \
        ALT_WRITE_BUFFERED_CONSOLE("             kernel - {}ms\n", static_cast<f64>(sysTimeEnd.kernelTimeNS - sysTimeStart.kernelTimeNS) / 1'000'000.0); \
        ALT_WRITE_BUFFERED_CONSOLE("             kernel - {}ns\n", static_cast<f64>(sysTimeEnd.kernelTimeNS - sysTimeStart.kernelTimeNS)); \
        ALT_WRITE_BUFFERED_CONSOLE("         avg kernel - {}ms\n", static_cast<f64>(sysTimeEnd.kernelTimeNS - sysTimeStart.kernelTimeNS) / static_cast<f64>(iterations) / 1'000'000.0); \
        ALT_WRITE_BUFFERED_CONSOLE("         avg kernel - {}ns\n", static_cast<f64>(sysTimeEnd.kernelTimeNS - sysTimeStart.kernelTimeNS) / static_cast<f64>(iterations)); \
        ALT_WRITE_BUFFERED_CONSOLE("               user - {}ms\n", static_cast<f64>(sysTimeEnd.userTimeNS - sysTimeStart.userTimeNS) / 1'000'000.0); \
        ALT_WRITE_BUFFERED_CONSOLE("               user - {}ns\n", static_cast<f64>(sysTimeEnd.userTimeNS - sysTimeStart.userTimeNS)); \
        ALT_WRITE_BUFFERED_CONSOLE("           avg user - {}ms\n", static_cast<f64>(sysTimeEnd.userTimeNS - sysTimeStart.userTimeNS) / static_cast<f64>(iterations) / 1'000'000.0); \
        ALT_WRITE_BUFFERED_CONSOLE("           avg user - {}ns\n", static_cast<f64>(sysTimeEnd.userTimeNS - sysTimeStart.userTimeNS) / static_cast<f64>(iterations)); \
        ALT_FLUSH_BUFFERED_CONSOLE(); \
    }); \
} while(0)

namespace _PlatformDetails {

class PlatformInstance;
class Thread;
class Mutex;
class Semaphore;
class Latch;

enum class StdIo : usize;

enum class Utf16BufferAppendBits : usize {

    None = 0,

    PathBuffer = 1 << 1,
    DoubleNullTerm = 1 << 2,
};
ALT_ENUM_CLASS_BIT_FLAGS_IMPL(Utf16BufferAppendBits);

template <typename FuncT>
ALT_NOINLINE void FuncNoInline(FuncT&& func) noexcept {
    (func)();
}

} // namespace _PlatformDetails

namespace Platform {

/*
 * https://cppreference.com/cpp/atomic/memory_order
 */
enum class MemoryOrder : usize {
    Relaxed,
    Acquire,
    Release,
    AcqRel,
    SeqCst,
};

/*
 * https://learn.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences
 */
struct VTS_t {};
constexpr static VTS_t VTS = VTS_t{};

/*
 * ALT Virtual Terminal Sequence, see. 'Format<Platform::VTS_t>'
 * 
 * ALT_PRINTLN("{fg=green_bold;cm=right4}Text{default}", ALT_VTS, ALT_VTS)
 */
#define ALT_VTS Alt::Platform::VTS

/*
 * ALT Directory Separator
 */
#define ALT_DS '\\'

class Error;

ALT_NODISCARD inline Result<void, Error> ConsoleWrite(const _PlatformDetails::StdIo stdio, const Slice<const char16_t> data) noexcept;
ALT_NODISCARD inline Result<void, Error> ConsoleWrite(const _PlatformDetails::StdIo stdio, const Slice<const wchar_t> data) noexcept;
ALT_NODISCARD inline Result<void, Error> ConsoleWrite(const _PlatformDetails::StdIo stdio, const Slice<const char> data) noexcept;
ALT_NODISCARD inline Result<void, Error> ConsoleWrite(const _PlatformDetails::StdIo stdio, const char* pData) noexcept;

enum class CPUArchitecture : usize {

    Amd64 = 9, // x64 (AMD or Intel)

    Arm = 5, // ARM
    Arm64 = 12, // ARM64

    IA64 = 6, // Intel Itanium-based

    Intel = 0, // x86

    Unknown = 0xffff,
};

struct SysTime {

    u64 kernelTimeNS = 0;
    u64 userTimeNS = 0;
};

enum class ThreadPriority : s32 {

    Idle = __Win32__THREAD_PRIORITY_IDLE,

    Low = __Win32__THREAD_PRIORITY_LOWEST,
    Med = __Win32__THREAD_PRIORITY_BELOW_NORMAL,
    Norm = __Win32__THREAD_PRIORITY_NORMAL,
    High = __Win32__THREAD_PRIORITY_ABOVE_NORMAL,

    Highest = __Win32__THREAD_PRIORITY_HIGHEST,

    Critical = __Win32__THREAD_PRIORITY_TIME_CRITICAL,
};

enum class WaitOnAdressResult : usize {

    Signaled,
    Timeout,
};

enum class WaitForSingleObjectResult : usize {

    Abandoned = __Win32__WAIT_ABANDONED,
    Signaled = __Win32__WAIT_OBJECT_0,
    Timeout = __Win32__WAIT_TIMEOUT,
};

class FileLogicalDriversIterator;

enum class FilePathSeparatorBits : usize {

    ForwardSlash = 1 << 0, // '/'
    BackSlash = 1 << 1, // '\'

    Any = ForwardSlash | BackSlash,
};
ALT_ENUM_CLASS_BIT_FLAGS_IMPL(FilePathSeparatorBits);

struct FilePathSplitParentPathAndFilenameResult {

    Slice<const char> parentPath;
    Slice<const char> filename;
};

struct FilePathSplitResult {

    Slice<const char> parentPath;
    Slice<const char> filename;
    Slice<const char> stem;

    // extension including '.'
    Slice<const char> extension;
};

class PathOffset {
public:

    ALT_FORCE_INLINE PathOffset(const PathOffset&) noexcept = default;
    ALT_FORCE_INLINE PathOffset& operator=(const PathOffset&) noexcept = default;

    ALT_FORCE_INLINE PathOffset(PathOffset&&) noexcept = default;
    ALT_FORCE_INLINE PathOffset& operator=(PathOffset&&) noexcept = default;

    ALT_NODISCARD ALT_FORCE_INLINE usize GetOffset() const noexcept {
        return m_Offset;
    }

    ALT_NODISCARD ALT_FORCE_INLINE usize GetOffsetEnd() const noexcept {
        return m_OffsetEnd;
    }

    ALT_NODISCARD ALT_FORCE_INLINE bool IsEmpty() const noexcept {
        return m_Offset == m_OffsetEnd;
    }

    ALT_NODISCARD inline bool CloneIntoAppend(String& buffer) const noexcept;

private:

    ALT_FORCE_INLINE PathOffset() noexcept = default;
    ALT_IFERR_INJECT_DEFAULT_CONSTRUCTOR(PathOffset);

private:

    friend class _PlatformDetails::PlatformInstance;

    usize m_Offset = 0;
    usize m_OffsetEnd = 0;

    _PlatformDetails::Utf16BufferAppendBits m_BufferAppendBits = _PlatformDetails::Utf16BufferAppendBits::None;
};

enum class FileOpenOrCreateBits : usize {

    None = 0,
    Clear = 1 << 0,
    Open = 1 << 1,

    // see. 'FileDelete(void* handle)'
    DeleteByHandle = 1 << 2,
};
ALT_ENUM_CLASS_BIT_FLAGS_IMPL(FileOpenOrCreateBits);

enum class FileAccessBits : usize {

    Read = __Win32__GENERIC_READ,
    Write = __Win32__GENERIC_WRITE,

    All = Read | Write,
};
ALT_ENUM_CLASS_BIT_FLAGS_IMPL(FileAccessBits);

enum class FileShareBits : usize {

    None = 0,

    Delete = __Win32__FILE_SHARE_DELETE,

    // prefer to use 'Read' flags to allow open handle for other programs
    Read = __Win32__FILE_SHARE_READ,

    Write = __Win32__FILE_SHARE_WRITE,

    All = Delete | Read | Write,
};
ALT_ENUM_CLASS_BIT_FLAGS_IMPL(FileShareBits);

enum class FileHintBits : usize {

    None = 0,

    SequentialAccess = __Win32__FILE_FLAG_SEQUENTIAL_SCAN,
    RandomAccess = __Win32__FILE_FLAG_RANDOM_ACCESS,

    Temporary = __Win32__FILE_ATTRIBUTE_TEMPORARY,
};
ALT_ENUM_CLASS_BIT_FLAGS_IMPL(FileHintBits);

enum class FileCreateMapMemoryObjectBits : usize {

    Read = __Win32__PAGE_READONLY,
    ReadWrite = __Win32__PAGE_READWRITE,

    CopyOnWrite = __Win32__PAGE_WRITECOPY,
};
ALT_ENUM_CLASS_BIT_FLAGS_IMPL(FileCreateMapMemoryObjectBits);

enum class FileMapMemoryBits : usize {

    Read = __Win32__SECTION_MAP_READ,
    Write = __Win32__SECTION_MAP_WRITE,

    CopyOnWrite = __Win32__FILE_MAP_COPY,
};
ALT_ENUM_CLASS_BIT_FLAGS_IMPL(FileMapMemoryBits);

enum class FileSeekOpt : usize {

    Begin,
    End,
};

enum class FileTypeBits : usize {

    None = 0,

    File = 1 << 0,

    Directory = 1 << 1,

    SymLink = 1 << 2,
    Junction = 1 << 3,

    // file types
    RegularFile = 1 << 4,
    Char = 1 << 5,
    Pipe = 1 << 6,
};
ALT_ENUM_CLASS_BIT_FLAGS_IMPL(FileTypeBits);

enum class FileRemoveAllType : usize {

    All = 0,
    Inside = 1,
};

enum class SymLinkTarget : usize {

    Unknown,

    File,
    Directory,
};

struct FileFreeSpace {

    usize totalBytes = 0;
    usize totalFreeBytes = 0;
    usize totalFreeUserAvailableBytes = 0;
};

struct FileInfo {

    Time creationTime = Time{};
    Time lastAccessTime = Time{};
    Time lastWriteTime = Time{};
    Time lastChangeTime = Time{};

    usize numOfHardLinks = 0;

    usize allocatedSizeInBytes = 0;
    usize logicalSizeInBytes = 0;
};

enum class FileForEachDirectoryEntryBits : usize {

    AppendToString = 1 << 0,
    CreatePathOffset = 1 << 1,

    All = AppendToString | CreatePathOffset,
};
ALT_ENUM_CLASS_BIT_FLAGS_IMPL(FileForEachDirectoryEntryBits);

struct FileForEachDirectoryEntryDesc {

    usize sizeInBytesTmpData = Memory::KB(16);
    FileForEachDirectoryEntryBits bits = FileForEachDirectoryEntryBits::All;
};

struct FileDirectoryEntry {

    // used when 'FileForEachDirectoryEntryBits::AppendToString' bit specified,
    // pointers to the String buffer which passed to 'FileForEachDirectoryEntry(...)'
    // the lifetime until next memory reallocation of String buffer (next call of
    // foreach file directory or call of 'Append(...)')
    Optional<Slice<const char>> filename;

    // used when 'FileForEachDirectoryEntryBits::CreatePathOffset' bit specified
    // the lifetime bounded by function scope
    Optional<PathOffset> filepath;

    Time creationTime = Time{};
    Time lastAccessTime = Time{};
    Time lastWriteTime = Time{};
    Time lastChangeTime = Time{};

    usize allocatedSizeInBytes = 0;
    usize logicalSizeInBytes = 0;

    bool isDir = false;
    bool isSymLinkLike = false;
    bool isFile = false;
};

enum class FileForEachDirectoryEntryContinuation {

    Continue,
    Retry,
    Break,
};

enum class FileDialogOpenType {

    Open,
    SaveAs
};

struct FileDialogOpenDesc {

    struct Filter {

        // Geometry
        Slice<const char> name = "";

        // stl, gltf, obj
        StaticVector<Slice<const char>, 16> formats;
    };

    static inline const Filter ALL_FILES_FILTER = Filter{
        "All Files"_s, StaticVector<Slice<const char>, 16>{ "*"_s }
    };

    // "Geometry\0*.stl;*.gltf;*.obj\0\0"
    Slice<const Filter> filters = Slice<const Filter>{ &ALL_FILES_FILTER, 1 };
    usize filterByDefaultIdx = 0;

    usize sizeFilepathOut = Memory::KB(16);
};

class Error {
public:

    ALT_NODISCARD ALT_FORCE_INLINE static Error PlatformLastError(const Optional<usize> errCode = Nullopt) noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE static Error PlatformNtError(const _PlatformDetails::NTSTATUS ntstatus) noexcept;
    ALT_NODISCARD ALT_FORCE_INLINE static Error PlatformCommDlgExtendedError(const usize dlgError) noexcept;

    ALT_NODISCARD ALT_FORCE_INLINE static Error MemoryAllocation() noexcept {

        Error err{};
        err.m_ErrCode = static_cast<usize>(Type::MemoryAllocation);

        return err;
    }

    ALT_NODISCARD ALT_FORCE_INLINE static Error Unsupported() noexcept {

        Error err{};
        err.m_ErrCode = static_cast<usize>(Type::Unsupported);

        return err;
    }

    ALT_NODISCARD ALT_FORCE_INLINE static Error FileNotSymLink() noexcept {

        Error err{};
        err.m_ErrCode = static_cast<usize>(Type::FileNotSymLink);

        return err;
    }

    ALT_NODISCARD ALT_FORCE_INLINE bool IsPlatformError() const noexcept {

        return (m_ErrCode & RESERVATION_BIT) == 0;
    }

    ALT_NODISCARD ALT_FORCE_INLINE bool IsApplicationError() const noexcept {

        return !IsPlatformError();
    }

    ALT_NODISCARD ALT_FORCE_INLINE usize GetErrCode() const noexcept {

        return m_ErrCode;
    }

    ALT_NODISCARD ALT_FORCE_INLINE bool IsOk() const noexcept {

        return m_ErrCode == __Win32__ERROR_SUCCESS;
    }

    ALT_NODISCARD ALT_FORCE_INLINE bool IsErr() const noexcept {

        return m_ErrCode != __Win32__ERROR_SUCCESS;
    }

    ALT_NODISCARD ALT_FORCE_INLINE bool IsAccessDeniedErr() const noexcept {

        return m_ErrCode != __Win32__ERROR_ACCESS_DENIED;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> FormatEstimateExactSize() const noexcept {

        if (IsPlatformError()) {
            return Nullopt;
        }

        const char* str = APPLICATION_ERROR_TO_CSTR[m_ErrCode & ~PLATFORM_MASK_BIT];
        return String::CalculateCStrLen(str);
    }

    template <typename FormatWriterT>
    ALT_NODISCARD ALT_FORCE_INLINE bool FormatWriteAppend(FormatWriter<FormatWriterT>& writer) const noexcept;

private:

    static_assert(sizeof(usize) >= sizeof(u32), "");

    // use 29 bit - it is reserved for application error codes
    // https://learn.microsoft.com/en-us/windows/win32/api/errhandlingapi/nf-errhandlingapi-getlasterror
    constexpr static usize RESERVATION_BIT = static_cast<usize>(1) << 29;

    // Still Win32 errors but custom for example from `CommDlgExtendedError()`
    constexpr static usize CUSTOM_WIN32_BIT = static_cast<usize>(1) << 30 | RESERVATION_BIT;

    constexpr static usize PLATFORM_MASK_BIT = CUSTOM_WIN32_BIT;

    enum class Type : usize {

        // https://learn.microsoft.com/en-us/windows/win32/api/commdlg/nf-commdlg-commdlgextendederror

        // General errors
        CDERR_DIALOGFAILURE_   = CUSTOM_WIN32_BIT | 0,  // 0b01100000 00000000 00000000 00000000
        CDERR_FINDRESFAILURE_  = CUSTOM_WIN32_BIT | 1,  // 0b01100000 00000000 00000000 00000001
        CDERR_INITIALIZATION_  = CUSTOM_WIN32_BIT | 2,  // 0b01100000 00000000 00000000 00000010
        CDERR_LOADRESFAILURE_  = CUSTOM_WIN32_BIT | 3,  // 0b01100000 00000000 00000000 00000011
        CDERR_LOADSTRFAILURE_  = CUSTOM_WIN32_BIT | 4,  // 0b01100000 00000000 00000000 00000100
        CDERR_LOCKRESFAILURE_  = CUSTOM_WIN32_BIT | 5,  // 0b01100000 00000000 00000000 00000101
        CDERR_MEMALLOCFAILURE_ = CUSTOM_WIN32_BIT | 6,  // 0b01100000 00000000 00000000 00000110
        CDERR_MEMLOCKFAILURE_  = CUSTOM_WIN32_BIT | 7,  // 0b01100000 00000000 00000000 00000111
        CDERR_NOHINSTANCE_     = CUSTOM_WIN32_BIT | 8,  // 0b01100000 00000000 00000000 00001000
        CDERR_NOHOOK_          = CUSTOM_WIN32_BIT | 9,  // 0b01100000 00000000 00000000 00001001
        CDERR_NOTEMPLATE_      = CUSTOM_WIN32_BIT | 10, // 0b01100000 00000000 00000000 00001010
        CDERR_REGISTERMSGFAIL_ = CUSTOM_WIN32_BIT | 11, // 0b01100000 00000000 00000000 00001011
        CDERR_STRUCTSIZE_      = CUSTOM_WIN32_BIT | 12, // 0b01100000 00000000 00000000 00001100

        // File Dialog Open errors
        FNERR_BUFFERTOOSMALL_  = CUSTOM_WIN32_BIT | 13, // 0b01100000 00000000 00000000 00001101
        FNERR_INVALIDFILENAME_ = CUSTOM_WIN32_BIT | 14, // 0b01100000 00000000 00000000 00001110
        FNERR_SUBCLASSFAILURE_ = CUSTOM_WIN32_BIT | 15, // 0b01100000 00000000 00000000 00001111

        MemoryAllocation       = RESERVATION_BIT | 16, // 0b00100000 00000000 00000000 00010000
        Unsupported            = RESERVATION_BIT | 17, // 0b00100000 00000000 00000000 00010001
        FileNotSymLink         = RESERVATION_BIT | 18, // 0b00100000 00000000 00000000 00010010
    };

    constexpr static const char* APPLICATION_ERROR_TO_CSTR[]{

        "The dialog box could not be created"
        "The common dialog box function failed to find a specified resource"
        "The common dialog box function failed during initialization"
        "The common dialog box function failed to load a specified resource"
        "The common dialog box function failed to load a specified string"
        "The common dialog box function failed to lock a specified resource"
        "The common dialog box function was unable to allocate memory"
        "The common dialog box function was unable to lock the memory"
        "Failed to provide a corresponding instance handle to a template"
        "Failed to provide a pointer to a corresponding hook procedure"
        "Failed to provide a corresponding template"
        "The Register Window Message function returned an error code when it was called by the common dialog box function"
        "The member of the initialization structure for the corresponding common dialog box is invalid"

        "The buffer pointed to by the is too small for the file name"
        "A file name is invalid"
        "An attempt to subclass a list box failed because sufficient memory was not available"

        "Failed to allocate memory",
        "Functionality is not supported on the current platform version",
        "File not an symbolic link",
    };

private:

    usize m_ErrCode = __Win32__ERROR_SUCCESS;
};

class Handle {
private:

    friend class _PlatformDetails::PlatformInstance;
    friend class _PlatformDetails::Thread;
    friend class _PlatformDetails::Mutex;
    friend class _PlatformDetails::Semaphore;
    friend class _PlatformDetails::Latch;

    _PlatformDetails::HANDLE handle = __Win32__INVALID_HANDLE_VALUE;
};

class CriticalSection {
private:

    friend class _PlatformDetails::PlatformInstance;

    _PlatformDetails::CRITICAL_SECTION critsect = _PlatformDetails::CRITICAL_SECTION{};
};

class RWMutex {
private:

    friend class _PlatformDetails::PlatformInstance;

    _PlatformDetails::SRWLOCK mutex = __Win32__SRWLOCK_INIT;
};

} // namespace Platform

namespace _PlatformDetails {

template <typename ReturnT, typename ...ArgsT>
using PlatformFuncPtr = ReturnT(__stdcall *)(ArgsT...) noexcept;

class Utf16BufferOffset {
public:

    ALT_FORCE_INLINE Utf16BufferOffset(const Utf16BufferOffset&) noexcept = default;
    ALT_FORCE_INLINE Utf16BufferOffset& operator=(const Utf16BufferOffset&) noexcept = default;

    ALT_FORCE_INLINE Utf16BufferOffset(Utf16BufferOffset&&) noexcept = default;
    ALT_FORCE_INLINE Utf16BufferOffset& operator=(Utf16BufferOffset&&) noexcept = default;

    ALT_NODISCARD ALT_FORCE_INLINE usize GetOffset() const noexcept {
        return m_Offset;
    }

    ALT_NODISCARD ALT_FORCE_INLINE usize GetOffsetEnd() const noexcept {
        return m_OffsetEnd;
    }

    ALT_NODISCARD ALT_FORCE_INLINE usize GetSize() const noexcept {
        return m_OffsetEnd - m_Offset;
    }

    ALT_NODISCARD inline bool CloneIntoAppend(String& buffer) const noexcept;

private:

    ALT_FORCE_INLINE Utf16BufferOffset() noexcept = default;
    ALT_IFERR_INJECT_DEFAULT_CONSTRUCTOR(Utf16BufferOffset);

private:

    friend class _PlatformDetails::PlatformInstance;

    usize m_Offset = 0;
    usize m_OffsetEnd = 0;
};

struct DirectoryEntrySizeResult {

    usize sizeToAllocate = 0;
    usize sizeInBytesFileDirInfo = 0;
};

enum class StdIo : usize {

    in = __Win32__STD_INPUT_HANDLE,
    out = __Win32__STD_OUTPUT_HANDLE,
    err = __Win32__STD_ERROR_HANDLE,
};

template <StdIo OUT>
ALT_FORCE_INLINE StringUTF16<>& GetString() noexcept {

    static thread_local StringUTF16 stl_String{};

    return stl_String;
}

template <typename ...ArgsT>
[[noreturn]] inline void Panic_(const char* fmt, const ArgsT&... args) noexcept {

    ALT_EPRINTLN(fmt, args...);
    ALT_PANIC_SILENT();
}

template <typename T, typename ...ArgsT>
inline void Print_(T&& fmt, const ArgsT&... args) noexcept {

    if (GetString<StdIo::out>().FormatAppend(std::forward<T>(fmt), args...)) {

        ALT_MAYBE_UNUSED Result<void, Platform::Error> err = Platform::ConsoleWrite(StdIo::out, GetString<StdIo::out>());
    }

    GetString<StdIo::out>().Clear();
}

template <typename T, typename ...ArgsT>
inline void Println_(T&& fmt, const ArgsT&... args) noexcept {

    if (GetString<StdIo::out>().FormatAppend(std::forward<T>(fmt), args...)) {

        if (GetString<StdIo::out>().Push(u'\n')) {

            ALT_MAYBE_UNUSED Result<void, Platform::Error> err = Platform::ConsoleWrite(StdIo::out, GetString<StdIo::out>());
        }
    }

    GetString<StdIo::out>().Clear();
}

template <typename T, typename ...ArgsT>
inline void EPrint_(T&& fmt, const ArgsT&... args) noexcept {

    if (GetString<StdIo::err>().FormatAppend(std::forward<T>(fmt), args...)) {

        ALT_MAYBE_UNUSED Result<void, Platform::Error> err = Platform::ConsoleWrite(StdIo::err, GetString<StdIo::err>());
    }

    GetString<StdIo::err>().Clear();
}

template <typename T, typename ...ArgsT>
inline void EPrintln_(T&& fmt, const ArgsT&... args) noexcept {

    if (GetString<StdIo::err>().FormatAppend(std::forward<T>(fmt), args...)) {

        if (GetString<StdIo::err>().Push(u'\n')) {

            ALT_MAYBE_UNUSED Result<void, Platform::Error> err = Platform::ConsoleWrite(StdIo::err, GetString<StdIo::err>());
        }
    }

    GetString<StdIo::err>().Clear();
}

class PlatformInstance {
private:

    ALT_MAKE_CLASS_NON_COPYABLE(PlatformInstance);

    class FileRemoveAllCtx;

    using GetLastError_FuncPtr = PlatformFuncPtr<DWORD>;
    GetLastError_FuncPtr pGetLastError = nullptr;

    using FormatMessageW_FuncPtr = PlatformFuncPtr<DWORD, DWORD, LPCVOID, DWORD, DWORD, LPWSTR, DWORD, va_list>;
    FormatMessageW_FuncPtr pFormatMessageW = nullptr;

    using TerminateProcess_FuncPtr = PlatformFuncPtr<BOOL, HANDLE, UINT>;
    TerminateProcess_FuncPtr pTerminateProcess = nullptr;

    using ExitProcess_FuncPtr = PlatformFuncPtr<void, UINT>;
    ExitProcess_FuncPtr pExitProcess = nullptr;

    using HeapAlloc_FuncPtr = PlatformFuncPtr<LPVOID, HANDLE, DWORD, SIZE_T>;
    HeapAlloc_FuncPtr pHeapAlloc = nullptr;

    using HeapReAlloc_FuncPtr = PlatformFuncPtr<LPVOID, HANDLE, DWORD, LPVOID, SIZE_T>;
    HeapReAlloc_FuncPtr pHeapReAlloc = nullptr;

    using HeapFree_FuncPtr = PlatformFuncPtr<BOOL, HANDLE, DWORD, LPVOID>;
    HeapFree_FuncPtr pHeapFree = nullptr;

    using VirtualAlloc_FuncPtr = PlatformFuncPtr<LPVOID, LPVOID, SIZE_T, DWORD, DWORD>;
    VirtualAlloc_FuncPtr pVirtualAlloc = nullptr;

    using VirtualFree_FuncPtr = PlatformFuncPtr<BOOL, LPVOID, SIZE_T, DWORD>;
    VirtualFree_FuncPtr pVirtualFree = nullptr;

    using LocalFree_FuncPtr = PlatformFuncPtr<HLOCAL, HLOCAL>;
    LocalFree_FuncPtr pLocalFree = nullptr;

    using QueryPerformanceCounter_FuncPtr = PlatformFuncPtr<BOOL, PLARGE_INTEGER>;
    QueryPerformanceCounter_FuncPtr pQueryPerformanceCounter = nullptr;

    using Sleep_FuncPtr = PlatformFuncPtr<void, DWORD>;
    Sleep_FuncPtr pSleep = nullptr;

    using CreateThread_FuncPtr = PlatformFuncPtr<HANDLE, LPSECURITY_ATTRIBUTES, SIZE_T, LPTHREAD_START_ROUTINE, LPVOID, DWORD, LPDWORD>;
    CreateThread_FuncPtr pCreateThread = nullptr;

    using GetCurrentThread_FuncPtr = PlatformFuncPtr<HANDLE>;
    GetCurrentThread_FuncPtr pGetCurrentThread = nullptr;

    using GetCurrentThreadId_FuncPtr = PlatformFuncPtr<DWORD>;
    GetCurrentThreadId_FuncPtr pGetCurrentThreadId = nullptr;

    using GetThreadId_FuncPtr = PlatformFuncPtr<DWORD, HANDLE>;
    GetThreadId_FuncPtr pGetThreadId = nullptr;

    using NtQueryInformationThread_FuncPtr = PlatformFuncPtr<NTSTATUS, HANDLE, THREADINFOCLASS, PVOID, ULONG, PULONG>;
    NtQueryInformationThread_FuncPtr pNtQueryInformationThread = nullptr;

    using SwitchToThread_FuncPtr = PlatformFuncPtr<BOOL>;
    SwitchToThread_FuncPtr pSwitchToThread = nullptr;

    using SetThreadPriorityBoost_FuncPtr = PlatformFuncPtr<BOOL, HANDLE, BOOL>;
    SetThreadPriorityBoost_FuncPtr pSetThreadPriorityBoost = nullptr;

    using GetThreadPriorityBoost_FuncPtr = PlatformFuncPtr<BOOL, HANDLE, PBOOL>;
    GetThreadPriorityBoost_FuncPtr pGetThreadPriorityBoost = nullptr;

    using SetThreadPriority_FuncPtr = PlatformFuncPtr<BOOL, HANDLE, int>;
    SetThreadPriority_FuncPtr pSetThreadPriority = nullptr;

    using GetThreadPriority_FuncPtr = PlatformFuncPtr<int, HANDLE>;
    GetThreadPriority_FuncPtr pGetThreadPriority = nullptr;

    using SetThreadAffinityMask_FuncPtr = PlatformFuncPtr<DWORD_PTR, HANDLE, DWORD_PTR>;
    SetThreadAffinityMask_FuncPtr pSetThreadAffinityMask = nullptr;

    using SetThreadDescription_FuncPtr = PlatformFuncPtr<HRESULT, HANDLE, PCWSTR>;
    SetThreadDescription_FuncPtr pSetThreadDescription = nullptr;

    using GetThreadDescription_FuncPtr = PlatformFuncPtr<HRESULT, HANDLE, PWSTR*>;
    GetThreadDescription_FuncPtr pGetThreadDescription = nullptr;

    using CreateSemaphoreW_FuncPtr = PlatformFuncPtr<HANDLE, LPSECURITY_ATTRIBUTES, LONG, LONG, LPCWSTR>;
    CreateSemaphoreW_FuncPtr pCreateSemaphoreW = nullptr;

    using ReleaseSemaphore_FuncPtr = PlatformFuncPtr<BOOL, HANDLE, LONG, LPLONG>;
    ReleaseSemaphore_FuncPtr pReleaseSemaphore = nullptr;

    using InitializeCriticalSection_FuncPtr = PlatformFuncPtr<void, LPCRITICAL_SECTION>;
    InitializeCriticalSection_FuncPtr pInitializeCriticalSection = nullptr;

    using DeleteCriticalSection_FuncPtr = PlatformFuncPtr<void, LPCRITICAL_SECTION>;
    DeleteCriticalSection_FuncPtr pDeleteCriticalSection = nullptr;

    using EnterCriticalSection_FuncPtr = PlatformFuncPtr<void, LPCRITICAL_SECTION>;
    EnterCriticalSection_FuncPtr pEnterCriticalSection = nullptr;

    using TryEnterCriticalSection_FuncPtr = PlatformFuncPtr<BOOL, LPCRITICAL_SECTION>;
    TryEnterCriticalSection_FuncPtr pTryEnterCriticalSection = nullptr;

    using LeaveCriticalSection_FuncPtr = PlatformFuncPtr<void, LPCRITICAL_SECTION>;
    LeaveCriticalSection_FuncPtr pLeaveCriticalSection = nullptr;

    using AcquireSRWLockExclusive_FuncPtr = PlatformFuncPtr<void, PSRWLOCK>;
    AcquireSRWLockExclusive_FuncPtr pAcquireSRWLockExclusive = nullptr;

    using AcquireSRWLockShared_FuncPtr = PlatformFuncPtr<void, PSRWLOCK>;
    AcquireSRWLockShared_FuncPtr pAcquireSRWLockShared = nullptr;

    using ReleaseSRWLockExclusive_FuncPtr = PlatformFuncPtr<void, PSRWLOCK>;
    ReleaseSRWLockExclusive_FuncPtr pReleaseSRWLockExclusive = nullptr;

    using ReleaseSRWLockShared_FuncPtr = PlatformFuncPtr<void, PSRWLOCK>;
    ReleaseSRWLockShared_FuncPtr pReleaseSRWLockShared = nullptr;

    using TryAcquireSRWLockExclusive_FuncPtr = PlatformFuncPtr<BOOLEAN, PSRWLOCK>;
    TryAcquireSRWLockExclusive_FuncPtr pTryAcquireSRWLockExclusive = nullptr;

    using TryAcquireSRWLockShared_FuncPtr = PlatformFuncPtr<BOOLEAN, PSRWLOCK>;
    TryAcquireSRWLockShared_FuncPtr pTryAcquireSRWLockShared = nullptr;

    using WaitOnAddress_FuncPtr = PlatformFuncPtr<BOOL, volatile void*, PVOID, SIZE_T, DWORD>;
    WaitOnAddress_FuncPtr pWaitOnAddress = nullptr;

    using WakeByAddressSingle_FuncPtr = PlatformFuncPtr<void, PVOID>;
    WakeByAddressSingle_FuncPtr pWakeByAddressSingle = nullptr;

    using WakeByAddressAll_FuncPtr = PlatformFuncPtr<void, PVOID>;
    WakeByAddressAll_FuncPtr pWakeByAddressAll = nullptr;

    using GetLogicalDrives_FuncPtr = PlatformFuncPtr<DWORD>;
    GetLogicalDrives_FuncPtr pGetLogicalDrives = nullptr;

    using DeleteFileW_FuncPtr = PlatformFuncPtr<BOOL, LPCWSTR>;
    DeleteFileW_FuncPtr pDeleteFileW = nullptr;

    using WriteFile_FuncPtr = PlatformFuncPtr<BOOL, HANDLE, LPCVOID, DWORD, LPDWORD, LPOVERLAPPED>;
    WriteFile_FuncPtr pWriteFile = nullptr;

    using ReadFile_FuncPtr = PlatformFuncPtr<BOOL, HANDLE, LPVOID, DWORD, LPDWORD, LPOVERLAPPED>;
    ReadFile_FuncPtr pReadFile = nullptr;

    using CopyFileW_FuncPtr = PlatformFuncPtr<BOOL, LPCWSTR, LPCWSTR, BOOL>;
    CopyFileW_FuncPtr pCopyFileW = nullptr;

    using MoveFileW_FuncPtr = PlatformFuncPtr<BOOL, LPCWSTR, LPCWSTR>;
    MoveFileW_FuncPtr pMoveFileW = nullptr;

    using GetFileAttributesW_FuncPtr = PlatformFuncPtr<DWORD, LPCWSTR>;
    GetFileAttributesW_FuncPtr pGetFileAttributesW = nullptr;

    using GetFileSizeEx_FuncPtr = PlatformFuncPtr<BOOL, HANDLE, PLARGE_INTEGER>;
    GetFileSizeEx_FuncPtr pGetFileSizeEx = nullptr;

    using SetFilePointerEx_FuncPtr = PlatformFuncPtr<BOOL, HANDLE, LARGE_INTEGER, PLARGE_INTEGER, DWORD>;
    SetFilePointerEx_FuncPtr pSetFilePointerEx = nullptr;

    using SetEndOfFile_FuncPtr = PlatformFuncPtr<BOOL, HANDLE>;
    SetEndOfFile_FuncPtr pSetEndOfFile = nullptr;

    using GetFileType_FuncPtr = PlatformFuncPtr<BOOL, HANDLE>;
    GetFileType_FuncPtr pGetFileType = nullptr;

    using GetTempPathW_FuncPtr = PlatformFuncPtr<DWORD, DWORD, LPWSTR>;
    GetTempPathW_FuncPtr pGetTempPathW = nullptr;

    using GetCurrentDirectoryW_FuncPtr = PlatformFuncPtr<DWORD, DWORD, LPWSTR>;
    GetCurrentDirectoryW_FuncPtr pGetCurrentDirectoryW = nullptr;

    using CreateDirectoryW_FuncPtr = PlatformFuncPtr<BOOL, LPCWSTR, LPSECURITY_ATTRIBUTES>;
    CreateDirectoryW_FuncPtr pCreateDirectoryW = nullptr;

    using RemoveDirectoryW_FuncPtr = PlatformFuncPtr<BOOL, LPCWSTR>;
    RemoveDirectoryW_FuncPtr pRemoveDirectoryW = nullptr;

    using CreateHardLinkW_FuncPtr = PlatformFuncPtr<BOOL, LPCWSTR, LPCWSTR, LPSECURITY_ATTRIBUTES>;
    CreateHardLinkW_FuncPtr pCreateHardLinkW = nullptr;

    using GetDiskFreeSpaceExW_FuncPtr = PlatformFuncPtr<BOOL, LPCWSTR, PULARGE_INTEGER, PULARGE_INTEGER, PULARGE_INTEGER>;
    GetDiskFreeSpaceExW_FuncPtr pGetDiskFreeSpaceExW = nullptr;

    using GetFullPathNameW_FuncPtr = PlatformFuncPtr<DWORD, LPCWSTR, DWORD, LPWSTR, LPWSTR>;
    GetFullPathNameW_FuncPtr pGetFullPathNameW = nullptr;

    using CreateFileMappingW_FuncPtr = PlatformFuncPtr<HANDLE, HANDLE, LPSECURITY_ATTRIBUTES, DWORD, DWORD, DWORD, LPCWSTR>;
    CreateFileMappingW_FuncPtr pCreateFileMappingW = nullptr;

    using MapViewOfFile_FuncPtr = PlatformFuncPtr<LPVOID, HANDLE, DWORD, DWORD, DWORD, SIZE_T>;
    MapViewOfFile_FuncPtr pMapViewOfFile = nullptr;

    using UnmapViewOfFile_FuncPtr = PlatformFuncPtr<BOOL, LPCVOID>;
    UnmapViewOfFile_FuncPtr pUnmapViewOfFile = nullptr;

    using FlushViewOfFile_FuncPtr = PlatformFuncPtr<BOOL, LPCVOID, SIZE_T>;
    FlushViewOfFile_FuncPtr pFlushViewOfFile = nullptr;

    using FlushFileBuffers_FuncPtr = PlatformFuncPtr<BOOL, HANDLE>;
    FlushFileBuffers_FuncPtr pFlushFileBuffers = nullptr;

    //using GetFileInformationByHandle_FuncPtr = PlatformFuncPtr<BOOL, HANDLE, LPBY_HANDLE_FILE_INFORMATION>;
    //GetFileInformationByHandle_FuncPtr pGetFileInformationByHandle = nullptr;

    //using DeviceIoControl_FuncPtr = PlatformFuncPtr<BOOL, HANDLE, DWORD, LPVOID, DWORD, LPVOID, DWORD, LPDWORD, LPOVERLAPPED>;
    //DeviceIoControl_FuncPtr pDeviceIoControl = nullptr;

    using GetStdHandle_FuncPtr = PlatformFuncPtr<HANDLE, DWORD>;
    GetStdHandle_FuncPtr pGetStdHandle = nullptr;

    using GetConsoleMode_FuncPtr = PlatformFuncPtr<BOOL, HANDLE, LPDWORD>;
    GetConsoleMode_FuncPtr pGetConsoleMode = nullptr;

    using WriteConsoleW_FuncPtr = PlatformFuncPtr<BOOL, HANDLE, const wchar_t*, DWORD, LPDWORD, LPVOID>;
    WriteConsoleW_FuncPtr pWriteConsoleW = nullptr;

    using ReadConsoleW_FuncPtr = PlatformFuncPtr<BOOL, HANDLE, const wchar_t*, DWORD, LPDWORD, LPVOID>;
    ReadConsoleW_FuncPtr pReadConsoleW = nullptr;

    using SetConsoleTitleW_FuncPtr = PlatformFuncPtr<BOOL, LPCWSTR>;
    SetConsoleTitleW_FuncPtr pSetConsoleTitleW = nullptr;

    using GetConsoleTitleW_FuncPtr = PlatformFuncPtr<DWORD, LPWSTR, DWORD>;
    GetConsoleTitleW_FuncPtr pGetConsoleTitleW = nullptr;

    using GetConsoleScreenBufferInfo_FuncPtr = PlatformFuncPtr<BOOL, HANDLE, PCONSOLE_SCREEN_BUFFER_INFO>;
    GetConsoleScreenBufferInfo_FuncPtr pGetConsoleScreenBufferInfo = nullptr;

    using GetEnvironmentVariableW_FuncPtr = PlatformFuncPtr<DWORD, LPCWSTR, LPWSTR, DWORD>;
    GetEnvironmentVariableW_FuncPtr pGetEnvironmentVariableW = nullptr;

    using RtlNtStatusToDosError_FuncPtr = PlatformFuncPtr<ULONG, NTSTATUS>;
    RtlNtStatusToDosError_FuncPtr pRtlNtStatusToDosError = nullptr;

    using NtSetInformationFile_FuncPtr = PlatformFuncPtr<NTSTATUS, HANDLE, PIO_STATUS_BLOCK, PVOID, ULONG, FILE_INFORMATION_CLASS>;
    NtSetInformationFile_FuncPtr pNtSetInformationFile = nullptr;

    using NtQueryInformationFile_FuncPtr = PlatformFuncPtr<NTSTATUS, HANDLE, PIO_STATUS_BLOCK, PVOID, ULONG, FILE_INFORMATION_CLASS>;
    NtQueryInformationFile_FuncPtr pNtQueryInformationFile = nullptr;

    using NtQueryDirectoryFile_FuncPtr = PlatformFuncPtr<NTSTATUS, HANDLE, HANDLE, PIO_APC_ROUTINE, PVOID, PIO_STATUS_BLOCK, PVOID, ULONG, FILE_INFORMATION_CLASS, BOOLEAN, PUNICODE_STRING, BOOLEAN>;
    NtQueryDirectoryFile_FuncPtr pNtQueryDirectoryFile = nullptr;

    using NtQueryInformationByName_FuncPtr = PlatformFuncPtr<NTSTATUS, POBJECT_ATTRIBUTES, PIO_STATUS_BLOCK, PVOID, ULONG, FILE_INFORMATION_CLASS>;
    NtQueryInformationByName_FuncPtr pNtQueryInformationByName = nullptr;

    using CreateSymbolicLinkW_FuncPtr = PlatformFuncPtr<BOOLEAN, LPCWSTR, LPCWSTR, DWORD>;
    CreateSymbolicLinkW_FuncPtr pCreateSymbolicLinkW = nullptr;

    using GetFinalPathNameByHandleW_FuncPtr = PlatformFuncPtr<DWORD, HANDLE, LPWSTR, DWORD, DWORD>;
    GetFinalPathNameByHandleW_FuncPtr pGetFinalPathNameByHandleW = nullptr;

    using GetFileInformationByHandleEx_FuncPtr = PlatformFuncPtr<BOOL, HANDLE, FILE_INFO_BY_HANDLE_CLASS, LPVOID, DWORD>;
    GetFileInformationByHandleEx_FuncPtr pGetFileInformationByHandleEx = nullptr;

    using SetFileInformationByHandle_FuncPtr = PlatformFuncPtr<BOOL, HANDLE, FILE_INFO_BY_HANDLE_CLASS, LPVOID, DWORD>;
    SetFileInformationByHandle_FuncPtr pSetFileInformationByHandle = nullptr;

    using GetFileInformationByName_FuncPtr = PlatformFuncPtr<BOOL, PCWSTR, FILE_INFO_BY_NAME_CLASS, PVOID, ULONG>;
    GetFileInformationByName_FuncPtr pGetFileInformationByName = nullptr;

    using GetSystemTimeAsFileTime_FuncPtr = PlatformFuncPtr<void, LPFILETIME>;
    GetSystemTimeAsFileTime_FuncPtr pGetSystemTimeAsFileTime = nullptr;

    using GetSystemTimePreciseAsFileTime_FuncPtr = PlatformFuncPtr<void, LPFILETIME>;
    GetSystemTimePreciseAsFileTime_FuncPtr pGetSystemTimePreciseAsFileTime = nullptr;

    using SystemTimeToTzSpecificLocalTime_FuncPtr = PlatformFuncPtr<BOOL, const TIME_ZONE_INFORMATION*, const SYSTEMTIME*, LPSYSTEMTIME>;
    SystemTimeToTzSpecificLocalTime_FuncPtr pSystemTimeToTzSpecificLocalTime = nullptr;

    using GetProcessTimes_FuncPtr = PlatformFuncPtr<BOOL, HANDLE, LPFILETIME, LPFILETIME, LPFILETIME, LPFILETIME>;
    GetProcessTimes_FuncPtr pGetProcessTimes = nullptr;

    using QueryProcessCycleTime_FuncPtr = PlatformFuncPtr<BOOL, HANDLE, PULONG64>;
    QueryProcessCycleTime_FuncPtr pQueryProcessCycleTime = nullptr;

    using GetOpenFileNameW_FuncPtr = PlatformFuncPtr<BOOL, LPOPENFILENAMEW>;
    GetOpenFileNameW_FuncPtr pGetOpenFileNameW = nullptr;

    using GetSaveFileNameW_FuncPtr = PlatformFuncPtr<BOOL, LPOPENFILENAMEW>;
    GetSaveFileNameW_FuncPtr pGetSaveFileNameW = nullptr;

    using CommDlgExtendedError_FuncPtr = PlatformFuncPtr<DWORD>;
    CommDlgExtendedError_FuncPtr pCommDlgExtendedError = nullptr;

public:

    using CreateFileW_FuncPtr = PlatformFuncPtr<HANDLE, LPCWSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE>;
    CreateFileW_FuncPtr pCreateFileW = nullptr;

    using CloseHandle_FuncPtr = PlatformFuncPtr<BOOL, HANDLE>;
    CloseHandle_FuncPtr pCloseHandle = nullptr;

    using CreateEventW_FuncPtr = PlatformFuncPtr<HANDLE, LPSECURITY_ATTRIBUTES, BOOL, BOOL, LPCWSTR>;
    CreateEventW_FuncPtr pCreateEventW = nullptr;

    using ReadDirectoryChangesW_FuncPtr = PlatformFuncPtr<BOOL, HANDLE, LPVOID, DWORD, BOOL, DWORD, LPDWORD, LPOVERLAPPED, LPOVERLAPPED_COMPLETION_ROUTINE>;
    ReadDirectoryChangesW_FuncPtr pReadDirectoryChangesW = nullptr;

    using WaitForSingleObject_FuncPtr = PlatformFuncPtr<DWORD, HANDLE, DWORD>;
    WaitForSingleObject_FuncPtr pWaitForSingleObject = nullptr;

    using GetOverlappedResult_FuncPtr = PlatformFuncPtr<BOOL, HANDLE, LPOVERLAPPED, LPDWORD, BOOL>;
    GetOverlappedResult_FuncPtr pGetOverlappedResult = nullptr;

    using CancelIoEx_FuncPtr = PlatformFuncPtr<BOOL, HANDLE, LPOVERLAPPED>;
    CancelIoEx_FuncPtr pCancelIoEx = nullptr;

    FuncPtr<Result<void, Platform::Error>, const PlatformInstance&, Platform::Handle> pFileDeleteByHandle = nullptr;
    FuncPtr<Result<Platform::FileTypeBits, Platform::Error>, const PlatformInstance&, Platform::Handle, const bool> pFileGetTypeByHandle = nullptr;
    FuncPtr<Result<Platform::FileTypeBits, Platform::Error>, const PlatformInstance&, const wchar_t*, const bool> pFileGetType = nullptr;
    FuncPtr<Result<Platform::FileInfo, Platform::Error>, const PlatformInstance&, const wchar_t*> pFileGetInfo = nullptr;
    FuncPtr<Result<bool, Platform::Error>, const PlatformInstance&, const Platform::Handle, const Platform::Handle> pFileSameByHandle = nullptr;
    FuncPtr<Result<bool, Platform::Error>, const PlatformInstance&, const wchar_t*, const wchar_t*> pFileSame = nullptr;

    FuncPtr<Time, const PlatformInstance&> pTimeGetSystemTimeUTC = nullptr;
    FuncPtr<Result<Time, Platform::Error>, const PlatformInstance&> pTimeGetSystemTimeLocalTime = nullptr;

    FuncPtr<u32, const PlatformInstance&, const Platform::Handle> pGetThreadIDByHandle = nullptr;

public:

    struct CreateDesc {

        bool isLoadCmdDialog = true;
    };

    static PlatformInstance Create(const CreateDesc& desc) noexcept {

        // 2000 -> XP -> Vista

        PlatformInstance instance{};

        const HMODULE hKernel32 = GetModuleHandleW(L"kernel32.dll");

        instance.PreInitializeTermination(hKernel32);

        // Minimum supported client	Windows XP [desktop apps | UWP apps]
        // Minimum supported server	Windows Server 2003 [desktop apps | UWP apps]
        instance.pTerminateProcess = reinterpret_cast<TerminateProcess_FuncPtr>(GetProcAddress(hKernel32, "TerminateProcess"));

        instance.PreInitializeSystemInfo(hKernel32);

        using LoadLibraryW_FuncPtr = PlatformFuncPtr<HMODULE, LPCWSTR>;
        LoadLibraryW_FuncPtr pLoadLibraryW = nullptr;

        // Minimum supported client	Windows XP [desktop apps only]
        // Minimum supported server	Windows Server 2003 [desktop apps only]
        pLoadLibraryW = reinterpret_cast<LoadLibraryW_FuncPtr>(GetProcAddress(hKernel32, "LoadLibraryW"));

        // Minimum supported client	Windows XP [desktop apps only]
        // Minimum supported server	Windows Server 2003 [desktop apps only]
        instance.pExitProcess = reinterpret_cast<ExitProcess_FuncPtr>(GetProcAddress(hKernel32, "ExitProcess"));

        // Minimum supported client	Windows XP [desktop apps | UWP apps]
        // Minimum supported server	Windows Server 2003 [desktop apps | UWP apps]
        instance.pGetLastError = reinterpret_cast<GetLastError_FuncPtr>(GetProcAddress(hKernel32, "GetLastError"));

        // Minimum supported client	Windows XP [desktop apps | UWP apps]
        // Minimum supported server	Windows Server 2003 [desktop apps | UWP apps]
        instance.pFormatMessageW = reinterpret_cast<FormatMessageW_FuncPtr>(GetProcAddress(hKernel32, "FormatMessageW"));

        // Minimum supported client	Windows XP [desktop apps | UWP apps]
        // Minimum supported server	Windows Server 2003 [desktop apps | UWP apps]
        instance.pHeapAlloc = reinterpret_cast<HeapAlloc_FuncPtr>(GetProcAddress(hKernel32, "HeapAlloc"));

        // Minimum supported client	Windows XP [desktop apps | UWP apps]
        // Minimum supported server	Windows Server 2003 [desktop apps | UWP apps]
        instance.pHeapReAlloc = reinterpret_cast<HeapReAlloc_FuncPtr>(GetProcAddress(hKernel32, "HeapReAlloc"));

        // Minimum supported client	Windows XP [desktop apps | UWP apps]
        // Minimum supported server	Windows Server 2003 [desktop apps | UWP apps]
        instance.pHeapFree = reinterpret_cast<HeapFree_FuncPtr>(GetProcAddress(hKernel32, "HeapFree"));

        // Minimum supported client	Windows XP [desktop apps | UWP apps]
        // Minimum supported server	Windows Server 2003 [desktop apps | UWP apps]
        instance.pVirtualAlloc = reinterpret_cast<VirtualAlloc_FuncPtr>(GetProcAddress(hKernel32, "VirtualAlloc"));

        // Minimum supported client	Windows XP [desktop apps | UWP apps]
        // Minimum supported server	Windows Server 2003 [desktop apps | UWP apps]
        instance.pVirtualFree = reinterpret_cast<VirtualFree_FuncPtr>(GetProcAddress(hKernel32, "VirtualFree"));

        // Minimum supported client	Windows XP [desktop apps | UWP apps]
        // Minimum supported server	Windows Server 2003 [desktop apps | UWP apps]
        instance.pLocalFree = reinterpret_cast<LocalFree_FuncPtr>(GetProcAddress(hKernel32, "LocalFree"));

        // Minimum supported client	Windows 2000 Professional [desktop apps | UWP apps]
        // Minimum supported server	Windows 2000 Server [desktop apps | UWP apps]
        instance.pQueryPerformanceCounter = reinterpret_cast<QueryPerformanceCounter_FuncPtr>(GetProcAddress(hKernel32, "QueryPerformanceCounter"));

        // Minimum supported client	Windows 2000 Professional [desktop apps | UWP apps]
        // Minimum supported server	Windows 2000 Server [desktop apps | UWP apps]
        instance.pCloseHandle = reinterpret_cast<CloseHandle_FuncPtr>(GetProcAddress(hKernel32, "CloseHandle"));

        // Minimum supported client	Windows XP [desktop apps | UWP apps]
        // Minimum supported server	Windows Server 2003 [desktop apps | UWP apps]
        instance.pSleep = reinterpret_cast<Sleep_FuncPtr>(GetProcAddress(hKernel32, "Sleep"));

        // Minimum supported client	Windows XP [desktop apps | UWP apps]
        // Minimum supported server	Windows Server 2003 [desktop apps | UWP apps]
        instance.pCreateThread = reinterpret_cast<CreateThread_FuncPtr>(GetProcAddress(hKernel32, "CreateThread"));

        // Minimum supported client	Windows XP [desktop apps | UWP apps]
        // Minimum supported server	Windows Server 2003 [desktop apps | UWP apps]
        instance.pGetCurrentThread = reinterpret_cast<GetCurrentThread_FuncPtr>(GetProcAddress(hKernel32, "GetCurrentThread"));

        // Minimum supported client	Windows XP [desktop apps | UWP apps]
        // Minimum supported server	Windows Server 2003 [desktop apps | UWP apps]
        instance.pGetCurrentThreadId = reinterpret_cast<GetCurrentThreadId_FuncPtr>(GetProcAddress(hKernel32, "GetCurrentThreadId"));

        // Minimum supported client	Windows XP [desktop apps | UWP apps]
        // Minimum supported server	Windows Server 2003 [desktop apps | UWP apps]
        instance.pSwitchToThread = reinterpret_cast<SwitchToThread_FuncPtr>(GetProcAddress(hKernel32, "SwitchToThread"));

        // Minimum supported client	Windows XP [desktop apps | UWP apps]
        // Minimum supported server	Windows Server 2003 [desktop apps | UWP apps]
        instance.pSetThreadPriorityBoost = reinterpret_cast<SetThreadPriorityBoost_FuncPtr>(GetProcAddress(hKernel32, "SetThreadPriorityBoost"));

        // Minimum supported client	Windows XP [desktop apps | UWP apps]
        // Minimum supported server	Windows Server 2003 [desktop apps | UWP apps]
        instance.pGetThreadPriorityBoost = reinterpret_cast<GetThreadPriorityBoost_FuncPtr>(GetProcAddress(hKernel32, "GetThreadPriorityBoost"));

        // Minimum supported client	Windows XP [desktop apps | UWP apps]
        // Minimum supported server	Windows Server 2003 [desktop apps | UWP apps]
        instance.pSetThreadPriority = reinterpret_cast<SetThreadPriority_FuncPtr>(GetProcAddress(hKernel32, "SetThreadPriority"));

        // Minimum supported client	Windows XP [desktop apps | UWP apps]
        // Minimum supported server	Windows Server 2003 [desktop apps | UWP apps]
        instance.pGetThreadPriority = reinterpret_cast<GetThreadPriority_FuncPtr>(GetProcAddress(hKernel32, "GetThreadPriority"));

        // Minimum supported client	Windows XP [desktop apps | UWP apps]
        // Minimum supported server	Windows Server 2003 [desktop apps | UWP apps]
        instance.pSetThreadAffinityMask = reinterpret_cast<SetThreadAffinityMask_FuncPtr>(GetProcAddress(hKernel32, "SetThreadAffinityMask"));

        // Minimum supported client	Windows XP [desktop apps | UWP apps]
        // Minimum supported server	Windows Server 2003 [desktop apps | UWP apps]
        instance.pCreateSemaphoreW = reinterpret_cast<CreateSemaphoreW_FuncPtr>(GetProcAddress(hKernel32, "CreateSemaphoreW"));

        // Minimum supported client	Windows XP [desktop apps | UWP apps]
        // Minimum supported server	Windows Server 2003 [desktop apps | UWP apps]
        instance.pReleaseSemaphore = reinterpret_cast<ReleaseSemaphore_FuncPtr>(GetProcAddress(hKernel32, "ReleaseSemaphore"));

        // Minimum supported client	Windows XP [desktop apps | UWP apps]
        // Minimum supported server	Windows Server 2003 [desktop apps | UWP apps]
        instance.pInitializeCriticalSection = reinterpret_cast<InitializeCriticalSection_FuncPtr>(GetProcAddress(hKernel32, "InitializeCriticalSection"));

        // Minimum supported client	Windows XP [desktop apps | UWP apps]
        // Minimum supported server	Windows Server 2003 [desktop apps | UWP apps]
        instance.pDeleteCriticalSection = reinterpret_cast<DeleteCriticalSection_FuncPtr>(GetProcAddress(hKernel32, "DeleteCriticalSection"));

        // Minimum supported client	Windows XP [desktop apps | UWP apps]
        // Minimum supported server	Windows Server 2003 [desktop apps | UWP apps]
        instance.pEnterCriticalSection = reinterpret_cast<EnterCriticalSection_FuncPtr>(GetProcAddress(hKernel32, "EnterCriticalSection"));

        // Minimum supported client	Windows XP [desktop apps | UWP apps]
        // Minimum supported server	Windows Server 2003 [desktop apps | UWP apps]
        instance.pTryEnterCriticalSection = reinterpret_cast<TryEnterCriticalSection_FuncPtr>(GetProcAddress(hKernel32, "TryEnterCriticalSection"));

        // Minimum supported client	Windows XP [desktop apps | UWP apps]
        // Minimum supported server	Windows Server 2003 [desktop apps | UWP apps]
        instance.pLeaveCriticalSection = reinterpret_cast<LeaveCriticalSection_FuncPtr>(GetProcAddress(hKernel32, "LeaveCriticalSection"));

        // Minimum supported client	Windows XP [desktop apps only]
        // Minimum supported server	Windows Server 2003 [desktop apps only]
        instance.pCreateFileW = reinterpret_cast<CreateFileW_FuncPtr>(GetProcAddress(hKernel32, "CreateFileW"));

        // Minimum supported client	Windows XP [desktop apps only]
        // Minimum supported server	Windows Server 2003 [desktop apps only]
        instance.pGetLogicalDrives = reinterpret_cast<GetLogicalDrives_FuncPtr>(GetProcAddress(hKernel32, "GetLogicalDrives"));

        // Minimum supported client	Windows XP [desktop apps only]
        // Minimum supported server	Windows Server 2003 [desktop apps only]
        instance.pDeleteFileW = reinterpret_cast<DeleteFileW_FuncPtr>(GetProcAddress(hKernel32, "DeleteFileW"));

        // Minimum supported client	Windows XP [desktop apps only]
        // Minimum supported server	Windows Server 2003 [desktop apps only]
        instance.pWriteFile = reinterpret_cast<WriteFile_FuncPtr>(GetProcAddress(hKernel32, "WriteFile"));

        // Minimum supported client	Windows XP [desktop apps only]
        // Minimum supported server	Windows Server 2003 [desktop apps only]
        instance.pReadFile = reinterpret_cast<ReadFile_FuncPtr>(GetProcAddress(hKernel32, "ReadFile"));

        // Minimum supported client	Windows XP [desktop apps only]
        // Minimum supported server	Windows Server 2003 [desktop apps only]
        instance.pCopyFileW = reinterpret_cast<CopyFileW_FuncPtr>(GetProcAddress(hKernel32, "CopyFileW"));

        // Minimum supported client	Windows XP [desktop apps only]
        // Minimum supported server	Windows Server 2003 [desktop apps only]
        instance.pMoveFileW = reinterpret_cast<MoveFileW_FuncPtr>(GetProcAddress(hKernel32, "MoveFileW"));

        // Minimum supported client	Windows XP [desktop apps only]
        // Minimum supported server	Windows Server 2003 [desktop apps only]
        instance.pGetFileAttributesW = reinterpret_cast<GetFileAttributesW_FuncPtr>(GetProcAddress(hKernel32, "GetFileAttributesW"));

        // Minimum supported client	Windows XP [desktop apps only]
        // Minimum supported server	Windows Server 2003 [desktop apps only]
        instance.pGetFileSizeEx = reinterpret_cast<GetFileSizeEx_FuncPtr>(GetProcAddress(hKernel32, "GetFileSizeEx"));

        // Minimum supported client	Windows XP [desktop apps only]
        // Minimum supported server	Windows Server 2003 [desktop apps only]
        instance.pSetFilePointerEx = reinterpret_cast<SetFilePointerEx_FuncPtr>(GetProcAddress(hKernel32, "SetFilePointerEx"));

        // Minimum supported client	Windows XP [desktop apps only]
        // Minimum supported server	Windows Server 2003 [desktop apps only]
        instance.pSetEndOfFile = reinterpret_cast<SetEndOfFile_FuncPtr>(GetProcAddress(hKernel32, "SetEndOfFile"));

        // Minimum supported client	Windows XP [desktop apps only]
        // Minimum supported server	Windows Server 2003 [desktop apps only]
        instance.pGetFileType = reinterpret_cast<GetFileType_FuncPtr>(GetProcAddress(hKernel32, "GetFileType"));

        // Minimum supported client	Windows XP [desktop apps only]
        // Minimum supported server	Windows Server 2003 [desktop apps only]
        instance.pGetTempPathW = reinterpret_cast<GetTempPathW_FuncPtr>(GetProcAddress(hKernel32, "GetTempPathW"));

        // Minimum supported client	Windows XP [desktop apps only]
        // Minimum supported server	Windows Server 2003 [desktop apps only]
        instance.pGetCurrentDirectoryW = reinterpret_cast<GetCurrentDirectoryW_FuncPtr>(GetProcAddress(hKernel32, "GetCurrentDirectoryW"));

        // Minimum supported client	Windows XP [desktop apps only]
        // Minimum supported server	Windows Server 2003 [desktop apps only]
        instance.pCreateDirectoryW = reinterpret_cast<CreateDirectoryW_FuncPtr>(GetProcAddress(hKernel32, "CreateDirectoryW"));

        // Minimum supported client	Windows XP [desktop apps only]
        // Minimum supported server	Windows Server 2003 [desktop apps only]
        instance.pRemoveDirectoryW = reinterpret_cast<RemoveDirectoryW_FuncPtr>(GetProcAddress(hKernel32, "RemoveDirectoryW"));

        // Minimum supported client	Windows XP [desktop apps only]
        // Minimum supported server	Windows Server 2003 [desktop apps only]
        instance.pCreateHardLinkW = reinterpret_cast<CreateHardLinkW_FuncPtr>(GetProcAddress(hKernel32, "CreateHardLinkW"));

        // Minimum supported client	Windows XP [desktop apps only]
        // Minimum supported server	Windows Server 2003 [desktop apps only]
        instance.pGetDiskFreeSpaceExW = reinterpret_cast<GetDiskFreeSpaceExW_FuncPtr>(GetProcAddress(hKernel32, "GetDiskFreeSpaceExW"));

        // Minimum supported client	Windows XP [desktop apps only]
        // Minimum supported server	Windows Server 2003 [desktop apps only]
        instance.pGetFullPathNameW = reinterpret_cast<GetFullPathNameW_FuncPtr>(GetProcAddress(hKernel32, "GetFullPathNameW"));

        // Minimum supported client	Windows XP [desktop apps only]
        // Minimum supported server	Windows Server 2003 [desktop apps only]
        instance.pCreateFileMappingW = reinterpret_cast<CreateFileMappingW_FuncPtr>(GetProcAddress(hKernel32, "CreateFileMappingW"));

        // Minimum supported client	Windows XP [desktop apps only]
        // Minimum supported server	Windows Server 2003 [desktop apps only]
        instance.pMapViewOfFile = reinterpret_cast<MapViewOfFile_FuncPtr>(GetProcAddress(hKernel32, "MapViewOfFile"));

        // Minimum supported client	Windows XP [desktop apps only]
        // Minimum supported server	Windows Server 2003 [desktop apps only]
        instance.pUnmapViewOfFile = reinterpret_cast<UnmapViewOfFile_FuncPtr>(GetProcAddress(hKernel32, "UnmapViewOfFile"));

        // Minimum supported client	Windows XP [desktop apps only]
        // Minimum supported server	Windows Server 2003 [desktop apps only]
        instance.pFlushViewOfFile = reinterpret_cast<FlushViewOfFile_FuncPtr>(GetProcAddress(hKernel32, "FlushViewOfFile"));

        // Minimum supported client	Windows XP [desktop apps only]
        // Minimum supported server	Windows Server 2003 [desktop apps only]
        instance.pFlushFileBuffers = reinterpret_cast<FlushFileBuffers_FuncPtr>(GetProcAddress(hKernel32, "FlushFileBuffers"));

        // Minimum supported client	Windows XP [desktop apps only]
        // Minimum supported server	Windows Server 2003 [desktop apps only]
        //instance.pGetFileInformationByHandle = reinterpret_cast<GetFileInformationByHandle_FuncPtr>(GetProcAddress(hKernel32, "GetFileInformationByHandle"));

        // Minimum supported client	Windows XP [desktop apps only]
        // Minimum supported server	Windows Server 2003 [desktop apps only]
        //instance.pDeviceIoControl = reinterpret_cast<DeviceIoControl_FuncPtr>(GetProcAddress(hKernel32, "DeviceIoControl"));

        // Minimum supported client	Windows XP [desktop apps | UWP apps]
        // Minimum supported server	Windows Server 2003 [desktop apps | UWP apps]
        instance.pCreateEventW = reinterpret_cast<CreateEventW_FuncPtr>(GetProcAddress(hKernel32, "CreateEventW"));

        // Minimum supported client	Windows XP [desktop apps | UWP apps]
        // Minimum supported server	Windows Server 2003 [desktop apps | UWP apps]
        instance.pReadDirectoryChangesW = reinterpret_cast<ReadDirectoryChangesW_FuncPtr>(GetProcAddress(hKernel32, "ReadDirectoryChangesW"));

        // Minimum supported client	Windows XP [desktop apps | UWP apps]
        // Minimum supported server	Windows Server 2003 [desktop apps | UWP apps]
        instance.pGetOverlappedResult = reinterpret_cast<GetOverlappedResult_FuncPtr>(GetProcAddress(hKernel32, "GetOverlappedResult"));

        // Minimum supported client	Windows XP [desktop apps | UWP apps]
        // Minimum supported server	Windows Server 2003 [desktop apps | UWP apps]
        instance.pWaitForSingleObject = reinterpret_cast<WaitForSingleObject_FuncPtr>(GetProcAddress(hKernel32, "WaitForSingleObject"));

        // Minimum supported client	Windows 2000 Professional [desktop apps only]
        // Minimum supported server	Windows 2000 Server [desktop apps only]
        instance.pGetStdHandle = reinterpret_cast<GetStdHandle_FuncPtr>(GetProcAddress(hKernel32, "GetStdHandle"));

        // Minimum supported client	Windows 2000 Professional [desktop apps only]
        // Minimum supported server	Windows 2000 Server [desktop apps only]
        instance.pGetConsoleMode = reinterpret_cast<GetConsoleMode_FuncPtr>(GetProcAddress(hKernel32, "GetConsoleMode"));

        // Minimum supported client	Windows 2000 Professional [desktop apps only]
        // Minimum supported server	Windows 2000 Server [desktop apps only]
        instance.pWriteConsoleW = reinterpret_cast<WriteConsoleW_FuncPtr>(GetProcAddress(hKernel32, "WriteConsoleW"));

        // Minimum supported client	Windows 2000 Professional [desktop apps only]
        // Minimum supported server	Windows 2000 Server [desktop apps only]
        instance.pReadConsoleW = reinterpret_cast<ReadConsoleW_FuncPtr>(GetProcAddress(hKernel32, "ReadConsoleW"));

        // Minimum supported client	Windows 2000 Professional [desktop apps only]
        // Minimum supported server	Windows 2000 Server [desktop apps only]
        instance.pSetConsoleTitleW = reinterpret_cast<SetConsoleTitleW_FuncPtr>(GetProcAddress(hKernel32, "SetConsoleTitleW"));

        // Minimum supported client	Windows 2000 Professional [desktop apps only]
        // Minimum supported server	Windows 2000 Server [desktop apps only]
        instance.pGetConsoleTitleW = reinterpret_cast<GetConsoleTitleW_FuncPtr>(GetProcAddress(hKernel32, "GetConsoleTitleW"));

        // Minimum supported client	Windows 2000 Professional [desktop apps only]
        // Minimum supported server	Windows 2000 Server [desktop apps only]
        instance.pGetConsoleScreenBufferInfo = reinterpret_cast<GetConsoleScreenBufferInfo_FuncPtr>(GetProcAddress(hKernel32, "GetConsoleScreenBufferInfo"));

        // Minimum supported client	Windows XP [desktop apps | UWP apps]
        // Minimum supported server	Windows Server 2003 [desktop apps | UWP apps]
        instance.pGetEnvironmentVariableW = reinterpret_cast<GetEnvironmentVariableW_FuncPtr>(GetProcAddress(hKernel32, "GetEnvironmentVariableW"));

        // Minimum supported client	Windows XP [desktop apps | UWP apps]
        // Minimum supported server	Windows Server 2003 [desktop apps | UWP apps]
        instance.pGetEnvironmentVariableW = reinterpret_cast<GetEnvironmentVariableW_FuncPtr>(GetProcAddress(hKernel32, "GetEnvironmentVariableW"));

        // Minimum supported client	Windows 2000 Professional [desktop apps | UWP apps]
        // Minimum supported server	Windows 2000 Server [desktop apps | UWP apps]
        instance.pGetSystemTimeAsFileTime = reinterpret_cast<GetSystemTimeAsFileTime_FuncPtr>(GetProcAddress(hKernel32, "GetSystemTimeAsFileTime"));

        // Minimum supported client	Windows 2000 Professional [desktop apps | UWP apps]
        // Minimum supported server	Windows 2000 Server [desktop apps | UWP apps]
        instance.pSystemTimeToTzSpecificLocalTime = reinterpret_cast<SystemTimeToTzSpecificLocalTime_FuncPtr>(GetProcAddress(hKernel32, "SystemTimeToTzSpecificLocalTime"));

        // Minimum supported client	Windows XP [desktop apps | UWP apps]
        // Minimum supported server	Windows Server 2003 [desktop apps | UWP apps]
        instance.pGetProcessTimes = reinterpret_cast<GetProcessTimes_FuncPtr>(GetProcAddress(hKernel32, "GetProcessTimes"));

        if (desc.isLoadCmdDialog) {

            instance.m_hComdlgModule = (*pLoadLibraryW)(L"comdlg32.dll");
            const HMODULE hComdlgModule = instance.m_hComdlgModule;

            // Minimum supported client	Windows 2000 Professional [desktop apps only]
            // Minimum supported server	Windows 2000 Server [desktop apps only]
            instance.pGetOpenFileNameW = reinterpret_cast<GetOpenFileNameW_FuncPtr>(GetProcAddress(hComdlgModule, "GetOpenFileNameW"));

            // Minimum supported client	Windows 2000 Professional [desktop apps only]
            // Minimum supported server	Windows 2000 Server [desktop apps only]
            instance.pGetSaveFileNameW = reinterpret_cast<GetSaveFileNameW_FuncPtr>(GetProcAddress(hComdlgModule, "GetSaveFileNameW"));

            // Minimum supported client	Windows 2000 Professional [desktop apps only]
            // Minimum supported server	Windows 2000 Server [desktop apps only]
            instance.pCommDlgExtendedError = reinterpret_cast<CommDlgExtendedError_FuncPtr>(GetProcAddress(hComdlgModule, "CommDlgExtendedError"));
        }

        // Optional functions - 'kernel32.dll'

        // Minimum supported client	Windows Vista [desktop apps only]
        // Minimum supported server	Windows Server 2008 [desktop apps only]
        instance.pCreateSymbolicLinkW = reinterpret_cast<CreateSymbolicLinkW_FuncPtr>(GetProcAddress(hKernel32, "CreateSymbolicLinkW"));

        // Minimum supported client Windows Vista [desktop apps | UWP apps]
        // Minimum supported server Windows Server 2008 [desktop apps | UWP apps]
        instance.pGetFinalPathNameByHandleW = reinterpret_cast<GetFinalPathNameByHandleW_FuncPtr>(GetProcAddress(hKernel32, "GetFinalPathNameByHandleW"));

        // Minimum supported client Windows Vista [desktop apps | UWP apps]
        // Minimum supported server Windows Server 2008 [desktop apps | UWP apps]
        instance.pGetFileInformationByHandleEx = reinterpret_cast<GetFileInformationByHandleEx_FuncPtr>(GetProcAddress(hKernel32, "GetFileInformationByHandleEx"));

        // Minimum supported client Windows Vista [desktop apps | UWP apps]
        // Minimum supported server Windows Server 2008 [desktop apps | UWP apps]
        instance.pSetFileInformationByHandle = reinterpret_cast<SetFileInformationByHandle_FuncPtr>(GetProcAddress(hKernel32, "SetFileInformationByHandle"));

        // Minimum supported client Windows Vista [desktop apps | UWP apps]
        // Minimum supported server Windows Server 2008 [desktop apps | UWP apps]
        instance.pCancelIoEx = reinterpret_cast<CancelIoEx_FuncPtr>(GetProcAddress(hKernel32, "CancelIoEx"));

        // Minimum supported client	Windows Vista [desktop apps only]
        // Minimum supported server	Windows Server 2008 [desktop apps only]
        instance.pQueryProcessCycleTime = reinterpret_cast<QueryProcessCycleTime_FuncPtr>(GetProcAddress(hKernel32, "QueryProcessCycleTime"));

        // Minimum supported client	Windows 8 [desktop apps | UWP apps]
        // Minimum supported server	Windows Server 2012 [desktop apps | UWP apps]
        instance.pGetSystemTimePreciseAsFileTime = reinterpret_cast<GetSystemTimePreciseAsFileTime_FuncPtr>(GetProcAddress(hKernel32, "GetSystemTimePreciseAsFileTime"));

        // Minimum supported client	Windows Vista [desktop apps | UWP apps]
        // Minimum supported server	Windows Server 2003 [desktop apps | UWP apps]
        instance.pGetThreadId = reinterpret_cast<GetThreadId_FuncPtr>(GetProcAddress(hKernel32, "GetThreadId"));

        // Minimum supported client	Windows 10, version 1607 [desktop apps | UWP apps]
        // Minimum supported server	Windows Server 2016 [desktop apps | UWP apps]
        instance.pSetThreadDescription = reinterpret_cast<SetThreadDescription_FuncPtr>(GetProcAddress(hKernel32, "SetThreadDescription"));

        // Minimum supported client	Windows 10, version 1607 [desktop apps | UWP apps]
        // Minimum supported server	Windows Server 2016 [desktop apps | UWP apps]
        instance.pGetThreadDescription = reinterpret_cast<GetThreadDescription_FuncPtr>(GetProcAddress(hKernel32, "GetThreadDescription"));

        // Minimum supported client	Windows Vista [desktop apps | UWP apps]
        // Minimum supported server	Windows Server 2008 [desktop apps | UWP apps]
        instance.pAcquireSRWLockExclusive = reinterpret_cast<AcquireSRWLockExclusive_FuncPtr>(GetProcAddress(hKernel32, "AcquireSRWLockExclusive"));

        // Minimum supported client	Windows Vista [desktop apps | UWP apps]
        // Minimum supported server	Windows Server 2008 [desktop apps | UWP apps]
        instance.pAcquireSRWLockShared = reinterpret_cast<AcquireSRWLockShared_FuncPtr>(GetProcAddress(hKernel32, "AcquireSRWLockShared"));

        // Minimum supported client	Windows Vista [desktop apps | UWP apps]
        // Minimum supported server	Windows Server 2008 [desktop apps | UWP apps]
        instance.pReleaseSRWLockExclusive = reinterpret_cast<ReleaseSRWLockExclusive_FuncPtr>(GetProcAddress(hKernel32, "ReleaseSRWLockExclusive"));

        // Minimum supported client	Windows Vista [desktop apps | UWP apps]
        // Minimum supported server	Windows Server 2008 [desktop apps | UWP apps]
        instance.pReleaseSRWLockShared = reinterpret_cast<ReleaseSRWLockShared_FuncPtr>(GetProcAddress(hKernel32, "ReleaseSRWLockShared"));

        // Minimum supported client	Windows Vista [desktop apps | UWP apps]
        // Minimum supported server	Windows Server 2008 [desktop apps | UWP apps]
        instance.pTryAcquireSRWLockExclusive = reinterpret_cast<TryAcquireSRWLockExclusive_FuncPtr>(GetProcAddress(hKernel32, "TryAcquireSRWLockExclusive"));

        // Minimum supported client	Windows Vista [desktop apps | UWP apps]
        // Minimum supported server	Windows Server 2008 [desktop apps | UWP apps]
        instance.pTryAcquireSRWLockShared = reinterpret_cast<TryAcquireSRWLockShared_FuncPtr>(GetProcAddress(hKernel32, "TryAcquireSRWLockShared"));

        const HMODULE hModule_API_MS_Win_Core_Synch_l1_2_0 = GetModuleHandleW(L"API-MS-Win-Core-Synch-l1-2-0.dll");

        // Minimum supported client	Windows 8 [desktop apps | UWP apps]
        // Minimum supported server	Windows Server 2012 [desktop apps | UWP apps]
        instance.pWaitOnAddress = reinterpret_cast<WaitOnAddress_FuncPtr>(GetProcAddress(hModule_API_MS_Win_Core_Synch_l1_2_0, "WaitOnAddress"));

        // Minimum supported client	Windows 8 [desktop apps | UWP apps]
        // Minimum supported server	Windows Server 2012 [desktop apps | UWP apps]
        instance.pWakeByAddressSingle = reinterpret_cast<WakeByAddressSingle_FuncPtr>(GetProcAddress(hModule_API_MS_Win_Core_Synch_l1_2_0, "WakeByAddressSingle"));

        // Minimum supported client	Windows 8 [desktop apps | UWP apps]
        // Minimum supported server	Windows Server 2012 [desktop apps | UWP apps]
        instance.pWakeByAddressAll = reinterpret_cast<WakeByAddressAll_FuncPtr>(GetProcAddress(hModule_API_MS_Win_Core_Synch_l1_2_0, "WakeByAddressAll"));

        // https://learn.microsoft.com/en-us/windows/win32/apiindex/windows-apisets#api-set-contract-names
        const HMODULE hModule_api_ms_win_core_file_l2_1_4 = GetModuleHandleW(L"api-ms-win-core-file-l2-1-4.dll");

        // Windows 24H2/26052
        instance.pGetFileInformationByName = reinterpret_cast<GetFileInformationByName_FuncPtr>(GetProcAddress(hModule_api_ms_win_core_file_l2_1_4, "GetFileInformationByName"));

        const HMODULE hNtModule = GetModuleHandleW(L"ntdll.dll");

        // Minimum supported client	Windows XP [desktop apps only]
        // Minimum supported server	Windows Server 2003 [desktop apps only]
        instance.pRtlNtStatusToDosError = reinterpret_cast<RtlNtStatusToDosError_FuncPtr>(GetProcAddress(hNtModule, "RtlNtStatusToDosError"));

        // Minimum supported client	Windows 2000.
        // Target Platform	Universal
        instance.pNtSetInformationFile = reinterpret_cast<NtSetInformationFile_FuncPtr>(GetProcAddress(hNtModule, "NtSetInformationFile"));

        // Minimum supported client	Windows 2000.
        // Target Platform	Universal
        instance.pNtQueryInformationFile = reinterpret_cast<NtQueryInformationFile_FuncPtr>(GetProcAddress(hNtModule, "NtQueryInformationFile"));

        // Minimum supported client	Windows XP
        // Target Platform	Universal
        // IRQL	PASSIVE_LEVEL (see Remarks section)
        instance.pNtQueryDirectoryFile = reinterpret_cast<NtQueryDirectoryFile_FuncPtr>(GetProcAddress(hNtModule, "NtQueryDirectoryFile"));

        // Minimum supported client	Windows 2000.
        instance.pNtQueryInformationThread = reinterpret_cast<NtQueryInformationThread_FuncPtr>(GetProcAddress(hNtModule, "NtQueryInformationThread"));

        // Optional functions - 'ntdll.dll'

        // Minimum supported client	Windows 10, version 1703
        // IRQL	PASSIVE_LEVEL (see Remarks section)
        instance.pNtQueryInformationByName = reinterpret_cast<NtQueryInformationByName_FuncPtr>(GetProcAddress(hNtModule, "NtQueryInformationByName"));

        instance.PostInitialize();
        instance.PostInitializeTickState(hKernel32);
        instance.PostInitializeConsole(hKernel32);
        instance.PostInitializeMemory(hKernel32);

        return instance;
    }

    PlatformInstance(PlatformInstance&& rhs) noexcept
        : m_hComdlgModule(Exchange(rhs.m_hComdlgModule, static_cast<HMODULE>(NULL)))
    {}

    PlatformInstance& operator=(PlatformInstance&& rhs) noexcept {

        m_hComdlgModule = Exchange(rhs.m_hComdlgModule, static_cast<HMODULE>(NULL));

        return *this;
    }

    ~PlatformInstance() noexcept {

        if (m_hComdlgModule == NULL) {
            return;
        }

        const HMODULE hKernel32 = GetModuleHandleW(L"kernel32.dll");

        using FreeLibrary_FuncPtr = PlatformFuncPtr<BOOL, HMODULE>;
        FreeLibrary_FuncPtr pFreeLibrary = nullptr;

        // Minimum supported client	Windows XP [desktop apps | UWP apps]
        // Minimum supported server	Windows Server 2003 [desktop apps | UWP apps]
        pFreeLibrary = reinterpret_cast<FreeLibrary_FuncPtr>(GetProcAddress(hKernel32, "FreeLibrary"));

        (pFreeLibrary)(m_hComdlgModule);
        m_hComdlgModule = NULL;
    }

    [[noreturn]] ALT_FORCE_INLINE void Terminate(const u32 exitCode) const noexcept {

        ALT_MAYBE_UNUSED const bool isOk = (*pTerminateProcess)(m_CurrentProcess, static_cast<UINT>(exitCode));
        ALT_UNREACHABLE();
    }

    [[noreturn]] ALT_FORCE_INLINE void Exit(const u32 exitCode) const noexcept {

        (*pExitProcess)(static_cast<UINT>(exitCode));
        ALT_UNREACHABLE();
    }

    ALT_NODISCARD ALT_FORCE_INLINE usize GetLastError() const noexcept {

        return static_cast<usize>((*pGetLastError)());
    }

    ALT_NODISCARD ALT_FORCE_INLINE usize GetNtStatusToError(const NTSTATUS ntstatus) const noexcept {

        return static_cast<usize>((*pRtlNtStatusToDosError)(ntstatus));
    }

    ALT_FORCE_INLINE void GetLastErrorAsStringAppend(String& buffer) noexcept {
        iferr_scope;

        const usize err = GetLastError();
        const Utf16BufferOffset bufferOffset = Utf16BufferErrorCode(err) iferr_return;
        defer{ Utf16BufferDeferGuard(bufferOffset.GetOffset()); };

        const Slice<const wchar_t> utf16{ Utf16BufferPtr(bufferOffset.GetOffset()), Utf16BufferPtr(bufferOffset.GetOffsetEnd()) };
        buffer.AppendUtf16NoValidation(utf16.CastToCompatibleChar()) iferr_return;
    }

    ALT_NODISCARD ALT_FORCE_INLINE String GetLastErrorAsString() noexcept {

        String result;
        GetLastErrorAsStringAppend(result);

        return result;
    }

    template <typename FormatWriterT>
    ALT_NODISCARD ALT_FORCE_INLINE bool GetLastErrorAsFormatWrite(FormatWriter<FormatWriterT>& writer) noexcept {
        iferr_scope;

        const usize err = GetLastError();
        const Utf16BufferOffset bufferOffset = Utf16BufferErrorCode(err) iferr_return false;
        defer{ Utf16BufferDeferGuard(bufferOffset.GetOffset()); };

        const Slice<const wchar_t> utf16{ Utf16BufferPtr(bufferOffset.GetOffset()), Utf16BufferPtr(bufferOffset.GetOffsetEnd()) };
        writer.Append(utf16) iferr_return false;

        return true;
    }

    ALT_NODISCARD ALT_FORCE_INLINE usize GetPageSizeInBytes() const noexcept {

        return m_PageSizeInBytes;
    }

    ALT_NODISCARD ALT_FORCE_INLINE usize GetPageAlign() const noexcept {

        return m_AllocationGranularity;
    }

    ALT_NODISCARD ALT_FORCE_INLINE usize ThreadGetNumOfLogicalProcessors() const noexcept {

        return m_NumOfLogicalProcessors;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Platform::CPUArchitecture GetCpuArchitecture() const noexcept {

        return m_CpuArchitecture;
    }

    ALT_NODISCARD ALT_FORCE_INLINE u8* MemoryAllocateBytes(usize sizeInBytes, const Platform::MemoryAllocateBytesBits bits) const noexcept {

        /*
         * Allocate at least 1 byte to ensure that 'nullptr' only returned on errors
         */
        sizeInBytes += sizeInBytes == 0 ? 1 : 0;

        return reinterpret_cast<u8*>((*pHeapAlloc)(m_MainMemoryHeap, static_cast<DWORD>(bits), static_cast<SIZE_T>(sizeInBytes)));
    }

    ALT_NODISCARD ALT_FORCE_INLINE u8* MemoryReallocateBytes(void* pMemoryOld, usize sizeInBytes, const Platform::MemoryReallocateBytesBits bits) const noexcept {

        /*
         * Allocate at least 1 byte to ensure that 'nullptr' only returned on errors
         */
        sizeInBytes += sizeInBytes == 0 ? 1 : 0;

        if (!BitFlag::IsSetEnumFlags(bits, Platform::MemoryReallocateBytesBits::InPlace)) {

            if (!pMemoryOld) {
                return reinterpret_cast<u8*>((*pHeapAlloc)(m_MainMemoryHeap, static_cast<DWORD>(bits), static_cast<SIZE_T>(sizeInBytes)));
            }
        }

        return reinterpret_cast<u8*>((*pHeapReAlloc)(m_MainMemoryHeap, static_cast<DWORD>(bits), pMemoryOld, static_cast<SIZE_T>(sizeInBytes)));
    }

    ALT_FORCE_INLINE void MemoryDeallocateBytes(void* pMemory) const noexcept {

        ALT_MAYBE_UNUSED const bool isOk = (*pHeapFree)(m_MainMemoryHeap, 0, pMemory);
    }

    ALT_NODISCARD ALT_FORCE_INLINE u8* VirtualMemoryAllocateBytes(
        void* pMemory,
        usize sizeInBytes,
        const Platform::VirtualMemoryAllocateBytesBits bits,
        const Platform::VirtualMemoryAccessBits accessBits
    ) const noexcept {

        /*
         * Allocate at least 1 byte to ensure that 'nullptr' only returned on errors
         */
        sizeInBytes += sizeInBytes == 0 ? 1 : 0;

        pMemory = (*pVirtualAlloc)(pMemory, static_cast<SIZE_T>(sizeInBytes), static_cast<DWORD>(bits), static_cast<DWORD>(accessBits));
        return reinterpret_cast<u8*>(pMemory);
    }

    ALT_FORCE_INLINE void VirtualMemoryDeallocateBytes(
        void* pMemory,
        usize sizeInBytes,
        const Platform::VirtualMemoryDeallocateBytesBits bits
    ) const noexcept {

        ALT_MAYBE_UNUSED const bool isOk = (*pVirtualFree)(pMemory, static_cast<SIZE_T>(sizeInBytes), static_cast<DWORD>(bits));
    }

    ALT_NODISCARD ALT_FORCE_INLINE u64 GetTicks() const noexcept {
        iferr_scope;

        LARGE_INTEGER time{};

        const bool isOk = (*pQueryPerformanceCounter)(&time);
        isOk iferr_panic_only_dev("What wrong? {}", Platform::Error::PlatformLastError());

        return time.QuadPart;
    }

    ALT_NODISCARD ALT_FORCE_INLINE f64 GetTicksSeconds() const noexcept {
        iferr_scope;

        LARGE_INTEGER time{};

        const bool isOk = (*pQueryPerformanceCounter)(&time);
        isOk iferr_panic_only_dev("What wrong? {}", Platform::Error::PlatformLastError());

        return (f64)(time.QuadPart - m_TickState.timeCounterStart.QuadPart) / (f64)m_TickState.timeFreq.QuadPart;
    }

    ALT_FORCE_INLINE void SleepMs(const usize milliseconds) const noexcept {

        (*pSleep)(static_cast<DWORD>(milliseconds));
    }

    ALT_NODISCARD ALT_FORCE_INLINE Result<Platform::Handle, Platform::Error> ThreadCreate(
        FuncPtr<u32, void*> pFunc,
        void* pArgs,
        const Optional<usize> stackSize = Nullopt
    ) const noexcept {

        static_assert(sizeof(u32) == sizeof(DWORD) && sizeof(void*) == sizeof(LPVOID), "");

        // The default stack reservation size used by the linker is 1 MB. To specify a different default stack reservation size for all threads and fibers, use the STACKSIZE statement in the module definition (.def) file. The operating system rounds up the specified size to the nearest multiple of the system's allocation granularity (typically 64 KB).

        // 'stackSize' value is rounded up to the nearest page. Generally, the reserve size is the default reserve size specified in the executable header. However, if the initially committed size specified by dwStackSize is larger than or equal to the default reserve size, the reserve size is this new commit size rounded up to the nearest multiple of 1 MB.

        HANDLE tHandle = (*pCreateThread)(nullptr, static_cast<SIZE_T>(stackSize.GetValueOr(0)), reinterpret_cast<PTHREAD_START_ROUTINE>(pFunc), pArgs, 0, nullptr);

        if (tHandle == NULL) {
            return Platform::Error::PlatformLastError();
        }

        Platform::Handle handle;
        handle.handle = tHandle;

        return handle;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Platform::Handle ThreadGetCurrentPseudoHandle() const noexcept {

        // A pseudo handle is a special constant that is interpreted as the current thread handle. The calling thread can use this handle to specify itself whenever a thread handle is required.

        // The function cannot be used by one thread to create a handle that can be used by other threads to refer to the first thread.

        HANDLE hThread = (*pGetCurrentThread)();

        Platform::Handle handle;
        handle.handle = hThread;

        return handle;
    }

    ALT_NODISCARD ALT_FORCE_INLINE u32 ThreadGetCurrentID() const noexcept {

        const DWORD id = (*pGetCurrentThreadId)();
        return static_cast<u32>(id);
    }

    ALT_NODISCARD ALT_FORCE_INLINE Result<void, Platform::Error> ThreadClose(const Platform::Handle handle) const noexcept {

        const bool isOk = (*pCloseHandle)(handle.handle);

        if (!isOk) {
            return Platform::Error::PlatformLastError();
        }

        return Ok;
    }

    ALT_FORCE_INLINE bool ThreadSwitchToAnother() const noexcept {

        const bool isSwitched = (*pSwitchToThread)();

        return isSwitched;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Result<void, Platform::Error> ThreadSetPriorityBoost(const Platform::Handle handle, const bool isBoostEnabled) const noexcept {

        const bool isOk = (*pSetThreadPriorityBoost)(handle.handle, !isBoostEnabled);

        if (!isOk) {
            return Platform::Error::PlatformLastError();
        }

        return Ok;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Result<bool, Platform::Error> ThreadGetPriorityBoost(const Platform::Handle handle) const noexcept {

        BOOL isBoostDisabled;
        const bool isOk = (*pGetThreadPriorityBoost)(handle.handle, &isBoostDisabled);

        if (!isOk) {
            return Platform::Error::PlatformLastError();
        }

        const bool isBoostEnabled = isBoostDisabled == 0;
        return isBoostEnabled;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Result<void, Platform::Error> ThreadSetPriority(const Platform::Handle handle, const Platform::ThreadPriority priority) const noexcept {

        const bool isOk = (*pSetThreadPriority)(handle.handle, static_cast<int>(priority));

        if (!isOk) {
            return Platform::Error::PlatformLastError();
        }

        return Ok;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Platform::ThreadPriority ThreadGetPriority(const Platform::Handle handle) const noexcept {

        const int priority = (*pGetThreadPriority)(handle.handle);
        return static_cast<Platform::ThreadPriority>(priority);
    }

    ALT_NODISCARD ALT_FORCE_INLINE Result<void, Platform::Error> ThreadSetAffinityMask(const Platform::Handle handle, const u64 mask) const noexcept {

        const DWORD_PTR affinity = (*pSetThreadAffinityMask)(handle.handle, static_cast<DWORD_PTR>(mask));

        if (affinity == 0) {
            return Platform::Error::PlatformLastError();
        }

        return Ok;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Result<void, Platform::Error> ThreadSetName(const Platform::Handle handle, const wchar_t* pName) const noexcept {

        if (!pSetThreadDescription) {
            return Platform::Error::Unsupported();
        }

        const HRESULT hRes = (*pSetThreadDescription)(handle.handle, pName);

        if (__Win32__FAILED(hRes)) {
            return Platform::Error::PlatformLastError(__Win32__LASTERROR_FROM_HRESULT(hRes));
        }

        return Ok;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Result<void, Platform::Error> ThreadGetNameAppend(const Platform::Handle handle, String& buffer) const noexcept {
        iferr_scope;

        if (!pSetThreadDescription) {
            iferr_return_return Platform::Error::Unsupported();
        }

        PWSTR ppszThreadDescription;
        const HRESULT hRes = (*pGetThreadDescription)(handle.handle, &ppszThreadDescription);

        if (__Win32__FAILED(hRes)) {
            return Platform::Error::PlatformLastError(__Win32__LASTERROR_FROM_HRESULT(hRes));
        }

        defer{ (*pLocalFree)(ppszThreadDescription); };

        const Slice<const u8> bytes{ reinterpret_cast<u8*>(ppszThreadDescription), String::CalculateCStrLen(ppszThreadDescription) * sizeof(wchar_t) };
        buffer.Append(bytes, Utf::Encoding::Utf16) iferr_return Platform::Error::MemoryAllocation();

        return Ok;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static usize SemaphoreGetMax() noexcept {
        return static_cast<usize>(Num::Desc<s32>::Max());
    }

    ALT_NODISCARD ALT_FORCE_INLINE Result<Platform::Handle, Platform::Error> SemaphoreCreate(
        const u32 valInit = 0,
        const u32 valMax = static_cast<u32>(SemaphoreGetMax())
    ) const noexcept {

        HANDLE hSemaphore = (*pCreateSemaphoreW)(nullptr, static_cast<LONG>(valInit), static_cast<LONG>(valMax), nullptr);

        if (hSemaphore == NULL) {
            return Platform::Error::PlatformLastError();
        }

        Platform::Handle handle;
        handle.handle = hSemaphore;

        return handle;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Result<void, Platform::Error> SemaphoreClose(const Platform::Handle handle) const noexcept {

        const bool isOk = (*pCloseHandle)(handle.handle);

        if (!isOk) {
            return Platform::Error::PlatformLastError();
        }

        return Ok;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Result<void, Platform::Error> SemaphoreRelease(const Platform::Handle handle, const u32 inc) const noexcept {

        const bool isOk = (*pReleaseSemaphore)(handle.handle, static_cast<LONG>(inc), nullptr);

        if (!isOk) {
            return Platform::Error::PlatformLastError();
        }

        return Ok;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Platform::CriticalSection CriticalSectionCreate() const noexcept {

        Platform::CriticalSection critsect;
        (*pInitializeCriticalSection)(&critsect.critsect);

        return critsect;
    }

    ALT_FORCE_INLINE void CriticalSectionDelete(Platform::CriticalSection& critsect) const noexcept {

        (*pDeleteCriticalSection)(&critsect.critsect);
    }

    ALT_FORCE_INLINE void CriticalSectionEnter(Platform::CriticalSection& critsect) const noexcept {

        (*pEnterCriticalSection)(&critsect.critsect);
    }

    ALT_NODISCARD ALT_FORCE_INLINE bool CriticalSectionTryEnter(Platform::CriticalSection& critsect) const noexcept {

        const bool isOk = (*pTryEnterCriticalSection)(&critsect.critsect);
        return isOk;
    }

    ALT_FORCE_INLINE void CriticalSectionLeave(Platform::CriticalSection& critsect) const noexcept {

        (*pLeaveCriticalSection)(&critsect.critsect);
    }

    ALT_NODISCARD ALT_FORCE_INLINE bool IsRWMutexSupported() const noexcept {

        return pAcquireSRWLockShared != nullptr;
    }

    ALT_FORCE_INLINE void RWMutexLock(Platform::RWMutex& mutex) const noexcept {
        ALT_ASSERT(IsRWMutexSupported());

        (*pAcquireSRWLockExclusive)(&mutex.mutex);
    }

    ALT_FORCE_INLINE void RWMutexLockRead(Platform::RWMutex& mutex) const noexcept {
        ALT_ASSERT(IsRWMutexSupported());

        (*pAcquireSRWLockShared)(&mutex.mutex);
    }

    ALT_FORCE_INLINE bool RWMutexTryLock(Platform::RWMutex& mutex) const noexcept {
        ALT_ASSERT(IsRWMutexSupported());

        const bool isLocked = (*pTryAcquireSRWLockExclusive)(&mutex.mutex);
        return isLocked;
    }

    ALT_FORCE_INLINE bool RWMutexTryLockRead(Platform::RWMutex& mutex) const noexcept {
        ALT_ASSERT(IsRWMutexSupported());

        const bool isLocked = (*pTryAcquireSRWLockShared)(&mutex.mutex);
        return isLocked;
    }

    ALT_FORCE_INLINE void RWMutexUnlock(Platform::RWMutex& mutex) const noexcept {
        ALT_ASSERT(IsRWMutexSupported());

        (*pReleaseSRWLockExclusive)(&mutex.mutex);
    }

    ALT_FORCE_INLINE void RWMutexUnlockRead(Platform::RWMutex& mutex) const noexcept {
        ALT_ASSERT(IsRWMutexSupported());

        (*pReleaseSRWLockShared)(&mutex.mutex);
    }

    ALT_NODISCARD ALT_FORCE_INLINE bool IsWaitWakeOnAdressSupported() const noexcept {

        return pWaitOnAddress != nullptr;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Result<Platform::WaitOnAdressResult, Platform::Error> WaitOnAdress(
        const void* pAddress,
        const void* pCompare,
        const usize sizeInBytes,
        const u32 milliseconds
    ) const noexcept {

        if (!IsWaitWakeOnAdressSupported()) {
            return Platform::Error::Unsupported();
        }

        const bool isOk = (*pWaitOnAddress)(
            static_cast<volatile void*>(const_cast<void*>(pAddress)),
            const_cast<void*>(pCompare),
            static_cast<SIZE_T>(sizeInBytes),
            static_cast<DWORD>(milliseconds)
        );

        if (!isOk) {

            Platform::Error err = Platform::Error::PlatformLastError();

            if (err.GetErrCode() == __Win32__ERROR_TIMEOUT) {
                return Platform::WaitOnAdressResult::Timeout;
            }

            return err;
        }

        return Platform::WaitOnAdressResult::Signaled;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Result<void, Platform::Error> WakeByAddressSingle(const void* pAddress) const noexcept {

        if (!IsWaitWakeOnAdressSupported()) {
            return Platform::Error::Unsupported();
        }

        (*pWakeByAddressSingle)(const_cast<void*>(pAddress));

        return Ok;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Result<void, Platform::Error> WakeByAddressAll(const void* pAddress) const noexcept {

        if (!IsWaitWakeOnAdressSupported()) {
            return Platform::Error::Unsupported();
        }

        (*pWakeByAddressAll)(const_cast<void*>(pAddress));

        return Ok;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Result<Platform::WaitForSingleObjectResult, Platform::Error> WaitForSingleObject(
        const Platform::Handle handle,
        const u32 milliseconds
    ) const noexcept {

        const DWORD result = (*pWaitForSingleObject)(handle.handle, static_cast<DWORD>(milliseconds));

        if (result == __Win32__WAIT_FAILED) {
            return Platform::Error::PlatformLastError();
        }

        return static_cast<Platform::WaitForSingleObjectResult>(result);
    }

    ALT_NODISCARD ALT_FORCE_INLINE Result<void, Platform::Error> ConsoleWrite(const StdIo stdio, const wchar_t* pData, usize size_) const noexcept {
        iferr_scope;

        HANDLE hConsole = (*pGetStdHandle)(static_cast<DWORD>(stdio));

        if (hConsole == NULL || hConsole == __Win32__INVALID_HANDLE_VALUE) {
            iferr_return_return Platform::Error::PlatformLastError();
        }

        // WriteConsole fails if it is used with a standard handle that is redirected to a file. If an application processes multilingual output that can be redirected, determine whether the output handle is a console handle (one method is to call the GetConsoleMode function and check whether it succeeds). If the handle is a console handle, call WriteConsole. If the handle is not a console handle, the output is redirected and you should call WriteFile to perform the I/O. Be sure to prefix a Unicode plain text file with a byte order mark. For more information, see Using Byte Order Marks.

        // Some applications operate outside the boundaries of their declared subsystem; for instance, a /SUBSYSTEM:WINDOWS application might check/use standard handles for logging or debugging purposes but operate normally with a graphical user interface. These applications will need to carefully probe the state of standard handles on startup and make use of AttachConsole, AllocConsole, and FreeConsole to add/remove a console if desired.

        // Some applications may also vary their behavior on the type of inherited handle. Disambiguating the type between console, pipe, file, and others can be performed with GetFileType.

        DWORD flags;
        if (!(*pGetConsoleMode)(hConsole, &flags)) {

            Platform::Handle handle;
            handle.handle = hConsole;

            Platform::Error err;

            const Slice<const u8> bom = Utf::EncodingToBom(Utf::Encoding::Utf16, Platform::IsLittleEndian());
            FileWrite(handle, bom.Data(), bom.GetSize()) iferr_return_with(err) err;

            FileWrite(handle, reinterpret_cast<const u8*>(pData), size_ * sizeof(u16)) iferr_return_with(err) err;

            return Ok;
        }

        // ERROR_NOT_ENOUGH_MEMORY
        constexpr usize MAX_WCHAR_PER_CONSOLE_WRITE = 8192;

        while (size_ > 0) {

            DWORD sizeWrite = 0;
            DWORD size = static_cast<DWORD>(Num::Min(size_, MAX_WCHAR_PER_CONSOLE_WRITE));

            size_ -= static_cast<usize>(size);

            while (size > 0) {

                const bool isOk = (*pWriteConsoleW)(
                    hConsole,
                    pData,
                    size,
                    &sizeWrite,
                    nullptr
                );

                if (!isOk) {
                    iferr_return_return Platform::Error::PlatformLastError();
                }

                pData += sizeWrite;
                size -= sizeWrite;
            }
        }

        return Ok;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Result<Utf16BufferOffset, Platform::Error> ConsoleRead() noexcept {

        const usize sizeOld = stl_Utf16Buffer.GetSize();
        iferr_scope_on_err {
            iferr_scope;

            stl_Utf16Buffer.Resize(sizeOld) iferr_panic_only_dev("resize to the less size - never panic");
        };

        HANDLE hConsole = (*pGetStdHandle)(static_cast<DWORD>(StdIo::in));

        if (hConsole == NULL || hConsole == __Win32__INVALID_HANDLE_VALUE) {
            iferr_return_return Platform::Error::PlatformLastError();
        }

        DWORD flags;
        if (!(*pGetConsoleMode)(hConsole, &flags)) {

            // TODO: We can read from file but need to handle encoding also check '\n' and
            // seek file pointer to align with default behaviour of 'ReadConsoleW(...)' below
            iferr_return_return Platform::Error::Unsupported();
        }

        constexpr usize MAX_WCHAR_PER_CONSOLE_READ = 8192;

        usize offsetEnd = sizeOld;
        DWORD sizeRead = 0;

        for (;;) {

            stl_Utf16Buffer.ResizeAddition(MAX_WCHAR_PER_CONSOLE_READ) iferr_return Platform::Error::MemoryAllocation();

            const bool isOk = (*pReadConsoleW)(
                hConsole,
                Utf16BufferPtr(offsetEnd),
                static_cast<DWORD>(MAX_WCHAR_PER_CONSOLE_READ),
                &sizeRead,
                nullptr
            );

            if (!isOk) {
                iferr_return_return Platform::Error::PlatformLastError();
            }

            offsetEnd += static_cast<usize>(sizeRead);

            if (sizeRead != static_cast<DWORD>(MAX_WCHAR_PER_CONSOLE_READ) || stl_Utf16Buffer[offsetEnd - 1] == static_cast<u16>('\n')) {
                break;
            }
        }

        stl_Utf16Buffer.Resize(offsetEnd) iferr_panic_only_dev("resize to the less size - never panic");

        Utf16BufferOffset bufferOffset{};
        bufferOffset.m_Offset = sizeOld;
        bufferOffset.m_OffsetEnd = offsetEnd;

        return bufferOffset;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Result<Utf16BufferOffset, Platform::Error> ConsoleGetTitle() noexcept {

        const usize sizeOld = stl_Utf16Buffer.GetSize();
        iferr_scope_on_err {
            iferr_scope;

            stl_Utf16Buffer.Resize(sizeOld) iferr_panic_only_dev("resize to the less size - never panic");
        };

        // https://learn.microsoft.com/en-us/windows/console/setconsoletitle
        // The string to be displayed in the title bar of the console window. The total size must be less than 64K.

        // Example uses MAX_PATH

        stl_Utf16Buffer.ResizeAddition(__Win32__MAX_PATH) iferr_return Platform::Error::MemoryAllocation();

        DWORD result = (*pGetConsoleTitleW)(Utf16BufferPtr(sizeOld), __Win32__MAX_PATH);
        if (result == 0) {
            iferr_return_return Platform::Error::PlatformLastError();
        }

        if (result <= __Win32__MAX_PATH) {

            stl_Utf16Buffer.Resize(sizeOld + static_cast<usize>(result)) iferr_panic_only_dev("resize to the less size - never panic");

            Utf16BufferOffset offset{};
            offset.m_Offset = sizeOld;
            offset.m_OffsetEnd = sizeOld + static_cast<usize>(result);

            return offset;
        }

        stl_Utf16Buffer.Resize(sizeOld + static_cast<usize>(result) + 1) iferr_return Platform::Error::MemoryAllocation();

        result = (*pGetConsoleTitleW)(Utf16BufferPtr(sizeOld), result);
        if (result == 0) {
            iferr_return_return Platform::Error::PlatformLastError();
        }

        stl_Utf16Buffer.Resize(sizeOld + static_cast<usize>(result)) iferr_panic_only_dev("resize to the less size - never panic");

        Utf16BufferOffset offset{};
        offset.m_Offset = sizeOld;
        offset.m_OffsetEnd = sizeOld + static_cast<usize>(result);

        return offset;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Result<void, Platform::Error> ConsoleSetTitle(const wchar_t* pTitle) const noexcept {

        const bool isOk = (*pSetConsoleTitleW)(pTitle);
        if (!isOk) {
            return Platform::Error::PlatformLastError();
        }

        return Ok;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Result<Pair<usize, usize>, Platform::Error> ConsoleGetSizeInChars() const noexcept {

        HANDLE hConsole = (*pGetStdHandle)(__Win32__STD_OUTPUT_HANDLE);
        if (hConsole == NULL || hConsole == __Win32__INVALID_HANDLE_VALUE) {
            return Platform::Error::PlatformLastError();
        }

        CONSOLE_SCREEN_BUFFER_INFO csbi;

        const bool isOk = (*pGetConsoleScreenBufferInfo)(hConsole, &csbi);
        if (!isOk) {
            return Platform::Error::PlatformLastError();
        }

        return MakePair(static_cast<usize>(csbi.dwSize.X), static_cast<usize>(csbi.dwSize.Y));
    }

    ALT_NODISCARD ALT_FORCE_INLINE Result<usize, Platform::Error> ConsoleGetCursorPosYInChars() const noexcept {

        HANDLE hConsole = (*pGetStdHandle)(__Win32__STD_OUTPUT_HANDLE);
        if (hConsole == NULL || hConsole == __Win32__INVALID_HANDLE_VALUE) {
            return Platform::Error::PlatformLastError();
        }

        CONSOLE_SCREEN_BUFFER_INFO csbi;

        const bool isOk = (*pGetConsoleScreenBufferInfo)(hConsole, &csbi);
        if (!isOk) {
            return Platform::Error::PlatformLastError();
        }

        return static_cast<usize>(csbi.dwCursorPosition.Y);
    }

    ALT_NODISCARD ALT_FORCE_INLINE Result<Utf16BufferOffset, Platform::Error> GetEnvironmentVariable(const usize offset) noexcept {
        iferr_scope;

        DWORD size = (*pGetEnvironmentVariableW)(Utf16BufferPtr(offset), nullptr, 0);

        if (size == 0) {
            iferr_return_return Platform::Error::PlatformLastError();
        }

        const usize sizeOld = stl_Utf16Buffer.GetSize();
        stl_Utf16Buffer.ResizeAddition(static_cast<usize>(size));

        size = (*pGetEnvironmentVariableW)(Utf16BufferPtr(offset), Utf16BufferPtr(sizeOld), size);

        Utf16BufferOffset pathOffset{};
        pathOffset.m_Offset = sizeOld;
        pathOffset.m_OffsetEnd = sizeOld + static_cast<usize>(size);

        return pathOffset;
    }

    ALT_NODISCARD ALT_FORCE_INLINE char FilePathSeparator() const noexcept {
        return ALT_DS;
    }

    ALT_NODISCARD ALT_FORCE_INLINE wchar_t FilePathSeparatorUTF16() const noexcept {
        return L'\\';
    }

    ALT_NODISCARD ALT_FORCE_INLINE usize FileGetLogicalDriversBits() const noexcept {
        return (*pGetLogicalDrives)();
    }

    ALT_NODISCARD ALT_FORCE_INLINE Result<Platform::Handle, Platform::Error> FileOpenOrCreate(
        wchar_t* pFilepath,
        const usize size,
        const Platform::FileOpenOrCreateBits createBits,
        const Platform::FileShareBits shareBits,
        const Platform::FileAccessBits accessBits_,
        const Platform::FileHintBits hintBits_
    ) const noexcept {

        Result<Platform::Handle, Platform::Error> result = FileOpenOrCreateJustOneFile(pFilepath, createBits, shareBits, accessBits_, hintBits_);
        if (result) {
            return result;
        }

        const usize errCode = result.GetErr().GetErrCode();
        if (errCode == __Win32__ERROR_PATH_NOT_FOUND) {

            const Slice<wchar_t> filepath{ pFilepath, size };
            return FileOpenOrCreateRecursive(filepath, createBits, shareBits, accessBits_, hintBits_, errCode);
        }

        return result;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Result<Platform::Handle, Platform::Error> FileCreateMapMemoryObject(
        const Platform::Handle handle,
        const Platform::FileCreateMapMemoryObjectBits bits,
        const u64 sizeInBytes
    ) const noexcept {

        // If this parameter and sizeInBytes are 0 (zero), the maximum size of the file mapping object is equal to the current size of the file that hFile identifies.
        // An attempt to map a file with a length of 0 (zero) fails with an error code of ERROR_FILE_INVALID. Applications should test for files with a length of 0 (zero) and reject those files.

        HANDLE win32Handle = (*pCreateFileMappingW)(
            handle.handle,
            nullptr,
            static_cast<DWORD>(bits),
            static_cast<DWORD>(sizeInBytes >> 32),
            static_cast<DWORD>(sizeInBytes),
            nullptr
        );

        if (win32Handle == NULL) {
            return Platform::Error::PlatformLastError();
        }

        Platform::Handle hMap;
        hMap.handle = win32Handle;

        return hMap;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Result<u8*, Platform::Error> FileMapMemory(
        const Platform::Handle handle,
        const Platform::FileMapMemoryBits bits,
        const u64 offset,
        const usize sizeInBytes
    ) const noexcept {

        ALT_ASSERT(Num::IsPowerOf2(m_AllocationGranularity));

        const u64 pageAlign = static_cast<u64>(m_AllocationGranularity);

        const u64 offsetPtr = offset & (pageAlign - 1);
        const u64 offsetBase = offset & ~(pageAlign - 1);

        // sizeInBytes - The number of bytes of a file mapping to map to the view. All bytes must be within the maximum size specified by CreateFileMapping. If this parameter is 0 (zero), the mapping extends from the specified offset to the end of the file mapping.

        usize sizeInBytesAdjusted;
        if (Num::IsOverflowCheckAdd(sizeInBytes, offsetPtr, &sizeInBytesAdjusted)) {
            sizeInBytesAdjusted = 0;
        }

        void* pMemory = (*pMapViewOfFile)(
            handle.handle,
            static_cast<DWORD>(bits),
            static_cast<DWORD>(offsetBase >> 32),
            static_cast<DWORD>(offsetBase),
            static_cast<SIZE_T>(sizeInBytesAdjusted)
        );

        if (!pMemory) {
            return Platform::Error::PlatformLastError();
        }

        return reinterpret_cast<u8*>(pMemory) + offsetPtr;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Result<void, Platform::Error> FileUnmapMemory(const void* pBaseMemory) const noexcept {

        const bool isOk = (*pUnmapViewOfFile)(pBaseMemory);

        if (!isOk) {
            return Platform::Error::PlatformLastError();
        }

        return Ok;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Result<void, Platform::Error> FileFlushMappedMemory(const void* pBaseMemory, const usize sizeInBytes) const noexcept {

        // The number of bytes to be flushed. If sizeInBytes is zero, the file is flushed from the base address to the end of the mapping.

        const bool isOk = (*pFlushViewOfFile)(pBaseMemory, static_cast<SIZE_T>(sizeInBytes));

        if (!isOk) {
            return Platform::Error::PlatformLastError();
        }

        return Ok;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Result<void, Platform::Error> FileFlush(const Platform::Handle handle) const noexcept {

        const bool isOk = (*pFlushFileBuffers)(handle.handle);

        if (!isOk) {
            return Platform::Error::PlatformLastError();
        }

        return Ok;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Result<void, Platform::Error> FileClose(const Platform::Handle handle) const noexcept {

        const bool isOk = (*pCloseHandle)(handle.handle);

        if (!isOk) {
            return Platform::Error::PlatformLastError();
        }

        return Ok;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Result<void, Platform::Error> FileDelete(const wchar_t* pFilepath) const noexcept {

        const bool isOk = (*pDeleteFileW)(pFilepath);

        if (!isOk) {
            return Platform::Error::PlatformLastError();
        }

        return Ok;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Result<void, Platform::Error> FileWrite(const Platform::Handle handle, const u8* pData, usize size_) const noexcept {

        while (size_ > 0) {

            DWORD sizeWrite = 0;
            DWORD size = static_cast<DWORD>(size_);

            size_ -= static_cast<usize>(size);

            while (size > 0) {

                const bool isOk = (*pWriteFile)(
                    handle.handle,
                    pData,
                    size,
                    &sizeWrite,
                    nullptr
                );

                if (!isOk) {
                    return Platform::Error::PlatformLastError();
                }

                pData += sizeWrite;
                size -= sizeWrite;
            }
        }

        return Ok;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Result<usize, Platform::Error> FileRead(const Platform::Handle handle, u8* pData, usize size_) const noexcept {

        u8* pDataBegin = pData;

        while (size_ > 0) {

            DWORD sizeRead = 0;
            DWORD size = static_cast<DWORD>(size_);

            size_ -= static_cast<usize>(size);

            do {

                const bool isOk = (*pReadFile)(
                    handle.handle,
                    pData,
                    size,
                    &sizeRead,
                    nullptr
                );

                if (!isOk) {
                    return Platform::Error::PlatformLastError();
                }

                pData += sizeRead;
                size -= sizeRead;
            } while (size != 0 && sizeRead != 0);
        }

        return static_cast<usize>(pData - pDataBegin);
    }

    ALT_NODISCARD ALT_FORCE_INLINE Result<usize, Platform::Error> FileSize(const Platform::Handle handle) const noexcept {

        LARGE_INTEGER result;
        const bool isOk = (*pGetFileSizeEx)(handle.handle, &result);

        if (!isOk) {
            return Platform::Error::PlatformLastError();
        }

        return static_cast<usize>(result.QuadPart);
    }

    ALT_NODISCARD ALT_FORCE_INLINE Result<usize, Platform::Error> FileSeek(const Platform::Handle handle, const Platform::FileSeekOpt opt) const noexcept {

        LARGE_INTEGER move{};
        LARGE_INTEGER result;

        const bool isOk = (*pSetFilePointerEx)(
            handle.handle,
            move,
            &result,
            opt == Platform::FileSeekOpt::Begin ? __Win32__FILE_BEGIN : __Win32__FILE_END
        );

        if (!isOk) {
            return Platform::Error::PlatformLastError();
        }

        return static_cast<usize>(result.QuadPart);
    }

    ALT_NODISCARD ALT_FORCE_INLINE Result<void, Platform::Error> FileClear(const Platform::Handle handle) const noexcept {
        iferr_scope;

        Platform::Error err;
        ALT_MAYBE_UNUSED const usize size = FileSeek(handle, Platform::FileSeekOpt::Begin) iferr_return_with(err) err;

        const bool isOk = (*pSetEndOfFile)(handle.handle);

        if (!isOk) {
            iferr_return_return Platform::Error::PlatformLastError();
        }

        return Ok;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Result<void, Platform::Error> FileResize(const Platform::Handle handle, const usize sizeInBytes) const noexcept {

        LARGE_INTEGER move{};
        move.QuadPart = static_cast<LONGLONG>(sizeInBytes);

        LARGE_INTEGER result;

        bool isOk = (*pSetFilePointerEx)(
            handle.handle,
            move,
            &result,
            __Win32__FILE_BEGIN
        );

        if (!isOk) {
            return Platform::Error::PlatformLastError();
        }

        isOk = (*pSetEndOfFile)(handle.handle);

        if (!isOk) {
            return Platform::Error::PlatformLastError();
        }

        return Ok;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Result<Utf16BufferOffset, Platform::Error> FileGetTempDirectory() noexcept {

        const usize sizeOld = stl_Utf16Buffer.GetSize();
        iferr_scope_on_err {
            iferr_scope;

            stl_Utf16Buffer.Resize(sizeOld) iferr_panic_only_dev("resize to the less size - never panic");
        };

        stl_Utf16Buffer.ResizeAddition(__Win32__MAX_PATH + 1) iferr_return Platform::Error::MemoryAllocation();

        DWORD result = (*pGetTempPathW)(__Win32__MAX_PATH + 1, Utf16BufferPtr(sizeOld));
        if (result == 0) {
            iferr_return_return Platform::Error::PlatformLastError();
        }

        // remove last folder separator
        result -= 1;

        stl_Utf16Buffer.Resize(sizeOld + static_cast<usize>(result)) iferr_panic_only_dev("resize to the less size - never panic");

        Utf16BufferOffset offset{};
        offset.m_Offset = sizeOld;
        offset.m_OffsetEnd = sizeOld + static_cast<usize>(result);

        return offset;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Result<Utf16BufferOffset, Platform::Error> FileGetCurrentDirectory() noexcept {

        const usize sizeOld = stl_Utf16Buffer.GetSize();
        iferr_scope_on_err {
            iferr_scope;

            stl_Utf16Buffer.Resize(sizeOld) iferr_panic_only_dev("resize to the less size - never panic");
        };

        const DWORD sizeToAllocate = (*pGetCurrentDirectoryW)(0, nullptr);
        if (sizeToAllocate == 0) {
            iferr_return_return Platform::Error::PlatformLastError();
        }

        stl_Utf16Buffer.ResizeAddition(static_cast<usize>(sizeToAllocate)) iferr_return Platform::Error::MemoryAllocation();

        const DWORD result = (*pGetCurrentDirectoryW)(sizeToAllocate, Utf16BufferPtr(sizeOld));
        if (result == 0) {
            iferr_return_return Platform::Error::PlatformLastError();
        }

        Utf16BufferOffset offset{};
        offset.m_Offset = sizeOld;
        offset.m_OffsetEnd = sizeOld + static_cast<usize>(result);

        return offset;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Platform::FileTypeBits FileGetFileType(const Platform::Handle handle) const noexcept {

        Platform::FileTypeBits result = Platform::FileTypeBits::None;
        const DWORD type = (*pGetFileType)(handle.handle);

        if (type == __Win32__FILE_TYPE_CHAR) {
            result = Platform::FileTypeBits::File | Platform::FileTypeBits::Char;
        }
        else if (type == __Win32__FILE_TYPE_DISK) {
            result = Platform::FileTypeBits::File | Platform::FileTypeBits::RegularFile;
        }
        else if (type == __Win32__FILE_TYPE_PIPE) {
            result = Platform::FileTypeBits::File | Platform::FileTypeBits::Pipe;
        }

        return result;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Result<Platform::FileTypeBits, Platform::Error> FileGetFileType(const wchar_t* pFilepath) const noexcept {

        const DWORD err = (*pGetFileAttributesW)(pFilepath);

        if (err == __Win32__INVALID_FILE_ATTRIBUTES) {
            return Platform::Error::PlatformLastError();
        }

        if ((err & __Win32__FILE_ATTRIBUTE_DIRECTORY) != 0) {
            return Platform::FileTypeBits::None;
        }

        HANDLE handle = (*pCreateFileW)(
            pFilepath,
            0,
            __Win32__FILE_SHARE_READ | __Win32__FILE_SHARE_WRITE | __Win32__FILE_SHARE_DELETE,
            nullptr,
            __Win32__OPEN_EXISTING,

            // do not use '__Win32__FILE_FLAG_OPEN_REPARSE_POINT'
            __Win32__FILE_FLAG_BACKUP_SEMANTICS,

            nullptr
        );

        if (handle == __Win32__INVALID_HANDLE_VALUE) {
            return Platform::Error::PlatformLastError();
        }

        defer{ (*pCloseHandle)(handle); };

        Platform::Handle hFile;
        hFile.handle = handle;

        return FileGetFileType(hFile);
    }

    ALT_NODISCARD ALT_FORCE_INLINE Result<Platform::FileInfo, Platform::Error> FileGetInfo(const Platform::Handle handle) const noexcept {

        return FileGetInfoByHandle_XP(*this, handle);
    }

    ALT_NODISCARD ALT_FORCE_INLINE Result<void, Platform::Error> FileDirectoryCreate(wchar_t* pFilepath, const usize size) const noexcept {

        const bool isOk = (*pCreateDirectoryW)(pFilepath, nullptr);
        const usize errCode = GetLastError();

        if (isOk || errCode == __Win32__ERROR_ALREADY_EXISTS) {
            return Ok;
        }

        if (errCode == __Win32__ERROR_PATH_NOT_FOUND) {

            const Slice<wchar_t> filepath{ pFilepath, size };
            return FileDirectoryCreateRecursive(filepath, errCode);
        }

        return Platform::Error::PlatformLastError(errCode);
    }

    ALT_NODISCARD ALT_FORCE_INLINE bool FileRemoveAll(
        const usize offsetToFileDirInfo,
        const usize offsetToPath,
        const usize sizeInBytesFileDirInfo,
        const _PlatformDetails::DirectoryEntrySizeResult& directoryEntrySizeResult,
        const Platform::FileRemoveAllType type
    ) noexcept {

        return FileRemoveAllCtx{}.Run(*this, offsetToFileDirInfo, offsetToPath, sizeInBytesFileDirInfo, directoryEntrySizeResult, type);
    }

    ALT_NODISCARD ALT_FORCE_INLINE DirectoryEntrySizeResult FileForEachDirectoryEntry_GetDirectoryEntrySize(const usize sizeInBytes) const noexcept {

        // https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntifs/ns-ntifs-_file_directory_information
        // This structure must be aligned on a LONGLONG (8-byte) boundary. If a buffer contains two or more of these
        // structures, the NextEntryOffset value in each entry, except the last, falls on an 8-byte boundary.

        constexpr usize ALIGNMENT = 8;

        const usize sizeInBytesFileDirInfo = Num::Max(sizeInBytes, sizeof(FILE_DIRECTORY_INFORMATION) + 256 * sizeof(wchar_t));

        DirectoryEntrySizeResult result{};
        result.sizeInBytesFileDirInfo = sizeInBytesFileDirInfo;
        result.sizeToAllocate = Num::CeilDiv(sizeInBytesFileDirInfo + (ALIGNMENT - 1), sizeof(u16));

        return result;
    }

    template <typename Func>
    ALT_NODISCARD ALT_FORCE_INLINE Result<void, Platform::Error> FileForEachDirectoryEntry(
        const usize offsetToFileDirInfo,
        const usize offsetToPath,
        const usize sizeInBytesFileDirInfo,
        Optional<String&> buffer,
        Func&& func,
        const Platform::FileForEachDirectoryEntryBits bits
    ) noexcept {
        iferr_scope;

        // NOTE: assumed that 'offsetToFileDirInfo' < 'offsetToPath' which allows to temporary append
        // filenames and form complete contiguous path
        ALT_ASSERT(offsetToFileDirInfo < offsetToPath);

        HANDLE handle = (*pCreateFileW)(
            Utf16BufferPtr(offsetToPath),
            __Win32__FILE_LIST_DIRECTORY | __Win32__SYNCHRONIZE,
            __Win32__FILE_SHARE_READ | __Win32__FILE_SHARE_WRITE | __Win32__FILE_SHARE_DELETE,
            nullptr,
            __Win32__OPEN_EXISTING,
            __Win32__FILE_FLAG_BACKUP_SEMANTICS,
            nullptr
        );

        if (handle == __Win32__INVALID_HANDLE_VALUE) {
            return Platform::Error::PlatformLastError();
        }

        defer{ (pCloseHandle)(handle); };

        // see. 'FileForEachDirectoryEntry_GetDirectoryEntrySize(...)'
        constexpr usize ALIGNMENT = 8;

        const bool isCreatePathOffset = BitFlag::IsSetEnumFlags(bits, Platform::FileForEachDirectoryEntryBits::CreatePathOffset);
        if (isCreatePathOffset) {

            const usize pathOffsetEnd = stl_Utf16Buffer.GetSize();
            const usize pathSize = Utf16BufferSizeAssumeLast(offsetToPath, Utf16BufferAppendBits::PathBuffer);

            const bool isAlreadyWithSeparator = pathSize > 1 && (stl_Utf16Buffer[pathOffsetEnd - 2] == ALT_DS || stl_Utf16Buffer[pathOffsetEnd - 2] == '/');

            if (isAlreadyWithSeparator) {

                // remove nullterm
                stl_Utf16Buffer.Resize(pathOffsetEnd - 1) iferr_panic_only_dev("resize to the less size - never panic");
            }
            else {

                stl_Utf16Buffer[pathOffsetEnd - 1] = ALT_DS;
            }
        }
        else {

            // we do not need this memory
            stl_Utf16Buffer.Resize(offsetToPath) iferr_panic_only_dev("resize to the less size - never panic");
        }

        IO_STATUS_BLOCK iosb;

        NTSTATUS ntstatus = (pNtQueryDirectoryFile)(
            handle,
            nullptr,
            nullptr,
            nullptr,
            &iosb,
            reinterpret_cast<u8*>(Memory::AlignPtr(Utf16BufferPtr(offsetToFileDirInfo), ALIGNMENT)),
            static_cast<ULONG>(sizeInBytesFileDirInfo),
            FileDirectoryInformation,
            false,
            nullptr,
            true
        );

        if (ntstatus == __Win32__STATUS_NO_MORE_FILES) {
            return Ok;
        }

        if (ntstatus == 0 && iosb.Information == 0) {
            ntstatus = __Win32__STATUS_BUFFER_OVERFLOW;
        }

        if (ntstatus != 0) {
            return Platform::Error::PlatformNtError(ntstatus);
        }

        for (;;) {

            usize offset = 0;
            for (;;) {

                FILE_DIRECTORY_INFORMATION* pFdi = reinterpret_cast<FILE_DIRECTORY_INFORMATION*>(
                    reinterpret_cast<u8*>(Memory::AlignPtr(Utf16BufferPtr(offsetToFileDirInfo), ALIGNMENT)) + offset
                );

                const usize offsetNext = static_cast<usize>(pFdi->NextEntryOffset);

                const usize filenameSizeInBytes = static_cast<usize>(pFdi->FileNameLength);
                Slice<const wchar_t> filename{ static_cast<const wchar_t*>(pFdi->FileName), filenameSizeInBytes / sizeof(wchar_t) };

                const bool isSkip = filename.IsEmpty() || filename == L"."_s || filename == L".."_s;
                if (!isSkip) {

                    Platform::FileDirectoryEntry entry{};

                    entry.creationTime = Time::FromWin32FileTime(static_cast<u64>(pFdi->CreationTime.QuadPart));
                    entry.lastAccessTime = Time::FromWin32FileTime(static_cast<u64>(pFdi->LastAccessTime.QuadPart));
                    entry.lastWriteTime = Time::FromWin32FileTime(static_cast<u64>(pFdi->LastWriteTime.QuadPart));
                    entry.lastChangeTime = Time::FromWin32FileTime(static_cast<u64>(pFdi->ChangeTime.QuadPart));

                    entry.allocatedSizeInBytes = static_cast<usize>(pFdi->AllocationSize.QuadPart);
                    entry.logicalSizeInBytes = static_cast<usize>(pFdi->EndOfFile.QuadPart);

                    entry.isDir = (pFdi->FileAttributes & __Win32__FILE_ATTRIBUTE_DIRECTORY) != 0;
                    entry.isSymLinkLike = (pFdi->FileAttributes & __Win32__FILE_ATTRIBUTE_REPARSE_POINT) != 0;
                    entry.isFile = !entry.isDir && !entry.isSymLinkLike;

                    const Optional<String::Checkpoint> checkpoint = buffer ? MakeOptional(buffer->GetCheckpoint()) : Nullopt;
                    defer{ if (buffer) buffer->TruncateToCheckpoint(*checkpoint); };

                    if (BitFlag::IsSetEnumFlags(bits, Platform::FileForEachDirectoryEntryBits::AppendToString)) {
                        ALT_ASSERT(buffer);

                        const bool isAlreadyWithSeparator = buffer->IsPopulated() && (buffer->GetLastByte() == ALT_DS || buffer->GetLastByte() == '/'); 
                        if (!isAlreadyWithSeparator) {
                            buffer->Push(ALT_DS) iferr_return Platform::Error::MemoryAllocation();
                        }

                        buffer->AppendUtf16NoValidation(filename.CastToCompatibleChar()) iferr_return Platform::Error::MemoryAllocation();

                        entry.filename = *buffer;
                        entry.filename = entry.filename->SubSlice(checkpoint->GetOffsetInBytes() + (isAlreadyWithSeparator ? 0 : 1));
                    }

                    const usize offsetToFilename = stl_Utf16Buffer.GetSize();
                    defer{ iferr_scope; stl_Utf16Buffer.Resize(offsetToFilename) iferr_panic_only_dev("resize to the less size - never panic"); };

                    if (isCreatePathOffset) {

                        stl_Utf16Buffer.ResizeAddition(filename.GetSize(), 1) iferr_return Platform::Error::MemoryAllocation();

                        FILE_DIRECTORY_INFORMATION* pFdi = reinterpret_cast<FILE_DIRECTORY_INFORMATION*>(
                            reinterpret_cast<u8*>(Memory::AlignPtr(Utf16BufferPtr(offsetToFileDirInfo), ALIGNMENT)) + offset
                        );

                        filename = Slice<const wchar_t>{ static_cast<const wchar_t*>(pFdi->FileName), filenameSizeInBytes / sizeof(wchar_t) };

                        Memory::CopyObjectsNonOverlap(
                            stl_Utf16Buffer.Data() + offsetToFilename,
                            filename.CastToCompatibleChar().Data(),
                            filename.CastToCompatibleChar().DataEnd()
                        );

                        stl_Utf16Buffer.GetLast() = 0;

                        Platform::PathOffset offsetToFilepath{};
                        offsetToFilepath.m_Offset = offsetToPath;
                        offsetToFilepath.m_OffsetEnd = stl_Utf16Buffer.GetSize();
                        offsetToFilepath.m_BufferAppendBits = Utf16BufferAppendBits::PathBuffer;

                        entry.filepath = offsetToFilepath;
                    }

                    Platform::Error err;
                    const Platform::FileForEachDirectoryEntryContinuation result = (func)(entry) iferr_return_with(err) err;

                    if (result == Platform::FileForEachDirectoryEntryContinuation::Retry) {
                        continue;
                    }
                    else if (result == Platform::FileForEachDirectoryEntryContinuation::Break) {
                        return Ok;
                    }
                }

                offset += offsetNext;
                if (offsetNext == 0) {
                    break;
                }
            }

            ntstatus = (pNtQueryDirectoryFile)(
                handle,
                nullptr,
                nullptr,
                nullptr,
                &iosb,
                reinterpret_cast<u8*>(Memory::AlignPtr(Utf16BufferPtr(offsetToFileDirInfo), ALIGNMENT)),
                static_cast<ULONG>(sizeInBytesFileDirInfo),
                FileDirectoryInformation,
                false,
                nullptr,
                false
            );

            if (ntstatus == __Win32__STATUS_NO_MORE_FILES) {
                break;
            }

            if (ntstatus == 0 && iosb.Information == 0) {
                ntstatus = __Win32__STATUS_BUFFER_OVERFLOW;
            }

            if (ntstatus != 0) {
                return Platform::Error::PlatformNtError(ntstatus);
            }
        }

        return Ok;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Result<void, Platform::Error> FileCopyFile(const wchar_t* pFilepathFrom, const wchar_t* pFilepathTo) const noexcept {

        const bool isOk = (*pCopyFileW)(pFilepathFrom, pFilepathTo, false);

        if (!isOk) {
            return Platform::Error::PlatformLastError();
        }

        return Ok;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Result<void, Platform::Error> FileMoveAny(const wchar_t* pFilepathFrom, const wchar_t* pFilepathTo) const noexcept {

        const bool isOk = (*pMoveFileW)(pFilepathFrom, pFilepathTo);

        if (!isOk) {
            return Platform::Error::PlatformLastError();
        }

        return Ok;
    }

    ALT_NODISCARD ALT_FORCE_INLINE bool FileExistAny(const wchar_t* pFilepath) const noexcept {

        const DWORD err = (*pGetFileAttributesW)(pFilepath);

        if (err == __Win32__INVALID_FILE_ATTRIBUTES) {

            Platform::Error errCode = Platform::Error::PlatformLastError();
            const usize code = errCode.GetErrCode();

            return
                code != __Win32__ERROR_PATH_NOT_FOUND &&
                code != __Win32__ERROR_FILE_NOT_FOUND &&
                code != __Win32__ERROR_INVALID_NAME &&
                code != __Win32__ERROR_BAD_NETPATH
            ;
        }

        return true;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Result<void, Platform::Error> FileCreateSymLink(
        wchar_t* pFilepath,
        const usize size,
        const wchar_t* pFilepathTarget,
        const Platform::SymLinkTarget symLinkTarget
    ) const noexcept {
        iferr_scope;

        if (!pCreateSymbolicLinkW) {
            iferr_return_return Platform::Error::Unsupported();
        }

        Platform::Error err;
        FileCreateSymLinkJustOneFile(pFilepath, pFilepathTarget, symLinkTarget) ifok_with(err) {
            return Ok;
        }

        const usize errCode = err.GetErrCode();
        if (errCode == __Win32__ERROR_PATH_NOT_FOUND) {

            Slice<wchar_t> filepath{ pFilepath, size };
            return FileCreateSymLinkRecursive(filepath, pFilepathTarget, symLinkTarget, errCode);
        }

        return err;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Result<Utf16BufferOffset, Platform::Error> FileReadSymLink(Platform::Handle handle) noexcept {

        if (!pGetFinalPathNameByHandleW) {
            return Platform::Error::Unsupported();
        }

        const usize sizeOld = stl_Utf16Buffer.GetSize();
        iferr_scope_on_err {
            iferr_scope;

            stl_Utf16Buffer.Resize(sizeOld) iferr_panic_only_dev("resize to the less size - never panic");
        };

        const DWORD sizeToAllocate = (*pGetFinalPathNameByHandleW)(
            handle.handle,
            nullptr,
            0,
            __Win32__FILE_NAME_NORMALIZED | __Win32__VOLUME_NAME_DOS
        );

        if (sizeToAllocate == 0) {
            iferr_return_return Platform::Error::PlatformLastError();
        }

        stl_Utf16Buffer.ResizeAddition(static_cast<usize>(sizeToAllocate)) iferr_return Platform::Error::MemoryAllocation();

        wchar_t* pResult = Utf16BufferPtr(sizeOld);

        DWORD result = (*pGetFinalPathNameByHandleW)(
            handle.handle,
            pResult,
            sizeToAllocate,
            __Win32__FILE_NAME_NORMALIZED | __Win32__VOLUME_NAME_DOS
        );

        if (result == 0) {
            iferr_return_return Platform::Error::PlatformLastError();
        }

        const Slice<const wchar_t> longPrefix = L"\\\\?\\"_s;
        const Slice<const wchar_t> uncPrefix = L"\\\\?\\UNC\\"_s;

        usize offset = 0;

        Slice<wchar_t> path{ pResult, static_cast<usize>(result) };
        if (path.StartWith(longPrefix)) {

            const usize size = longPrefix.GetSize();

            offset += size;
        }
        else if (path.StartWith(uncPrefix)) {

            const usize size = uncPrefix.GetSize() - 1;

            offset += size;

            // Extended UNC -> UNC
            path[0] = '\\';
        }

        stl_Utf16Buffer.Resize(sizeOld + static_cast<usize>(result)) iferr_panic_only_dev("resize to the less size - never panic");

        Utf16BufferOffset bufferOffset{};
        bufferOffset.m_Offset = sizeOld + offset;
        bufferOffset.m_OffsetEnd = sizeOld + static_cast<usize>(result);

        return bufferOffset;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Result<Utf16BufferOffset, Platform::Error> FileReadSymLink(const wchar_t* pFilepath) noexcept {

        if (!pGetFinalPathNameByHandleW) {
            return Platform::Error::Unsupported();
        }

        HANDLE handle = (*pCreateFileW)(
            pFilepath,
            0,
            __Win32__FILE_SHARE_READ | __Win32__FILE_SHARE_WRITE | __Win32__FILE_SHARE_DELETE,
            nullptr,
            __Win32__OPEN_EXISTING,
            __Win32__FILE_FLAG_OPEN_REPARSE_POINT | __Win32__FILE_FLAG_BACKUP_SEMANTICS,
            nullptr
        );

        if (handle == __Win32__INVALID_HANDLE_VALUE) {
            return Platform::Error::PlatformLastError();
        }

        defer{ (*pCloseHandle)(handle); };

        Platform::Handle hFile;
        hFile.handle = handle;

        return FileReadSymLink(hFile);
    }

    ALT_NODISCARD ALT_FORCE_INLINE Result<void, Platform::Error> FileCopySymLink(const wchar_t* pFilepathFrom, const wchar_t* pFilepathTo) const noexcept {
        iferr_scope;

        if (!pCreateSymbolicLinkW) {
            iferr_return_return Platform::Error::Unsupported();
        }

        Platform::Error err;
        const Platform::FileTypeBits type = (*pFileGetType)(*this, pFilepathFrom, false) iferr_return_with(err) err;

        if (!BitFlag::IsSetEnumFlags(type, Platform::FileTypeBits::SymLink)) {
            iferr_return_return Platform::Error::FileNotSymLink();
        }

        const Platform::SymLinkTarget targetType = BitFlag::IsSetEnumFlags(type, Platform::FileTypeBits::Directory)
            ? Platform::SymLinkTarget::Directory
            : Platform::SymLinkTarget::File;

        return FileCreateSymLinkJustOneFile(pFilepathFrom, pFilepathTo, targetType);
    }

    ALT_NODISCARD ALT_FORCE_INLINE Result<void, Platform::Error> FileCreateHardLink(
        wchar_t* pFilepath,
        const usize size,
        const wchar_t* pFilepathTarget
    ) const noexcept {
        iferr_scope;

        Platform::Error err;
        FileCreateHardLinkJustOneFile(pFilepath, pFilepathTarget) ifok_with(err) {
            return Ok;
        }

        const usize errCode = err.GetErrCode();
        if (errCode == __Win32__ERROR_PATH_NOT_FOUND) {

            Slice<wchar_t> filepath{ pFilepath, size };
            return FileCreateHardLinkRecursive(filepath, pFilepathTarget, errCode);
        }

        return err;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Result<Platform::FileFreeSpace, Platform::Error> FileGetFreeSpace(const wchar_t* pFilepath) const noexcept {

        ULARGE_INTEGER totalBytes{};
        ULARGE_INTEGER totalFreeBytes{};
        ULARGE_INTEGER totalFreeUserAvailableBytes{};
        const bool isOk = (*pGetDiskFreeSpaceExW)(pFilepath, &totalBytes, &totalFreeBytes, &totalFreeUserAvailableBytes);

        if (!isOk) {
            return Platform::Error::PlatformLastError();
        }

        Platform::FileFreeSpace result{};
        result.totalBytes = static_cast<usize>(totalBytes.QuadPart);
        result.totalFreeBytes = static_cast<usize>(totalFreeBytes.QuadPart);
        result.totalFreeUserAvailableBytes = static_cast<usize>(totalFreeUserAvailableBytes.QuadPart);

        return result;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Result<Utf16BufferOffset, Platform::Error> FilePathAbsoluteAppend(const usize offset) noexcept {

        const Slice<const wchar_t> longPrefix = L"\\\\?\\"_s;

        DWORD size = (*pGetFullPathNameW)(Utf16BufferPtr(offset), 0, nullptr, nullptr);

        const usize sizeOld = stl_Utf16Buffer.GetSize();
        stl_Utf16Buffer.ResizeAddition(static_cast<usize>(size));

        size = (*pGetFullPathNameW)(Utf16BufferPtr(offset), size, Utf16BufferPtr(sizeOld), nullptr);

        Utf16BufferOffset bufferOffset{};
        bufferOffset.m_Offset = sizeOld + longPrefix.GetSize();
        bufferOffset.m_OffsetEnd = sizeOld + static_cast<usize>(size);

        return bufferOffset;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Result<Utf16BufferOffset, Platform::Error> FilePathCanonicalAppend(const usize offset) noexcept {

        if (!pGetFinalPathNameByHandleW) {
            return Platform::Error::Unsupported();
        }

        const usize sizeOld = stl_Utf16Buffer.GetSize();
        iferr_scope_on_err {
            iferr_scope;

            stl_Utf16Buffer.Resize(sizeOld) iferr_panic_only_dev("resize to the less size - never panic");
        };

        HANDLE handle = (*pCreateFileW)(
            Utf16BufferPtr(offset),
            __Win32__FILE_READ_ATTRIBUTES,
            __Win32__FILE_SHARE_READ | __Win32__FILE_SHARE_WRITE | __Win32__FILE_SHARE_DELETE,
            nullptr,
            __Win32__OPEN_EXISTING,
            __Win32__FILE_FLAG_BACKUP_SEMANTICS,
            nullptr
        );

        if (handle == __Win32__INVALID_HANDLE_VALUE) {
            iferr_return_return Platform::Error::PlatformLastError();
        }

        defer{ (*pCloseHandle)(handle); };

        const DWORD sizeToAllocate = (*pGetFinalPathNameByHandleW)(
            handle,
            nullptr,
            0,
            __Win32__FILE_NAME_NORMALIZED | __Win32__VOLUME_NAME_DOS
        );

        if (sizeToAllocate == 0) {
            iferr_return_return Platform::Error::PlatformLastError();
        }

        stl_Utf16Buffer.ResizeAddition(static_cast<usize>(sizeToAllocate)) iferr_return Platform::Error::MemoryAllocation();

        wchar_t* pResult = Utf16BufferPtr(sizeOld);

        DWORD result = (*pGetFinalPathNameByHandleW)(
            handle,
            pResult,
            sizeToAllocate,
            __Win32__FILE_NAME_NORMALIZED | __Win32__VOLUME_NAME_DOS
        );

        if (result == 0) {
            iferr_return_return Platform::Error::PlatformLastError();
        }

        const Slice<const wchar_t> longPrefix = L"\\\\?\\"_s;
        const Slice<const wchar_t> uncPrefix = L"\\\\?\\UNC\\"_s;

        usize offsetResult = 0;

        Slice<wchar_t> path{ pResult, static_cast<usize>(result) };
        if (path.StartWith(longPrefix)) {

            const usize size = longPrefix.GetSize();

            offsetResult += size;
        }
        else if (path.StartWith(uncPrefix)) {

            const usize size = uncPrefix.GetSize() - 1;

            offsetResult += size;

            // Extended UNC -> UNC
            path[0] = '\\';
        }

        stl_Utf16Buffer.Resize(sizeOld + static_cast<usize>(result)) iferr_panic_only_dev("resize to the less size - never panic");

        Utf16BufferOffset bufferOffset{};
        bufferOffset.m_Offset = sizeOld + offsetResult;
        bufferOffset.m_OffsetEnd = sizeOld + static_cast<usize>(result);

        return bufferOffset;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Result<u64, Platform::Error> PerformanceGetCurrentProcessCycleTime() const noexcept {

        if (!pQueryProcessCycleTime) {
            return Platform::Error::Unsupported();
        }

        ULONG64 cycles;
        const bool isOk = (*pQueryProcessCycleTime)(m_CurrentProcess, &cycles);
        if (!isOk) {
            return Platform::Error::PlatformLastError();
        }

        return static_cast<u64>(cycles);
    }

    ALT_NODISCARD ALT_FORCE_INLINE Result<Platform::SysTime, Platform::Error> PerformanceGetCurrentProcessTimes() const noexcept {

        FILETIME creation;
        FILETIME exit;
        FILETIME kernel;
        FILETIME user;

        const bool isOk = (*pGetProcessTimes)(m_CurrentProcess, &creation, &exit, &kernel, &user);
        if (!isOk) {
            return Platform::Error::PlatformLastError();
        }

        Platform::SysTime result{};
        result.kernelTimeNS = (static_cast<u64>(kernel.dwHighDateTime) << 32) | static_cast<u64>(kernel.dwLowDateTime);
        result.userTimeNS = (static_cast<u64>(user.dwHighDateTime) << 32) | static_cast<u64>(user.dwLowDateTime);

        return result;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Result<Platform::PathOffset, Platform::Error> FileDialogOpen(
        const usize offsetFilepath,
        const Platform::FileDialogOpenType type,
        const Platform::FileDialogOpenDesc& desc
    ) noexcept {

        // https://learn.microsoft.com/en-us/windows/win32/dlgbox/open-and-save-as-dialog-boxes
        // https://learn.microsoft.com/en-us/windows/win32/api/commdlg/ns-commdlg-openfilenamea

        if (!pGetOpenFileNameW) {
            return Platform::Error::Unsupported();
        }

        const usize offsetFilepathOut = stl_Utf16Buffer.GetSize();
        iferr_scope_on_err {
            iferr_scope;

            stl_Utf16Buffer.Resize(offsetFilepathOut) iferr_panic_only_dev("resize to the less size - never panic");
        };

        stl_Utf16Buffer.ResizeAddition(desc.sizeFilepathOut) iferr_return Platform::Error::MemoryAllocation();

        OPENFILENAMEW ofn{};

        if (desc.filters.IsPopulated()) {

            usize size = 0;
            for (const Platform::FileDialogOpenDesc::Filter& filter : desc.filters.GetIterator()) {

                size += Utf::LengthUtf8ToUtf16IgnoreErrors(filter.name.CastToBytes());
                size += 1;

                for (usize i = 0; i < filter.formats.GetSize(); ++i) {

                    if (filter.formats.IsEmpty()) {
                        continue;
                    }

                    // *.ext;
                    size += 2;
                    size += Utf::LengthUtf8ToUtf16IgnoreErrors(filter.formats[i].CastToBytes());
                    size += i + 1 != filter.formats.GetSize() ? 1 : 0;
                }

                size += 1;
            }

            size += 1;

            const usize sizeOld = stl_Utf16Buffer.GetSize();
            stl_Utf16Buffer.ResizeAddition(size) iferr_return Platform::Error::MemoryAllocation();

            u16* pUtf16 = reinterpret_cast<u16*>(Utf16BufferPtr(sizeOld));

            for (const Platform::FileDialogOpenDesc::Filter& filter : desc.filters.GetIterator()) {

                const usize sizeOld = size;
                Utf::Utf8ToUtf16IgnoreErrors(filter.name.CastToBytes(), pUtf16, size);
                pUtf16 += sizeOld - size;

                *pUtf16 = 0;
                pUtf16 += 1;
                size -= 1;

                for (usize i = 0; i < filter.formats.GetSize(); ++i) {

                    if (filter.formats.IsEmpty()) {
                        continue;
                    }

                    *pUtf16 = L'*';
                    pUtf16 += 1;
                    size -= 1;

                    *pUtf16 = L'.';
                    pUtf16 += 1;
                    size -= 1;

                    const usize sizeOld = size;
                    Utf::Utf8ToUtf16IgnoreErrors(filter.formats[i].CastToBytes(), pUtf16, size);
                    pUtf16 += sizeOld - size;

                    if (i + 1 != filter.formats.GetSize()) {

                        *pUtf16 = L';';
                        pUtf16 += 1;
                        size -= 1;
                    }
                }

                *pUtf16 = 0;
                pUtf16 += 1;
                size -= 1;
            }

            *pUtf16 = 0;
            pUtf16 += 1;
            size -= 1;

            ofn.lpstrFilter = reinterpret_cast<const wchar_t*>(Utf16BufferPtr(sizeOld));
            ofn.nFilterIndex = static_cast<DWORD>(desc.filterByDefaultIdx + 1);
        }

        ofn.lStructSize = sizeof(OPENFILENAMEW);
        ofn.lpstrFile = reinterpret_cast<wchar_t*>(Utf16BufferPtr(offsetFilepathOut));
        ofn.nMaxFile = static_cast<u32>(desc.sizeFilepathOut);
        ofn.lpstrInitialDir = reinterpret_cast<const wchar_t*>(Utf16BufferPtr(offsetFilepath));

        ofn.Flags =
              __Win32__OFN_PATHMUSTEXIST
            | __Win32__OFN_EXPLORER
            | __Win32__OFN_NOCHANGEDIR
            | __Win32__OFN_NODEREFERENCELINKS
            | __Win32__OFN_FORCESHOWHIDDEN
        ;

        if (type == Platform::FileDialogOpenType::Open) {
            ofn.Flags |= __Win32__OFN_FILEMUSTEXIST;
        }

        bool isOk = true;
        if (type == Platform::FileDialogOpenType::Open) {

            isOk = (*pGetOpenFileNameW)(&ofn);
        }
        else if (type == Platform::FileDialogOpenType::SaveAs) {

            isOk = (*pGetSaveFileNameW)(&ofn);
        }
        else {

            ALT_UNREACHABLE();
        }

        if (!isOk) {

            const DWORD err = (*pCommDlgExtendedError)();
            if (err != __Win32__CDERR_GENERALCODES) {
                iferr_return_return Platform::Error::PlatformCommDlgExtendedError(err);
            }

            return Platform::PathOffset{};
        }

        usize size = static_cast<usize>(ofn.nFileExtension);
        size += String::CalculateCStrLen(ofn.lpstrFile + size);

        Platform::PathOffset result{};
        result.m_Offset = offsetFilepathOut;
        result.m_OffsetEnd = offsetFilepathOut + size + 1;
        result.m_BufferAppendBits = Utf16BufferAppendBits::None;

        return result;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> Utf16BufferAppend(
        const char* pFilepath, const Utf16BufferAppendBits bits, const Optional<usize> prefixSizeInBytes = Nullopt
    ) noexcept {

        const usize sizeOld = stl_Utf16Buffer.GetSize();
        iferr_scope_on_err {
            iferr_scope;

            stl_Utf16Buffer.Resize(sizeOld) iferr_panic_only_dev("resize to the less size - never panic");
        };

        if (prefixSizeInBytes) {

            stl_Utf16Buffer.ResizeAddition(*prefixSizeInBytes) iferr_return Nullopt;
        }

        usize size = 0;
        const u8* pBytes = reinterpret_cast<const u8*>(pFilepath);

        usize advance = 0;
        usize prevLocalSize = 0;
        for (;;) {

            usize localSize = *pBytes != 0 ? 1 : 0;
            localSize += localSize > 0 && *(pBytes + 1) != 0 ? 1 : 0;
            localSize += localSize > 1 && *(pBytes + 2) != 0 ? 1 : 0;
            localSize += localSize > 2 && *(pBytes + 3) != 0 ? 1 : 0;

            if (localSize == 0) {
                break;
            }

            const u8 utf8[4]{
                *pBytes,
                1 < localSize ? *(pBytes + 1) : static_cast<u8>(0),
                2 < localSize ? *(pBytes + 2) : static_cast<u8>(0),
                3 < localSize ? *(pBytes + 3) : static_cast<u8>(0),
            };

            advance = Utf::_8::ForwardUtf8IgnoreErrors(utf8);

            prevLocalSize = localSize;
            size += advance == 4 ? 2 : 1;
            pBytes += advance;
        }

        pBytes -= advance;
        pBytes += prevLocalSize;

        const bool isPath = BitFlag::IsSetEnumFlags(bits, Utf16BufferAppendBits::PathBuffer);
        const bool isDoubleNullTerm = BitFlag::IsSetEnumFlags(bits, Utf16BufferAppendBits::DoubleNullTerm);

        if (isPath) {

            const Slice<const wchar_t> prefix = L"\\\\?\\"_s;
            stl_Utf16Buffer.AppendClone(prefix.CastToCompatibleChar()) iferr_return Nullopt;
        }

        const usize offset = stl_Utf16Buffer.GetSize();
        const usize sizeToAllocate = Num::AddOverflowCheck(size, isDoubleNullTerm ? 2 : 1) iferr_return Nullopt;
        stl_Utf16Buffer.ResizeAddition(sizeToAllocate) iferr_return Nullopt;

        const u8* pBytesEnd = pBytes;
        pBytes = reinterpret_cast<const u8*>(pFilepath);

        u16* pBytesOut = stl_Utf16Buffer.Data() + offset;

        bool isOk = true;
        usize consumed = 4;

        for (;;) {

            const usize totalSize = pBytesEnd - pBytes;
            if (size == 0 || consumed == 0 || totalSize == 0) {
                break;
            }

            const u8 utf8[4]{
                *pBytes,
                1 < totalSize ? *(pBytes + 1) : static_cast<u8>(0),
                2 < totalSize ? *(pBytes + 2) : static_cast<u8>(0),
                3 < totalSize ? *(pBytes + 3) : static_cast<u8>(0),
            };

            u16 utf16[2];
            Utf::Result result = Utf::_8::Utf8ToUtf16(utf8, utf16);
            utf16[0] += isPath && utf16[0] == static_cast<u8>('/') ? 45 : 0;

            isOk = result.consumed <= size;
            consumed = isOk ? result.consumed : size;

            if (0 < consumed) *(pBytesOut + 0) = utf16[0];
            if (1 < consumed) *(pBytesOut + 1) = utf16[1];

            pBytes += result.advance;
            pBytesOut += consumed;
            size -= isOk ? consumed : 0;
        }

        *pBytesOut = 0;
        if (isDoubleNullTerm) *(pBytesOut + 1) = 0;

        if (!isOk || consumed == 0) {
            iferr_return_return Nullopt;
        }

        return sizeOld;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> Utf16BufferAppend(
        const Slice<const char> filepath, const Utf16BufferAppendBits bits, const Optional<usize> prefixSizeInBytes = Nullopt
    ) noexcept {

        const usize sizeOld = stl_Utf16Buffer.GetSize();
        iferr_scope_on_err {
            iferr_scope;

            stl_Utf16Buffer.Resize(sizeOld) iferr_panic_only_dev("resize to the less size - never panic");
        };

        if (prefixSizeInBytes) {

            stl_Utf16Buffer.ResizeAddition(*prefixSizeInBytes) iferr_return Nullopt;
        }

        usize size = 0;

        const u8* pBytes = filepath.CastToCompatibleChar().Data();
        const u8* pBytesEnd = filepath.CastToCompatibleChar().DataEnd();

        while (pBytes != pBytesEnd) {

            const u8 utf8[4]{
                *pBytes,
                (pBytes + 1) < pBytesEnd ? *(pBytes + 1) : static_cast<u8>(0),
                (pBytes + 2) < pBytesEnd ? *(pBytes + 2) : static_cast<u8>(0),
                (pBytes + 3) < pBytesEnd ? *(pBytes + 3) : static_cast<u8>(0),
            };

            const usize advance = Utf::_8::ForwardUtf8IgnoreErrors(utf8);

            size += advance == 4 ? 2 : 1;
            pBytes += advance;
        }

        const bool isPath = BitFlag::IsSetEnumFlags(bits, Utf16BufferAppendBits::PathBuffer);
        const bool isDoubleNullTerm = BitFlag::IsSetEnumFlags(bits, Utf16BufferAppendBits::DoubleNullTerm);

        if (isPath) {

            const Slice<const wchar_t> prefix = L"\\\\?\\"_s;
            stl_Utf16Buffer.AppendClone(prefix.CastToCompatibleChar()) iferr_return Nullopt;
        }

        const usize offset = stl_Utf16Buffer.GetSize();
        const usize sizeToAllocate = Num::AddOverflowCheck(size, isDoubleNullTerm ? 2 : 1) iferr_return Nullopt;
        stl_Utf16Buffer.ResizeAddition(sizeToAllocate) iferr_return Nullopt;

        pBytes = filepath.CastToCompatibleChar().Data();
        u16* pBytesOut = stl_Utf16Buffer.Data() + offset;

        bool isOk = true;
        usize consumed = 4;

        while (size != 0 && consumed != 0 && pBytes != pBytesEnd) {

            const u8 utf8[4]{
                *pBytes,
                (pBytes + 1) < pBytesEnd ? *(pBytes + 1) : static_cast<u8>(0),
                (pBytes + 2) < pBytesEnd ? *(pBytes + 2) : static_cast<u8>(0),
                (pBytes + 3) < pBytesEnd ? *(pBytes + 3) : static_cast<u8>(0),
            };

            u16 utf16[2];
            Utf::Result result = Utf::_8::Utf8ToUtf16(utf8, utf16);
            utf16[0] += isPath && utf16[0] == static_cast<u8>('/') ? 45 : 0;

            isOk = result.consumed <= size;
            consumed = isOk ? result.consumed : size;

            if (0 < consumed) *(pBytesOut + 0) = utf16[0];
            if (1 < consumed) *(pBytesOut + 1) = utf16[1];

            pBytes += result.advance;
            pBytesOut += consumed;
            size -= isOk ? consumed : 0;
        }

        *pBytesOut = 0;
        if (isDoubleNullTerm) *(pBytesOut + 1) = 0;

        if (!isOk || consumed == 0) {
            iferr_return_return Nullopt;
        }

        return sizeOld;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Optional<usize> Utf16BufferAppend(
        const Platform::PathOffset pathOffset, const Optional<usize> prefixSizeInBytes = Nullopt
    ) noexcept {

        const usize sizeOld = stl_Utf16Buffer.GetSize();
        iferr_scope_on_err {
            iferr_scope;

            stl_Utf16Buffer.Resize(sizeOld) iferr_panic_only_dev("resize to the less size - never panic");
        };

        if (prefixSizeInBytes) {

            stl_Utf16Buffer.ResizeAddition(*prefixSizeInBytes) iferr_return Nullopt;
        }

        const usize offset = stl_Utf16Buffer.GetSize();
        stl_Utf16Buffer.ResizeAddition(pathOffset.GetOffsetEnd() - pathOffset.GetOffset()) iferr_return Nullopt;

        Memory::CopyObjectsNonOverlap(
            stl_Utf16Buffer.Data() + offset,
            stl_Utf16Buffer.Data() + pathOffset.GetOffset(),
            stl_Utf16Buffer.Data() + pathOffset.GetOffsetEnd()
        );

        return sizeOld;
    }

    ALT_FORCE_INLINE void Utf16BufferDeferGuard(const usize offset) noexcept {
        iferr_scope;

        ALT_ASSERT(offset <= stl_Utf16Buffer.GetSize());

        stl_Utf16Buffer.Resize(offset) iferr_panic_only_dev("resize to the less size - never panic");
    }

    ALT_NODISCARD ALT_FORCE_INLINE usize Utf16BufferGetSize(const Platform::PathOffset pathOffset) const noexcept {

        ALT_MAYBE_UNUSED const Utf16BufferAppendBits pathTypeBits = pathOffset.m_BufferAppendBits;

        // 'PathOffset' always contains nullterm
        usize nulltermOffset = pathOffset.IsEmpty() ? 0 : 1;

        ALT_ASSERT(!BitFlag::IsSetEnumFlags(pathTypeBits, Utf16BufferAppendBits::DoubleNullTerm));

        return static_cast<usize>(Utf16BufferPtr(pathOffset.GetOffsetEnd() - nulltermOffset) - Utf16BufferPtr(pathOffset.GetOffset()));
    }

    ALT_NODISCARD ALT_FORCE_INLINE bool Utf16BufferCloneIntoAppend(const Platform::PathOffset pathOffset, String& buffer) const noexcept {

        ALT_MAYBE_UNUSED const Utf16BufferAppendBits pathTypeBits = pathOffset.m_BufferAppendBits;

        // 'PathOffset' always contains nullterm
        usize nulltermOffset = pathOffset.IsEmpty() ? 0 : 1;

        ALT_ASSERT(!BitFlag::IsSetEnumFlags(pathTypeBits, Utf16BufferAppendBits::DoubleNullTerm));

        Slice<const wchar_t> utf16{ Utf16BufferPtr(pathOffset.GetOffset()), Utf16BufferPtr(pathOffset.GetOffsetEnd() - nulltermOffset) };

        if (BitFlag::IsSetEnumFlags(pathTypeBits, Utf16BufferAppendBits::PathBuffer)) {

            const Slice<const wchar_t> longPrefix = L"\\\\?\\"_s;

            ALT_ASSERT(utf16.StartWith(longPrefix));
            utf16 = utf16.SubSlice(longPrefix.GetSize());
        }

        return buffer.AppendUtf16NoValidation(utf16.CastToCompatibleChar());
    }

    ALT_NODISCARD ALT_FORCE_INLINE bool Utf16BufferCloneIntoAppend(const _PlatformDetails::Utf16BufferOffset bufferOffset, String& buffer) const noexcept {

        const Slice<const wchar_t> utf16{ Utf16BufferPtr(bufferOffset.GetOffset()), Utf16BufferPtr(bufferOffset.GetOffsetEnd()) };
        return buffer.AppendUtf16NoValidation(utf16.CastToCompatibleChar());
    }

    ALT_NODISCARD ALT_FORCE_INLINE usize Utf16BufferGetOffset() const noexcept {

        return stl_Utf16Buffer.GetSize();
    }

    ALT_NODISCARD ALT_FORCE_INLINE wchar_t* Utf16BufferPtr(const usize offset) noexcept {
        ALT_ASSERT(offset <= stl_Utf16Buffer.GetSize());

        return reinterpret_cast<wchar_t*>(stl_Utf16Buffer.Data() + offset);
    }

    ALT_NODISCARD ALT_FORCE_INLINE const wchar_t* Utf16BufferPtr(const usize offset) const noexcept {
        ALT_ASSERT(offset <= stl_Utf16Buffer.GetSize());

        return reinterpret_cast<const wchar_t*>(stl_Utf16Buffer.Data() + offset);
    }

    ALT_NODISCARD ALT_FORCE_INLINE usize Utf16BufferSizeAssumeLast(const usize offset, const Utf16BufferAppendBits bits) noexcept {
        ALT_ASSERT(offset <= stl_Utf16Buffer.GetSize());

        usize nulltermOffset = BitFlag::IsSetEnumFlags(bits, Utf16BufferAppendBits::PathBuffer)
            ? 1
            : 0;

        nulltermOffset = BitFlag::IsSetEnumFlags(bits, Utf16BufferAppendBits::DoubleNullTerm)
            ? 2
            : nulltermOffset;

        return stl_Utf16Buffer.GetSize() - offset - nulltermOffset;
    }

private:

    ALT_FORCE_INLINE PlatformInstance() noexcept = default;

    ALT_FORCE_INLINE void PreInitializeSystemInfo(const HMODULE hKernel32) noexcept {

        // Minimum supported client	Windows XP [desktop apps | UWP apps]
        // Minimum supported server	Windows Server 2003 [desktop apps | UWP apps]
        using GetNativeSystemInfo_FuncPtr = PlatformFuncPtr<void, LPSYSTEM_INFO>;
        const GetNativeSystemInfo_FuncPtr pGetNativeSystemInfo = reinterpret_cast<GetNativeSystemInfo_FuncPtr>(GetProcAddress(hKernel32, "GetNativeSystemInfo"));

        SYSTEM_INFO info;
        (*pGetNativeSystemInfo)(&info);

        m_PageSizeInBytes = static_cast<usize>(info.dwPageSize);
        ALT_ASSERT(Num::IsPowerOf2(m_PageSizeInBytes)); // see. 'RoundUpToPageSize(...)/RoundUpToPageSizePreserveZero(...)'

        m_AllocationGranularity = static_cast<usize>(info.dwAllocationGranularity);
        m_NumOfLogicalProcessors = static_cast<usize>(info.dwNumberOfProcessors);
        m_CpuArchitecture = static_cast<Platform::CPUArchitecture>(info.u.s.wProcessorArchitecture);
    }

    ALT_FORCE_INLINE void PreInitializeTermination(const HMODULE hKernel32) noexcept {

        // Minimum supported client	Windows XP [desktop apps | UWP apps]
        // Minimum supported server	Windows Server 2003 [desktop apps | UWP apps]
        using GetCurrentProcess_FuncPtr = PlatformFuncPtr<HANDLE>;
        const GetCurrentProcess_FuncPtr pGetCurrentProcess = reinterpret_cast<GetCurrentProcess_FuncPtr>(GetProcAddress(hKernel32, "GetCurrentProcess"));

        m_CurrentProcess = (*pGetCurrentProcess)();
    }

    ALT_FORCE_INLINE void PostInitializeTickState(const HMODULE hKernel32) noexcept {
        iferr_scope;

        // Minimum supported client	Windows 2000 Professional [desktop apps | UWP apps]
        // Minimum supported server	Windows 2000 Server [desktop apps | UWP apps]
        using QueryPerformanceFrequency_FuncPtr = PlatformFuncPtr<BOOL, PLARGE_INTEGER>;
        const QueryPerformanceFrequency_FuncPtr pQueryPerformanceFrequency = reinterpret_cast<QueryPerformanceFrequency_FuncPtr>(GetProcAddress(hKernel32, "QueryPerformanceFrequency"));

        bool isOk;

        isOk = (*pQueryPerformanceFrequency)(&m_TickState.timeFreq);
        isOk iferr_panic_only_dev("What wrong? {}", Platform::Error::PlatformLastError());

        isOk = (*pQueryPerformanceCounter)(&m_TickState.timeCounterStart);
        isOk iferr_panic_only_dev("What wrong? {}", Platform::Error::PlatformLastError());
    }

    using SetConsoleMode_FuncPtr = PlatformFuncPtr<BOOL, HANDLE, DWORD>;

    ALT_FORCE_INLINE void PostInitializeConsoleModeVT100(
        const SetConsoleMode_FuncPtr pSetConsoleMode,
        const HANDLE hConsole
    ) const noexcept {

        DWORD mode;
        BOOL isOk = (*pGetConsoleMode)(hConsole, &mode);

        if (!isOk) {
            return;
        }

        mode |= __Win32__ENABLE_LINE_INPUT | __Win32__ENABLE_PROCESSED_OUTPUT | __Win32__ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        isOk = (*pSetConsoleMode)(hConsole, mode);
    }

    ALT_FORCE_INLINE void PostInitializeConsole(const HMODULE hKernel32) const noexcept {

        // Minimum supported client	Windows 2000 Professional [desktop apps only]
        // Minimum supported server	Windows 2000 Server [desktop apps only]
        const SetConsoleMode_FuncPtr pSetConsoleMode = reinterpret_cast<SetConsoleMode_FuncPtr>(GetProcAddress(hKernel32, "SetConsoleMode"));

        HANDLE hConsole = (*pGetStdHandle)(__Win32__STD_OUTPUT_HANDLE);
        if (hConsole != NULL && hConsole != __Win32__INVALID_HANDLE_VALUE) {

            PostInitializeConsoleModeVT100(pSetConsoleMode, hConsole);
        }

        hConsole = (*pGetStdHandle)(__Win32__STD_ERROR_HANDLE);
        if (hConsole != NULL && hConsole != __Win32__INVALID_HANDLE_VALUE) {

            PostInitializeConsoleModeVT100(pSetConsoleMode, hConsole);
        }
    }

    ALT_FORCE_INLINE void PostInitializeMemory(const HMODULE hKernel32) noexcept {

        // Minimum supported client	Windows XP [desktop apps | UWP apps]
        // Minimum supported server	Windows Server 2003 [desktop apps | UWP apps]
        using GetProcessHeap_FuncPtr = PlatformFuncPtr<HANDLE>;
        const GetProcessHeap_FuncPtr pGetProcessHeap = reinterpret_cast<GetProcessHeap_FuncPtr>(GetProcAddress(hKernel32, "GetProcessHeap"));

        m_MainMemoryHeap = (*pGetProcessHeap)();
        ALT_ASSERT(m_MainMemoryHeap != NULL);
    }

    ALT_FORCE_INLINE void PostInitialize() noexcept {

        pFileDeleteByHandle = pSetFileInformationByHandle ? FileDeleteByHandle_Vista : FileDeleteByHandle_XP;
        pFileGetTypeByHandle = pGetFileInformationByHandleEx ? FileGetTypeByHandle_Vista : FileGetTypeByHandle_XP;

        if (pGetFileInformationByName) {
            pFileGetType = FileGetType_24H2_26052;
        }
        else if (pNtQueryInformationByName) {
            pFileGetType = FileGetType_Win10_1703;
        }
        else if (pGetFileInformationByHandleEx) {
            pFileGetType = FileGetType_Vista;
        }
        else {
            pFileGetType = FileGetType_XP;
        }

        if (pGetFileInformationByName) {
            pFileGetInfo = FileGetInfo_24H2_26052;
        }
        else if (pNtQueryInformationByName) {
            pFileGetInfo = FileGetInfo_Win10_1703;
        }
        else {
            pFileGetInfo = FileGetInfo_XP;
        }

        pFileSameByHandle = pGetFileInformationByHandleEx ? FileSameByHandle_Vista : FileSameByHandle_XP;

        if (pGetFileInformationByName) {
            pFileSame = FileSame_24H2_26052;
        }
        else if (pGetFileInformationByHandleEx) {
            pFileSame = FileSame_Vista;
        }
        else {
            pFileSame = FileSame_XP;
        }

        pTimeGetSystemTimeUTC = pGetSystemTimePreciseAsFileTime ? TimeGetSystemTimeUTC_Win8 : TimeGetSystemTimeUTC_XP;
        pTimeGetSystemTimeLocalTime = pGetSystemTimePreciseAsFileTime ? TimeGetSystemTimeLocalTime_Win8 : TimeGetSystemTimeLocalTime_XP;

        pGetThreadIDByHandle = pGetThreadIDByHandle ? ThreadGetIDByHandle_Vista : ThreadGetIDByHandle_XP;
    }

    static Result<void, Platform::Error> FileDeleteByHandle_XP(const PlatformInstance& instance, const Platform::Handle handle) noexcept {

        IO_STATUS_BLOCK iosb;

        FILE_DISPOSITION_INFO fdi;
        fdi.DeleteFile = true;

        const NTSTATUS ntstatus = (*instance.pNtSetInformationFile)(
            handle.handle,
            &iosb,
            &fdi,
            sizeof(FILE_DISPOSITION_INFO),
            FileDispositionInformation
        );

        if (ntstatus != 0) {
            return Platform::Error::PlatformNtError(ntstatus);
        }

        return Ok;
    }

    static Result<void, Platform::Error> FileDeleteByHandle_Vista(const PlatformInstance& instance, const Platform::Handle handle) noexcept {

        FILE_DISPOSITION_INFO fdi;
        fdi.DeleteFile = true;

        const bool isOk = (*instance.pSetFileInformationByHandle)(handle.handle, FileDispositionInfo, &fdi, sizeof(FILE_DISPOSITION_INFO));

        if (!isOk) {
            return Platform::Error::PlatformLastError();
        }

        return Ok;
    }

    //static Result<Platform::FileTypeBits, Platform::Error> FileGetTypeByHandle_XP(
    //	PlatformInstance& instance, const Platform::Handle handle, const bool isTryGetFileTypeByHandle
    //) noexcept {
    //	iferr_scope;

    //	BY_HANDLE_FILE_INFORMATION info;
    //	const bool isOk = (*instance.pGetFileInformationByHandle)(handle.handle, &info);

    //	if (!isOk) {
    //		return Platform::Error::PlatformLastError();
    //	}

    //	Platform::FileTypeBits result = Platform::FileTypeBits::None;

    //	if ((info.dwFileAttributes & __Win32__FILE_ATTRIBUTE_DIRECTORY) != 0) {

    //		result |= Platform::FileTypeBits::Directory;
    //	}

    //	if ((info.dwFileAttributes & __Win32__FILE_ATTRIBUTE_REPARSE_POINT) != 0) {

    //		const usize sizeOld = instance.stl_Utf16Buffer.GetSize();
    //		defer{ instance.Utf16BufferDeferGuard(sizeOld); };

    //		instance.stl_Utf16Buffer.ResizeAddition(Num::CeilDiv((usize)__Win32__MAXIMUM_REPARSE_DATA_BUFFER_SIZE, sizeof(u16))) iferr_return Platform::Error::MemoryAllocation();

    //		PREPARSE_DATA_BUFFER pReparseData = reinterpret_cast<PREPARSE_DATA_BUFFER>(instance.stl_Utf16Buffer.Data() + sizeOld);

    //		DWORD bytesRead;
    //		const bool isOk = (*instance.pDeviceIoControl)(
    //			handle.handle,
    //			__Win32__FSCTL_GET_REPARSE_POINT,
    //			nullptr,
    //			0,
    //			pReparseData,
    //			__Win32__MAXIMUM_REPARSE_DATA_BUFFER_SIZE,
    //			&bytesRead,
    //			nullptr
    //		);

    //		if (!isOk) {
    //			return Platform::Error::PlatformLastError();
    //		}

    //		if (pReparseData->ReparseTag == __Win32__IO_REPARSE_TAG_SYMLINK) {
    //			result |= Platform::FileTypeBits::SymLink;
    //		}

    //		// wsl symlink
    //		if (pReparseData->ReparseTag == __Win32__IO_REPARSE_TAG_LX_SYMLINK) {
    //			result |= Platform::FileTypeBits::SymLink;
    //		}

    //		if (pReparseData->ReparseTag == __Win32__IO_REPARSE_TAG_MOUNT_POINT) {
    //			result |= Platform::FileTypeBits::Junction;
    //		}
    //	}

    //	if (!BitFlag::IsSetAnyEnumFlags(result)) {

    //		if (isTryGetFileTypeByHandle) {

    //			result |= Platform::FileTypeBits::File | instance.FileGetFileType(handle);
    //		}
    //		else {

    //			result = Platform::FileTypeBits::File;
    //		}
    //	}

    //	return result;
    //}

    static Result<Platform::FileTypeBits, Platform::Error> FileGetTypeByHandle_XP(
        const PlatformInstance& instance, const Platform::Handle handle, const bool isTryGetFileTypeByHandle
    ) noexcept {

        IO_STATUS_BLOCK iosb;
        FILE_ATTRIBUTE_TAG_INFORMATION fati;

        const NTSTATUS ntstatus = (*instance.pNtQueryInformationFile)(
            handle.handle,
            &iosb,
            &fati,
            sizeof(FILE_ATTRIBUTE_TAG_INFORMATION),
            FileAttributeTagInformation
        );

        if (ntstatus != 0) {
            return Platform::Error::PlatformNtError(ntstatus);
        }

        Platform::FileTypeBits result = Platform::FileTypeBits::None;

        if ((fati.FileAttributes & __Win32__FILE_ATTRIBUTE_DIRECTORY) != 0) {

            result |= Platform::FileTypeBits::Directory;
        }

        if ((fati.FileAttributes & __Win32__FILE_ATTRIBUTE_REPARSE_POINT) != 0) {

            if (fati.ReparseTag == __Win32__IO_REPARSE_TAG_SYMLINK) {
                result |= Platform::FileTypeBits::SymLink;
            }

            // wsl symlink
            if (fati.ReparseTag == __Win32__IO_REPARSE_TAG_LX_SYMLINK) {
                result |= Platform::FileTypeBits::SymLink;
            }

            if (fati.ReparseTag == __Win32__IO_REPARSE_TAG_MOUNT_POINT) {
                result |= Platform::FileTypeBits::Junction;
            }
        }

        if (!BitFlag::IsSetAnyEnumFlags(result)) {

            if (isTryGetFileTypeByHandle) {

                result |= Platform::FileTypeBits::File | instance.FileGetFileType(handle);
            }
            else {

                result = Platform::FileTypeBits::File;
            }
        }

        return result;
    }

    static Result<Platform::FileTypeBits, Platform::Error> FileGetTypeByHandle_Vista(
        const PlatformInstance& instance, const Platform::Handle handle, const bool isTryGetFileTypeByHandle
    ) noexcept {

        FILE_ATTRIBUTE_TAG_INFO info;
        const bool isOk = (*instance.pGetFileInformationByHandleEx)(handle.handle, FileAttributeTagInfo, &info, sizeof(FILE_ATTRIBUTE_TAG_INFO));
        if (!isOk) {
            return Platform::Error::PlatformLastError();
        }

        Platform::FileTypeBits result = Platform::FileTypeBits::None;

        if ((info.FileAttributes & __Win32__FILE_ATTRIBUTE_DIRECTORY) != 0) {

            result |= Platform::FileTypeBits::Directory;
        }

        if ((info.FileAttributes & __Win32__FILE_ATTRIBUTE_REPARSE_POINT) != 0) {

            if (info.ReparseTag == __Win32__IO_REPARSE_TAG_SYMLINK) {
                result |= Platform::FileTypeBits::SymLink;
            }

            // wsl symlink
            if (info.ReparseTag == __Win32__IO_REPARSE_TAG_LX_SYMLINK) {
                result |= Platform::FileTypeBits::SymLink;
            }

            if (info.ReparseTag == __Win32__IO_REPARSE_TAG_MOUNT_POINT) {
                result |= Platform::FileTypeBits::Junction;
            }
        }

        if (!BitFlag::IsSetAnyEnumFlags(result)) {

            if (isTryGetFileTypeByHandle) {

                result |= Platform::FileTypeBits::File | instance.FileGetFileType(handle);
            }
            else {

                result = Platform::FileTypeBits::File;
            }
        }

        return result;
    }

    static Result<Platform::FileTypeBits, Platform::Error> FileGetType_XP(
        const PlatformInstance& instance, const wchar_t* pFilepath, const bool isTryGetFileTypeByHandle
    ) noexcept {

        HANDLE handle = (*instance.pCreateFileW)(
            pFilepath,
            __Win32__FILE_READ_ATTRIBUTES,
            __Win32__FILE_SHARE_READ | __Win32__FILE_SHARE_WRITE | __Win32__FILE_SHARE_DELETE,
            nullptr,
            __Win32__OPEN_EXISTING,
            __Win32__FILE_FLAG_OPEN_REPARSE_POINT | __Win32__FILE_FLAG_BACKUP_SEMANTICS,
            nullptr
        );

        if (handle == __Win32__INVALID_HANDLE_VALUE) {
            return Platform::Error::PlatformLastError();
        }

        defer{ (instance.pCloseHandle)(handle); };

        Platform::Handle hFile;
        hFile.handle = handle;

        return FileGetTypeByHandle_XP(instance, hFile, isTryGetFileTypeByHandle);
    }

    static Result<Platform::FileTypeBits, Platform::Error> FileGetType_Vista(
        const PlatformInstance& instance, const wchar_t* pFilepath, const bool isTryGetFileTypeByHandle
    ) noexcept {

        HANDLE handle = (*instance.pCreateFileW)(
            pFilepath,
            __Win32__FILE_READ_ATTRIBUTES,
            __Win32__FILE_SHARE_READ | __Win32__FILE_SHARE_WRITE | __Win32__FILE_SHARE_DELETE,
            nullptr,
            __Win32__OPEN_EXISTING,
            __Win32__FILE_FLAG_OPEN_REPARSE_POINT | __Win32__FILE_FLAG_BACKUP_SEMANTICS,
            nullptr
        );

        if (handle == __Win32__INVALID_HANDLE_VALUE) {
            return Platform::Error::PlatformLastError();
        }

        defer{ (instance.pCloseHandle)(handle); };

        Platform::Handle hFile;
        hFile.handle = handle;

        return FileGetTypeByHandle_Vista(instance, hFile, isTryGetFileTypeByHandle);
    }

    static Result<Platform::FileTypeBits, Platform::Error> FileGetType_Win10_1703(
        const PlatformInstance& instance, const wchar_t* pFilepath_, const bool isTryGetFileTypeByHandle
    ) noexcept {

        wchar_t* pFilepath = const_cast<wchar_t*>(pFilepath_);

        // NOTE: assumed that 'pFilepath' is 'stl_Utf16Buffer' and last u16 of 'stl_Utf16Buffer' is nullterm for 'pFilepath',
        // 'pFilepath' start with '\\?\'
        ALT_ASSERT(reinterpret_cast<const void*>(pFilepath) >= stl_Utf16Buffer.Data() && reinterpret_cast<const void*>(pFilepath) <= stl_Utf16Buffer.DataEnd());

        const usize size = static_cast<usize>(stl_Utf16Buffer.DataEnd() - reinterpret_cast<const u16*>(pFilepath) - 1);
        if (size > Num::Desc<USHORT>::Max()) {
            return Platform::Error::PlatformNtError(__Win32__STATUS_NAME_TOO_LONG);
        }

        // '\\?\' -> '\??\'
        pFilepath[1] = '?';

        IO_STATUS_BLOCK iosb;
        FILE_STAT_INFORMATION fsi;

        UNICODE_STRING utfString;
        utfString.Buffer = reinterpret_cast<PWSTR>(pFilepath);
        utfString.Length = static_cast<USHORT>(size * sizeof(u16));
        utfString.MaximumLength = utfString.Length;

        OBJECT_ATTRIBUTES attributes;
        attributes.Length = sizeof(OBJECT_ATTRIBUTES);
        attributes.RootDirectory = 0;
        attributes.ObjectName = &utfString;
        attributes.Attributes = __Win32__OBJ_CASE_INSENSITIVE;
        attributes.SecurityDescriptor = nullptr;
        attributes.SecurityQualityOfService = nullptr;

        const NTSTATUS ntstatus = (*instance.pNtQueryInformationByName)(
            &attributes,
            &iosb,
            &fsi,
            sizeof(FILE_STAT_INFORMATION),
            FileStatInformation
        );

        if (ntstatus != 0) {
            return Platform::Error::PlatformNtError(ntstatus);
        }

        Platform::FileTypeBits result = Platform::FileTypeBits::None;

        if ((fsi.FileAttributes & __Win32__FILE_ATTRIBUTE_DIRECTORY) != 0) {

            result |= Platform::FileTypeBits::Directory;
        }

        if ((fsi.FileAttributes & __Win32__FILE_ATTRIBUTE_REPARSE_POINT) != 0) {

            if (fsi.ReparseTag == __Win32__IO_REPARSE_TAG_SYMLINK) {
                result |= Platform::FileTypeBits::SymLink;
            }

            // wsl symlink
            if (fsi.ReparseTag == __Win32__IO_REPARSE_TAG_LX_SYMLINK) {
                result |= Platform::FileTypeBits::SymLink;
            }

            if (fsi.ReparseTag == __Win32__IO_REPARSE_TAG_MOUNT_POINT) {
                result |= Platform::FileTypeBits::Junction;
            }
        }

        if (!BitFlag::IsSetAnyEnumFlags(result)) {

            if (isTryGetFileTypeByHandle) {

                HANDLE handle = (*instance.pCreateFileW)(
                    pFilepath,
                    0,
                    __Win32__FILE_SHARE_READ | __Win32__FILE_SHARE_WRITE | __Win32__FILE_SHARE_DELETE,
                    nullptr,
                    __Win32__OPEN_EXISTING,
                    __Win32__FILE_FLAG_BACKUP_SEMANTICS,
                    nullptr
                );

                if (handle == __Win32__INVALID_HANDLE_VALUE) {
                    return Platform::Error::PlatformLastError();
                }

                defer{ (*instance.pCloseHandle)(handle); };

                Platform::Handle hFile;
                hFile.handle = handle;

                result |= Platform::FileTypeBits::File | instance.FileGetFileType(hFile);
            }
            else {

                result = Platform::FileTypeBits::File;
            }
        }

        return result;
    }

    static Result<Platform::FileTypeBits, Platform::Error> FileGetType_24H2_26052(
        const PlatformInstance& instance, const wchar_t* pFilepath, const bool isTryGetFileTypeByHandle
    ) noexcept {

        FILE_STAT_INFORMATION info;
        const bool isOk = (*instance.pGetFileInformationByName)(pFilepath, FileStatByNameInfo, &info, sizeof(FILE_STAT_INFORMATION));
        if (!isOk) {
            return Platform::Error::PlatformLastError();
        }

        Platform::FileTypeBits result = Platform::FileTypeBits::None;

        if ((info.FileAttributes & __Win32__FILE_ATTRIBUTE_DIRECTORY) != 0) {

            result |= Platform::FileTypeBits::Directory;
        }

        if ((info.FileAttributes & __Win32__FILE_ATTRIBUTE_REPARSE_POINT) != 0) {

            if (info.ReparseTag == __Win32__IO_REPARSE_TAG_SYMLINK) {
                result |= Platform::FileTypeBits::SymLink;
            }

            // wsl symlink
            if (info.ReparseTag == __Win32__IO_REPARSE_TAG_LX_SYMLINK) {
                result |= Platform::FileTypeBits::SymLink;
            }

            if (info.ReparseTag == __Win32__IO_REPARSE_TAG_MOUNT_POINT) {
                result |= Platform::FileTypeBits::Junction;
            }
        }

        if (!BitFlag::IsSetAnyEnumFlags(result)) {

            if (isTryGetFileTypeByHandle) {

                HANDLE handle = (*instance.pCreateFileW)(
                    pFilepath,
                    0,
                    __Win32__FILE_SHARE_READ | __Win32__FILE_SHARE_WRITE | __Win32__FILE_SHARE_DELETE,
                    nullptr,
                    __Win32__OPEN_EXISTING,
                    __Win32__FILE_FLAG_BACKUP_SEMANTICS,
                    nullptr
                );

                if (handle == __Win32__INVALID_HANDLE_VALUE) {
                    return Platform::Error::PlatformLastError();
                }

                defer{ (*instance.pCloseHandle)(handle); };

                Platform::Handle hFile;
                hFile.handle = handle;

                result |= Platform::FileTypeBits::File | instance.FileGetFileType(hFile);
            }
            else {

                result = Platform::FileTypeBits::File;
            }
        }

        return result;
    }

    static Result<Platform::FileInfo, Platform::Error> FileGetInfoByHandle_XP(const PlatformInstance& instance, const Platform::Handle handle) noexcept {

        IO_STATUS_BLOCK iosb;
        FILE_STAT_INFORMATION fsi;

        const NTSTATUS ntstatus = (*instance.pNtQueryInformationFile)(
            handle.handle,
            &iosb,
            &fsi,
            sizeof(FILE_STAT_INFORMATION),
            FileStatInformation
        );

        if (ntstatus != 0) {
            return Platform::Error::PlatformNtError(ntstatus);
        }

        Platform::FileInfo result{};
        result.creationTime = Time::FromWin32FileTime(static_cast<u64>(fsi.CreationTime.QuadPart));
        result.lastAccessTime = Time::FromWin32FileTime(static_cast<u64>(fsi.LastAccessTime.QuadPart));
        result.lastWriteTime = Time::FromWin32FileTime(static_cast<u64>(fsi.LastWriteTime.QuadPart));
        result.lastChangeTime = Time::FromWin32FileTime(static_cast<u64>(fsi.ChangeTime.QuadPart));
        result.numOfHardLinks = static_cast<usize>(fsi.NumberOfLinks);
        result.allocatedSizeInBytes = static_cast<usize>(fsi.AllocationSize.QuadPart);
        result.logicalSizeInBytes = static_cast<usize>(fsi.EndOfFile.QuadPart);

        return result;
    }

    static Result<Platform::FileInfo, Platform::Error> FileGetInfo_XP(const PlatformInstance& instance, const wchar_t* pFilepath) noexcept {

        HANDLE handle = (*instance.pCreateFileW)(
            pFilepath,
            0,
            __Win32__FILE_SHARE_READ | __Win32__FILE_SHARE_WRITE | __Win32__FILE_SHARE_DELETE,
            nullptr,
            __Win32__OPEN_EXISTING,
            __Win32__FILE_FLAG_OPEN_REPARSE_POINT | __Win32__FILE_FLAG_BACKUP_SEMANTICS,
            nullptr
        );

        if (handle == __Win32__INVALID_HANDLE_VALUE) {
            return Platform::Error::PlatformLastError();
        }

        defer{ (instance.pCloseHandle)(handle); };

        Platform::Handle hFile;
        hFile.handle = handle;

        return FileGetInfoByHandle_XP(instance, hFile);
    }

    static Result<Platform::FileInfo, Platform::Error> FileGetInfo_Win10_1703(const PlatformInstance& instance, const wchar_t* pFilepath_) noexcept {

        wchar_t* pFilepath = const_cast<wchar_t*>(pFilepath_);

        // NOTE: assumed that 'pFilepath' is 'stl_Utf16Buffer' and last u16 of 'stl_Utf16Buffer' is nullterm for 'pFilepath',
        // 'pFilepath' start with '\\?\'
        ALT_ASSERT(reinterpret_cast<const void*>(pFilepath) >= stl_Utf16Buffer.Data() && reinterpret_cast<const void*>(pFilepath) <= stl_Utf16Buffer.DataEnd());

        const usize size = static_cast<usize>(stl_Utf16Buffer.DataEnd() - reinterpret_cast<const u16*>(pFilepath) - 1);
        if (size > Num::Desc<USHORT>::Max()) {
            return Platform::Error::PlatformNtError(__Win32__STATUS_NAME_TOO_LONG);
        }

        // '\\?\' -> '\??\'
        pFilepath[1] = '?';

        IO_STATUS_BLOCK iosb;
        FILE_STAT_INFORMATION fsi;

        UNICODE_STRING utfString;
        utfString.Buffer = reinterpret_cast<PWSTR>(pFilepath);
        utfString.Length = static_cast<USHORT>(size * sizeof(u16));
        utfString.MaximumLength = utfString.Length;

        OBJECT_ATTRIBUTES attributes;
        attributes.Length = sizeof(OBJECT_ATTRIBUTES);
        attributes.RootDirectory = 0;
        attributes.ObjectName = &utfString;
        attributes.Attributes = __Win32__OBJ_CASE_INSENSITIVE;
        attributes.SecurityDescriptor = nullptr;
        attributes.SecurityQualityOfService = nullptr;

        const NTSTATUS ntstatus = (*instance.pNtQueryInformationByName)(
            &attributes,
            &iosb,
            &fsi,
            sizeof(FILE_STAT_INFORMATION),
            FileStatInformation
        );

        if (ntstatus != 0) {
            return Platform::Error::PlatformNtError(ntstatus);
        }

        Platform::FileInfo result{};
        result.creationTime = Time::FromWin32FileTime(static_cast<u64>(fsi.CreationTime.QuadPart));
        result.lastAccessTime = Time::FromWin32FileTime(static_cast<u64>(fsi.LastAccessTime.QuadPart));
        result.lastWriteTime = Time::FromWin32FileTime(static_cast<u64>(fsi.LastWriteTime.QuadPart));
        result.lastChangeTime = Time::FromWin32FileTime(static_cast<u64>(fsi.ChangeTime.QuadPart));
        result.numOfHardLinks = static_cast<usize>(fsi.NumberOfLinks);
        result.allocatedSizeInBytes = static_cast<usize>(fsi.AllocationSize.QuadPart);
        result.logicalSizeInBytes = static_cast<usize>(fsi.EndOfFile.QuadPart);

        return result;
    }

    static Result<Platform::FileInfo, Platform::Error> FileGetInfo_24H2_26052(const PlatformInstance& instance, const wchar_t* pFilepath) noexcept {

        FILE_STAT_INFORMATION info;
        const bool isOk = (*instance.pGetFileInformationByName)(pFilepath, FileStatByNameInfo, &info, sizeof(FILE_STAT_INFORMATION));

        if (!isOk) {
            return Platform::Error::PlatformLastError();
        }

        Platform::FileInfo result{};
        result.creationTime = Time::FromWin32FileTime(static_cast<u64>(info.CreationTime.QuadPart));
        result.lastAccessTime = Time::FromWin32FileTime(static_cast<u64>(info.LastAccessTime.QuadPart));
        result.lastWriteTime = Time::FromWin32FileTime(static_cast<u64>(info.LastWriteTime.QuadPart));
        result.lastChangeTime = Time::FromWin32FileTime(static_cast<u64>(info.ChangeTime.QuadPart));
        result.numOfHardLinks = static_cast<usize>(info.NumberOfLinks);
        result.allocatedSizeInBytes = static_cast<usize>(info.AllocationSize.QuadPart);
        result.logicalSizeInBytes = static_cast<usize>(info.EndOfFile.QuadPart);

        return result;
    }

    static Result<bool, Platform::Error> FileSameByHandle_XP(const PlatformInstance& instance, const Platform::Handle handle0, const Platform::Handle handle1) noexcept {

        IO_STATUS_BLOCK iosb0;
        FILE_ID_INFORMATION fii0;

        const NTSTATUS ntstatus0 = (*instance.pNtQueryInformationFile)(
            handle0.handle,
            &iosb0,
            &fii0,
            sizeof(FILE_ID_INFORMATION),
            FileIdInformation
        );

        if (ntstatus0 != 0) {
            return Platform::Error::PlatformNtError(ntstatus0);
        }

        IO_STATUS_BLOCK iosb1;
        FILE_ID_INFORMATION fii1;

        const NTSTATUS ntstatus1 = (*instance.pNtQueryInformationFile)(
            handle1.handle,
            &iosb1,
            &fii1,
            sizeof(FILE_ID_INFORMATION),
            FileIdInformation
        );

        if (ntstatus1 != 0) {
            return Platform::Error::PlatformNtError(ntstatus1);
        }

        return
            fii0.VolumeSerialNumber == fii1.VolumeSerialNumber &&
            std::memcmp(fii0.FileId.Identifier, fii1.FileId.Identifier, sizeof(fii0.FileId.Identifier)) == 0;
    }

    static Result<bool, Platform::Error> FileSameByHandle_Vista(const PlatformInstance& instance, const Platform::Handle handle0, const Platform::Handle handle1) noexcept {

        FILE_ID_INFO fii0;
        bool isOk = (*instance.pGetFileInformationByHandleEx)(
            handle0.handle,
            FileIdInfo,
            &fii0,
            sizeof(FILE_ID_INFO)
        );

        if (!isOk) {
            return Platform::Error::PlatformLastError();
        }

        FILE_ID_INFO fii1;
        isOk = (*instance.pGetFileInformationByHandleEx)(
            handle1.handle,
            FileIdInfo,
            &fii1,
            sizeof(FILE_ID_INFO)
        );

        if (!isOk) {
            return Platform::Error::PlatformLastError();
        }

        return
            fii0.VolumeSerialNumber == fii1.VolumeSerialNumber &&
            std::memcmp(fii0.FileId.Identifier, fii1.FileId.Identifier, sizeof(fii0.FileId.Identifier)) == 0;
    }

    static Result<bool, Platform::Error> FileSame_XP(const PlatformInstance& instance, const wchar_t* pFilepath0, const wchar_t* pFilepath1) noexcept {

        HANDLE handle0 = (*instance.pCreateFileW)(
            pFilepath0,
            0,
            __Win32__FILE_SHARE_READ | __Win32__FILE_SHARE_WRITE | __Win32__FILE_SHARE_DELETE,
            nullptr,
            __Win32__OPEN_EXISTING,
            __Win32__FILE_FLAG_OPEN_REPARSE_POINT | __Win32__FILE_FLAG_BACKUP_SEMANTICS,
            nullptr
        );

        if (handle0 == __Win32__INVALID_HANDLE_VALUE) {
            return Platform::Error::PlatformLastError();
        }

        defer{ (instance.pCloseHandle)(handle0); };

        HANDLE handle1 = (*instance.pCreateFileW)(
            pFilepath1,
            0,
            __Win32__FILE_SHARE_READ | __Win32__FILE_SHARE_WRITE | __Win32__FILE_SHARE_DELETE,
            nullptr,
            __Win32__OPEN_EXISTING,
            __Win32__FILE_FLAG_OPEN_REPARSE_POINT | __Win32__FILE_FLAG_BACKUP_SEMANTICS,
            nullptr
        );

        if (handle1 == __Win32__INVALID_HANDLE_VALUE) {
            return Platform::Error::PlatformLastError();
        }

        defer{ (instance.pCloseHandle)(handle1); };

        Platform::Handle hFile0;
        hFile0.handle = handle0;

        Platform::Handle hFile1;
        hFile1.handle = handle1;

        return FileSameByHandle_XP(instance, hFile0, hFile1);
    }

    static Result<bool, Platform::Error> FileSame_Vista(const PlatformInstance& instance, const wchar_t* pFilepath0, const wchar_t* pFilepath1) noexcept {

        HANDLE handle0 = (*instance.pCreateFileW)(
            pFilepath0,
            0,
            __Win32__FILE_SHARE_READ | __Win32__FILE_SHARE_WRITE | __Win32__FILE_SHARE_DELETE,
            nullptr,
            __Win32__OPEN_EXISTING,
            __Win32__FILE_FLAG_OPEN_REPARSE_POINT | __Win32__FILE_FLAG_BACKUP_SEMANTICS,
            nullptr
        );

        if (handle0 == __Win32__INVALID_HANDLE_VALUE) {
            return Platform::Error::PlatformLastError();
        }

        defer{ (instance.pCloseHandle)(handle0); };

        HANDLE handle1 = (*instance.pCreateFileW)(
            pFilepath1,
            0,
            __Win32__FILE_SHARE_READ | __Win32__FILE_SHARE_WRITE | __Win32__FILE_SHARE_DELETE,
            nullptr,
            __Win32__OPEN_EXISTING,
            __Win32__FILE_FLAG_OPEN_REPARSE_POINT | __Win32__FILE_FLAG_BACKUP_SEMANTICS,
            nullptr
        );

        if (handle1 == __Win32__INVALID_HANDLE_VALUE) {
            return Platform::Error::PlatformLastError();
        }

        defer{ (instance.pCloseHandle)(handle1); };
        
        Platform::Handle hFile0;
        hFile0.handle = handle0;

        Platform::Handle hFile1;
        hFile1.handle = handle1;

        return FileSameByHandle_Vista(instance, hFile0, hFile1);
    }

    static Result<bool, Platform::Error> FileSame_24H2_26052(const PlatformInstance& instance, const wchar_t* pFilepath0, const wchar_t* pFilepath1) noexcept {

        FILE_STAT_BASIC_INFORMATION info0;
        bool isOk = (*instance.pGetFileInformationByName)(pFilepath0, FileStatBasicByNameInfo, &info0, sizeof(FILE_STAT_BASIC_INFORMATION));
        if (!isOk) {
            return Platform::Error::PlatformLastError();
        }

        FILE_STAT_BASIC_INFORMATION info1;
        isOk = (*instance.pGetFileInformationByName)(pFilepath1, FileStatBasicByNameInfo, &info1, sizeof(FILE_STAT_BASIC_INFORMATION));
        if (!isOk) {
            return Platform::Error::PlatformLastError();
        }

        return
            info0.VolumeSerialNumber.QuadPart == info1.VolumeSerialNumber.QuadPart &&
            //info0.FileId.QuadPart == info1.FileId.QuadPart &&
            // For modern Windows are enough to check 128 bit version?
            std::memcmp(info0.FileId128.Identifier, info1.FileId128.Identifier, sizeof(info0.FileId128.Identifier)) == 0;
    }

    static Time TimeGetSystemTimeUTC_XP(const PlatformInstance& instance) noexcept {

        FILETIME ft;
        (*instance.pGetSystemTimeAsFileTime)(&ft);

        return Time::FromWin32FileTime((static_cast<u64>(ft.dwHighDateTime) << 32) | static_cast<u64>(ft.dwLowDateTime));
    }

    static Time TimeGetSystemTimeUTC_Win8(const PlatformInstance& instance) noexcept {

        FILETIME ft;
        (*instance.pGetSystemTimePreciseAsFileTime)(&ft);

        return Time::FromWin32FileTime((static_cast<u64>(ft.dwHighDateTime) << 32) | static_cast<u64>(ft.dwLowDateTime));
    }

    static Result<Time, Platform::Error> TimeGetSystemTimeLocalTime_XP(const PlatformInstance& instance) noexcept {

        FILETIME ft;
        (*instance.pGetSystemTimeAsFileTime)(&ft);

        const Time time = Time::FromWin32FileTime(static_cast<u64>(ft.dwHighDateTime) << 32 | static_cast<u64>(ft.dwLowDateTime));
        const DateTime dateTime = DateTime::FromTime(time);

        SYSTEMTIME sys;
        sys.wYear = static_cast<WORD>(dateTime.GetYear());
        sys.wMonth = static_cast<WORD>(dateTime.GetMonth());
        sys.wDayOfWeek = static_cast<WORD>(dateTime.GetDayOfWeek());
        sys.wDay = static_cast<WORD>(dateTime.GetDay());
        sys.wHour = static_cast<WORD>(dateTime.GetHour());
        sys.wMinute = static_cast<WORD>(dateTime.GetMinute());
        sys.wSecond = static_cast<WORD>(dateTime.GetSecond());
        sys.wMilliseconds = static_cast<WORD>(dateTime.GetMillisecond());

        SYSTEMTIME result;

        const bool isOk = (*instance.pSystemTimeToTzSpecificLocalTime)(nullptr, &sys, &result);
        if (!isOk) {
            return Platform::Error::PlatformLastError();
        }

        DateTime::Desc desc{};
        desc.year = static_cast<usize>(result.wYear);
        desc.month = static_cast<usize>(result.wMonth);
        desc.day = static_cast<usize>(result.wDay);
        desc.hour = static_cast<usize>(result.wHour);
        desc.minute = static_cast<usize>(result.wMinute);
        desc.second = static_cast<usize>(result.wSecond);
        desc.nanosecond = static_cast<usize>(result.wMilliseconds) * 1'000'000;

        return Time::FromSince1Jan1970NS(DateTime::From(desc).GetSince1Jan1970NS());
    }

    static Result<Time, Platform::Error> TimeGetSystemTimeLocalTime_Win8(const PlatformInstance& instance) noexcept {

        FILETIME ft;
        (*instance.pGetSystemTimePreciseAsFileTime)(&ft);

        const Time time = Time::FromWin32FileTime(static_cast<u64>(ft.dwHighDateTime) << 32 | static_cast<u64>(ft.dwLowDateTime));
        const DateTime dateTime = DateTime::FromTime(time);

        SYSTEMTIME sys;
        sys.wYear = static_cast<WORD>(dateTime.GetYear());
        sys.wMonth = static_cast<WORD>(dateTime.GetMonth());
        sys.wDayOfWeek = static_cast<WORD>(dateTime.GetDayOfWeek());
        sys.wDay = static_cast<WORD>(dateTime.GetDay());
        sys.wHour = static_cast<WORD>(dateTime.GetHour());
        sys.wMinute = static_cast<WORD>(dateTime.GetMinute());
        sys.wSecond = static_cast<WORD>(dateTime.GetSecond());
        sys.wMilliseconds = static_cast<WORD>(dateTime.GetMillisecond());

        SYSTEMTIME result;

        const bool isOk = (*instance.pSystemTimeToTzSpecificLocalTime)(nullptr, &sys, &result);
        if (!isOk) {
            return Platform::Error::PlatformLastError();
        }

        DateTime::Desc desc{};
        desc.year = static_cast<u64>(result.wYear);
        desc.month = static_cast<u64>(result.wMonth);
        desc.day = static_cast<u64>(result.wDay);
        desc.hour = static_cast<u64>(result.wHour);
        desc.minute = static_cast<u64>(result.wMinute);
        desc.second = static_cast<u64>(result.wSecond);
        desc.nanosecond = static_cast<u64>(result.wMilliseconds) * 1'000'000;

        return Time::FromSince1Jan1970NS(DateTime::From(desc).GetSince1Jan1970NS());
    }

    static u32 ThreadGetIDByHandle_Vista(const PlatformInstance& instance, const Platform::Handle handle) noexcept {

        const DWORD id = (*instance.pGetThreadId)(handle.handle);
        return static_cast<u32>(id);
    }

    static u32 ThreadGetIDByHandle_XP(const PlatformInstance& instance, const Platform::Handle handle) noexcept {
        iferr_scope;

        _THREAD_BASIC_INFORMATION tbi;
        const NTSTATUS ntstatus = (*instance.pNtQueryInformationThread)(handle.handle, ThreadBasicInformation, &tbi, sizeof(tbi), nullptr);
        (ntstatus == 0) iferr_panic_only_dev("What wrong? {}", Platform::Error::PlatformNtError(ntstatus));

        return static_cast<u32>(reinterpret_cast<uptr>(tbi.ClientId.UniqueThread));
    }

    ALT_NODISCARD ALT_FORCE_INLINE Result<Platform::Handle, Platform::Error> FileOpenOrCreateJustOneFile(
        const wchar_t* pFilepath,
        const Platform::FileOpenOrCreateBits createBits,
        const Platform::FileShareBits shareBits,
        const Platform::FileAccessBits accessBits_,
        const Platform::FileHintBits hintBits_
    ) const noexcept {

        DWORD accessBits = static_cast<DWORD>(accessBits_);
        DWORD hintBits = __Win32__FILE_ATTRIBUTE_NORMAL | static_cast<DWORD>(hintBits_);

        if (BitFlag::IsSetEnumFlags(createBits, Platform::FileOpenOrCreateBits::DeleteByHandle)) {
            accessBits |= __Win32__DELETE;
        }

        DWORD creationFlag = 0;
        if (BitFlag::IsSetEnumFlags(createBits, Platform::FileOpenOrCreateBits::Open)) {

            if (BitFlag::IsSetEnumFlags(createBits, Platform::FileOpenOrCreateBits::Clear)) {

                ALT_ASSERT(BitFlag::IsSetEnumFlags(accessBits_, Platform::FileAccessBits::Write));

                creationFlag = __Win32__TRUNCATE_EXISTING;
            }
            else {

                creationFlag = __Win32__OPEN_EXISTING;
            }
        }
        else {

            if (BitFlag::IsSetEnumFlags(createBits, Platform::FileOpenOrCreateBits::Clear)) {

                creationFlag = __Win32__CREATE_ALWAYS;
            }
            else {

                creationFlag = __Win32__OPEN_ALWAYS;
            }
        }

        HANDLE handle = (*pCreateFileW)(
            pFilepath,
            accessBits,
            static_cast<DWORD>(shareBits),
            nullptr,
            creationFlag,
            hintBits,
            nullptr
        );

        if (handle == __Win32__INVALID_HANDLE_VALUE) {
            return Platform::Error::PlatformLastError();
        }

        Platform::Handle hFile;
        hFile.handle = handle;

        return hFile;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Result<Platform::Handle, Platform::Error> FileOpenOrCreateRecursive(
        const Slice<wchar_t> filepath,
        const Platform::FileOpenOrCreateBits createBits,
        const Platform::FileShareBits shareBits,
        const Platform::FileAccessBits accessBits_,
        const Platform::FileHintBits hintBits_,
        const usize errCode
    ) const noexcept {
        iferr_scope;

        const usize rootOffset = filepath.FindLast(ALT_DS) iferr_return Platform::Error::PlatformLastError(errCode);
        const Slice<wchar_t> rootPath = filepath.SubSlice(0, rootOffset + 1);

        Platform::Error err;
        FileDirectoryCreateRecursive(rootPath, errCode) iferr_return_with(err) err;

        return FileOpenOrCreateJustOneFile(filepath.Data(), createBits, shareBits, accessBits_, hintBits_);
    }

    ALT_NODISCARD ALT_FORCE_INLINE Result<void, Platform::Error> FileCreateSymLinkJustOneFile(
        const wchar_t* pFilepath,
        const wchar_t* pFilepathTarget,
        const Platform::SymLinkTarget symLinkTarget
    ) const noexcept {

        ALT_ASSERT(pCreateSymbolicLinkW);

        DWORD target = 0;
        if (symLinkTarget == Platform::SymLinkTarget::File) {

            target = 0;
        }
        else if (symLinkTarget == Platform::SymLinkTarget::Directory) {

            target = __Win32__SYMBOLIC_LINK_FLAG_DIRECTORY;
        }
        else if (symLinkTarget == Platform::SymLinkTarget::Unknown) {

            const DWORD err = (*pGetFileAttributesW)(pFilepath);
            if (err == __Win32__INVALID_FILE_ATTRIBUTES) {
                return Platform::Error::PlatformLastError();
            }

            if ((err & __Win32__FILE_ATTRIBUTE_DIRECTORY) != 0) {

                target = __Win32__SYMBOLIC_LINK_FLAG_DIRECTORY;
            }
            else {

                target = 0;
            }
        }

        const bool isOk = (*pCreateSymbolicLinkW)(pFilepath, pFilepathTarget, target);

        if (!isOk) {
            return Platform::Error::PlatformLastError();
        }

        return Ok;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Result<void, Platform::Error> FileCreateSymLinkRecursive(
        Slice<wchar_t> filepath,
        const wchar_t* pFilepathTarget,
        const Platform::SymLinkTarget symLinkTarget,
        const usize errCode
    ) const noexcept {
        iferr_scope;

        const usize rootOffset = filepath.FindLast(ALT_DS) iferr_return Platform::Error::PlatformLastError(errCode);
        const Slice<wchar_t> rootPath = filepath.SubSlice(0, rootOffset + 1);

        Platform::Error err;
        FileDirectoryCreateRecursive(rootPath, errCode) iferr_return_with(err) err;

        return FileCreateSymLinkJustOneFile(filepath.Data(), pFilepathTarget, symLinkTarget);
    }

    ALT_NODISCARD ALT_FORCE_INLINE Result<void, Platform::Error> FileCreateHardLinkJustOneFile(
        const wchar_t* pFilepath,
        const wchar_t* pFilepathTarget
    ) const noexcept {

        const bool isOk = (*pCreateHardLinkW)(pFilepath, pFilepathTarget, nullptr);

        if (!isOk) {
            return Platform::Error::PlatformLastError();
        }

        return Ok;
    }

    ALT_NODISCARD ALT_FORCE_INLINE Result<void, Platform::Error> FileCreateHardLinkRecursive(
        Slice<wchar_t> filepath,
        const wchar_t* pFilepathTarget,
        const usize errCode
    ) const noexcept {
        iferr_scope;

        const usize rootOffset = filepath.FindLast(ALT_DS) iferr_return Platform::Error::PlatformLastError(errCode);
        const Slice<wchar_t> rootPath = filepath.SubSlice(0, rootOffset + 1);

        Platform::Error err;
        FileDirectoryCreateRecursive(rootPath, errCode) iferr_return_with(err) err;

        return FileCreateHardLinkJustOneFile(filepath.Data(), pFilepathTarget);
    }

    ALT_NODISCARD Result<void, Platform::Error> FileDirectoryCreateRecursive(Slice<wchar_t> filepath, usize errCode) const noexcept {
        iferr_scope;

        const usize rootOffset = filepath.FindLast(ALT_DS) iferr_return Platform::Error::PlatformLastError(errCode);
        Slice<wchar_t> rootPath = filepath.SubSlice(0, rootOffset);

        while (rootPath.IsPopulated() && rootPath.GetLast() == ALT_DS) {

            rootPath = rootPath.SubSlice(0, rootPath.GetSize() - 1);
        }

        *(filepath.Data() + rootPath.GetSize()) = '\0';
        defer{ *(filepath.Data() + rootPath.GetSize()) = ALT_DS; };

        const bool isOk = (*pCreateDirectoryW)(filepath.Data(), nullptr);
        errCode = GetLastError();

        if (isOk || errCode == __Win32__ERROR_ALREADY_EXISTS) {
            return Ok;
        }

        if (errCode == __Win32__ERROR_PATH_NOT_FOUND) {

            Platform::Error err;
            FileDirectoryCreateRecursive(rootPath, errCode) iferr_return_with(err) err;

            const bool isOk = (*pCreateDirectoryW)(filepath.Data(), nullptr);
            if (isOk) {
                return Ok;
            }

            errCode = GetLastError();
        }

        return Platform::Error::PlatformLastError(errCode);
    }

    ALT_NODISCARD ALT_FORCE_INLINE Optional<Utf16BufferOffset> Utf16BufferErrorCode(const usize err) noexcept {

        const usize sizeOld = stl_Utf16Buffer.GetSize();
        iferr_scope_on_err {
            iferr_scope;

            stl_Utf16Buffer.Resize(sizeOld) iferr_panic_only_dev("resize to the less size - never panic");
        };

        const DWORD MSG_SIZE = 256;
        stl_Utf16Buffer.ResizeAddition(static_cast<usize>(MSG_SIZE)) iferr_return Nullopt;

        DWORD size = (*pFormatMessageW)(
            __Win32__FORMAT_MESSAGE_FROM_SYSTEM | __Win32__FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr,
            static_cast<DWORD>(err),
            __Win32__MAKELANGID(__Win32__LANG_NEUTRAL, __Win32__SUBLANG_DEFAULT),
            Utf16BufferPtr(sizeOld),
            MSG_SIZE,
            nullptr
        );

        if (size == 0) {
            iferr_return_return Nullopt;
        }

        ALT_ASSERT(size > 0 && *Utf16BufferPtr(size - 1) == static_cast<u16>('\n'));
        size -= 1;

        if (size > 0) {
            ALT_ASSERT(*Utf16BufferPtr(size - 1) != static_cast<u16>('\r'));
        }

        stl_Utf16Buffer.Resize(sizeOld + static_cast<usize>(size)) iferr_panic_only_dev("resize to the less size - never panic");

        Utf16BufferOffset bufferOffset{};
        bufferOffset.m_Offset = sizeOld;
        bufferOffset.m_OffsetEnd = sizeOld + static_cast<usize>(size);

        return bufferOffset;
    }

    class FileRemoveAllCtx {
    public:

        ALT_NODISCARD ALT_FORCE_INLINE bool Run(
            PlatformInstance& instance,
            const usize offsetToFileDirInfo,
            const usize offsetToPath,
            const usize sizeInBytesFileDirInfo,
            const _PlatformDetails::DirectoryEntrySizeResult& directoryEntrySizeResult,
            const Platform::FileRemoveAllType type
        ) noexcept {
            iferr_scope;

            m_pInstance = &instance;
            m_pDirectoryEntrySizeResult = &directoryEntrySizeResult;

            switch (type) {
            case Platform::FileRemoveAllType::All: {

                const usize posNullterm = m_pInstance->Utf16BufferSizeAssumeLast(offsetToPath, Utf16BufferAppendBits::PathBuffer);

                const bool isTryGetFileTypeByHandle = false;
                const Platform::FileTypeBits fileTypeBits = (*m_pInstance->pFileGetType)(*m_pInstance, m_pInstance->Utf16BufferPtr(offsetToPath), isTryGetFileTypeByHandle)
                    iferr_return false;

                if (BitFlag::IsSetEnumFlags(fileTypeBits, Platform::FileTypeBits::Directory)) {

                    const bool isAllOk = FileRemoveAllRecursive(offsetToFileDirInfo, offsetToPath, sizeInBytesFileDirInfo);
                    if (isAllOk) {

                        wchar_t* path = m_pInstance->Utf16BufferPtr(offsetToPath);
                        path[posNullterm] = L'\0';

                        return (*m_pInstance->pRemoveDirectoryW)(path);
                    }

                    return isAllOk;
                }

                m_pInstance->FileDelete(m_pInstance->Utf16BufferPtr(offsetToPath)) iferr_return false;

                return true;
            }
            case Platform::FileRemoveAllType::Inside: {

                return FileRemoveAllRecursive(offsetToFileDirInfo, offsetToPath, sizeInBytesFileDirInfo);
            }
            }

            ALT_UNREACHABLE();
        }

    private:

        bool FileRemoveAllRecursive(
            const usize offsetToFileDirInfo,
            const usize offsetToPath,
            const usize sizeInBytesFileDirInfo
        ) noexcept {

            bool isAllOk = true;
            usize numRetries = 0;

            ALT_MAYBE_UNUSED const Result<void, Platform::Error> result =
                m_pInstance->FileForEachDirectoryEntry(offsetToFileDirInfo, offsetToPath, sizeInBytesFileDirInfo, Nullopt,
                [this, &numRetries, &isAllOk](const Platform::FileDirectoryEntry& entry) -> Result<Platform::FileForEachDirectoryEntryContinuation, Platform::Error> {
                    iferr_scope;

                    if (entry.isDir) {

                        bool isOk = false;
                        {
                            const usize offset = m_pInstance->Utf16BufferAppend(*entry.filepath, m_pDirectoryEntrySizeResult->sizeToAllocate)
                                iferr_return Platform::FileForEachDirectoryEntryContinuation::Continue;

                            defer{ m_pInstance->Utf16BufferDeferGuard(offset); };

                            isOk = FileRemoveAllRecursive(offset, offset + m_pDirectoryEntrySizeResult->sizeToAllocate, m_pDirectoryEntrySizeResult->sizeInBytesFileDirInfo);
                        }

                        if (isOk) {
                            isOk = (*m_pInstance->pRemoveDirectoryW)(m_pInstance->Utf16BufferPtr(entry.filepath->GetOffset()));
                        }

                        if (!isOk) {
                            return ReturnRetryOrContinueContinuation(numRetries, isAllOk);
                        }
                    }
                    else {

                        m_pInstance->FileDelete(m_pInstance->Utf16BufferPtr(entry.filepath->GetOffset())) iferr {
                            return ReturnRetryOrContinueContinuation(numRetries, isAllOk);
                        }
                    }

                    return ReturnContinueContinuation(numRetries);
                }, Platform::FileForEachDirectoryEntryBits::CreatePathOffset);

            return isAllOk;
        }

        ALT_FORCE_INLINE Platform::FileForEachDirectoryEntryContinuation ReturnRetryOrContinueContinuation(usize& numRetries, bool& isAllOk) noexcept {

            // ERROR_ACCESS_DENIED
            // ERROR_DIRECTORY_NOT_EMPTY
            if (numRetries < MAX_NUM_RETRIES) {

                numRetries += 1;
                return Platform::FileForEachDirectoryEntryContinuation::Retry;
            }

            if (numRetries >= MAX_NUM_RETRIES) {
                isAllOk = false;
            }

            numRetries = 0;
            return Platform::FileForEachDirectoryEntryContinuation::Continue;
        }

        ALT_FORCE_INLINE Platform::FileForEachDirectoryEntryContinuation ReturnContinueContinuation(usize& numRetries) noexcept {

            numRetries = 0;
            return Platform::FileForEachDirectoryEntryContinuation::Continue;
        }

    private:

        // MSVC uses 10
        constexpr static usize MAX_NUM_RETRIES = 10;

        PlatformInstance* m_pInstance = nullptr;
        const _PlatformDetails::DirectoryEntrySizeResult* m_pDirectoryEntrySizeResult = nullptr;
    };

private:

    struct TickState {

        LARGE_INTEGER timeFreq;
        LARGE_INTEGER timeCounterStart;
    };

    TickState m_TickState;

    usize m_PageSizeInBytes;
    usize m_AllocationGranularity;
    usize m_NumOfLogicalProcessors;
    Platform::CPUArchitecture m_CpuArchitecture;

    HANDLE m_MainMemoryHeap = NULL;
    HMODULE m_hComdlgModule = NULL;

    // A pseudo handle is a special constant, currently (HANDLE)-1, that is interpreted as the current process handle.
    // For compatibility with future operating systems, it is best to call GetCurrentProcess instead of hard-coding this constant value.
    HANDLE m_CurrentProcess = (HANDLE)-1;

    inline static thread_local Vector<u16> stl_Utf16Buffer;
};

inline PlatformInstance* s_pPlatformInstance = nullptr;

#ifdef ALT_USE_MANUAL_PLATFORM_INSTANCE_INITIALIZATION

ALT_NODISCARD ALT_FORCE_INLINE PlatformInstance& GetPlatformInstance() noexcept {

    /*
     * Use manual initialization of _PlatformDetails::PlatformInstance:
     * 
     * int main() {
     * 
     *     Alt::_PlatformDetails::PlatformInstance::CreateDesc desc{};
     * 
     *     Alt::_PlatformDetails::PlatformInstance altPlatformIntance = Alt::_PlatformDetails::PlatformInstance::Create(desc);
     *     Alt::InitializePlatformInstance(&altPlatformIntance);
     * }
     */
    assert(s_pPlatformInstance); // use InitializePlatformInstance()

    return *s_pPlatformInstance;
}
#else

ALT_NODISCARD static PlatformInstance& GetPlatformInstance() noexcept {

    struct AutoInitializePlatformInstance {

        AutoInitializePlatformInstance() noexcept {

            PlatformInstance::CreateDesc desc{};

            static PlatformInstance s_PlatformInstance = PlatformInstance::Create(desc);
            s_pPlatformInstance = &s_PlatformInstance;
        }

        ALT_FORCE_INLINE PlatformInstance& GetPlatformInstance() noexcept {

            return *s_pPlatformInstance;
        }
    };

    static AutoInitializePlatformInstance instance{};
    return instance.GetPlatformInstance();
}
#endif // ALT_USE_MANUAL_PLATFORM_INSTANCE_INITIALIZATION

ALT_NODISCARD inline bool Utf16BufferOffset::CloneIntoAppend(String& buffer) const noexcept {

    const _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    return instance.Utf16BufferCloneIntoAppend(*this, buffer);
}

} // namespace _PlatformDetails

/*
 * @example
 * 
 * Alt::_PlatformDetails::PlatformInstance::CreateDesc desc{};
 * 
 * Alt::_PlatformDetails::PlatformInstance altPlatformIntance = Alt::_PlatformDetails::PlatformInstance::Create(desc);
 * Alt::InitializePlatformInstance(&altPlatformIntance);
 */
ALT_FORCE_INLINE void InitializePlatformInstance(ALT_MAYBE_UNUSED _PlatformDetails::PlatformInstance* pPlatformInstance) noexcept {

#ifdef ALT_USE_MANUAL_PLATFORM_INSTANCE_INITIALIZATION

    _PlatformDetails::s_pPlatformInstance = pPlatformInstance;
#endif // ALT_USE_MANUAL_PLATFORM_INSTANCE_INITIALIZATION
}

namespace Platform {

ALT_NODISCARD ALT_FORCE_INLINE uptr GetSeedAddress() noexcept {

    return reinterpret_cast<uptr>(_PlatformDetails::s_pPlatformInstance);
}

ALT_NODISCARD ALT_FORCE_INLINE uptr GetReturnAddress() noexcept {

#if defined(ALT_COMPILER_MSVC)

    return reinterpret_cast<uptr>(_ReturnAddress());

#elif defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC)

    return reinterpret_cast<uptr>(__builtin_return_address(0));

#else
    #error provide implementation for specific compiler
#endif
}

/*
 * Return the total number of logical cores/threads on the system
 */
ALT_NODISCARD ALT_FORCE_INLINE usize ThreadGetNumOfLogicalProcessors() noexcept {

    return _PlatformDetails::GetPlatformInstance().ThreadGetNumOfLogicalProcessors();
}

/*
 * Return the system CPU architecture
 */
ALT_NODISCARD ALT_FORCE_INLINE CPUArchitecture GetCpuArchitecture() noexcept {

    return _PlatformDetails::GetPlatformInstance().GetCpuArchitecture();
}

[[noreturn]] ALT_FORCE_INLINE void FastCrash() noexcept {

#if defined(ALT_COMPILER_MSVC)

    __fastfail(__Win32__FAST_FAIL_FATAL_APP_EXIT);
    __assume(0);

#elif defined(ALT_COMPILER_CLANG) || defined(ALT_COMPILER_GCC)

    __builtin_trap();

#else
    #error provide implementation for specific compiler
#endif
}

[[noreturn]] ALT_FORCE_INLINE void Terminate(const u32 exitCode) noexcept {

    _PlatformDetails::GetPlatformInstance().Terminate(exitCode);
}

[[noreturn]] ALT_FORCE_INLINE void Exit(const u32 exitCode = ALT_EXIT_CODE_FAILURE) noexcept {

    _PlatformDetails::GetPlatformInstance().Exit(exitCode);
}

ALT_NODISCARD ALT_FORCE_INLINE usize GetPageSizeInBytes() noexcept {

    return _PlatformDetails::GetPlatformInstance().GetPageSizeInBytes();
}

ALT_NODISCARD ALT_FORCE_INLINE usize GetPageAlign() noexcept {

    return _PlatformDetails::GetPlatformInstance().GetPageAlign();
}

ALT_NODISCARD ALT_FORCE_INLINE u32 MemoryRoundUpToPageSizeWrap(u32 sizeInBytes) noexcept {

    const u32 pageSizePow2 = static_cast<u32>(_PlatformDetails::GetPlatformInstance().GetPageSizeInBytes());

    sizeInBytes += sizeInBytes == 0 ? 1 : 0;

    const u32 low = sizeInBytes & (pageSizePow2 - 1);
    const u32 roundOffset = (pageSizePow2 - low) & ~pageSizePow2;

    return Num::AddWrap(sizeInBytes, roundOffset);
}

ALT_NODISCARD ALT_FORCE_INLINE u32 MemoryRoundUpToPageSizeWrap(u32 sizeInBytes, const u32 wrap) noexcept {

    const u32 pageSizePow2 = static_cast<u32>(_PlatformDetails::GetPlatformInstance().GetPageSizeInBytes());

    sizeInBytes += sizeInBytes == 0 ? 1 : 0;

    const u32 low = sizeInBytes & (pageSizePow2 - 1);
    const u32 roundOffset = (pageSizePow2 - low) & ~pageSizePow2;

    return Num::AddWrap(sizeInBytes, roundOffset, wrap);
}

ALT_NODISCARD ALT_FORCE_INLINE u64 MemoryRoundUpToPageSizeWrap(u64 sizeInBytes) noexcept {

    const u64 pageSizePow2 = static_cast<u64>(_PlatformDetails::GetPlatformInstance().GetPageSizeInBytes());

    sizeInBytes += sizeInBytes == 0 ? 1 : 0;

    const u64 low = sizeInBytes & (pageSizePow2 - 1);
    const u64 roundOffset = (pageSizePow2 - low) & ~pageSizePow2;

    return Num::AddWrap(sizeInBytes, roundOffset);
}

ALT_NODISCARD ALT_FORCE_INLINE u64 MemoryRoundUpToPageSizeWrap(u64 sizeInBytes, const u64 wrap) noexcept {

    const u64 pageSizePow2 = static_cast<u64>(_PlatformDetails::GetPlatformInstance().GetPageSizeInBytes());

    sizeInBytes += sizeInBytes == 0 ? 1 : 0;

    const u64 low = sizeInBytes & (pageSizePow2 - 1);
    const u64 roundOffset = (pageSizePow2 - low) & ~pageSizePow2;

    return Num::AddWrap(sizeInBytes, roundOffset, wrap);
}

ALT_NODISCARD ALT_FORCE_INLINE u32 MemoryRoundUpToPageSizeWrapPreserveZero(const u32 sizeInBytes) noexcept {

    const u32 pageSizePow2 = static_cast<u32>(_PlatformDetails::GetPlatformInstance().GetPageSizeInBytes());

    const u32 low = sizeInBytes & (pageSizePow2 - 1);
    const u32 roundOffset = (pageSizePow2 - low) & ~pageSizePow2;

    return Num::AddWrap(sizeInBytes, roundOffset);
}

ALT_NODISCARD ALT_FORCE_INLINE u32 MemoryRoundUpToPageSizeWrapPreserveZero(const u32 sizeInBytes, const u32 wrap) noexcept {

    const u32 pageSizePow2 = static_cast<u32>(_PlatformDetails::GetPlatformInstance().GetPageSizeInBytes());

    const u32 low = sizeInBytes & (pageSizePow2 - 1);
    const u32 roundOffset = (pageSizePow2 - low) & ~pageSizePow2;

    return Num::AddWrap(sizeInBytes, roundOffset, wrap);
}

ALT_NODISCARD ALT_FORCE_INLINE u64 MemoryRoundUpToPageSizeWrapPreserveZero(const u64 sizeInBytes) noexcept {

    const u64 pageSizePow2 = static_cast<u64>(_PlatformDetails::GetPlatformInstance().GetPageSizeInBytes());

    const u64 low = sizeInBytes & (pageSizePow2 - 1);
    const u64 roundOffset = (pageSizePow2 - low) & ~pageSizePow2;

    return Num::AddWrap(sizeInBytes, roundOffset);
}

ALT_NODISCARD ALT_FORCE_INLINE u64 MemoryRoundUpToPageSizeWrapPreserveZero(const u64 sizeInBytes, const u64 wrap) noexcept {

    const u64 pageSizePow2 = static_cast<u64>(_PlatformDetails::GetPlatformInstance().GetPageSizeInBytes());

    const u64 low = sizeInBytes & (pageSizePow2 - 1);
    const u64 roundOffset = (pageSizePow2 - low) & ~pageSizePow2;

    return Num::AddWrap(sizeInBytes, roundOffset, wrap);
}

ALT_NODISCARD ALT_FORCE_INLINE u32 MemoryRoundDownToPageSize(const u32 sizeInBytes) noexcept {

    const u32 pageSizePow2 = static_cast<u32>(_PlatformDetails::GetPlatformInstance().GetPageSizeInBytes());

    return sizeInBytes & ~(pageSizePow2 - 1);
}

ALT_NODISCARD ALT_FORCE_INLINE u64 MemoryRoundDownToPageSize(const u64 sizeInBytes) noexcept {

    const u64 pageSizePow2 = static_cast<u64>(_PlatformDetails::GetPlatformInstance().GetPageSizeInBytes());

    return sizeInBytes & ~(pageSizePow2 - 1);
}

ALT_NODISCARD ALT_FORCE_INLINE u8* MemoryAllocateBytes(const usize sizeInBytes, const MemoryAllocateBytesBits bits) noexcept {

    return _PlatformDetails::GetPlatformInstance().MemoryAllocateBytes(sizeInBytes, bits);
}

ALT_NODISCARD ALT_FORCE_INLINE u8* MemoryReallocateBytes(void* pMemoryOld, const usize sizeInBytes, const MemoryReallocateBytesBits bits) noexcept {

    return _PlatformDetails::GetPlatformInstance().MemoryReallocateBytes(pMemoryOld, sizeInBytes, bits);
}

ALT_FORCE_INLINE void MemoryDeallocateBytes(void* pMemory) noexcept {

    _PlatformDetails::GetPlatformInstance().MemoryDeallocateBytes(pMemory);
}

ALT_NODISCARD ALT_FORCE_INLINE u8* MemoryAlignedAllocateBytes(const usize sizeInBytes, const usize alignment, const MemoryAllocateBytesBits bits) noexcept {
    iferr_scope;

    ALT_ASSERT(Num::IsPowerOf2(alignment) && alignment <= Num::Desc<u32>::Max());

    constexpr static usize ALIGN_SIZE_OF = sizeof(u32);

    const usize totalSizeInBytes = Num::AddOverflowCheck(sizeInBytes, alignment) iferr_return nullptr;
    u8* pMemory = _PlatformDetails::GetPlatformInstance().MemoryAllocateBytes(totalSizeInBytes, bits) iferr_return nullptr;

    pMemory += ALIGN_SIZE_OF;

    const usize offset = Memory::Align(pMemory, alignment);
    u8* pResult = pMemory + offset;

    const u32 alignOffset = static_cast<u32>(offset);
    std::memcpy(pResult - ALIGN_SIZE_OF, &alignOffset, ALIGN_SIZE_OF);

    return pResult;
}

ALT_FORCE_INLINE void MemoryAlignedDeallocateBytes(void* pMemory_) noexcept {

    constexpr static usize ALIGN_SIZE_OF = sizeof(u32);

    u8* pMemory = reinterpret_cast<u8*>(pMemory_);

    if (!pMemory) {
        return;
    }

    pMemory -= ALIGN_SIZE_OF;

    u32 alignOffset;
    std::memcpy(&alignOffset, pMemory, ALIGN_SIZE_OF);

    pMemory -= static_cast<usize>(alignOffset);

    _PlatformDetails::GetPlatformInstance().MemoryDeallocateBytes(pMemory);
}

/*
 * https://learn.microsoft.com/en-us/windows/win32/memory/memory-limits-for-windows-releases
 */
ALT_NODISCARD ALT_FORCE_INLINE u8* VirtualMemoryAllocateBytes(
    void* pMemory,
    usize sizeInBytes,
    const VirtualMemoryAllocateBytesBits bits,
    const VirtualMemoryAccessBits accessBits
) noexcept {

    return _PlatformDetails::GetPlatformInstance().VirtualMemoryAllocateBytes(pMemory, sizeInBytes, bits, accessBits);
}

ALT_FORCE_INLINE void VirtualMemoryDeallocateBytes(
    void* pMemory,
    const usize sizeInBytes,
    const VirtualMemoryDeallocateBytesBits bits
) noexcept {

    return _PlatformDetails::GetPlatformInstance().VirtualMemoryDeallocateBytes(pMemory, sizeInBytes, bits);
}

/*
 * Return numbers of ticks since first initialization of `GetTicks...()` call
 */
ALT_NODISCARD ALT_FORCE_INLINE u64 GetTicks() noexcept {

    return _PlatformDetails::GetPlatformInstance().GetTicks();
}

/*
 * Return elapsed time in seconds since first initialization of `GetTicks...()` call
 */
ALT_NODISCARD ALT_FORCE_INLINE f64 GetTicksSeconds() noexcept {

    return _PlatformDetails::GetPlatformInstance().GetTicksSeconds();
}

/*
 * Return elapsed time in milliseconds since first initialization of `GetTicks...()` call
 */
ALT_NODISCARD ALT_FORCE_INLINE f64 GetTicksMs() noexcept {

    return GetTicksSeconds() * 1'000.0;
}

/*
 * Return elapsed time in nanoseconds since first initialization of `GetTicks...()` call
 */
ALT_NODISCARD ALT_FORCE_INLINE f64 GetTicksNs() noexcept {

    return GetTicksSeconds() * 1'000'000'000.0;
}

/*
 * Suspends the execution of the current thread until the time-out interval elapses
 */
ALT_FORCE_INLINE void SleepMs(const usize milliseconds) noexcept {

    _PlatformDetails::GetPlatformInstance().SleepMs(milliseconds);
}

/*
 * The hint tells the processor that this thread is not doing anything useful
 */
ALT_FORCE_INLINE void SpinHint() noexcept {

    _mm_pause();
}

ALT_FORCE_INLINE void ThreadMemoryOrderBarrier(const MemoryOrder order = MemoryOrder::SeqCst) noexcept {

    switch (order) {
    case MemoryOrder::SeqCst: {
        _PlatformAtomicDetails::ThreadMemoryOrderBarrierSeqCst();
        return;
    }
    case MemoryOrder::Acquire: {
        _PlatformAtomicDetails::ThreadMemoryOrderBarrierAcquire();
        return;
    }
    case MemoryOrder::Release: {
        _PlatformAtomicDetails::ThreadMemoryOrderBarrierRelease();
        return;
    }
    case MemoryOrder::AcqRel:
    case MemoryOrder::Relaxed:
        ALT_UNREACHABLE();
    }

    ALT_UNREACHABLE();
}

template <typename T, typename U,
    std::enable_if_t<
        std::is_convertible_v<U, T> &&
        BitFlag::IsSetEnumFlags(_PlatformAtomicDetails::supported_operations_v<T>, _PlatformAtomicDetails::SupportedOperationsTrait::Store)
    >* = nullptr>
ALT_FORCE_INLINE void AtomicStore(T* pAtomic, const U value, const MemoryOrder order = MemoryOrder::SeqCst) noexcept {

    switch (order) {
    case MemoryOrder::SeqCst: {
        _PlatformAtomicDetails::AtomicStoreSeqCst(pAtomic, static_cast<T>(value));
        return;
    }
    case MemoryOrder::Release: {
        _PlatformAtomicDetails::AtomicStoreRelease(pAtomic, static_cast<T>(value));
        return;
    }
    case MemoryOrder::Relaxed: {
        _PlatformAtomicDetails::AtomicStoreRelaxed(pAtomic, static_cast<T>(value));
        return;
    }
    case MemoryOrder::AcqRel:
    case MemoryOrder::Acquire:
        ALT_UNREACHABLE();
    }

    ALT_UNREACHABLE();
}

template <typename T,
    std::enable_if_t<
        BitFlag::IsSetEnumFlags(_PlatformAtomicDetails::supported_operations_v<T>, _PlatformAtomicDetails::SupportedOperationsTrait::Load)
    >* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE T AtomicLoad(const T* pAtomic, const MemoryOrder order = MemoryOrder::SeqCst) noexcept {

    switch (order) {
    case MemoryOrder::SeqCst: {
        return _PlatformAtomicDetails::AtomicLoadSeqCst(pAtomic);
    }
    case MemoryOrder::Acquire: {
        return _PlatformAtomicDetails::AtomicLoadAcquire(pAtomic);
    }
    case MemoryOrder::Relaxed: {
        return _PlatformAtomicDetails::AtomicLoadRelaxed(pAtomic);
    }
    case MemoryOrder::AcqRel:
    case MemoryOrder::Release:
        ALT_UNREACHABLE();
    }

    ALT_UNREACHABLE();
}

template <typename T, typename U,
    std::enable_if_t<
        std::is_convertible_v<U, T> &&
        BitFlag::IsSetEnumFlags(_PlatformAtomicDetails::supported_operations_v<T>, _PlatformAtomicDetails::SupportedOperationsTrait::Exchange)
    >* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE T AtomicExchange(T* pAtomic, const U value, const MemoryOrder order = MemoryOrder::SeqCst) noexcept {

    switch (order) {
    case MemoryOrder::SeqCst: {
        return _PlatformAtomicDetails::AtomicExchangeSeqCst(pAtomic, static_cast<T>(value));
    }
    case MemoryOrder::AcqRel: {
        return _PlatformAtomicDetails::AtomicExchangeAcqRel(pAtomic, static_cast<T>(value));
    }
    case MemoryOrder::Acquire: {
        return _PlatformAtomicDetails::AtomicExchangeAcquire(pAtomic, static_cast<T>(value));
    }
    case MemoryOrder::Release: {
        return _PlatformAtomicDetails::AtomicExchangeRelease(pAtomic, static_cast<T>(value));
    }
    case MemoryOrder::Relaxed: {
        return _PlatformAtomicDetails::AtomicExchangeRelaxed(pAtomic, static_cast<T>(value));
    }
    }

    ALT_UNREACHABLE();
}

template <typename T, typename U0, typename U1,
    std::enable_if_t<
        std::is_convertible_v<U0, T> &&
        std::is_convertible_v<U1, T> &&
        BitFlag::IsSetEnumFlags(_PlatformAtomicDetails::supported_operations_v<T>, _PlatformAtomicDetails::SupportedOperationsTrait::CompareExchange)
    >* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE T AtomicCompareExchange(
    T* pAtomic,
    const U0 compare,
    const U1 value,
    const MemoryOrder successOrder = MemoryOrder::SeqCst,
    const MemoryOrder failureOrder = MemoryOrder::SeqCst
) noexcept {

    switch (successOrder) {
    case MemoryOrder::SeqCst: {

        switch (failureOrder) {
        case MemoryOrder::SeqCst: {
            return _PlatformAtomicDetails::AtomicCompareExchangeSeqCstSeqCst(pAtomic, static_cast<T>(compare), static_cast<T>(value));
        }
        case MemoryOrder::Acquire: {
            return _PlatformAtomicDetails::AtomicCompareExchangeSeqCstAcquire(pAtomic, static_cast<T>(compare), static_cast<T>(value));
        }
        case MemoryOrder::Relaxed: {
            return _PlatformAtomicDetails::AtomicCompareExchangeSeqCstRelaxed(pAtomic, static_cast<T>(compare), static_cast<T>(value));
        }
        case MemoryOrder::AcqRel:
        case MemoryOrder::Release:
            ALT_UNREACHABLE();
        }
    }
    case MemoryOrder::AcqRel: {

        switch (failureOrder) {
        case MemoryOrder::SeqCst: {
            return _PlatformAtomicDetails::AtomicCompareExchangeAcqRelSeqCst(pAtomic, static_cast<T>(compare), static_cast<T>(value));
        }
        case MemoryOrder::Acquire: {
            return _PlatformAtomicDetails::AtomicCompareExchangeAcqRelAcquire(pAtomic, static_cast<T>(compare), static_cast<T>(value));
        }
        case MemoryOrder::Relaxed: {
            return _PlatformAtomicDetails::AtomicCompareExchangeAcqRelRelaxed(pAtomic, static_cast<T>(compare), static_cast<T>(value));
        }
        case MemoryOrder::AcqRel:
        case MemoryOrder::Release:
            ALT_UNREACHABLE();
        }
    }
    case MemoryOrder::Acquire: {

        switch (failureOrder) {
        case MemoryOrder::SeqCst: {
            return _PlatformAtomicDetails::AtomicCompareExchangeAcquireSeqCst(pAtomic, static_cast<T>(compare), static_cast<T>(value));
        }
        case MemoryOrder::Acquire: {
            return _PlatformAtomicDetails::AtomicCompareExchangeAcquireAcquire(pAtomic, static_cast<T>(compare), static_cast<T>(value));
        }
        case MemoryOrder::Relaxed: {
            return _PlatformAtomicDetails::AtomicCompareExchangeAcquireRelaxed(pAtomic, static_cast<T>(compare), static_cast<T>(value));
        }
        case MemoryOrder::AcqRel:
        case MemoryOrder::Release:
            ALT_UNREACHABLE();
        }
    }
    case MemoryOrder::Release: {

        switch (failureOrder) {
        case MemoryOrder::SeqCst: {
            return _PlatformAtomicDetails::AtomicCompareExchangeReleaseSeqCst(pAtomic, static_cast<T>(compare), static_cast<T>(value));
        }
        case MemoryOrder::Acquire: {
            return _PlatformAtomicDetails::AtomicCompareExchangeReleaseAcquire(pAtomic, static_cast<T>(compare), static_cast<T>(value));
        }
        case MemoryOrder::Relaxed: {
            return _PlatformAtomicDetails::AtomicCompareExchangeReleaseRelaxed(pAtomic, static_cast<T>(compare), static_cast<T>(value));
        }
        case MemoryOrder::AcqRel:
        case MemoryOrder::Release:
            ALT_UNREACHABLE();
        }
    }
    case MemoryOrder::Relaxed: {

        switch (failureOrder) {
        case MemoryOrder::SeqCst: {
            return _PlatformAtomicDetails::AtomicCompareExchangeRelaxedSeqCst(pAtomic, static_cast<T>(compare), static_cast<T>(value));
        }
        case MemoryOrder::Acquire: {
            return _PlatformAtomicDetails::AtomicCompareExchangeRelaxedAcquire(pAtomic, static_cast<T>(compare), static_cast<T>(value));
        }
        case MemoryOrder::Relaxed: {
            return _PlatformAtomicDetails::AtomicCompareExchangeRelaxedRelaxed(pAtomic, static_cast<T>(compare), static_cast<T>(value));
        }
        case MemoryOrder::AcqRel:
        case MemoryOrder::Release:
            ALT_UNREACHABLE();
        }
    }
    }

    ALT_UNREACHABLE();
}

template <typename T, typename U,
    std::enable_if_t<
        std::is_convertible_v<U, T> &&
        BitFlag::IsSetEnumFlags(_PlatformAtomicDetails::supported_operations_v<T>, _PlatformAtomicDetails::SupportedOperationsTrait::Additive)
    >* = nullptr>
ALT_FORCE_INLINE T AtomicAdd(T* pAtomic, const U value, const MemoryOrder order = MemoryOrder::SeqCst) noexcept {

    switch (order) {
    case MemoryOrder::SeqCst: {
        return _PlatformAtomicDetails::AtomicAddSeqCst(pAtomic, static_cast<T>(value));
    }
    case MemoryOrder::AcqRel: {
        return _PlatformAtomicDetails::AtomicAddAcqRel(pAtomic, static_cast<T>(value));
    }
    case MemoryOrder::Acquire: {
        return _PlatformAtomicDetails::AtomicAddAcquire(pAtomic, static_cast<T>(value));
    }
    case MemoryOrder::Release: {
        return _PlatformAtomicDetails::AtomicAddRelease(pAtomic, static_cast<T>(value));
    }
    case MemoryOrder::Relaxed: {
        return _PlatformAtomicDetails::AtomicAddRelaxed(pAtomic, static_cast<T>(value));
    }
    }

    ALT_UNREACHABLE();
}

template <typename T, typename U,
    std::enable_if_t<
        std::is_convertible_v<U, T> &&
        BitFlag::IsSetEnumFlags(_PlatformAtomicDetails::supported_operations_v<T>, _PlatformAtomicDetails::SupportedOperationsTrait::Additive)
    >* = nullptr>
ALT_FORCE_INLINE T AtomicSub(T* pAtomic, const U value, const MemoryOrder order = MemoryOrder::SeqCst) noexcept {

    switch (order) {
    case MemoryOrder::SeqCst: {
        return _PlatformAtomicDetails::AtomicSubSeqCst(pAtomic, static_cast<T>(value));
    }
    case MemoryOrder::AcqRel: {
        return _PlatformAtomicDetails::AtomicSubAcqRel(pAtomic, static_cast<T>(value));
    }
    case MemoryOrder::Acquire: {
        return _PlatformAtomicDetails::AtomicSubAcquire(pAtomic, static_cast<T>(value));
    }
    case MemoryOrder::Release: {
        return _PlatformAtomicDetails::AtomicSubRelease(pAtomic, static_cast<T>(value));
    }
    case MemoryOrder::Relaxed: {
        return _PlatformAtomicDetails::AtomicSubRelaxed(pAtomic, static_cast<T>(value));
    }
    }

    ALT_UNREACHABLE();
}

template <typename T,
    std::enable_if_t<
        BitFlag::IsSetEnumFlags(_PlatformAtomicDetails::supported_operations_v<T>, _PlatformAtomicDetails::SupportedOperationsTrait::Additive)
    >* = nullptr>
ALT_FORCE_INLINE T AtomicInc(T* pAtomic, const MemoryOrder order = MemoryOrder::SeqCst) noexcept {

    switch (order) {
    case MemoryOrder::SeqCst: {
        return _PlatformAtomicDetails::AtomicIncSeqCst(pAtomic);
    }
    case MemoryOrder::AcqRel: {
        return _PlatformAtomicDetails::AtomicIncAcqRel(pAtomic);
    }
    case MemoryOrder::Acquire: {
        return _PlatformAtomicDetails::AtomicIncAcquire(pAtomic);
    }
    case MemoryOrder::Release: {
        return _PlatformAtomicDetails::AtomicIncRelease(pAtomic);
    }
    case MemoryOrder::Relaxed: {
        return _PlatformAtomicDetails::AtomicIncRelaxed(pAtomic);
    }
    }

    ALT_UNREACHABLE();
}

template <typename T,
    std::enable_if_t<
        BitFlag::IsSetEnumFlags(_PlatformAtomicDetails::supported_operations_v<T>, _PlatformAtomicDetails::SupportedOperationsTrait::Additive)
    >* = nullptr>
ALT_FORCE_INLINE T AtomicDec(T* pAtomic, const MemoryOrder order = MemoryOrder::SeqCst) noexcept {

    switch (order) {
    case MemoryOrder::SeqCst: {
        return _PlatformAtomicDetails::AtomicDecSeqCst(pAtomic);
    }
    case MemoryOrder::AcqRel: {
        return _PlatformAtomicDetails::AtomicDecAcqRel(pAtomic);
    }
    case MemoryOrder::Acquire: {
        return _PlatformAtomicDetails::AtomicDecAcquire(pAtomic);
    }
    case MemoryOrder::Release: {
        return _PlatformAtomicDetails::AtomicDecRelease(pAtomic);
    }
    case MemoryOrder::Relaxed: {
        return _PlatformAtomicDetails::AtomicDecRelaxed(pAtomic);
    }
    }

    ALT_UNREACHABLE();
}

template <typename T, typename U,
    std::enable_if_t<
        std::is_convertible_v<U, T> &&
        BitFlag::IsSetEnumFlags(_PlatformAtomicDetails::supported_operations_v<T>, _PlatformAtomicDetails::SupportedOperationsTrait::And)
    >* = nullptr>
ALT_FORCE_INLINE T AtomicAnd(T* pAtomic, const U value, const MemoryOrder order = MemoryOrder::SeqCst) noexcept {

    switch (order) {
    case MemoryOrder::SeqCst: {
        return _PlatformAtomicDetails::AtomicAndSeqCst(pAtomic, static_cast<T>(value));
    }
    case MemoryOrder::AcqRel: {
        return _PlatformAtomicDetails::AtomicAndAcqRel(pAtomic, static_cast<T>(value));
    }
    case MemoryOrder::Acquire: {
        return _PlatformAtomicDetails::AtomicAndAcquire(pAtomic, static_cast<T>(value));
    }
    case MemoryOrder::Release: {
        return _PlatformAtomicDetails::AtomicAndRelease(pAtomic, static_cast<T>(value));
    }
    case MemoryOrder::Relaxed: {
        return _PlatformAtomicDetails::AtomicAndRelaxed(pAtomic, static_cast<T>(value));
    }
    }

    ALT_UNREACHABLE();
}

template <typename T, typename U,
    std::enable_if_t<
        std::is_convertible_v<U, T> &&
        BitFlag::IsSetEnumFlags(_PlatformAtomicDetails::supported_operations_v<T>, _PlatformAtomicDetails::SupportedOperationsTrait::Or)
    >* = nullptr>
ALT_FORCE_INLINE T AtomicOr(T* pAtomic, const U value, const MemoryOrder order = MemoryOrder::SeqCst) noexcept {

    switch (order) {
    case MemoryOrder::SeqCst: {
        return _PlatformAtomicDetails::AtomicOrSeqCst(pAtomic, static_cast<T>(value));
    }
    case MemoryOrder::AcqRel: {
        return _PlatformAtomicDetails::AtomicOrAcqRel(pAtomic, static_cast<T>(value));
    }
    case MemoryOrder::Acquire: {
        return _PlatformAtomicDetails::AtomicOrAcquire(pAtomic, static_cast<T>(value));
    }
    case MemoryOrder::Release: {
        return _PlatformAtomicDetails::AtomicOrRelease(pAtomic, static_cast<T>(value));
    }
    case MemoryOrder::Relaxed: {
        return _PlatformAtomicDetails::AtomicOrRelaxed(pAtomic, static_cast<T>(value));
    }
    }

    ALT_UNREACHABLE();
}

template <typename T, typename U,
    std::enable_if_t<
        std::is_convertible_v<U, T> &&
        BitFlag::IsSetEnumFlags(_PlatformAtomicDetails::supported_operations_v<T>, _PlatformAtomicDetails::SupportedOperationsTrait::Xor)
    >* = nullptr>
ALT_FORCE_INLINE T AtomicXor(T* pAtomic, const U value, const MemoryOrder order = MemoryOrder::SeqCst) noexcept {

    switch (order) {
    case MemoryOrder::SeqCst: {
        return _PlatformAtomicDetails::AtomicXorSeqCst(pAtomic, static_cast<T>(value));
    }
    case MemoryOrder::AcqRel: {
        return _PlatformAtomicDetails::AtomicXorAcqRel(pAtomic, static_cast<T>(value));
    }
    case MemoryOrder::Acquire: {
        return _PlatformAtomicDetails::AtomicXorAcquire(pAtomic, static_cast<T>(value));
    }
    case MemoryOrder::Release: {
        return _PlatformAtomicDetails::AtomicXorRelease(pAtomic, static_cast<T>(value));
    }
    case MemoryOrder::Relaxed: {
        return _PlatformAtomicDetails::AtomicXorRelaxed(pAtomic, static_cast<T>(value));
    }
    }

    ALT_UNREACHABLE();
}

ALT_NODISCARD ALT_FORCE_INLINE Result<Handle, Error> ThreadCreate(
    FuncPtr<u32, void*> pFunc,
    void* pArgs,
    const Optional<usize> stackSize = Nullopt
) noexcept {

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    return instance.ThreadCreate(pFunc, pArgs, stackSize);
}

ALT_NODISCARD ALT_FORCE_INLINE Result<void, Error> ThreadClose(const Handle handle) noexcept {

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    return instance.ThreadClose(handle);
}

ALT_NODISCARD ALT_FORCE_INLINE Handle ThreadGetCurrentPseudoHandle() noexcept {

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    return instance.ThreadGetCurrentPseudoHandle();
}

ALT_NODISCARD ALT_FORCE_INLINE u32 ThreadGetCurrentID() noexcept {

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    return instance.ThreadGetCurrentID();
}

ALT_NODISCARD ALT_FORCE_INLINE u32 ThreadGetIDByHandle(const Handle handle) noexcept {

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    return (*instance.pGetThreadIDByHandle)(instance, handle);
}

ALT_FORCE_INLINE bool ThreadSwitchToAnother() noexcept {

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    return instance.ThreadSwitchToAnother();
}

ALT_NODISCARD ALT_FORCE_INLINE Result<void, Error> ThreadSetPriorityBoost(const Handle handle, const bool isBoostEnabled) noexcept {

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    return instance.ThreadSetPriorityBoost(handle, isBoostEnabled);
}

ALT_NODISCARD ALT_FORCE_INLINE Result<bool, Error> ThreadGetPriorityBoost(const Handle handle) noexcept {

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    return instance.ThreadGetPriorityBoost(handle);
}

ALT_NODISCARD ALT_FORCE_INLINE Result<void, Error> ThreadSetPriority(const Handle handle, const ThreadPriority priority) noexcept {

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    return instance.ThreadSetPriority(handle, priority);
}

ALT_NODISCARD ALT_FORCE_INLINE ThreadPriority ThreadGetPriority(const Handle handle) noexcept {

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    return instance.ThreadGetPriority(handle);
}

ALT_NODISCARD ALT_FORCE_INLINE Result<void, Error> ThreadSetAffinityMask(const Handle handle, const u64 mask) noexcept {

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    return instance.ThreadSetAffinityMask(handle, mask);
}

inline Result<void, Error> ThreadSetName(const Handle handle, const Slice<const char> name) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offset = instance.Utf16BufferAppend(name, _PlatformDetails::Utf16BufferAppendBits::None) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offset); };

    return instance.ThreadSetName(handle, instance.Utf16BufferPtr(offset));
}

inline Result<void, Error> ThreadSetName(const Handle handle, const char* pName) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offset = instance.Utf16BufferAppend(pName, _PlatformDetails::Utf16BufferAppendBits::None) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offset); };

    return instance.ThreadSetName(handle, instance.Utf16BufferPtr(offset));
}

inline Result<void, Error> ThreadGetNameAppend(const Handle handle, String& buffer) noexcept {

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    return instance.ThreadGetNameAppend(handle, buffer);
}

inline Result<void, Error> ThreadGetName(const Handle handle, String& buffer) noexcept {

    buffer.Clear();
    return ThreadGetNameAppend(handle, buffer);
}

ALT_NODISCARD inline Result<String, Error> ThreadGetName(const Handle handle) noexcept {
    iferr_scope;

    Error err;
    String buffer;
    ThreadGetNameAppend(handle, buffer) iferr_return_with(err) err;

    return buffer;
}

ALT_NODISCARD ALT_FORCE_INLINE constexpr usize SemaphoreGetMax() noexcept {

    return _PlatformDetails::PlatformInstance::SemaphoreGetMax();
}

ALT_NODISCARD ALT_FORCE_INLINE Result<Handle, Error> SemaphoreCreate(const u32 valInit = 0, const u32 valMax = static_cast<u32>(SemaphoreGetMax())) noexcept {

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    return instance.SemaphoreCreate(valInit, valMax);
}

ALT_NODISCARD ALT_FORCE_INLINE Result<void, Error> SemaphoreClose(const Handle handle) noexcept {

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    return instance.SemaphoreClose(handle);
}

ALT_NODISCARD ALT_FORCE_INLINE Result<void, Error> SemaphoreRelease(const Handle handle, const u32 inc) noexcept {

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    return instance.SemaphoreRelease(handle, inc);
}

ALT_NODISCARD ALT_FORCE_INLINE CriticalSection CriticalSectionCreate() noexcept {

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    return instance.CriticalSectionCreate();
}

ALT_FORCE_INLINE void CriticalSectionDelete(CriticalSection& critsect) noexcept {

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    instance.CriticalSectionDelete(critsect);
}

ALT_FORCE_INLINE void CriticalSectionEnter(CriticalSection& critsect) noexcept {

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    instance.CriticalSectionEnter(critsect);
}

ALT_NODISCARD ALT_FORCE_INLINE bool CriticalSectionTryEnter(CriticalSection& critsect) noexcept {

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    return instance.CriticalSectionTryEnter(critsect);
}

ALT_FORCE_INLINE void CriticalSectionLeave(CriticalSection& critsect) noexcept {

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    instance.CriticalSectionLeave(critsect);
}

ALT_NODISCARD ALT_FORCE_INLINE bool IsRWMutexSupported() noexcept {

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    return instance.IsRWMutexSupported();
}

ALT_FORCE_INLINE void RWMutexLock(RWMutex& mutex) noexcept {

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    instance.RWMutexLock(mutex);
}

ALT_FORCE_INLINE void RWMutexLockRead(RWMutex& mutex) noexcept {

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    instance.RWMutexLockRead(mutex);
}

ALT_FORCE_INLINE bool RWMutexTryLock(RWMutex& mutex) noexcept {

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    return instance.RWMutexTryLock(mutex);
}

ALT_FORCE_INLINE bool RWMutexTryLockRead(RWMutex& mutex) noexcept {

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    return instance.RWMutexTryLockRead(mutex);
}

ALT_FORCE_INLINE void RWMutexUnlock(RWMutex& mutex) noexcept {

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    instance.RWMutexUnlock(mutex);
}

ALT_FORCE_INLINE void RWMutexUnlockRead(RWMutex& mutex) noexcept {

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    instance.RWMutexUnlockRead(mutex);
}

ALT_NODISCARD ALT_FORCE_INLINE bool IsWaitWakeOnAdressSupported() noexcept {

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    return instance.IsWaitWakeOnAdressSupported();
}

ALT_NODISCARD ALT_FORCE_INLINE Result<WaitOnAdressResult, Error> WaitOnAdress(
    const void* pAddress,
    const void* pCompare,
    const usize sizeInBytes,
    const u32 milliseconds
) noexcept {

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    return instance.WaitOnAdress(pAddress, pCompare, sizeInBytes, milliseconds);
}

ALT_NODISCARD ALT_FORCE_INLINE Result<void, Error> WakeByAddressSingle(const void* pAddress) noexcept {

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    return instance.WakeByAddressSingle(pAddress);
}

ALT_NODISCARD ALT_FORCE_INLINE Result<void, Error> WakeByAddressAll(const void* pAddress) noexcept {

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    return instance.WakeByAddressAll(pAddress);
}

ALT_NODISCARD ALT_FORCE_INLINE Result<WaitForSingleObjectResult, Error> WaitForSingleObject(
    const Handle handle,
    const u32 milliseconds
) noexcept {

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    return instance.WaitForSingleObject(handle, milliseconds);
}

ALT_NODISCARD inline Result<void, Error> ConsoleWrite(const _PlatformDetails::StdIo stdio, const Slice<const char16_t> data) noexcept {

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    return instance.ConsoleWrite(stdio, reinterpret_cast<const wchar_t*>(data.Data()), data.GetSize());
}

ALT_NODISCARD inline Result<void, Error> ConsoleWrite(const _PlatformDetails::StdIo stdio, const Slice<const wchar_t> data) noexcept {

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    return instance.ConsoleWrite(stdio, data.Data(), data.GetSize());
}

ALT_NODISCARD inline Result<void, Error> ConsoleWrite(const _PlatformDetails::StdIo stdio, const Slice<const char> data) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offset = instance.Utf16BufferAppend(data, _PlatformDetails::Utf16BufferAppendBits::None) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offset); };

    return instance.ConsoleWrite(stdio, instance.Utf16BufferPtr(offset), instance.Utf16BufferSizeAssumeLast(offset, _PlatformDetails::Utf16BufferAppendBits::None));
}

ALT_NODISCARD inline Result<void, Error> ConsoleWrite(const _PlatformDetails::StdIo stdio, const char* pData) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offset = instance.Utf16BufferAppend(pData, _PlatformDetails::Utf16BufferAppendBits::None) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offset); };

    return instance.ConsoleWrite(stdio, instance.Utf16BufferPtr(offset), instance.Utf16BufferSizeAssumeLast(offset, _PlatformDetails::Utf16BufferAppendBits::None));
}

ALT_NODISCARD inline Result<void, Error> ConsoleReadAppend(String& buffer) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    Error err;
    const _PlatformDetails::Utf16BufferOffset bufferOffset = instance.ConsoleRead() iferr_return_with(err) err;
    defer{ instance.Utf16BufferDeferGuard(bufferOffset.GetOffset()); };

    bufferOffset.CloneIntoAppend(buffer) iferr_return Error::MemoryAllocation();

    return Ok;
}

ALT_NODISCARD inline Result<void, Error> ConsoleRead(String& buffer) noexcept {

    buffer.Clear();
    return ConsoleReadAppend(buffer);
}

ALT_NODISCARD inline Result<String, Error> ConsoleRead() noexcept {
    iferr_scope;

    Error err;
    String buffer;
    ConsoleReadAppend(buffer) iferr_return_with(err) err;

    return buffer;
}

ALT_NODISCARD inline Result<void, Error> ConsoleGetTitleAppend(String& buffer) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    Error err;
    const _PlatformDetails::Utf16BufferOffset bufferOffset = instance.ConsoleGetTitle() iferr_return_with(err) err;
    defer{ instance.Utf16BufferDeferGuard(bufferOffset.GetOffset()); };

    bufferOffset.CloneIntoAppend(buffer) iferr_return Error::MemoryAllocation();

    return Ok;
}

ALT_NODISCARD inline Result<void, Error> ConsoleGetTitle(String& buffer) noexcept {

    buffer.Clear();
    return ConsoleGetTitleAppend(buffer);
}

ALT_NODISCARD inline Result<String, Error> ConsoleGetTitle() noexcept {
    iferr_scope;

    Error err;
    String title;
    ConsoleGetTitleAppend(title) iferr_return_with(err) err;

    return title;
}

ALT_NODISCARD inline Result<void, Error> ConsoleSetTitle(const Slice<const char> title) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offset = instance.Utf16BufferAppend(title, _PlatformDetails::Utf16BufferAppendBits::None) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offset); };

    return instance.ConsoleSetTitle(instance.Utf16BufferPtr(offset));
}

ALT_NODISCARD inline Result<void, Error> ConsoleSetTitle(const char* pTitle) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offset = instance.Utf16BufferAppend(pTitle, _PlatformDetails::Utf16BufferAppendBits::None) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offset); };

    return instance.ConsoleSetTitle(instance.Utf16BufferPtr(offset));
}

ALT_NODISCARD inline Result<Pair<usize, usize>, Error> ConsoleGetSizeInChars() noexcept {

    return _PlatformDetails::GetPlatformInstance().ConsoleGetSizeInChars();
}

ALT_NODISCARD inline Result<void, Error> GetEnvironmentVariableAppend(const Slice<const char> name, String& var) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offset = instance.Utf16BufferAppend(name, _PlatformDetails::Utf16BufferAppendBits::None) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offset); };

    Error err;
    const _PlatformDetails::Utf16BufferOffset bufferOffset = instance.GetEnvironmentVariable(offset) iferr_return_with(err) err;

    bufferOffset.CloneIntoAppend(var) iferr_return Error::MemoryAllocation();

    return Ok;
}

ALT_NODISCARD inline Result<void, Error> GetEnvironmentVariable(const Slice<const char> name, String& var) noexcept {

    var.Clear();
    return GetEnvironmentVariableAppend(name, var);
}

ALT_NODISCARD inline Result<String, Error> GetEnvironmentVariable(const Slice<const char> name) noexcept {
    iferr_scope;

    Error err;
    String var;
    GetEnvironmentVariableAppend(name, var) iferr_return_with(err) err;

    return var;
}

ALT_NODISCARD inline Result<void, Error> GetEnvironmentVariableAppend(const char* pName, String& var) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offset = instance.Utf16BufferAppend(pName, _PlatformDetails::Utf16BufferAppendBits::None) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offset); };

    Error err;
    const _PlatformDetails::Utf16BufferOffset bufferOffset = instance.GetEnvironmentVariable(offset) iferr_return_with(err) err;

    bufferOffset.CloneIntoAppend(var) iferr_return Error::MemoryAllocation();

    return Ok;
}

ALT_NODISCARD inline Result<void, Error> GetEnvironmentVariable(const char* pName, String& var) noexcept {

    var.Clear();
    return GetEnvironmentVariableAppend(pName, var);
}

ALT_NODISCARD inline Result<String, Error> GetEnvironmentVariable(const char* pName) noexcept {
    iferr_scope;

    Error err;
    String var;
    GetEnvironmentVariableAppend(pName, var) iferr_return_with(err) err;

    return var;
}

class FileLogicalDriversIterator final : public Iterator<FileLogicalDriversIterator, char, IIteratorForwardTrait, IIteratorBackwardTrait, IIteratorExactSizeTrait> {
public:

    ALT_FORCE_INLINE FileLogicalDriversIterator(const usize driversBits) noexcept
        : m_DriversBits(driversBits)
    {}

    ALT_FORCE_INLINE usize GetSizeHint() const noexcept {
        return GetSize();
    }

    ALT_FORCE_INLINE usize GetSize() const noexcept {
        return Num::Popcount(m_DriversBits);
    }

    ALT_FORCE_INLINE bool IsEmpty() const noexcept {
        return m_DriversBits == 0;
    }

    ALT_FORCE_INLINE Optional<char> Next() noexcept {

        if (m_DriversBits == 0) {
            return Nullopt;
        }

        const usize idx = Num::Count0BitsRight(m_DriversBits);
        m_DriversBits &= ~(static_cast<usize>(1) << idx);

        return static_cast<char>(static_cast<u8>('A') + static_cast<u8>(idx));
    }

    ALT_FORCE_INLINE usize Advance(const usize advance) noexcept {

        usize result = 0;
        for (;;) {

            if (result == advance) {
                break;
            }

            Optional<char> driver = Next();
            if (!driver){
                break;
            }

            result += 1;
        }

        return result;
    }

    ALT_FORCE_INLINE Optional<char> NextBack() noexcept {

        if (m_DriversBits == 0) {
            return Nullopt;
        }

        const usize idx = (Num::Desc<usize>::Bits() - 1) - Num::Count0BitsLeft(m_DriversBits);
        m_DriversBits &= ~(static_cast<usize>(1) << idx);

        return static_cast<char>(static_cast<u8>('A') + static_cast<u8>(idx));
    }

    ALT_FORCE_INLINE usize AdvanceBack(const usize advance) noexcept {

        usize result = 0;
        for (;;) {

            if (result == advance) {
                break;
            }

            Optional<char> driver = NextBack();
            if (!driver){
                break;
            }

            result += 1;
        }

        return result;
    }

private:

    usize m_DriversBits = 0;
};

ALT_NODISCARD ALT_FORCE_INLINE FileLogicalDriversIterator FileGetLogicalDriversIterator() noexcept {

    const _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    return FileLogicalDriversIterator(instance.FileGetLogicalDriversBits());
}

ALT_NODISCARD inline Result<Handle, Error> FileOpenOrCreate(
    const Slice<const char> filepath,
    const FileOpenOrCreateBits createBits,
    const FileShareBits shareBits,
    const FileAccessBits accessBits,
    const FileHintBits hintBits
) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offset = instance.Utf16BufferAppend(filepath, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offset); };

    return instance.FileOpenOrCreate(
        instance.Utf16BufferPtr(offset),
        instance.Utf16BufferSizeAssumeLast(offset, _PlatformDetails::Utf16BufferAppendBits::PathBuffer),
        createBits,
        shareBits,
        accessBits,
        hintBits
    );
}

ALT_NODISCARD inline Result<Handle, Error> FileOpenOrCreate(
    const char* pFilepath,
    const FileOpenOrCreateBits createBits,
    const FileShareBits shareBits,
    const FileAccessBits accessBits,
    const FileHintBits hintBits
) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offset = instance.Utf16BufferAppend(pFilepath, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offset); };

    return instance.FileOpenOrCreate(
        instance.Utf16BufferPtr(offset),
        instance.Utf16BufferSizeAssumeLast(offset, _PlatformDetails::Utf16BufferAppendBits::PathBuffer),
        createBits,
        shareBits,
        accessBits,
        hintBits
    );
}

ALT_NODISCARD inline Result<Handle, Error> FileOpenOrCreate(
    const PathOffset pathOffset,
    const FileOpenOrCreateBits createBits,
    const FileShareBits shareBits,
    const FileAccessBits accessBits,
    const FileHintBits hintBits
) noexcept {

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    return instance.FileOpenOrCreate(
        instance.Utf16BufferPtr(pathOffset.GetOffset()),
        instance.Utf16BufferSizeAssumeLast(pathOffset.GetOffset(), _PlatformDetails::Utf16BufferAppendBits::PathBuffer),
        createBits,
        shareBits,
        accessBits,
        hintBits
    );
}

ALT_NODISCARD ALT_FORCE_INLINE Result<Handle, Error> FileCreateMapMemoryObject(
    const Handle handle,
    const FileCreateMapMemoryObjectBits bits,
    const u64 sizeInBytes
) noexcept {

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    return instance.FileCreateMapMemoryObject(handle, bits, sizeInBytes);
}

ALT_NODISCARD ALT_FORCE_INLINE Result<u8*, Error> FileMapMemory(
    const Handle handle,
    const FileMapMemoryBits bits,
    const u64 offset,
    const usize sizeInBytes
) noexcept {

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    return instance.FileMapMemory(handle, bits, offset, sizeInBytes);
}

ALT_NODISCARD ALT_FORCE_INLINE Result<void, Error> FileUnmapMemory(const void* pBaseMemory) noexcept {

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    return instance.FileUnmapMemory(pBaseMemory);
}

ALT_NODISCARD ALT_FORCE_INLINE Result<void, Error> FileFlushMappedMemory(const void* pBaseMemory, const usize sizeInBytes) noexcept {

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    return instance.FileFlushMappedMemory(pBaseMemory, sizeInBytes);
}

ALT_NODISCARD ALT_FORCE_INLINE Result<void, Error> FileFlush(const Handle handle) noexcept {

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    return instance.FileFlush(handle);
}

ALT_NODISCARD ALT_FORCE_INLINE Result<void, Error> FileClose(const Handle handle) noexcept {

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    return instance.FileClose(handle);
}

ALT_NODISCARD inline Result<void, Error> FileDelete(const Slice<const char> filepath) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offset = instance.Utf16BufferAppend(filepath, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offset); };

    return instance.FileDelete(instance.Utf16BufferPtr(offset));
}

ALT_NODISCARD inline Result<void, Error> FileDelete(const char* pFilepath) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offset = instance.Utf16BufferAppend(pFilepath, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offset); };

    return instance.FileDelete(instance.Utf16BufferPtr(offset));
}

ALT_NODISCARD ALT_FORCE_INLINE Result<void, Error> FileDelete(const PathOffset pathOffset) noexcept {

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    return instance.FileDelete(instance.Utf16BufferPtr(pathOffset.GetOffset()));
}

ALT_NODISCARD ALT_FORCE_INLINE Result<void, Error> FileDelete(const Handle handle) noexcept {

    const _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    return (*instance.pFileDeleteByHandle)(instance, handle);
}

ALT_NODISCARD inline Result<void, Error> FileWrite(const Handle handle, const u8* pData, const usize size) noexcept {

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    return instance.FileWrite(handle, pData, size);
}

ALT_NODISCARD inline Result<usize, Error> FileRead(const Handle handle, u8* pData, const usize size) noexcept {

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    return instance.FileRead(handle, pData, size);
}

ALT_NODISCARD ALT_FORCE_INLINE Result<usize, Error> FileSize(const Handle handle) noexcept {

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    return instance.FileSize(handle);
}

ALT_NODISCARD ALT_FORCE_INLINE Result<usize, Error> FileSeek(const Handle handle, const FileSeekOpt opt) noexcept {

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    return instance.FileSeek(handle, opt);
}

ALT_NODISCARD ALT_FORCE_INLINE Result<void, Error> FileClear(const Handle handle) noexcept {

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    return instance.FileClear(handle);
}

ALT_NODISCARD ALT_FORCE_INLINE Result<void, Error> FileResize(const Handle handle, const usize sizeInBytes) noexcept {

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    return instance.FileResize(handle, sizeInBytes);
}

ALT_NODISCARD inline Result<void, Error> FileGetTempDirectoryAppend(String& path) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    Error err;
    const _PlatformDetails::Utf16BufferOffset bufferOffset = instance.FileGetTempDirectory() iferr_return_with(err) err;
    defer{ instance.Utf16BufferDeferGuard(bufferOffset.GetOffset()); };

    bufferOffset.CloneIntoAppend(path) iferr_return Error::MemoryAllocation();

    return Ok;
}

ALT_NODISCARD inline Result<void, Error> FileGetTempDirectory(String& path) noexcept {

    path.Clear();
    return FileGetTempDirectoryAppend(path);
}

ALT_NODISCARD inline Result<String, Error> FileGetTempDirectory() noexcept {
    iferr_scope;

    Error err;
    String path;
    FileGetTempDirectoryAppend(path) iferr_return_with(err) err;

    return path;
}

ALT_NODISCARD inline Result<void, Error> FileGetCurrentDirectoryAppend(String& path) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    Error err;
    const _PlatformDetails::Utf16BufferOffset bufferOffset = instance.FileGetCurrentDirectory() iferr_return_with(err) err;
    defer{ instance.Utf16BufferDeferGuard(bufferOffset.GetOffset()); };

    bufferOffset.CloneIntoAppend(path) iferr_return Error::MemoryAllocation();

    return Ok;
}

ALT_NODISCARD inline Result<void, Error> FileGetCurrentDirectory(String& path) noexcept {

    path.Clear();
    return FileGetCurrentDirectoryAppend(path);
}

ALT_NODISCARD inline Result<String, Error> FileGetCurrentDirectory() noexcept {
    iferr_scope;

    Error err;
    String path;
    FileGetCurrentDirectoryAppend(path) iferr_return_with(err) err;

    return path;
}

ALT_NODISCARD inline FileTypeBits FileGetFileType(const Handle handle) noexcept {

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    return instance.FileGetFileType(handle);
}

ALT_NODISCARD inline Result<FileTypeBits, Error> FileGetFileType(const Slice<const char> filepath) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offset = instance.Utf16BufferAppend(filepath, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offset); };

    return instance.FileGetFileType(instance.Utf16BufferPtr(offset));
}

ALT_NODISCARD inline Result<FileTypeBits, Error> FileGetFileType(const char* pFilepath) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offset = instance.Utf16BufferAppend(pFilepath, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offset); };

    return instance.FileGetFileType(instance.Utf16BufferPtr(offset));
}

ALT_NODISCARD inline Result<FileTypeBits, Error> FileGetFileType(const PathOffset pathOffset) noexcept {

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    return instance.FileGetFileType(instance.Utf16BufferPtr(pathOffset.GetOffset()));
}

ALT_NODISCARD ALT_FORCE_INLINE Result<FileTypeBits, Error> FileGetType(const Handle handle, const bool isTryGetFileTypeByHandle = false) noexcept {

    const _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    return (*instance.pFileGetTypeByHandle)(instance, handle, isTryGetFileTypeByHandle);
}

ALT_NODISCARD inline Result<FileTypeBits, Error> FileGetType(const Slice<const char> filepath, const bool isTryGetFileTypeByHandle = false) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offset = instance.Utf16BufferAppend(filepath, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offset); };

    return (*instance.pFileGetType)(instance, instance.Utf16BufferPtr(offset), isTryGetFileTypeByHandle);
}

ALT_NODISCARD inline Result<FileTypeBits, Error> FileGetType(const char* pFilepath, const bool isTryGetFileTypeByHandle = false) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offset = instance.Utf16BufferAppend(pFilepath, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offset); };

    return (*instance.pFileGetType)(instance, instance.Utf16BufferPtr(offset), isTryGetFileTypeByHandle);
}

ALT_NODISCARD ALT_FORCE_INLINE Result<FileTypeBits, Error> FileGetType(const PathOffset pathOffset, const bool isTryGetFileTypeByHandle = false) noexcept {

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    return (*instance.pFileGetType)(instance, instance.Utf16BufferPtr(pathOffset.GetOffset()), isTryGetFileTypeByHandle);
}

ALT_NODISCARD inline Result<void, Error> FileDirectoryCreate(const Slice<const char> filepath) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offset = instance.Utf16BufferAppend(filepath, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offset); };

    return instance.FileDirectoryCreate(instance.Utf16BufferPtr(offset), instance.Utf16BufferSizeAssumeLast(offset, _PlatformDetails::Utf16BufferAppendBits::PathBuffer));
}

ALT_NODISCARD inline Result<void, Error> FileDirectoryCreate(const char* pFilepath) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offset = instance.Utf16BufferAppend(pFilepath, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offset); };

    return instance.FileDirectoryCreate(instance.Utf16BufferPtr(offset), instance.Utf16BufferSizeAssumeLast(offset, _PlatformDetails::Utf16BufferAppendBits::PathBuffer));
}

ALT_NODISCARD inline bool FileRemoveAll(const Slice<const char> filepath, const FileRemoveAllType type) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    FileForEachDirectoryEntryDesc desc{};

    const _PlatformDetails::DirectoryEntrySizeResult directoryEntrySizeResult =
        instance.FileForEachDirectoryEntry_GetDirectoryEntrySize(desc.sizeInBytesTmpData);

    const usize offset = instance.Utf16BufferAppend(filepath, _PlatformDetails::Utf16BufferAppendBits::PathBuffer, directoryEntrySizeResult.sizeToAllocate)
        iferr_return false;

    defer{ instance.Utf16BufferDeferGuard(offset); };

    return instance.FileRemoveAll(offset, offset + directoryEntrySizeResult.sizeToAllocate, directoryEntrySizeResult.sizeInBytesFileDirInfo, directoryEntrySizeResult, type);
}

ALT_NODISCARD inline bool FileRemoveAll(const char* pFilepath, const FileRemoveAllType type) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    FileForEachDirectoryEntryDesc desc{};

    const _PlatformDetails::DirectoryEntrySizeResult directoryEntrySizeResult =
        instance.FileForEachDirectoryEntry_GetDirectoryEntrySize(desc.sizeInBytesTmpData);

    const usize offset = instance.Utf16BufferAppend(pFilepath, _PlatformDetails::Utf16BufferAppendBits::PathBuffer, directoryEntrySizeResult.sizeToAllocate)
        iferr_return false;

    defer{ instance.Utf16BufferDeferGuard(offset); };

    return instance.FileRemoveAll(offset, offset + directoryEntrySizeResult.sizeToAllocate, directoryEntrySizeResult.sizeInBytesFileDirInfo, directoryEntrySizeResult, type);
}

ALT_NODISCARD inline bool FileRemoveAll(const PathOffset pathOffset, const FileRemoveAllType type) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    FileForEachDirectoryEntryDesc desc{};

    const _PlatformDetails::DirectoryEntrySizeResult directoryEntrySizeResult =
        instance.FileForEachDirectoryEntry_GetDirectoryEntrySize(desc.sizeInBytesTmpData);

    const usize offset = instance.Utf16BufferAppend(pathOffset, directoryEntrySizeResult.sizeToAllocate)
        iferr_return false;

    defer{ instance.Utf16BufferDeferGuard(offset); };

    return instance.FileRemoveAll(offset, offset + directoryEntrySizeResult.sizeToAllocate, directoryEntrySizeResult.sizeInBytesFileDirInfo, directoryEntrySizeResult, type);
}

ALT_NODISCARD inline Result<void, Error> FileCopyFile(const Slice<const char> filepathFrom, const Slice<const char> filepathTo) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offsetFrom = instance.Utf16BufferAppend(filepathFrom, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offsetFrom); };

    const usize offsetTo = instance.Utf16BufferAppend(filepathTo, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();

    return instance.FileCopyFile(instance.Utf16BufferPtr(offsetFrom), instance.Utf16BufferPtr(offsetTo));
}

ALT_NODISCARD inline Result<void, Error> FileCopyFile(const char* pFilepathFrom, const Slice<const char> filepathTo) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offsetFrom = instance.Utf16BufferAppend(pFilepathFrom, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offsetFrom); };

    const usize offsetTo = instance.Utf16BufferAppend(filepathTo, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();

    return instance.FileCopyFile(instance.Utf16BufferPtr(offsetFrom), instance.Utf16BufferPtr(offsetTo));
}

ALT_NODISCARD inline Result<void, Error> FileCopyFile(const PathOffset pathOffsetFrom, const Slice<const char> filepathTo) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offsetTo = instance.Utf16BufferAppend(filepathTo, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offsetTo); };

    return instance.FileCopyFile(instance.Utf16BufferPtr(pathOffsetFrom.GetOffset()), instance.Utf16BufferPtr(offsetTo));
}

ALT_NODISCARD inline Result<void, Error> FileCopyFile(const Slice<const char> filepathFrom, const char* pFilepathTo) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offsetFrom = instance.Utf16BufferAppend(filepathFrom, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offsetFrom); };

    const usize offsetTo = instance.Utf16BufferAppend(pFilepathTo, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();

    return instance.FileCopyFile(instance.Utf16BufferPtr(offsetFrom), instance.Utf16BufferPtr(offsetTo));
}

ALT_NODISCARD inline Result<void, Error> FileCopyFile(const char* pFilepathFrom, const char* pFilepathTo) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offsetFrom = instance.Utf16BufferAppend(pFilepathFrom, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offsetFrom); };

    const usize offsetTo = instance.Utf16BufferAppend(pFilepathTo, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();

    return instance.FileCopyFile(instance.Utf16BufferPtr(offsetFrom), instance.Utf16BufferPtr(offsetTo));
}

ALT_NODISCARD inline Result<void, Error> FileCopyFile(const PathOffset pathOffsetFrom, const char* pFilepathTo) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offsetTo = instance.Utf16BufferAppend(pFilepathTo, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offsetTo); };

    return instance.FileCopyFile(instance.Utf16BufferPtr(pathOffsetFrom.GetOffset()), instance.Utf16BufferPtr(offsetTo));
}

ALT_NODISCARD inline Result<void, Error> FileCopyFile(const Slice<const char> filepathFrom, const PathOffset pathOffsetTo) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offsetFrom = instance.Utf16BufferAppend(filepathFrom, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offsetFrom); };

    return instance.FileCopyFile(instance.Utf16BufferPtr(offsetFrom), instance.Utf16BufferPtr(pathOffsetTo.GetOffset()));
}

ALT_NODISCARD inline Result<void, Error> FileCopyFile(const char* pFilepathFrom, const PathOffset pathOffsetTo) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offsetFrom = instance.Utf16BufferAppend(pFilepathFrom, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offsetFrom); };

    return instance.FileCopyFile(instance.Utf16BufferPtr(offsetFrom), instance.Utf16BufferPtr(pathOffsetTo.GetOffset()));
}

ALT_NODISCARD ALT_FORCE_INLINE Result<void, Error> FileCopyFile(const PathOffset pathOffsetFrom, const PathOffset pathOffsetTo) noexcept {

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    return instance.FileCopyFile(instance.Utf16BufferPtr(pathOffsetFrom.GetOffset()), instance.Utf16BufferPtr(pathOffsetTo.GetOffset()));
}

ALT_NODISCARD inline Result<void, Error> FileMoveAny(const Slice<const char> filepathFrom, const Slice<const char> filepathTo) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offsetFrom = instance.Utf16BufferAppend(filepathFrom, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offsetFrom); };

    const usize offsetTo = instance.Utf16BufferAppend(filepathTo, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();

    return instance.FileMoveAny(instance.Utf16BufferPtr(offsetFrom), instance.Utf16BufferPtr(offsetTo));
}

ALT_NODISCARD inline Result<void, Error> FileMoveAny(const char* pFilepathFrom, const Slice<const char> filepathTo) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offsetFrom = instance.Utf16BufferAppend(pFilepathFrom, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offsetFrom); };

    const usize offsetTo = instance.Utf16BufferAppend(filepathTo, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();

    return instance.FileMoveAny(instance.Utf16BufferPtr(offsetFrom), instance.Utf16BufferPtr(offsetTo));
}

ALT_NODISCARD inline Result<void, Error> FileMoveAny(const PathOffset pathOffsetFrom, const Slice<const char> filepathTo) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offsetTo = instance.Utf16BufferAppend(filepathTo, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offsetTo); };

    return instance.FileMoveAny(instance.Utf16BufferPtr(pathOffsetFrom.GetOffset()), instance.Utf16BufferPtr(offsetTo));
}

ALT_NODISCARD inline Result<void, Error> FileMoveAny(const Slice<const char> filepathFrom, const char* pFilepathTo) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offsetFrom = instance.Utf16BufferAppend(filepathFrom, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offsetFrom); };

    const usize offsetTo = instance.Utf16BufferAppend(pFilepathTo, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();

    return instance.FileMoveAny(instance.Utf16BufferPtr(offsetFrom), instance.Utf16BufferPtr(offsetTo));
}

ALT_NODISCARD inline Result<void, Error> FileMoveAny(const char* pFilepathFrom, const char* pFilepathTo) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offsetFrom = instance.Utf16BufferAppend(pFilepathFrom, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offsetFrom); };

    const usize offsetTo = instance.Utf16BufferAppend(pFilepathTo, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();

    return instance.FileMoveAny(instance.Utf16BufferPtr(offsetFrom), instance.Utf16BufferPtr(offsetTo));
}

ALT_NODISCARD inline Result<void, Error> FileMoveAny(const PathOffset pathOffsetFrom, const char* pFilepathTo) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offsetTo = instance.Utf16BufferAppend(pFilepathTo, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offsetTo); };

    return instance.FileMoveAny(instance.Utf16BufferPtr(pathOffsetFrom.GetOffset()), instance.Utf16BufferPtr(offsetTo));
}

ALT_NODISCARD inline Result<void, Error> FileMoveAny(const Slice<const char> filepathFrom, const PathOffset pathOffsetTo) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offsetFrom = instance.Utf16BufferAppend(filepathFrom, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offsetFrom); };

    return instance.FileMoveAny(instance.Utf16BufferPtr(offsetFrom), instance.Utf16BufferPtr(pathOffsetTo.GetOffset()));
}

ALT_NODISCARD inline Result<void, Error> FileMoveAny(const char* pFilepathFrom, const PathOffset pathOffsetTo) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offsetFrom = instance.Utf16BufferAppend(pFilepathFrom, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offsetFrom); };

    return instance.FileMoveAny(instance.Utf16BufferPtr(offsetFrom), instance.Utf16BufferPtr(pathOffsetTo.GetOffset()));
}

ALT_NODISCARD ALT_FORCE_INLINE Result<void, Error> FileMoveAny(const PathOffset pathOffsetFrom, const PathOffset pathOffsetTo) noexcept {

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    return instance.FileMoveAny(instance.Utf16BufferPtr(pathOffsetFrom.GetOffset()), instance.Utf16BufferPtr(pathOffsetTo.GetOffset()));
}

ALT_NODISCARD inline Result<bool, Error> FileExistAny(const Slice<const char> filepath) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offset = instance.Utf16BufferAppend(filepath, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offset); };

    return instance.FileExistAny(instance.Utf16BufferPtr(offset));
}

ALT_NODISCARD inline Result<bool, Error> FileExistAny(const char* pFilepath) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offset = instance.Utf16BufferAppend(pFilepath, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offset); };

    return instance.FileExistAny(instance.Utf16BufferPtr(offset));
}

ALT_NODISCARD ALT_FORCE_INLINE Result<bool, Error> FileExistAny(const PathOffset pathOffset) noexcept {

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    return instance.FileExistAny(instance.Utf16BufferPtr(pathOffset.GetOffset()));
}

ALT_NODISCARD inline Result<void, Error> FileCreateSymLink(const Slice<const char> filepathFrom, const Slice<const char> filepathTo, const SymLinkTarget symLinkTarget) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offsetFrom = instance.Utf16BufferAppend(filepathFrom, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offsetFrom); };

    const usize size = instance.Utf16BufferSizeAssumeLast(offsetFrom, _PlatformDetails::Utf16BufferAppendBits::PathBuffer);

    const usize offsetTo = instance.Utf16BufferAppend(filepathTo, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();

    return instance.FileCreateSymLink(
        instance.Utf16BufferPtr(offsetFrom),
        size,
        instance.Utf16BufferPtr(offsetTo),
        symLinkTarget
    );
}

ALT_NODISCARD inline Result<void, Error> FileCreateSymLink(const char* pFilepathFrom, const Slice<const char> filepathTo, const SymLinkTarget symLinkTarget) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offsetFrom = instance.Utf16BufferAppend(pFilepathFrom, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offsetFrom); };

    const usize size = instance.Utf16BufferSizeAssumeLast(offsetFrom, _PlatformDetails::Utf16BufferAppendBits::PathBuffer);

    const usize offsetTo = instance.Utf16BufferAppend(filepathTo, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();

    return instance.FileCreateSymLink(
        instance.Utf16BufferPtr(offsetFrom),
        size,
        instance.Utf16BufferPtr(offsetTo),
        symLinkTarget
    );
}

ALT_NODISCARD inline Result<void, Error> FileCreateSymLink(const PathOffset pathOffsetFrom, const Slice<const char> filepathTo, const SymLinkTarget symLinkTarget) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offsetTo = instance.Utf16BufferAppend(filepathTo, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offsetTo); };

    return instance.FileCreateSymLink(
        instance.Utf16BufferPtr(pathOffsetFrom.GetOffset()),
        instance.Utf16BufferGetSize(pathOffsetFrom),
        instance.Utf16BufferPtr(offsetTo),
        symLinkTarget
    );
}

ALT_NODISCARD inline Result<void, Error> FileCreateSymLink(const Slice<const char> filepathFrom, const char* pFilepathTo, const SymLinkTarget symLinkTarget) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offsetFrom = instance.Utf16BufferAppend(filepathFrom, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offsetFrom); };

    const usize size = instance.Utf16BufferSizeAssumeLast(offsetFrom, _PlatformDetails::Utf16BufferAppendBits::PathBuffer);

    const usize offsetTo = instance.Utf16BufferAppend(pFilepathTo, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();

    return instance.FileCreateSymLink(
        instance.Utf16BufferPtr(offsetFrom),
        size,
        instance.Utf16BufferPtr(offsetTo),
        symLinkTarget
    );
}

ALT_NODISCARD inline Result<void, Error> FileCreateSymLink(const char* pFilepathFrom, const char* pFilepathTo, const SymLinkTarget symLinkTarget) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offsetFrom = instance.Utf16BufferAppend(pFilepathFrom, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offsetFrom); };

    const usize size = instance.Utf16BufferSizeAssumeLast(offsetFrom, _PlatformDetails::Utf16BufferAppendBits::PathBuffer);

    const usize offsetTo = instance.Utf16BufferAppend(pFilepathTo, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();

    return instance.FileCreateSymLink(
        instance.Utf16BufferPtr(offsetFrom),
        size,
        instance.Utf16BufferPtr(offsetTo),
        symLinkTarget
    );
}

ALT_NODISCARD inline Result<void, Error> FileCreateSymLink(const PathOffset pathOffsetFrom, const char* pFilepathTo, const SymLinkTarget symLinkTarget) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offsetTo = instance.Utf16BufferAppend(pFilepathTo, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offsetTo); };

    return instance.FileCreateSymLink(
        instance.Utf16BufferPtr(pathOffsetFrom.GetOffset()),
        instance.Utf16BufferGetSize(pathOffsetFrom),
        instance.Utf16BufferPtr(offsetTo),
        symLinkTarget
    );
}

ALT_NODISCARD inline Result<void, Error> FileCreateSymLink(const Slice<const char> filepathFrom, const PathOffset pathOffsetTo, const SymLinkTarget symLinkTarget) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offsetFrom = instance.Utf16BufferAppend(filepathFrom, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offsetFrom); };

    const usize size = instance.Utf16BufferSizeAssumeLast(offsetFrom, _PlatformDetails::Utf16BufferAppendBits::PathBuffer);

    return instance.FileCreateSymLink(
        instance.Utf16BufferPtr(offsetFrom),
        size,
        instance.Utf16BufferPtr(pathOffsetTo.GetOffset()),
        symLinkTarget
    );
}

ALT_NODISCARD inline Result<void, Error> FileCreateSymLink(const char* pFilepathFrom, const PathOffset pathOffsetTo, const SymLinkTarget symLinkTarget) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offsetFrom = instance.Utf16BufferAppend(pFilepathFrom, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offsetFrom); };

    const usize size = instance.Utf16BufferSizeAssumeLast(offsetFrom, _PlatformDetails::Utf16BufferAppendBits::PathBuffer);

    return instance.FileCreateSymLink(
        instance.Utf16BufferPtr(offsetFrom),
        size,
        instance.Utf16BufferPtr(pathOffsetTo.GetOffset()),
        symLinkTarget
    );
}

ALT_NODISCARD ALT_FORCE_INLINE Result<void, Error> FileCreateSymLink(const PathOffset pathOffsetFrom, const PathOffset pathOffsetTo, const SymLinkTarget symLinkTarget) noexcept {

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    return instance.FileCreateSymLink(
        instance.Utf16BufferPtr(pathOffsetFrom.GetOffset()),
        instance.Utf16BufferGetSize(pathOffsetFrom),
        instance.Utf16BufferPtr(pathOffsetTo.GetOffset()),
        symLinkTarget
    );
}

ALT_NODISCARD inline Result<void, Error> FileReadSymLinkAppend(const Handle handle, String& path) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    Error err;
    const _PlatformDetails::Utf16BufferOffset bufferOffset = instance.FileReadSymLink(handle) iferr_return_with(err) err;
    defer{ instance.Utf16BufferDeferGuard(bufferOffset.GetOffset()); };

    bufferOffset.CloneIntoAppend(path) iferr_return Error::MemoryAllocation();

    return Ok;
}

ALT_NODISCARD inline Result<void, Error> FileReadSymLink(const Handle handle, String& path) noexcept {

    path.Clear();
    return FileReadSymLinkAppend(handle, path);
}

ALT_NODISCARD inline Result<String, Error> FileReadSymLink(const Handle handle) noexcept {
    iferr_scope;

    Error err;
    String path;
    FileReadSymLinkAppend(handle, path) iferr_return_with(err) err;

    return path;
}

ALT_NODISCARD inline Result<void, Error> FileReadSymLinkAppend(const Slice<const char> filepath, String& path) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offset = instance.Utf16BufferAppend(filepath, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offset); };

    Error err;
    const _PlatformDetails::Utf16BufferOffset bufferOffset = instance.FileReadSymLink(instance.Utf16BufferPtr(offset)) iferr_return_with(err) err;

    bufferOffset.CloneIntoAppend(path) iferr_return Error::MemoryAllocation();

    return Ok;
}

ALT_NODISCARD inline Result<void, Error> FileReadSymLink(const Slice<const char> filepath, String& path) noexcept {

    path.Clear();
    return FileReadSymLinkAppend(filepath, path);
}

ALT_NODISCARD inline Result<String, Error> FileReadSymLink(const Slice<const char> filepath) noexcept {
    iferr_scope;

    Error err;
    String path;
    FileReadSymLinkAppend(filepath, path) iferr_return_with(err) err;

    return path;
}

ALT_NODISCARD inline Result<void, Error> FileReadSymLinkAppend(const char* pFilepath, String& path) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offset = instance.Utf16BufferAppend(pFilepath, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offset); };

    Error err;
    const _PlatformDetails::Utf16BufferOffset bufferOffset = instance.FileReadSymLink(instance.Utf16BufferPtr(offset)) iferr_return_with(err) err;

    bufferOffset.CloneIntoAppend(path) iferr_return Error::MemoryAllocation();

    return Ok;
}

ALT_NODISCARD inline Result<void, Error> FileReadSymLink(const char* pFilepath, String& path) noexcept {

    path.Clear();
    return FileReadSymLinkAppend(pFilepath, path);
}

ALT_NODISCARD inline Result<String, Error> FileReadSymLink(const char* pFilepath) noexcept {
    iferr_scope;

    Error err;
    String path;
    FileReadSymLinkAppend(pFilepath, path) iferr_return_with(err) err;

    return path;
}

ALT_NODISCARD inline Result<void, Error> FileReadSymLinkAppend(const PathOffset pathOffset, String& path) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    Error err;
    const _PlatformDetails::Utf16BufferOffset bufferOffset = instance.FileReadSymLink(instance.Utf16BufferPtr(pathOffset.GetOffset())) iferr_return_with(err) err;
    defer{ instance.Utf16BufferDeferGuard(bufferOffset.GetOffset()); };

    bufferOffset.CloneIntoAppend(path) iferr_return Error::MemoryAllocation();

    return Ok;
}

ALT_NODISCARD inline Result<void, Error> FileReadSymLink(const PathOffset pathOffset, String& path) noexcept {

    path.Clear();
    return FileReadSymLinkAppend(pathOffset, path);
}

ALT_NODISCARD inline Result<String, Error> FileReadSymLink(const PathOffset pathOffset) noexcept {
    iferr_scope;

    Error err;
    String path;
    FileReadSymLinkAppend(pathOffset, path) iferr_return_with(err) err;

    return path;
}

ALT_NODISCARD inline Result<void, Error> FileCopySymLink(const Slice<const char> filepathFrom, const Slice<const char> filepathTo) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offsetFrom = instance.Utf16BufferAppend(filepathFrom, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offsetFrom); };

    const usize offsetTo = instance.Utf16BufferAppend(filepathTo, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();

    return instance.FileCopySymLink(instance.Utf16BufferPtr(offsetFrom), instance.Utf16BufferPtr(offsetTo));
}

ALT_NODISCARD inline Result<void, Error> FileCopySymLink(const char* pFilepathFrom, const Slice<const char> filepathTo) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offsetFrom = instance.Utf16BufferAppend(pFilepathFrom, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offsetFrom); };

    const usize offsetTo = instance.Utf16BufferAppend(filepathTo, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();

    return instance.FileCopySymLink(instance.Utf16BufferPtr(offsetFrom), instance.Utf16BufferPtr(offsetTo));
}

ALT_NODISCARD inline Result<void, Error> FileCopySymLink(const PathOffset pathOffsetFrom, const Slice<const char> filepathTo) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offsetTo = instance.Utf16BufferAppend(filepathTo, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offsetTo); };

    return instance.FileCopySymLink(instance.Utf16BufferPtr(pathOffsetFrom.GetOffset()), instance.Utf16BufferPtr(offsetTo));
}

ALT_NODISCARD inline Result<void, Error> FileCopySymLink(const Slice<const char> filepathFrom, const char* pFilepathTo) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offsetFrom = instance.Utf16BufferAppend(filepathFrom, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offsetFrom); };

    const usize offsetTo = instance.Utf16BufferAppend(pFilepathTo, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();

    return instance.FileCopySymLink(instance.Utf16BufferPtr(offsetFrom), instance.Utf16BufferPtr(offsetTo));
}

ALT_NODISCARD inline Result<void, Error> FileCopySymLink(const char* pFilepathFrom, const char* pFilepathTo) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offsetFrom = instance.Utf16BufferAppend(pFilepathFrom, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offsetFrom); };

    const usize offsetTo = instance.Utf16BufferAppend(pFilepathTo, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();

    return instance.FileCopySymLink(instance.Utf16BufferPtr(offsetFrom), instance.Utf16BufferPtr(offsetTo));
}

ALT_NODISCARD inline Result<void, Error> FileCopySymLink(const PathOffset pathOffsetFrom, const char* pFilepathTo) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offsetTo = instance.Utf16BufferAppend(pFilepathTo, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offsetTo); };

    return instance.FileCopySymLink(instance.Utf16BufferPtr(pathOffsetFrom.GetOffset()), instance.Utf16BufferPtr(offsetTo));
}

ALT_NODISCARD inline Result<void, Error> FileCopySymLink(const Slice<const char> filepathFrom, const PathOffset pathOffsetTo) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offsetFrom = instance.Utf16BufferAppend(filepathFrom, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offsetFrom); };

    return instance.FileCopySymLink(instance.Utf16BufferPtr(offsetFrom), instance.Utf16BufferPtr(pathOffsetTo.GetOffset()));
}

ALT_NODISCARD inline Result<void, Error> FileCopySymLink(const char* pFilepathFrom, const PathOffset pathOffsetTo) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offsetFrom = instance.Utf16BufferAppend(pFilepathFrom, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offsetFrom); };

    return instance.FileCopySymLink(instance.Utf16BufferPtr(offsetFrom), instance.Utf16BufferPtr(pathOffsetTo.GetOffset()));
}

ALT_NODISCARD ALT_FORCE_INLINE Result<void, Error> FileCopySymLink(const PathOffset pathOffsetFrom, const PathOffset pathOffsetTo) noexcept {

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    return instance.FileCopySymLink(instance.Utf16BufferPtr(pathOffsetFrom.GetOffset()), instance.Utf16BufferPtr(pathOffsetTo.GetOffset()));
}

ALT_NODISCARD inline Result<void, Error> FileCreateHardLink(const Slice<const char> filepath, const Slice<const char> filepathTarget) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offset = instance.Utf16BufferAppend(filepath, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offset); };

    const usize size = instance.Utf16BufferSizeAssumeLast(offset, _PlatformDetails::Utf16BufferAppendBits::PathBuffer);

    const usize offsetTarget = instance.Utf16BufferAppend(filepathTarget, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();

    return instance.FileCreateHardLink(
        instance.Utf16BufferPtr(offset),
        size,
        instance.Utf16BufferPtr(offsetTarget)
    );
}

ALT_NODISCARD inline Result<void, Error> FileCreateHardLink(const char* pFilepath, const Slice<const char> filepathTarget) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offset = instance.Utf16BufferAppend(pFilepath, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();

    defer{ instance.Utf16BufferDeferGuard(offset); };

    const usize size = instance.Utf16BufferSizeAssumeLast(offset, _PlatformDetails::Utf16BufferAppendBits::PathBuffer);

    const usize offsetTarget = instance.Utf16BufferAppend(filepathTarget, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();

    return instance.FileCreateHardLink(
        instance.Utf16BufferPtr(offset),
        size,
        instance.Utf16BufferPtr(offsetTarget)
    );
}

ALT_NODISCARD inline Result<void, Error> FileCreateHardLink(const PathOffset pathOffset, const Slice<const char> filepathTarget) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offsetTarget = instance.Utf16BufferAppend(filepathTarget, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offsetTarget); };

    return instance.FileCreateHardLink(
        instance.Utf16BufferPtr(pathOffset.GetOffset()),
        instance.Utf16BufferGetSize(pathOffset),
        instance.Utf16BufferPtr(offsetTarget)
    );
}

ALT_NODISCARD inline Result<void, Error> FileCreateHardLink(const Slice<const char> filepath, const char* pFilepathTarget) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offset = instance.Utf16BufferAppend(filepath, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offset); };

    const usize size = instance.Utf16BufferSizeAssumeLast(offset, _PlatformDetails::Utf16BufferAppendBits::PathBuffer);

    const usize offsetTarget = instance.Utf16BufferAppend(pFilepathTarget, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();

    return instance.FileCreateHardLink(
        instance.Utf16BufferPtr(offset),
        size,
        instance.Utf16BufferPtr(offsetTarget)
    );
}

ALT_NODISCARD inline Result<void, Error> FileCreateHardLink(const char* pFilepath, const char* pFilepathTarget) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offset = instance.Utf16BufferAppend(pFilepath, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offset); };

    const usize size = instance.Utf16BufferSizeAssumeLast(offset, _PlatformDetails::Utf16BufferAppendBits::PathBuffer);

    const usize offsetTarget = instance.Utf16BufferAppend(pFilepathTarget, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();

    return instance.FileCreateHardLink(
        instance.Utf16BufferPtr(offset),
        size,
        instance.Utf16BufferPtr(offsetTarget)
    );
}

ALT_NODISCARD inline Result<void, Error> FileCreateHardLink(const PathOffset pathOffset, const char* pFilepathTarget) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offsetTarget = instance.Utf16BufferAppend(pFilepathTarget, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offsetTarget); };

    return instance.FileCreateHardLink(
        instance.Utf16BufferPtr(pathOffset.GetOffset()),
        instance.Utf16BufferGetSize(pathOffset),
        instance.Utf16BufferPtr(offsetTarget)
    );
}

ALT_NODISCARD inline Result<void, Error> FileCreateHardLink(const Slice<const char> filepath, const PathOffset pathOffsetTarget) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offset = instance.Utf16BufferAppend(filepath, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offset); };

    const usize size = instance.Utf16BufferSizeAssumeLast(offset, _PlatformDetails::Utf16BufferAppendBits::PathBuffer);

    return instance.FileCreateHardLink(
        instance.Utf16BufferPtr(offset),
        size,
        instance.Utf16BufferPtr(pathOffsetTarget.GetOffset())
    );
}

ALT_NODISCARD inline Result<void, Error> FileCreateHardLink(const char* pFilepath, const PathOffset pathOffsetTarget) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offset = instance.Utf16BufferAppend(pFilepath, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offset); };

    const usize size = instance.Utf16BufferSizeAssumeLast(offset, _PlatformDetails::Utf16BufferAppendBits::PathBuffer);

    return instance.FileCreateHardLink(
        instance.Utf16BufferPtr(offset),
        size,
        instance.Utf16BufferPtr(pathOffsetTarget.GetOffset())
    );
}

ALT_NODISCARD ALT_FORCE_INLINE Result<void, Error> FileCreateHardLink(const PathOffset pathOffset, const PathOffset pathOffsetTarget) noexcept {

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    return instance.FileCreateHardLink(
        instance.Utf16BufferPtr(pathOffset.GetOffset()),
        instance.Utf16BufferGetSize(pathOffset),
        instance.Utf16BufferPtr(pathOffsetTarget.GetOffset())
    );
}

ALT_NODISCARD ALT_FORCE_INLINE Result<FileInfo, Error> FileGetInfo(const Handle handle) noexcept {

    const _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    return instance.FileGetInfo(handle);
}

ALT_NODISCARD inline Result<FileInfo, Error> FileGetInfo(const Slice<const char> filepath) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offset = instance.Utf16BufferAppend(filepath, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offset); };

    return (*instance.pFileGetInfo)(instance, instance.Utf16BufferPtr(offset));
}

ALT_NODISCARD inline Result<FileInfo, Error> FileGetInfo(const char* pFilepath) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offset = instance.Utf16BufferAppend(pFilepath, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offset); };

    return (*instance.pFileGetInfo)(instance, instance.Utf16BufferPtr(offset));
}

ALT_NODISCARD ALT_FORCE_INLINE Result<FileInfo, Error> FileGetInfo(const PathOffset pathOffset) noexcept {

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    return (*instance.pFileGetInfo)(instance, instance.Utf16BufferPtr(pathOffset.GetOffset()));
}

ALT_NODISCARD inline Result<FileFreeSpace, Error> FileGetFreeSpace(const Slice<const char> filepath) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offset = instance.Utf16BufferAppend(filepath, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offset); };

    return instance.FileGetFreeSpace(instance.Utf16BufferPtr(offset));
}

ALT_NODISCARD inline Result<FileFreeSpace, Error> FileGetFreeSpace(const char* pFilepath) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offset = instance.Utf16BufferAppend(pFilepath, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offset); };

    return instance.FileGetFreeSpace(instance.Utf16BufferPtr(offset));
}

ALT_NODISCARD ALT_FORCE_INLINE Result<FileFreeSpace, Error> FileGetFreeSpace(const PathOffset pathOffset) noexcept {

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    return instance.FileGetFreeSpace(instance.Utf16BufferPtr(pathOffset.GetOffset()));
}

ALT_NODISCARD ALT_FORCE_INLINE Result<bool, Error> FileSame(const Handle handle0, const Handle handle1) noexcept {

    const _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    return (*instance.pFileSameByHandle)(instance, handle0, handle1);
}

ALT_NODISCARD inline Result<bool, Error> FileSame(const Slice<const char> filepath0, const Slice<const char> filepath1) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offset0 = instance.Utf16BufferAppend(filepath0, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offset0); };

    const usize offset1 = instance.Utf16BufferAppend(filepath1, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();

    return (*instance.pFileSame)(instance, instance.Utf16BufferPtr(offset0), instance.Utf16BufferPtr(offset1));
}

ALT_NODISCARD inline Result<bool, Error> FileSame(const char* pFilepath0, const Slice<const char> filepath1) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offset0 = instance.Utf16BufferAppend(pFilepath0, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offset0); };

    const usize offset1 = instance.Utf16BufferAppend(filepath1, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();

    return (*instance.pFileSame)(instance, instance.Utf16BufferPtr(offset0), instance.Utf16BufferPtr(offset1));
}

ALT_NODISCARD inline Result<bool, Error> FileSame(const PathOffset pathOffset0, const Slice<const char> filepath1) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offset1 = instance.Utf16BufferAppend(filepath1, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offset1); };

    return (*instance.pFileSame)(instance, instance.Utf16BufferPtr(pathOffset0.GetOffset()), instance.Utf16BufferPtr(offset1));
}

ALT_NODISCARD inline Result<bool, Error> FileSame(const Slice<const char> filepath0, const char* pFilepath1) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offset0 = instance.Utf16BufferAppend(filepath0, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offset0); };

    const usize offset1 = instance.Utf16BufferAppend(pFilepath1, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();

    return (*instance.pFileSame)(instance, instance.Utf16BufferPtr(offset0), instance.Utf16BufferPtr(offset1));
}

ALT_NODISCARD inline Result<bool, Error> FileSame(const char* pFilepath0, const char* pFilepath1) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offset0 = instance.Utf16BufferAppend(pFilepath0, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offset0); };

    const usize offset1 = instance.Utf16BufferAppend(pFilepath1, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();

    return (*instance.pFileSame)(instance, instance.Utf16BufferPtr(offset0), instance.Utf16BufferPtr(offset1));
}

ALT_NODISCARD inline Result<bool, Error> FileSame(const PathOffset pathOffset0, const char* pFilepath1) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offset1 = instance.Utf16BufferAppend(pFilepath1, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offset1); };

    return (*instance.pFileSame)(instance, instance.Utf16BufferPtr(pathOffset0.GetOffset()), instance.Utf16BufferPtr(offset1));
}

ALT_NODISCARD inline Result<bool, Error> FileSame(const Slice<const char> filepath0, const PathOffset pathOffset1) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offset0 = instance.Utf16BufferAppend(filepath0, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offset0); };

    return (*instance.pFileSame)(instance, instance.Utf16BufferPtr(offset0), instance.Utf16BufferPtr(pathOffset1.GetOffset()));
}

ALT_NODISCARD inline Result<bool, Error> FileSame(const char* pFilepath0, const PathOffset pathOffset1) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offset0 = instance.Utf16BufferAppend(pFilepath0, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offset0); };

    return (*instance.pFileSame)(instance, instance.Utf16BufferPtr(offset0), instance.Utf16BufferPtr(pathOffset1.GetOffset()));
}

ALT_NODISCARD ALT_FORCE_INLINE Result<bool, Error> FileSame(const PathOffset pathOffset0, const PathOffset pathOffset1) noexcept {

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    return (*instance.pFileSame)(instance, instance.Utf16BufferPtr(pathOffset0.GetOffset()), instance.Utf16BufferPtr(pathOffset1.GetOffset()));
}

/*
 * @example
 * 
 * static Alt::Result<void, Alt::Platform::Error> DirectoryEntry(
 *     Alt::String& path,
 *     const Alt::Optional<Alt::Platform::PathOffset>& pathOffset,
 *     const usize deep
 * ) noexcept {
 * 
 *     const auto func = [&](const Alt::Platform::FileDirectoryEntry& entry) {
 * 
 *         // entry.filename lifetime bounded by 'path' reallocation
 *         // entry.filepath lifetime bounded by current function scope
 * 
 *         for (usize i = 0; i < deep; ++i) {
 *             ALT_WRITE_BUFFERED_CONSOLE("    ");
 *         }
 * 
 *         if (entry.isDir) {
 * 
 *             ALT_WRITE_BUFFERED_CONSOLE("{} - {}\\\n", *entry.filename, Alt::DateTime::FromTime(entry.lastChangeTime));
 * 
 *             if (deep < 4) {
 * 
 *                 DirectoryEntry(path, entry.filepath, deep + 1);
 *             }
 *         }
 *         else {
 * 
 *             ALT_WRITE_BUFFERED_CONSOLE("{} - {}\n", *entry.filename, Alt::DateTime::FromTime(entry.lastChangeTime));
 *         }
 *     };
 * 
 *     Alt::Platform::FileForEachDirectoryEntryDesc desc{};
 * 
 *     if (!pathOffset) {
 *         return Alt::Platform::FileForEachDirectoryEntry(path, path, func, desc);
 *     }
 *     else {
 *         return Alt::Platform::FileForEachDirectoryEntry(*pathOffset, path, func, desc);
 *     }
 * }
 * 
 * Alt::Platform::FileLogicalDriversIterator iterator = Alt::Platform::FileGetLogicalDriversIterator();
 * while (Alt::Optional<char> driver = iterator.Next()) {
 *
 *     Alt::String path = ALT_FORMAT("{}:\\", *driver);
 *     ALT_WRITE_BUFFERED_CONSOLE(path);
 *     ALT_WRITE_BUFFERED_CONSOLE("\n");
 *
 *     DirectoryEntry(path, Alt::Nullopt, 1) iferr_return_with(err);
 * }
 * 
 * ALT_FLUSH_BUFFERED_CONSOLE();
 */
template <typename Func,
    std::enable_if_t<std::is_same_v<std::invoke_result_t<Func, const FileDirectoryEntry&>, Result<FileForEachDirectoryEntryContinuation, Error>>>* = nullptr>
ALT_NODISCARD inline Result<void, Error> FileForEachDirectoryEntry(
    const Slice<const char> filepath,
    Optional<String&> buffer,
    Func&& func,
    const FileForEachDirectoryEntryDesc& desc
) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const _PlatformDetails::DirectoryEntrySizeResult directoryEntrySizeResult =
        instance.FileForEachDirectoryEntry_GetDirectoryEntrySize(desc.sizeInBytesTmpData);

    const usize offset = instance.Utf16BufferAppend(filepath, _PlatformDetails::Utf16BufferAppendBits::PathBuffer, directoryEntrySizeResult.sizeToAllocate)
        iferr_return Error::MemoryAllocation();

    defer{ instance.Utf16BufferDeferGuard(offset); };

    return instance.FileForEachDirectoryEntry(offset, offset + directoryEntrySizeResult.sizeToAllocate, directoryEntrySizeResult.sizeInBytesFileDirInfo, buffer, std::forward<Func>(func), desc.bits);
}

template <typename Func,
    std::enable_if_t<std::is_same_v<std::invoke_result_t<Func, const FileDirectoryEntry&>, void>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Result<void, Error> FileForEachDirectoryEntry(
    const Slice<const char> filepath,
    Optional<String&> buffer,
    Func&& func,
    const FileForEachDirectoryEntryDesc& desc
) noexcept {

    return FileForEachDirectoryEntry(filepath, buffer,
        [func = std::forward<Func>(func)](const FileDirectoryEntry& entry) -> Result<FileForEachDirectoryEntryContinuation, Error> {

            (func)(entry);
            return FileForEachDirectoryEntryContinuation::Continue;
        }, desc);
}

template <typename Func,
    std::enable_if_t<std::is_same_v<std::invoke_result_t<Func, const FileDirectoryEntry&>, Result<FileForEachDirectoryEntryContinuation, Error>>>* = nullptr>
ALT_NODISCARD inline Result<void, Error> FileForEachDirectoryEntry(
    const char* pFilepath,
    Optional<String&> buffer,
    Func&& func,
    const FileForEachDirectoryEntryDesc& desc
) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const _PlatformDetails::DirectoryEntrySizeResult directoryEntrySizeResult =
        instance.FileForEachDirectoryEntry_GetDirectoryEntrySize(desc.sizeInBytesTmpData);

    const usize offset = instance.Utf16BufferAppend(pFilepath, _PlatformDetails::Utf16BufferAppendBits::PathBuffer, directoryEntrySizeResult.sizeToAllocate)
        iferr_return Error::MemoryAllocation();

    defer{ instance.Utf16BufferDeferGuard(offset); };

    return instance.FileForEachDirectoryEntry(offset, offset + directoryEntrySizeResult.sizeToAllocate, directoryEntrySizeResult.sizeInBytesFileDirInfo, buffer, std::forward<Func>(func), desc.bits);
}

template <typename Func,
    std::enable_if_t<std::is_same_v<std::invoke_result_t<Func, const FileDirectoryEntry&>, void>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Result<void, Error> FileForEachDirectoryEntry(
    const char* pFilepath,
    Optional<String&> buffer,
    Func&& func,
    const FileForEachDirectoryEntryDesc& desc
) noexcept {

    return FileForEachDirectoryEntry(pFilepath, buffer,
        [func = std::forward<Func>(func)](const FileDirectoryEntry& entry) -> Result<FileForEachDirectoryEntryContinuation, Error> {

            (func)(entry);
            return FileForEachDirectoryEntryContinuation::Continue;
        }, desc);
}

template <typename Func,
    std::enable_if_t<std::is_same_v<std::invoke_result_t<Func, const FileDirectoryEntry&>, Result<FileForEachDirectoryEntryContinuation, Error>>>* = nullptr>
ALT_NODISCARD inline Result<void, Error> FileForEachDirectoryEntry(
    const PathOffset pathOffset,
    Optional<String&> buffer,
    Func&& func,
    const FileForEachDirectoryEntryDesc& desc
) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const _PlatformDetails::DirectoryEntrySizeResult directoryEntrySizeResult =
        instance.FileForEachDirectoryEntry_GetDirectoryEntrySize(desc.sizeInBytesTmpData);

    const usize offset = instance.Utf16BufferAppend(pathOffset, directoryEntrySizeResult.sizeToAllocate)
        iferr_return Error::MemoryAllocation();

    defer{ instance.Utf16BufferDeferGuard(offset); };

    return instance.FileForEachDirectoryEntry(offset, offset + directoryEntrySizeResult.sizeToAllocate, directoryEntrySizeResult.sizeInBytesFileDirInfo, buffer, std::forward<Func>(func), desc.bits);
}

template <typename Func,
    std::enable_if_t<std::is_same_v<std::invoke_result_t<Func, const FileDirectoryEntry&>, void>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE Result<void, Error> FileForEachDirectoryEntry(
    const PathOffset pathOffset,
    Optional<String&> buffer,
    Func&& func,
    const FileForEachDirectoryEntryDesc& desc
) noexcept {

    return FileForEachDirectoryEntry(pathOffset, buffer,
        [func = std::forward<Func>(func)](const FileDirectoryEntry& entry) -> Result<FileForEachDirectoryEntryContinuation, Error> {

            (func)(entry);
            return FileForEachDirectoryEntryContinuation::Continue;
        }, desc);
}

ALT_NODISCARD inline Result<void, Error> FilePathAbsoluteAppend(const Slice<const char> filepath, String& path) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offset = instance.Utf16BufferAppend(filepath, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offset); };

    Error err;
    const _PlatformDetails::Utf16BufferOffset bufferOffset = instance.FilePathAbsoluteAppend(offset) iferr_return_with(err) err;

    bufferOffset.CloneIntoAppend(path) iferr_return Error::MemoryAllocation();

    return Ok;
}

ALT_NODISCARD inline Result<void, Error> FilePathAbsolute(const Slice<const char> filepath, String& path) noexcept {

    path.Clear();
    return FilePathAbsoluteAppend(filepath, path);
}

ALT_NODISCARD inline Result<String, Error> FilePathAbsolute(const Slice<const char> filepath) noexcept {
    iferr_scope;

    Error err;
    String path;
    FilePathAbsoluteAppend(filepath, path) iferr_return_with(err) err;

    return path;
}

ALT_NODISCARD inline Result<void, Error> FilePathAbsoluteAppend(const char* pFilepath, String& path) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offset = instance.Utf16BufferAppend(pFilepath, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offset); };

    Error err;
    const _PlatformDetails::Utf16BufferOffset bufferOffset = instance.FilePathAbsoluteAppend(offset) iferr_return_with(err) err;

    bufferOffset.CloneIntoAppend(path) iferr_return Error::MemoryAllocation();

    return Ok;
}

ALT_NODISCARD inline Result<void, Error> FilePathAbsolute(const char* pFilepath, String& path) noexcept {

    path.Clear();
    return FilePathAbsoluteAppend(pFilepath, path);
}

ALT_NODISCARD inline Result<String, Error> FilePathAbsolute(const char* pFilepath) noexcept {
    iferr_scope;

    Error err;
    String path;
    FilePathAbsoluteAppend(pFilepath, path) iferr_return_with(err) err;

    return path;
}

ALT_NODISCARD inline Result<void, Error> FilePathAbsoluteAppend(const PathOffset pathOffset, String& path) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    Error err;
    const _PlatformDetails::Utf16BufferOffset bufferOffset = instance.FilePathAbsoluteAppend(pathOffset.GetOffset()) iferr_return_with(err) err;
    defer{ instance.Utf16BufferDeferGuard(bufferOffset.GetOffset()); };

    bufferOffset.CloneIntoAppend(path) iferr_return Error::MemoryAllocation();

    return Ok;
}

ALT_NODISCARD inline Result<void, Error> FilePathAbsolute(const PathOffset pathOffset, String& path) noexcept {

    path.Clear();
    return FilePathAbsoluteAppend(pathOffset, path);
}

ALT_NODISCARD inline Result<String, Error> FilePathAbsolute(const PathOffset pathOffset) noexcept {
    iferr_scope;

    Error err;
    String path;
    FilePathAbsoluteAppend(pathOffset, path) iferr_return_with(err) err;

    return path;
}

ALT_NODISCARD inline Result<void, Error> FilePathCanonicalAppend(const Slice<const char> filepath, String& path) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offset = instance.Utf16BufferAppend(filepath, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offset); };

    Error err;
    const _PlatformDetails::Utf16BufferOffset bufferOffset = instance.FilePathCanonicalAppend(offset) iferr_return_with(err) err;

    bufferOffset.CloneIntoAppend(path) iferr_return Error::MemoryAllocation();

    return Ok;
}

ALT_NODISCARD inline Result<void, Error> FilePathCanonical(const Slice<const char> filepath, String& path) noexcept {

    path.Clear();
    return FilePathCanonicalAppend(filepath, path);
}

ALT_NODISCARD inline Result<String, Error> FilePathCanonical(const Slice<const char> filepath) noexcept {
    iferr_scope;

    Error err;
    String path;
    FilePathCanonicalAppend(filepath, path) iferr_return_with(err) err;

    return path;
}

ALT_NODISCARD inline Result<void, Error> FilePathCanonicalAppend(const char* pFilepath, String& path) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offset = instance.Utf16BufferAppend(pFilepath, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offset); };

    Error err;
    const _PlatformDetails::Utf16BufferOffset bufferOffset = instance.FilePathCanonicalAppend(offset) iferr_return_with(err) err;

    bufferOffset.CloneIntoAppend(path) iferr_return Error::MemoryAllocation();

    return Ok;
}

ALT_NODISCARD inline Result<void, Error> FilePathCanonical(const char* pFilepath, String& path) noexcept {

    path.Clear();
    return FilePathCanonicalAppend(pFilepath, path);
}

ALT_NODISCARD inline Result<String, Error> FilePathCanonical(const char* pFilepath) noexcept {
    iferr_scope;

    Error err;
    String path;
    FilePathCanonicalAppend(pFilepath, path) iferr_return_with(err) err;

    return path;
}

ALT_NODISCARD inline Result<void, Error> FilePathCanonicalAppend(const PathOffset pathOffset, String& path) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    Error err;
    const _PlatformDetails::Utf16BufferOffset bufferOffset = instance.FilePathCanonicalAppend(pathOffset.GetOffset()) iferr_return_with(err) err;
    defer{ instance.Utf16BufferDeferGuard(bufferOffset.GetOffset()); };

    bufferOffset.CloneIntoAppend(path) iferr_return Error::MemoryAllocation();

    return Ok;
}

ALT_NODISCARD inline Result<void, Error> FilePathCanonical(const PathOffset pathOffset, String& path) noexcept {

    path.Clear();
    return FilePathCanonicalAppend(pathOffset, path);
}

ALT_NODISCARD inline Result<String, Error> FilePathCanonical(const PathOffset pathOffset) noexcept {
    iferr_scope;

    Error err;
    String path;
    FilePathCanonicalAppend(pathOffset, path) iferr_return_with(err) err;

    return path;
}

ALT_NODISCARD ALT_FORCE_INLINE Slice<const char> FilePathGetLogicalDriver(Slice<const char> filepath) noexcept {

    const Slice<const char> longPrefix = "\\\\?\\"_s;
    if (filepath.StartWith(longPrefix)) {

        filepath = filepath.SubSlice(longPrefix.GetSize());
    }

    if (filepath.GetSize() >= 2 && String::IsAsciiAlphabet(filepath[0]) && filepath[1] == ':') {

        return filepath.SubSlice(0, 2);
    }

    return ""_s;
}

ALT_NODISCARD ALT_FORCE_INLINE Slice<const char> FilePathGetLogicalDriver(const char* pFilepath, const usize size) noexcept {

    const Slice<const char> filepath{ pFilepath, size };
    return FilePathGetLogicalDriver(filepath);
}

ALT_NODISCARD ALT_FORCE_INLINE Slice<const char> FilePathGetLogicalDriver(const char* pFilepath) noexcept {

    const Slice<const char> filepath{ pFilepath, String::CalculateCStrLen(pFilepath) };
    return FilePathGetLogicalDriver(filepath);
}

ALT_NODISCARD ALT_FORCE_INLINE bool FilePathIsAbsolute(const Slice<const char> filepath) noexcept {

    return FilePathGetLogicalDriver(filepath).IsPopulated();
}

ALT_NODISCARD ALT_FORCE_INLINE bool FilePathIsAbsolute(const char* pFilepath, const usize size) noexcept {

    const Slice<const char> filepath{ pFilepath, size };
    return FilePathIsAbsolute(filepath);
}

ALT_NODISCARD ALT_FORCE_INLINE bool FilePathIsAbsolute(const char* pFilepath) noexcept {

    const Slice<const char> filepath{ pFilepath, String::CalculateCStrLen(pFilepath) };
    return FilePathIsAbsolute(filepath);
}

ALT_NODISCARD ALT_FORCE_INLINE FilePathSplitParentPathAndFilenameResult FilePathSplitParentPathAndFilename(
    const Slice<const char> filepath, const FilePathSeparatorBits separatorBits = FilePathSeparatorBits::Any
) noexcept {
    iferr_scope;

    FilePathSplitParentPathAndFilenameResult result{};
    result.parentPath = ""_s;
    result.filename = filepath;

    if (BitFlag::IsSetEnumFlags(separatorBits, FilePathSeparatorBits::Any)) {

        const Optional<usize> offset0 = filepath.FindLast('/');
        const Optional<usize> offset1 = filepath.FindLast(ALT_DS);

        if (!offset0 && !offset1) {
            return result;
        }

        usize offsetDir = 0;
        if (offset0 && offset1) {
            offsetDir = Num::Max(*offset0, *offset1);
        }
        else if (offset0 && !offset1) {
            offsetDir = *offset0;
        }
        else if (!offset0 && offset1) {
            offsetDir = *offset1;
        }

        result.parentPath = filepath.SubSlice(0, Num::Max(offsetDir, 1));
        result.filename = filepath.SubSlice(offsetDir + 1);

        return result;
    }
    else if (BitFlag::IsSetEnumFlags(separatorBits, FilePathSeparatorBits::ForwardSlash)) {

        const usize offsetDir = filepath.FindLast('/') iferr_return result;

        result.parentPath = filepath.SubSlice(0, Num::Max(offsetDir, 1));
        result.filename = filepath.SubSlice(offsetDir + 1);

        return result;
    }
    else if (BitFlag::IsSetEnumFlags(separatorBits, FilePathSeparatorBits::BackSlash)) {

        const usize offsetDir = filepath.FindLast(ALT_DS) iferr_return result;

        result.parentPath = filepath.SubSlice(0, Num::Max(offsetDir, 1));
        result.filename = filepath.SubSlice(offsetDir + 1);

        return result;
    }

    return result;
}

ALT_NODISCARD ALT_FORCE_INLINE FilePathSplitParentPathAndFilenameResult FilePathSplitParentPathAndFilename(
    const char* pFilepath, const usize size, const FilePathSeparatorBits separatorBits = FilePathSeparatorBits::Any
) noexcept {

    const Slice<const char> filepath{ pFilepath, size };
    return FilePathSplitParentPathAndFilename(filepath, separatorBits);
}

ALT_NODISCARD ALT_FORCE_INLINE FilePathSplitParentPathAndFilenameResult FilePathSplitParentPathAndFilename(
    const char* pFilepath, const FilePathSeparatorBits separatorBits = FilePathSeparatorBits::Any
) noexcept {

    const Slice<const char> filepath{ pFilepath, String::CalculateCStrLen(pFilepath) };
    return FilePathSplitParentPathAndFilename(filepath, separatorBits);
}

ALT_NODISCARD ALT_FORCE_INLINE FilePathSplitResult FilePathSplit(
    const Slice<const char> filepath, const FilePathSeparatorBits separatorBits = FilePathSeparatorBits::Any
) noexcept {
    iferr_scope;

    FilePathSplitResult result{};

    const FilePathSplitParentPathAndFilenameResult pathSplited = FilePathSplitParentPathAndFilename(filepath, separatorBits);

    result.parentPath = pathSplited.parentPath;
    result.filename = pathSplited.filename;
    result.stem = pathSplited.filename;

    const usize offsetStem = pathSplited.filename.FindLast('.') iferr_return result;

    const Slice<const char> stem = pathSplited.filename.SubSlice(0, offsetStem);
    const Slice<const char> ext = pathSplited.filename.SubSlice(offsetStem);

    if (stem.IsEmpty()) {

        return result;
    }

    if (ext.GetSize() > 1) {

        result.stem = stem;
        result.extension = ext;

        return result;
    }

    if (stem.GetSize() == 1 && stem.GetFirst() == '.') {

        result.stem = ".."_s;

        return result;
    }

    result.stem = stem;
    result.extension = ext;

    return result;
}

ALT_NODISCARD ALT_FORCE_INLINE FilePathSplitResult FilePathSplit(
    const char* pFilepath, const usize size, const FilePathSeparatorBits separatorBits = FilePathSeparatorBits::Any
) noexcept {

    const Slice<const char> filepath{ pFilepath, size };
    return FilePathSplit(filepath, separatorBits);
}

ALT_NODISCARD ALT_FORCE_INLINE FilePathSplitResult FilePathSplit(
    const char* pFilepath, const FilePathSeparatorBits separatorBits = FilePathSeparatorBits::Any
) noexcept {

    const Slice<const char> filepath{ pFilepath, String::CalculateCStrLen(pFilepath) };
    return FilePathSplit(filepath, separatorBits);
}

ALT_NODISCARD ALT_FORCE_INLINE char FilePathSeparator() noexcept {
    return _PlatformDetails::GetPlatformInstance().FilePathSeparator();
}

ALT_NODISCARD ALT_FORCE_INLINE wchar_t FilePathSeparatorUTF16() noexcept {
    return _PlatformDetails::GetPlatformInstance().FilePathSeparatorUTF16();
}

ALT_NODISCARD inline Result<bool, Error> FileDialogOpenAppend(const Slice<const char> filepath, const FileDialogOpenType type, const FileDialogOpenDesc& desc, String& buffer) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offset = instance.Utf16BufferAppend(filepath, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offset); };

    Error err;
    const PathOffset pathOffset = instance.FileDialogOpen(offset, type, desc) iferr_return_with(err) err;

    if (pathOffset.IsEmpty()) {
        return false;
    }

    pathOffset.CloneIntoAppend(buffer) iferr_return Error::MemoryAllocation();

    return true;
}

ALT_NODISCARD inline Result<bool, Error> FileDialogOpen(const Slice<const char> filepath, const FileDialogOpenType type, const FileDialogOpenDesc& desc, String& buffer) noexcept {

    buffer.Clear();
    return FileDialogOpenAppend(filepath, type, desc, buffer);
}

ALT_NODISCARD inline Result<Optional<String>, Error> FileDialogOpen(const Slice<const char> filepath, const FileDialogOpenType type, const FileDialogOpenDesc& desc) noexcept {
    iferr_scope;

    Error err;
    String buffer;
    const bool isNotInterrupted = FileDialogOpenAppend(filepath, type, desc, buffer) iferr_return_with(err) err;

    return isNotInterrupted
        ? MakeOptional(std::move(buffer))
        : Nullopt;
}

ALT_NODISCARD inline Result<bool, Error> FileDialogOpenAppend(const char* pFilepath, const FileDialogOpenType type, const FileDialogOpenDesc& desc, String& buffer) noexcept {
    iferr_scope;

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    const usize offset = instance.Utf16BufferAppend(pFilepath, _PlatformDetails::Utf16BufferAppendBits::PathBuffer) iferr_return Error::MemoryAllocation();
    defer{ instance.Utf16BufferDeferGuard(offset); };

    Error err;
    const PathOffset pathOffset = instance.FileDialogOpen(offset, type, desc) iferr_return_with(err) err;

    if (pathOffset.IsEmpty()) {
        return false;
    }

    pathOffset.CloneIntoAppend(buffer) iferr_return Error::MemoryAllocation();

    return true;
}

ALT_NODISCARD inline Result<bool, Error> FileDialogOpen(const char* pFilepath, const FileDialogOpenType type, const FileDialogOpenDesc& desc, String& buffer) noexcept {

    buffer.Clear();
    return FileDialogOpenAppend(pFilepath, type, desc, buffer);
}

ALT_NODISCARD inline Result<Optional<String>, Error> FileDialogOpen(const char* pFilepath, const FileDialogOpenType type, const FileDialogOpenDesc& desc) noexcept {
    iferr_scope;

    Error err;
    String buffer;
    const bool isNotInterrupted = FileDialogOpenAppend(pFilepath, type, desc, buffer) iferr_return_with(err) err;

    return isNotInterrupted
        ? MakeOptional(std::move(buffer))
        : Nullopt;
}

ALT_NODISCARD ALT_FORCE_INLINE Time TimeGetSystemTimeUTC() noexcept {

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    return (*instance.pTimeGetSystemTimeUTC)(instance);
}

ALT_NODISCARD ALT_FORCE_INLINE Result<Time, Error> TimeGetSystemTimeLocalTime() noexcept {

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    return (*instance.pTimeGetSystemTimeLocalTime)(instance);
}

ALT_NODISCARD ALT_FORCE_INLINE Result<u64, Error> PerformanceGetCurrentProcessCycles() noexcept {

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    return instance.PerformanceGetCurrentProcessCycleTime();
}

ALT_NODISCARD ALT_FORCE_INLINE Result<SysTime, Error> PerformanceGetCurrentProcessSysTime() noexcept {

    _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    return instance.PerformanceGetCurrentProcessTimes();
}

ALT_NODISCARD ALT_FORCE_INLINE Error Error::PlatformLastError(const Optional<usize> errCode) noexcept {

    Error err{};

    if (errCode) {

        err.m_ErrCode = *errCode;
    }
    else {

        err.m_ErrCode = _PlatformDetails::GetPlatformInstance().GetLastError();
    }

    return err;
}

ALT_NODISCARD inline bool PathOffset::CloneIntoAppend(String& buffer) const noexcept {

    const _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

    return instance.Utf16BufferCloneIntoAppend(*this, buffer);
}

ALT_NODISCARD ALT_FORCE_INLINE Error Error::PlatformNtError(const _PlatformDetails::NTSTATUS ntstatus) noexcept {

    Error err{};
    err.m_ErrCode = _PlatformDetails::GetPlatformInstance().GetNtStatusToError(ntstatus);

    return err;
}

ALT_NODISCARD ALT_FORCE_INLINE Error Error::PlatformCommDlgExtendedError(const usize dlgError) noexcept {

    // 0 - mean success
    ALT_ASSERT(dlgError != __Win32__CDERR_GENERALCODES);

    Error err{};

    switch (dlgError) {
    case __Win32__CDERR_DIALOGFAILURE: {
        err.m_ErrCode = static_cast<usize>(Error::Type::CDERR_DIALOGFAILURE_);
        break;
    }
    case __Win32__CDERR_STRUCTSIZE: {
        err.m_ErrCode = static_cast<usize>(Error::Type::CDERR_STRUCTSIZE_);
        break;
    }
    case __Win32__CDERR_INITIALIZATION: {
        err.m_ErrCode = static_cast<usize>(Error::Type::CDERR_INITIALIZATION_);
        break;
    }
    case __Win32__CDERR_NOTEMPLATE: {
        err.m_ErrCode = static_cast<usize>(Error::Type::CDERR_NOTEMPLATE_);
        break;
    }
    case __Win32__CDERR_NOHINSTANCE: {
        err.m_ErrCode = static_cast<usize>(Error::Type::CDERR_NOHINSTANCE_);
        break;
    }
    case __Win32__CDERR_LOADSTRFAILURE: {
        err.m_ErrCode = static_cast<usize>(Error::Type::CDERR_LOADSTRFAILURE_);
        break;
    }
    case __Win32__CDERR_FINDRESFAILURE: {
        err.m_ErrCode = static_cast<usize>(Error::Type::CDERR_FINDRESFAILURE_);
        break;
    }
    case __Win32__CDERR_LOADRESFAILURE: {
        err.m_ErrCode = static_cast<usize>(Error::Type::CDERR_LOADRESFAILURE_);
        break;
    }
    case __Win32__CDERR_LOCKRESFAILURE: {
        err.m_ErrCode = static_cast<usize>(Error::Type::CDERR_LOCKRESFAILURE_);
        break;
    }
    case __Win32__CDERR_MEMALLOCFAILURE: {
        err.m_ErrCode = static_cast<usize>(Error::Type::CDERR_MEMALLOCFAILURE_);
        break;
    }
    case __Win32__CDERR_MEMLOCKFAILURE: {
        err.m_ErrCode = static_cast<usize>(Error::Type::CDERR_MEMLOCKFAILURE_);
        break;
    }
    case __Win32__CDERR_NOHOOK: {
        err.m_ErrCode = static_cast<usize>(Error::Type::CDERR_NOHOOK_);
        break;
    }
    case __Win32__CDERR_REGISTERMSGFAIL: {
        err.m_ErrCode = static_cast<usize>(Error::Type::CDERR_REGISTERMSGFAIL_);
        break;
    }
    default: {
        err.m_ErrCode = static_cast<usize>(Error::Type::CDERR_DIALOGFAILURE_);
    }
    }

    return err;
}

template <typename FormatWriterT>
ALT_NODISCARD ALT_FORCE_INLINE bool Error::FormatWriteAppend(FormatWriter<FormatWriterT>& writer) const noexcept {

    if (IsPlatformError()) {

        return _PlatformDetails::GetPlatformInstance().GetLastErrorAsFormatWrite(writer);
    }

    const char* str = APPLICATION_ERROR_TO_CSTR[m_ErrCode & ~(static_cast<usize>(1) << 29)];
    return writer.Append(str);
}

// TODO: Revisit - implement separately
class DirectoryWatcher {
public:

    ALT_MAKE_CLASS_NON_COPYABLE(DirectoryWatcher);
    ALT_MAKE_CLASS_NON_MOVABLE(DirectoryWatcher);

    enum class NotifyFilter : usize {

        ChangedFileName = 1 << 0,
        ChangedDirName = 1 << 1,
        ChangedAttributes = 1 << 2,
        ChangedSize = 1 << 3,
        ChangedWrite = 1 << 4,
        ChangedAccess = 1 << 5,
        ChangedCreation = 1 << 6,
        ChangedSecurity = 1 << 7,

        ALL = 
              ChangedFileName
            | ChangedDirName
            | ChangedAttributes
            | ChangedSize
            | ChangedWrite
            | ChangedAccess
            | ChangedCreation
            | ChangedSecurity,
    };
    ALT_ENUM_CLASS_BIT_FLAGS_IN_CLASS_DECL(NotifyFilter);

    struct EventBase {

        enum class Type : usize {
            Added,
            Removed,
            Modified,
            Renamed,
        };

        Type type;
        Slice<const char> filename;
    };

    struct Event : public EventBase {
    };

    struct EventRenamed : public EventBase {

        Slice<const char> newFilename;
    };

    struct Desc {

        _PlatformDetails::LPCWSTR directory = L"/";
        NotifyFilter filter = NotifyFilter::ALL;

        usize bufferSize = Memory::KB(256);

        bool isWatchSubDirectories = true;
    };

    ALT_NODISCARD static Result<void, String> Create(DirectoryWatcher& watcherOut, const Desc& opt) noexcept {
        iferr_scope;

        watcherOut.m_Buffer.Resize(opt.bufferSize, 0) iferr_return "Failed to allocate memory"_salloc;
        return watcherOut.Initialize(opt);
    }

    DirectoryWatcher() noexcept = default;

    ~DirectoryWatcher() noexcept {

        if (m_hDir == __Win32__INVALID_HANDLE_VALUE) {
            return;
        }

        _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

        (*instance.pCancelIoEx)(m_hDir, &m_OverlappedResult);

        (*instance.pCloseHandle)(m_OverlappedResult.hEvent);
        m_OverlappedResult.hEvent = NULL;

        (*instance.pCloseHandle)(m_hDir);
        m_hDir = __Win32__INVALID_HANDLE_VALUE;
    }

    template <typename FuncOnEventT>
    Result<void, String> ProcessEventsNonBlocking(FuncOnEventT&& funcOnEvent) {

        class ProcessEventsNonBlockingCtx : public ProcessEventsNonBlockingCtxBase {
        protected:

            using ProcessEventsNonBlockingCtxBase::ProcessEventsNonBlockingCtxBase;

            void OnEvent(const EventBase& event) noexcept override {

                (*pFuncOnEvent)(event);
            }

        public:

            FuncOnEventT* pFuncOnEvent;
        };

        ProcessEventsNonBlockingCtx ctx{ *this };
        ctx.pFuncOnEvent = &funcOnEvent;

        return ctx.RunProcessEventsNonBlocking();
    }

private:

    Result<void, String> Initialize(const Desc& opt) noexcept {

        _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

        if (!instance.pCancelIoEx) {
            return "Async operations not supported"_salloc;
        }

        m_hDir = (*instance.pCreateFileW)(
            opt.directory,
            __Win32__FILE_LIST_DIRECTORY,
            __Win32__FILE_SHARE_READ | __Win32__FILE_SHARE_WRITE | __Win32__FILE_SHARE_DELETE,
            nullptr,
            __Win32__OPEN_EXISTING,
            __Win32__FILE_FLAG_BACKUP_SEMANTICS | __Win32__FILE_FLAG_OVERLAPPED,
            nullptr
        );

        if (m_hDir == __Win32__INVALID_HANDLE_VALUE) {
            return instance.GetLastErrorAsString();
        }

        m_OverlappedResult.hEvent = (*instance.pCreateEventW)(nullptr, true, false, nullptr);

        if (m_OverlappedResult.hEvent == NULL) {
            return instance.GetLastErrorAsString();
        }

        m_Filter = opt.filter;
        m_IsWatchSubDirectories = opt.isWatchSubDirectories;

        return ReadDirectoryChange();
    }

    Result<void, String> ReadDirectoryChange() noexcept {

        _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();

        _PlatformDetails::DWORD bytesRaed = 0;

        const _PlatformDetails::BOOL isOk = (*instance.pReadDirectoryChangesW)(
            m_hDir,
            m_Buffer.Data(),
            (_PlatformDetails::DWORD)(m_Buffer.GetSize() * sizeof(_PlatformDetails::BYTE)),
            m_IsWatchSubDirectories ? true : false,
            (_PlatformDetails::DWORD)m_Filter,
            &bytesRaed,
            &m_OverlappedResult,
            nullptr
        );

        if (!isOk) {
            return instance.GetLastErrorAsString();
        }

        return Ok;
    }

private:

    class ProcessEventsNonBlockingCtxBase {
    public:

        ProcessEventsNonBlockingCtxBase(DirectoryWatcher& self)
            : m_pSelf(&self)
        {}

        Result<void, String> RunProcessEventsNonBlocking() noexcept {
            iferr_scope;

            _PlatformDetails::PlatformInstance& instance = _PlatformDetails::GetPlatformInstance();
            ALT_ASSERT(m_pSelf->m_hDir != __Win32__INVALID_HANDLE_VALUE);

            String buffer;
            for (;;) {

                if ((*instance.pWaitForSingleObject)(m_pSelf->m_OverlappedResult.hEvent, 0) != __Win32__WAIT_OBJECT_0) {
                    break;
                }

                _PlatformDetails::DWORD bytesReturned = 0;
                _PlatformDetails::BOOL isOk = (*instance.pGetOverlappedResult)(
                    m_pSelf->m_hDir,
                    &m_pSelf->m_OverlappedResult,
                    &bytesReturned,
                    false
                );

                if (!isOk) {

                    buffer = instance.GetLastErrorAsString();
                    m_pSelf->ReadDirectoryChange() iferr_return_with(buffer) buffer;

                    return buffer;
                }

                if (bytesReturned == 0) {

                    m_pSelf->ReadDirectoryChange() iferr_return_with(buffer) buffer;
                    break;
                }

                _PlatformDetails::FILE_NOTIFY_INFORMATION* pFni =
                    reinterpret_cast<_PlatformDetails::FILE_NOTIFY_INFORMATION*>(m_pSelf->m_Buffer.Data());

                for (;;) {

                    const Slice<const wchar_t> filename{
                        reinterpret_cast<const wchar_t*>(pFni->FileName),
                        static_cast<usize>(pFni->FileNameLength) / sizeof(wchar_t),
                    };

                    m_pSelf->m_Filename.Clear();
                    m_pSelf->m_Filename.Append(filename.CastToBytes(), Utf::Encoding::Utf16) iferr_return "Failed to allocate string memory"_salloc;

                    switch (pFni->Action) {
                    case __Win32__FILE_ACTION_ADDED: {

                        Event event{};
                        event.type = EventBase::Type::Added;
                        event.filename = m_pSelf->m_Filename;

                        OnEvent(event);

                        break;
                    }
                    case __Win32__FILE_ACTION_REMOVED: {

                        Event event{};
                        event.type = EventBase::Type::Removed;
                        event.filename = m_pSelf->m_Filename;

                        OnEvent(event);

                        break;
                    }
                    case __Win32__FILE_ACTION_MODIFIED: {

                        Event event{};
                        event.type = EventBase::Type::Modified;
                        event.filename = m_pSelf->m_Filename;

                        OnEvent(event);

                        break;
                    }
                    case __Win32__FILE_ACTION_RENAMED_OLD_NAME: {

                        buffer.CloneFrom(m_pSelf->m_Filename) iferr_return "Failed to allocate string memory"_salloc;
                        break;
                    }
                    case __Win32__FILE_ACTION_RENAMED_NEW_NAME: {

                        EventRenamed event{};
                        event.type = EventBase::Type::Renamed;
                        event.filename = buffer;
                        event.newFilename = m_pSelf->m_Filename;

                        OnEvent(event);

                        break;
                    }
                    default: {
                        break;
                    }
                    }

                    if (pFni->NextEntryOffset == 0) {
                        break;
                    }

                    pFni = reinterpret_cast<_PlatformDetails::FILE_NOTIFY_INFORMATION*>(
                        reinterpret_cast<_PlatformDetails::BYTE*>(pFni) + pFni->NextEntryOffset);
                }

                m_pSelf->ReadDirectoryChange() iferr_return_with(buffer) buffer;
            }

            return Ok;
        }

        virtual ~ProcessEventsNonBlockingCtxBase() noexcept
        {}

    protected:

        virtual void OnEvent(const EventBase& event) noexcept = 0;

    private:

        DirectoryWatcher* m_pSelf = nullptr;
    };

private:

    _PlatformDetails::HANDLE m_hDir = __Win32__INVALID_HANDLE_VALUE;
    _PlatformDetails::OVERLAPPED m_OverlappedResult = _PlatformDetails::OVERLAPPED{};

    Vector<_PlatformDetails::BYTE> m_Buffer;
    String m_Filename;

    NotifyFilter m_Filter = NotifyFilter::ALL;

    bool m_IsWatchSubDirectories = true;
};

ALT_ENUM_CLASS_BIT_FLAGS_IMPL(DirectoryWatcher::NotifyFilter);

} // namespace Platform

template <>
class Format<Platform::Error> {
public:

    ALT_FORCE_INLINE static Optional<EstimateExactSizeResult> EstimateExactSize(
        FormatEstimator&, const Slice<const char> parameters, const Platform::Error& err
    ) noexcept {
        iferr_scope;

        if (parameters.IsPopulated()) {
            return Nullopt;
        }

        EstimateExactSizeResult result{};
        result.size = err.FormatEstimateExactSize() iferr_return Nullopt;

        return result;
    }

    template <typename FormatWriterT>
    ALT_FORCE_INLINE static bool WriteAppend(
        FormatWriter<FormatWriterT>& writer, const Slice<const char> parameters, const Platform::Error& err
    ) noexcept {
 
        if (parameters.IsPopulated()) {
            return false;
        }
 
        return err.FormatWriteAppend(writer);
    }
};

template <>
class Format<Platform::CPUArchitecture> {
public:

    ALT_FORCE_INLINE static Optional<EstimateExactSizeResult> EstimateExactSize(
        FormatEstimator& estimator, const Slice<const char> parameters, const Platform::CPUArchitecture arch
    ) noexcept {

        if (parameters.IsEmpty()) {

            EstimateExactSizeResult result{};

            switch (arch) {
                case Platform::CPUArchitecture::Amd64: {
                    result.size = "x64 (AMD or Intel)"_s.GetSize();
                    return result;
                }
                case Platform::CPUArchitecture::Arm: {
                    result.size = "ARM"_s.GetSize();
                    return result;
                }
                case Platform::CPUArchitecture::Arm64: {
                    result.size = "ARM64"_s.GetSize();
                    return result;
                }
                case Platform::CPUArchitecture::IA64: {
                    result.size = "Intel Itanium-based"_s.GetSize();
                    return result;
                }
                case Platform::CPUArchitecture::Intel: {
                    result.size = "x86"_s.GetSize();
                    return result;
                }
                case Platform::CPUArchitecture::Unknown: {
                    result.size = "Unknown"_s.GetSize();
                    return result;
                }
            }

            ALT_UNREACHABLE();
        }

        if (parameters == "digit"_s) {

            return Format<usize>::EstimateExactSize(estimator, ""_s, static_cast<usize>(arch));
        }

        return Nullopt;
    }

    template <typename FormatWriterT>
    ALT_FORCE_INLINE static bool WriteAppend(
        FormatWriter<FormatWriterT>& writer, const Slice<const char> parameters, const Platform::CPUArchitecture arch
    ) noexcept {
 
        if (parameters.IsEmpty()) {

            switch (arch) {
                case Platform::CPUArchitecture::Amd64: {
                    return writer.Append("x64 (AMD or Intel)"_s); 
                }
                case Platform::CPUArchitecture::Arm: {
                    return writer.Append("ARM"_s); 
                }
                case Platform::CPUArchitecture::Arm64: {
                    return writer.Append("ARM64"_s); 
                }
                case Platform::CPUArchitecture::IA64: {
                    return writer.Append("Intel Itanium-based"_s); 
                }
                case Platform::CPUArchitecture::Intel: {
                    return writer.Append("x86"_s); 
                }
                case Platform::CPUArchitecture::Unknown: {
                    return writer.Append("Unknown"_s); 
                }
            }

            ALT_UNREACHABLE();
        }

        if (parameters == "digit"_s) {

            return Format<usize>::WriteAppend(writer, ""_s, static_cast<usize>(arch));
        }

        return false;
    }
};

template <>
class Format<Platform::VTS_t> {
public:

    template <typename FormatWriterT>
    static bool WriteAppend(
        FormatWriter<FormatWriterT>& writer, const Slice<const char> parameters, const Platform::VTS_t
    ) noexcept {
        iferr_scope;

        /*
         * https://learn.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences
         * 
         * <N> - number <= Num::Desc<s16>::Max()
         * 
         * save - save actual state (cursor, colors, ...)
         * load - load previouse state (cursor, colors, ...)
         * 
         * new - scrolls viewport up to align with current cursor Y position
         * 
         * cm= - cursor move
         *     cm=begin - move to the begining of line
         *     cm=end - move to the end of line
         *     cm=top - move to top of the current viewport
         *     cm=bottom - move to bottom of the current viewport
         *     cm=top_begin - move to top and begin (start corner) of the current viewport
         *     cm=up_begin<N> - move up by N and to the begining of line
         *     cm=down_begin<N> - move down by N and to the begining of line
         *     cm=up<N> - move up by N
         *     cm=down<N> - move down by N
         *     cm=left<N> - move left by N
         *     cm=right<N> - move right by N
         *     cm=<N>,<N> - move to position x,y
         * 
         * cl= - clear
         *     cl=whole - clear whole viewport
         *     cl=before_cursor - clear viewport from begining to and including cursor position
         *     cl=after_cursor - clear viewport from cursor (inclusive) position to the end of line
         *     cl=whole_line - clear whole line
         *     cl=before_cursor_line - clear line from begining to and including cursor position
         *     cl=after_cursor_line - clear line from cursor (inclusive) position to the end of line
         * 
         * cs= - cursor style
         *     cs=default - default cursor style
         *     cs=steady - non-blinking cursor
         *     cs=blink - blinking cursor
         *     cs=show - show cursor
         *     cs=hide - hide cursor
         *     cs=block - block cursor
         *     cs=block_blink - block blinking
         *     cs=underline - underline cursor
         *     cs=underline_blink - underline blinking
         *     cs=bar - bar cursor
         *     cs=bar_blink - bar blinking
         * 
         * default - reset to default settings below (can be also empty)
         * 
         * ft= - font
         *     ft=bold - bold font
         *     ft=no_bold - no bold font (default)
         *     ft=underline - adds underline
         *     ft=no_underline - no underline
         * 
         * fg= - foreground
         *     fg=black
         *     fg=red
         *     fg=green
         *     fg=yellow
         *     fg=blue
         *     fg=magneta
         *     fg=cyan
         *     fg=white
         *     fg=black_bold
         *     fg=red_bold
         *     fg=green_bold
         *     fg=yellow_bold
         *     fg=blue_bold
         *     fg=magneta_bold
         *     fg=cyan_bold
         *     fg=white_bold
         *     fg=<N>,<N>,<N> - custom r,g,b foreground color (<N> <= 255)
         *     fg=default - default foreground color
         * 
         * bg= - background
         *     bg=black
         *     bg=red
         *     bg=green
         *     bg=yellow
         *     bg=blue
         *     bg=magneta
         *     bg=cyan
         *     bg=white
         *     bg=black_bold
         *     bg=red_bold
         *     bg=green_bold
         *     bg=yellow_bold
         *     bg=blue_bold
         *     bg=magneta_bold
         *     bg=cyan_bold
         *     bg=white_bold
         *     bg=<N>,<N>,<N> - custom r,g,b background color (<N> <= 255)
         *     bg=default - default background color
         * 
         * @example
         * 
         * ALT_PRINTLN("{fg=green_bold}Text{default}", ALT_VTS, ALT_VTS)
         */

        if (parameters.IsEmpty()) {

            return writer.Append("\x1b[0m");
        }

        bool isSGR = false;

        SplitIterator<const char> splitIterator = parameters.GetSplitIterator(";"_s);
        while (Optional<Slice<const char>> split_ = splitIterator.Next()) {

            Slice<const char> split = *split_;
            if (split.IsEmpty()) {
                continue;
            }

            if (split == "save"_s) {

                if (isSGR) {

                    isSGR = false;
                    writer.Append('m') iferr_return false;
                }

                writer.Append("\x1b""7") iferr_return false;
                continue;
            }

            if (split == "load"_s) {

                if (isSGR) {

                    isSGR = false;
                    writer.Append('m') iferr_return false;
                }

                writer.Append("\x1b""8") iferr_return false;
                continue;
            }

            if (split == "new"_s) {

                const usize crpY = _PlatformDetails::GetPlatformInstance().ConsoleGetCursorPosYInChars() iferr_return false;
                if (crpY == 0) {
                    continue;
                }

                if (isSGR) {

                    isSGR = false;
                    writer.Append('m') iferr_return false;
                }

                for (usize i = 0; i < crpY; ++i) {
                    writer.Append('\n') iferr_return false;
                }

                writer.Append("\x1b[d") iferr_return false;

                //writer.Append("\x1b[") iferr_return false;
                //Format<usize>::WriteAppend(writer, ""_s, static_cast<u16>(crpY)) iferr_return false;
                //writer.Append('S') iferr_return false;

                continue;
            }

            if (split == "cm=begin"_s) {

                if (isSGR) {

                    isSGR = false;
                    writer.Append('m') iferr_return false;
                }

                writer.Append("\x1b[G") iferr_return false;
                continue;
            }

            if (split == "cm=end"_s) {

                if (isSGR) {

                    isSGR = false;
                    writer.Append('m') iferr_return false;
                }

                const Pair<usize, usize> sizeInChars = Platform::ConsoleGetSizeInChars() iferr_return false;

                writer.Append("\x1b[") iferr_return false;
                Format<usize>::WriteAppend(writer, ""_s, static_cast<u16>(sizeInChars.f())) iferr_return false;
                writer.Append('G') iferr_return false;

                continue;
            }

            if (split == "cm=top"_s) {

                if (isSGR) {

                    isSGR = false;
                    writer.Append('m') iferr_return false;
                }

                writer.Append("\x1b[d") iferr_return false;
                continue;
            }

            if (split == "cm=bottom"_s) {

                if (isSGR) {

                    isSGR = false;
                    writer.Append('m') iferr_return false;
                }

                const Pair<usize, usize> sizeInChars = Platform::ConsoleGetSizeInChars() iferr_return false;

                writer.Append("\x1b[") iferr_return false;
                Format<usize>::WriteAppend(writer, ""_s, static_cast<u16>(sizeInChars.s())) iferr_return false;
                writer.Append('d') iferr_return false;

                continue;
            }

            if (split == "cm=top_begin"_s) {

                if (isSGR) {

                    isSGR = false;
                    writer.Append('m') iferr_return false;
                }

                writer.Append("\x1b[0;0H") iferr_return false;

                continue;
            }

            if (split.StartWith("cm=up_begin"_s)) {

                if (isSGR) {

                    isSGR = false;
                    writer.Append('m') iferr_return false;
                }

                split = split.SubSlice(11);

                if (split.IsEmpty()) {

                    writer.Append("\x1b[F") iferr_return false;
                }
                else {

                    if (!split.StartWith('0')) {

                        writer.Append("\x1b[") iferr_return false;
                        writer.Append(split) iferr_return false;
                        writer.Append('F') iferr_return false;
                    }
                }

                continue;
            }

            if (split.StartWith("cm=down_begin"_s)) {

                if (isSGR) {

                    isSGR = false;
                    writer.Append('m') iferr_return false;
                }

                split = split.SubSlice(13);

                if (split.IsEmpty()) {

                    writer.Append("\x1b[E") iferr_return false;
                }
                else {

                    if (!split.StartWith('0')) {

                        writer.Append("\x1b[") iferr_return false;
                        writer.Append(split) iferr_return false;
                        writer.Append('E') iferr_return false;
                    }
                }

                continue;
            }

            if (split.StartWith("cm=up"_s)) {

                if (isSGR) {

                    isSGR = false;
                    writer.Append('m') iferr_return false;
                }

                split = split.SubSlice(5);

                if (split.IsEmpty()) {

                    writer.Append("\x1b[A") iferr_return false;
                }
                else {

                    if (!split.StartWith('0')) {

                        writer.Append("\x1b[") iferr_return false;
                        writer.Append(split) iferr_return false;
                        writer.Append('A') iferr_return false;
                    }
                }

                continue;
            }

            if (split.StartWith("cm=down"_s)) {

                if (isSGR) {

                    isSGR = false;
                    writer.Append('m') iferr_return false;
                }

                split = split.SubSlice(7);

                if (split.IsEmpty()) {

                    writer.Append("\x1b[B") iferr_return false;
                }
                else {

                    if (!split.StartWith('0')) {

                        writer.Append("\x1b[") iferr_return false;
                        writer.Append(split) iferr_return false;
                        writer.Append('B') iferr_return false;
                    }
                }

                continue;
            }

            if (split.StartWith("cm=left"_s)) {

                if (isSGR) {

                    isSGR = false;
                    writer.Append('m') iferr_return false;
                }

                split = split.SubSlice(7);

                if (split.IsEmpty()) {

                    writer.Append("\x1b[D") iferr_return false;
                }
                else {

                    if (!split.StartWith('0')) {

                        writer.Append("\x1b[") iferr_return false;
                        writer.Append(split) iferr_return false;
                        writer.Append('D') iferr_return false;
                    }
                }

                continue;
            }

            if (split.StartWith("cm=right"_s)) {

                if (isSGR) {

                    isSGR = false;
                    writer.Append('m') iferr_return false;
                }

                split = split.SubSlice(8);

                if (split.IsEmpty()) {

                    writer.Append("\x1b[C") iferr_return false;
                }
                else {

                    if (!split.StartWith('0')) {

                        writer.Append("\x1b[") iferr_return false;
                        writer.Append(split) iferr_return false;
                        writer.Append('C') iferr_return false;
                    }
                }

                continue;
            }

            if (split.StartWith("cm="_s)) {

                split = split.SubSlice(3);
                if (split.IsEmpty()) {
                    return false;
                }

                SplitIterator<const char> digitIterator = split.GetSplitIterator(",");

                const Slice<const char> x = digitIterator.Next() iferr_return false;
                const Slice<const char> y = digitIterator.Next() iferr_return false;

                if (isSGR) {

                    isSGR = false;
                    writer.Append('m') iferr_return false;
                }

                writer.Append("\x1b[") iferr_return false;
                writer.Append(y) iferr_return false;
                writer.Append(';') iferr_return false;
                writer.Append(x) iferr_return false;
                writer.Append('H') iferr_return false;

                continue;
            }

            if (split == "cs=default"_s) {

                if (isSGR) {

                    isSGR = false;
                    writer.Append('m') iferr_return false;
                }

                writer.Append("\x1b[0\x20q") iferr_return false;
                continue;
            }

            if (split == "cs=steady"_s) {

                if (isSGR) {

                    isSGR = false;
                    writer.Append('m') iferr_return false;
                }

                writer.Append("\x1b[?12l") iferr_return false;
                continue;
            }

            if (split == "cs=blink"_s) {

                if (isSGR) {

                    isSGR = false;
                    writer.Append('m') iferr_return false;
                }

                writer.Append("\x1b[?12h") iferr_return false;
                continue;
            }

            if (split == "cs=show"_s) {

                if (isSGR) {

                    isSGR = false;
                    writer.Append('m') iferr_return false;
                }

                writer.Append("\x1b[?25h") iferr_return false;
                continue;
            }

            if (split == "cs=hide"_s) {

                if (isSGR) {

                    isSGR = false;
                    writer.Append('m') iferr_return false;
                }

                writer.Append("\x1b[?25l") iferr_return false;
                continue;
            }

            if (split == "cs=block"_s) {

                if (isSGR) {

                    isSGR = false;
                    writer.Append('m') iferr_return false;
                }

                writer.Append("\x1b[2\x20q") iferr_return false;
                continue;
            }

            if (split == "cs=block_blink"_s) {

                if (isSGR) {

                    isSGR = false;
                    writer.Append('m') iferr_return false;
                }

                writer.Append("\x1b[1\x20q") iferr_return false;
                continue;
            }

            if (split == "cs=underline"_s) {

                if (isSGR) {

                    isSGR = false;
                    writer.Append('m') iferr_return false;
                }

                writer.Append("\x1b[4\x20q") iferr_return false;
                continue;
            }

            if (split == "cs=underline_blink"_s) {

                if (isSGR) {

                    isSGR = false;
                    writer.Append('m') iferr_return false;
                }

                writer.Append("\x1b[3\x20q") iferr_return false;
                continue;
            }

            if (split == "cs=bar"_s) {

                if (isSGR) {

                    isSGR = false;
                    writer.Append('m') iferr_return false;
                }

                writer.Append("\x1b[6\x20q") iferr_return false;
                continue;
            }

            if (split == "cs=bar_blink"_s) {

                if (isSGR) {

                    isSGR = false;
                    writer.Append('m') iferr_return false;
                }

                writer.Append("\x1b[5\x20q") iferr_return false;
                continue;
            }

            if (split == "cl=whole"_s) {

                if (isSGR) {

                    isSGR = false;
                    writer.Append('m') iferr_return false;
                }

                writer.Append("\x1b[2J") iferr_return false;
                continue;
            }

            if (split == "cl=before_cursor"_s) {

                if (isSGR) {

                    isSGR = false;
                    writer.Append('m') iferr_return false;
                }

                writer.Append("\x1b[0J") iferr_return false;
                continue;
            }

            if (split == "cl=after_cursor"_s) {

                if (isSGR) {

                    isSGR = false;
                    writer.Append('m') iferr_return false;
                }

                writer.Append("\x1b[1J") iferr_return false;
                continue;
            }


            if (split == "cl=whole_line"_s) {

                if (isSGR) {

                    isSGR = false;
                    writer.Append('m') iferr_return false;
                }

                writer.Append("\x1b[2K") iferr_return false;
                continue;
            }

            if (split == "cl=before_cursor_line"_s) {

                if (isSGR) {

                    isSGR = false;
                    writer.Append('m') iferr_return false;
                }

                writer.Append("\x1b[0K") iferr_return false;
                continue;
            }

            if (split == "cl=after_cursor_line"_s) {

                if (isSGR) {

                    isSGR = false;
                    writer.Append('m') iferr_return false;
                }

                writer.Append("\x1b[1K") iferr_return false;
                continue;
            }

            if (split.StartWith("cl=until"_s)) {

                if (isSGR) {

                    isSGR = false;
                    writer.Append('m') iferr_return false;
                }

                split = split.SubSlice(8);

                if (split.IsEmpty()) {

                    writer.Append("\x1b[X") iferr_return false;
                }
                else {

                    writer.Append("\x1b[") iferr_return false;
                    writer.Append(split) iferr_return false;
                    writer.Append('X') iferr_return false;
                }

                continue;
            }

            if (split == "default"_s) {

                if (isSGR) {

                    writer.Append(";0") iferr_return false;
                }
                else {

                    writer.Append("\x1b[0") iferr_return false;
                }

                isSGR = true;
                continue;
            }

            if (split == "ft=bold"_s) {

                if (isSGR) {

                    writer.Append(";1") iferr_return false;
                }
                else {

                    writer.Append("\x1b[1") iferr_return false;
                }

                isSGR = true;
                continue;
            }

            if (split == "ft=no_bold"_s) {

                if (isSGR) {

                    writer.Append(";22") iferr_return false;
                }
                else {

                    writer.Append("\x1b[22") iferr_return false;
                }

                isSGR = true;
                continue;
            }

            if (split == "ft=underline"_s) {

                if (isSGR) {

                    writer.Append(";4") iferr_return false;
                }
                else {

                    writer.Append("\x1b[4") iferr_return false;
                }

                isSGR = true;
                continue;
            }

            if (split == "ft=no_underline"_s) {

                if (isSGR) {

                    writer.Append(";24") iferr_return false;
                }
                else {

                    writer.Append("\x1b[24") iferr_return false;
                }

                isSGR = true;
                continue;
            }

            if (split == "fg=black"_s) {

                if (isSGR) {

                    writer.Append(";30") iferr_return false;
                }
                else {

                    writer.Append("\x1b[30") iferr_return false;
                }

                isSGR = true;
                continue;
            }

            if (split == "fg=red"_s) {

                if (isSGR) {

                    writer.Append(";31") iferr_return false;
                }
                else {

                    writer.Append("\x1b[31") iferr_return false;
                }

                isSGR = true;
                continue;
            }

            if (split == "fg=green"_s) {

                if (isSGR) {

                    writer.Append(";32") iferr_return false;
                }
                else {

                    writer.Append("\x1b[32") iferr_return false;
                }

                isSGR = true;
                continue;
            }

            if (split == "fg=yellow"_s) {

                if (isSGR) {

                    writer.Append(";33") iferr_return false;
                }
                else {

                    writer.Append("\x1b[33") iferr_return false;
                }

                isSGR = true;
                continue;
            }

            if (split == "fg=blue"_s) {

                if (isSGR) {

                    writer.Append(";34") iferr_return false;
                }
                else {

                    writer.Append("\x1b[34") iferr_return false;
                }

                isSGR = true;
                continue;
            }

            if (split == "fg=magneta"_s) {

                if (isSGR) {

                    writer.Append(";35") iferr_return false;
                }
                else {

                    writer.Append("\x1b[35") iferr_return false;
                }

                isSGR = true;
                continue;
            }

            if (split == "fg=cyan"_s) {

                if (isSGR) {

                    writer.Append(";36") iferr_return false;
                }
                else {

                    writer.Append("\x1b[36") iferr_return false;
                }

                isSGR = true;
                continue;
            }

            if (split == "fg=white"_s) {

                if (isSGR) {

                    writer.Append(";37") iferr_return false;
                }
                else {

                    writer.Append("\x1b[37") iferr_return false;
                }

                isSGR = true;
                continue;
            }

            if (split == "fg=black_bold"_s) {

                if (isSGR) {

                    writer.Append(";90") iferr_return false;
                }
                else {

                    writer.Append("\x1b[90") iferr_return false;
                }

                isSGR = true;
                continue;
            }

            if (split == "fg=red_bold"_s) {

                if (isSGR) {

                    writer.Append(";91") iferr_return false;
                }
                else {

                    writer.Append("\x1b[91") iferr_return false;
                }

                isSGR = true;
                continue;
            }

            if (split == "fg=green_bold"_s) {

                if (isSGR) {

                    writer.Append(";92") iferr_return false;
                }
                else {

                    writer.Append("\x1b[92") iferr_return false;
                }

                isSGR = true;
                continue;
            }

            if (split == "fg=yellow_bold"_s) {

                if (isSGR) {

                    writer.Append(";93") iferr_return false;
                }
                else {

                    writer.Append("\x1b[93") iferr_return false;
                }

                isSGR = true;
                continue;
            }

            if (split == "fg=blue_bold"_s) {

                if (isSGR) {

                    writer.Append(";94") iferr_return false;
                }
                else {

                    writer.Append("\x1b[94") iferr_return false;
                }

                isSGR = true;
                continue;
            }

            if (split == "fg=magneta_bold"_s) {

                if (isSGR) {

                    writer.Append(";95") iferr_return false;
                }
                else {

                    writer.Append("\x1b[95") iferr_return false;
                }

                isSGR = true;
                continue;
            }

            if (split == "fg=cyan_bold"_s) {

                if (isSGR) {

                    writer.Append(";96") iferr_return false;
                }
                else {

                    writer.Append("\x1b[96") iferr_return false;
                }

                isSGR = true;
                continue;
            }

            if (split == "fg=white_bold"_s) {

                if (isSGR) {

                    writer.Append(";97") iferr_return false;
                }
                else {

                    writer.Append("\x1b[97") iferr_return false;
                }

                isSGR = true;
                continue;
            }

            if (split == "fg=default"_s) {

                if (isSGR) {

                    writer.Append(";39") iferr_return false;
                }
                else {

                    writer.Append("\x1b[39") iferr_return false;
                }

                isSGR = true;
                continue;
            }

            if (split.StartWith("fg=")) {

                split = split.SubSlice(3);
                if (split.IsEmpty()) {
                    return false;
                }

                SplitIterator<const char> digitIterator = split.GetSplitIterator(",");

                const Slice<const char> r = digitIterator.Next() iferr_return false;
                const Slice<const char> g = digitIterator.Next() iferr_return false;
                const Slice<const char> b = digitIterator.Next() iferr_return false;

                if (isSGR) {

                    writer.Append(';') iferr_return false;
                }
                else {

                    writer.Append("\x1b[") iferr_return false;
                }

                writer.Append("38;2;") iferr_return false;
                writer.Append(r) iferr_return false;
                writer.Append(';') iferr_return false;
                writer.Append(g) iferr_return false;
                writer.Append(';') iferr_return false;
                writer.Append(b) iferr_return false;

                isSGR = true;
                continue;
            }

            if (split == "bg=black"_s) {

                if (isSGR) {

                    writer.Append(";40") iferr_return false;
                }
                else {

                    writer.Append("\x1b[40") iferr_return false;
                }

                isSGR = true;
                continue;
            }

            if (split == "bg=red"_s) {

                if (isSGR) {

                    writer.Append(";41") iferr_return false;
                }
                else {

                    writer.Append("\x1b[41") iferr_return false;
                }

                isSGR = true;
                continue;
            }

            if (split == "bg=green"_s) {

                if (isSGR) {

                    writer.Append(";42") iferr_return false;
                }
                else {

                    writer.Append("\x1b[42") iferr_return false;
                }

                isSGR = true;
                continue;
            }

            if (split == "bg=yellow"_s) {

                if (isSGR) {

                    writer.Append(";43") iferr_return false;
                }
                else {

                    writer.Append("\x1b[43") iferr_return false;
                }

                isSGR = true;
                continue;
            }

            if (split == "bg=blue"_s) {

                if (isSGR) {

                    writer.Append(";44") iferr_return false;
                }
                else {

                    writer.Append("\x1b[44") iferr_return false;
                }

                isSGR = true;
                continue;
            }

            if (split == "bg=magneta"_s) {

                if (isSGR) {

                    writer.Append(";45") iferr_return false;
                }
                else {

                    writer.Append("\x1b[45") iferr_return false;
                }

                isSGR = true;
                continue;
            }

            if (split == "bg=cyan"_s) {

                if (isSGR) {

                    writer.Append(";46") iferr_return false;
                }
                else {

                    writer.Append("\x1b[46") iferr_return false;
                }

                isSGR = true;
                continue;
            }

            if (split == "bg=white"_s) {

                if (isSGR) {

                    writer.Append(";47") iferr_return false;
                }
                else {

                    writer.Append("\x1b[47") iferr_return false;
                }

                isSGR = true;
                continue;
            }

            if (split == "bg=black_bold"_s) {

                if (isSGR) {

                    writer.Append(";100") iferr_return false;
                }
                else {

                    writer.Append("\x1b[100") iferr_return false;
                }

                isSGR = true;
                continue;
            }

            if (split == "bg=red_bold"_s) {

                if (isSGR) {

                    writer.Append(";101") iferr_return false;
                }
                else {

                    writer.Append("\x1b[101") iferr_return false;
                }

                isSGR = true;
                continue;
            }

            if (split == "bg=green_bold"_s) {

                if (isSGR) {

                    writer.Append(";102") iferr_return false;
                }
                else {

                    writer.Append("\x1b[102") iferr_return false;
                }

                isSGR = true;
                continue;
            }

            if (split == "bg=yellow_bold"_s) {

                if (isSGR) {

                    writer.Append(";103") iferr_return false;
                }
                else {

                    writer.Append("\x1b[103") iferr_return false;
                }

                isSGR = true;
                continue;
            }

            if (split == "bg=blue_bold"_s) {

                if (isSGR) {

                    writer.Append(";104") iferr_return false;
                }
                else {

                    writer.Append("\x1b[104") iferr_return false;
                }

                isSGR = true;
                continue;
            }

            if (split == "bg=magneta_bold"_s) {

                if (isSGR) {

                    writer.Append(";105") iferr_return false;
                }
                else {

                    writer.Append("\x1b[105") iferr_return false;
                }

                isSGR = true;
                continue;
            }

            if (split == "bg=cyan_bold"_s) {

                if (isSGR) {

                    writer.Append(";106") iferr_return false;
                }
                else {

                    writer.Append("\x1b[106") iferr_return false;
                }

                isSGR = true;
                continue;
            }

            if (split == "bg=white_bold"_s) {

                if (isSGR) {

                    writer.Append(";107") iferr_return false;
                }
                else {

                    writer.Append("\x1b[107") iferr_return false;
                }

                isSGR = true;
                continue;
            }

            if (split == "bg=default"_s) {

                if (isSGR) {

                    writer.Append(";49") iferr_return false;
                }
                else {

                    writer.Append("\x1b[49") iferr_return false;
                }

                isSGR = true;
                continue;
            }

            if (split.StartWith("bg=")) {

                split = split.SubSlice(3);
                if (split.IsEmpty()) {
                    return false;
                }

                SplitIterator<const char> digitIterator = split.GetSplitIterator(",");

                const Slice<const char> r = digitIterator.Next() iferr_return false;
                const Slice<const char> g = digitIterator.Next() iferr_return false;
                const Slice<const char> b = digitIterator.Next() iferr_return false;

                if (isSGR) {

                    writer.Append(';') iferr_return false;
                }
                else {

                    writer.Append("\x1b[") iferr_return false;
                }

                writer.Append("48;2;") iferr_return false;
                writer.Append(r) iferr_return false;
                writer.Append(';') iferr_return false;
                writer.Append(g) iferr_return false;
                writer.Append(';') iferr_return false;
                writer.Append(b) iferr_return false;

                isSGR = true;
                continue;
            }

            return false;
        }

        if (isSGR) {

            isSGR = false;
            writer.Append('m') iferr_return false;
        }

        return true;
    }
};

} // namespace Alt

#ifdef ALT_COMPILER_CLANG
#pragma clang diagnostic pop
#endif// ALT_COMPILER_CLANG

#endif // ALT_OS_WINDOWS

#endif // __ALT_STD_PLATFORM_WIN32__