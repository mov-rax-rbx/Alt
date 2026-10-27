#include "pch.h"

#define ALT_USE_SCALAR_TYPES_IN_GLOBAL_NAMESPACE
#define ALT_USE_MATH_TYPES_IN_GLOBAL_NAMESPACE

#include "../alt-std/src/Platform/Platform.h"
#include "../alt-std/src/Random/Xoshiro256.h"
#include "../alt-std/src/Math/Math.h"

TEST(Platform, ConsoleTitle) {

	Alt::Platform::Error err;
	iferr_scope_on_err {

		if (err.IsErr()) ALT_PRINTLN("{}", err);
		ALT_BREAKPOINT;

		FAIL();
	};

	Alt::String title = Alt::Platform::ConsoleGetTitle() iferr_return_with(err);
	ALT_PRINTLN("{}", title);

	/*
	 * On 'ALT_PLATFORM_X86_32', MSVC debug, line below lead to crash.
	 */
	Alt::Platform::ConsoleSetTitle("Alt - название консольки") iferr_return_with(err);
}

TEST(Platform, Info) {

	ALT_PRINTLN("System Info:");
	ALT_PRINTLN("   - Page Size: {} bytes", Alt::Platform::GetPageSizeInBytes());
	ALT_PRINTLN("   - Page Align: {}", Alt::Platform::GetPageAlign());
	ALT_PRINTLN("   - Num Cores: {}", Alt::Platform::ThreadGetNumOfLogicalProcessors());
	ALT_PRINTLN("   - Cpu Architecture: {}", Alt::Platform::GetCpuArchitecture());
}

TEST(Platform, SystemTime) {

	Alt::Platform::Error err;
	iferr_scope_on_err {

		if (err.IsErr()) ALT_PRINTLN("{}", err);
		ALT_BREAKPOINT;

		FAIL();
	};

	const Alt::Time timeUTC = Alt::Platform::TimeGetSystemTimeUTC();
	ALT_PRINTLN("{}", Alt::DateTime::FromTime(timeUTC));

	const Alt::Time timeLocalTime = Alt::Platform::TimeGetSystemTimeLocalTime() iferr_return_with(err);
	ALT_PRINTLN("{}", Alt::DateTime::FromTime(timeLocalTime));
}

TEST(Platform, GetTick) {

	const u64 tick0 = Alt::Platform::GetTicks();

	Alt::Platform::SleepMs(1);

	const u64 tick1 = Alt::Platform::GetTicks();
	EXPECT_GT(tick1, tick0);
}

TEST(Platform, DirectoryWatcher) {

	Alt::String err;
	iferr_scope_on_err {

		ALT_EPRINTLN(err.CStr());
		FAIL();
	};

	Alt::Platform::DirectoryWatcher::Desc desc{};
	desc.directory = L"/";
	desc.filter = Alt::Platform::DirectoryWatcher::NotifyFilter::ChangedCreation;

	Alt::Platform::DirectoryWatcher watcher;
	Alt::Platform::DirectoryWatcher::Create(watcher, desc) iferr_return_with(err);

	watcher.ProcessEventsNonBlocking([&](const Alt::Platform::DirectoryWatcher::EventBase& event) {

		if (event.type == Alt::Platform::DirectoryWatcher::EventBase::Type::Modified) {

			if (event.filename == "VertexShader.vert" || event.filename == "FragmentShader.frag") {
			}
		}
	}) iferr_return_with(err);
}

TEST(Platform, FileSystem_GetCommonDirectories) {

	Alt::Platform::Error err;
	iferr_scope_on_err {

		ALT_PRINTLN("{}", err);
		FAIL();
	};

	const Alt::String tempPath = Alt::Platform::FileGetTempDirectory() iferr_return_with(err);
	const Alt::String currentPath = Alt::Platform::FileGetCurrentDirectory() iferr_return_with(err);

	ALT_PRINTLN("{}", tempPath);
	ALT_PRINTLN("{}", currentPath);

	const Alt::String absoluteTempPath = Alt::Platform::FilePathAbsolute(tempPath) iferr_return_with(err);
	const Alt::String absoluteCurrentPath = Alt::Platform::FilePathAbsolute(currentPath) iferr_return_with(err);

	EXPECT_EQ(tempPath, absoluteTempPath);
	EXPECT_EQ(currentPath, absoluteCurrentPath);
}

TEST(Platform, FileSystem_CreateFiles) {

	Alt::Platform::Error err;
	iferr_scope_on_err {

		if (err.IsErr()) ALT_PRINTLN("{}", err);
		ALT_BREAKPOINT;

		FAIL();
	};

	const Alt::Slice<const char> testDir = "test\\FileSystem_CreateFiles"_s;
	Alt::String currentPath = Alt::Platform::FileGetCurrentDirectory() iferr_return_with(err);

	Alt::String directory = currentPath.Clone() iferr_return;
	directory.FormatAppend("{}{}{}", ALT_DS, testDir, ALT_DS) iferr_return;

	bool isExist = Alt::Platform::FileExistAny(directory) iferr_return_with(err);
	EXPECT_FALSE(isExist);

	Alt::Platform::FileDirectoryCreate(directory) iferr_return_with(err);
	defer {
		iferr_scope;

		Alt::String dir = Alt::Platform::FileGetCurrentDirectory() iferr_return;
		dir.Append("\\test") iferr_return;

		Alt::Platform::FileRemoveAll(dir, Alt::Platform::FileRemoveAllType::All) iferr_return;
	};

	isExist = Alt::Platform::FileExistAny(directory) iferr_return_with(err);
	EXPECT_TRUE(isExist);

	{
		const Alt::Platform::FileTypeBits fileType = Alt::Platform::FileGetFileType(directory) iferr_return_with(err);
		EXPECT_FALSE(Alt::BitFlag::IsSetAnyEnumFlags(fileType));
	}

	{
		const Alt::Platform::FileTypeBits fileType = Alt::Platform::FileGetType(directory) iferr_return_with(err);
		EXPECT_FALSE(Alt::BitFlag::IsSetEnumFlags(fileType, Alt::Platform::FileTypeBits::File));
		EXPECT_FALSE(Alt::BitFlag::IsSetEnumFlags(fileType, Alt::Platform::FileTypeBits::RegularFile));
		EXPECT_FALSE(Alt::BitFlag::IsSetEnumFlags(fileType, Alt::Platform::FileTypeBits::Char));
		EXPECT_FALSE(Alt::BitFlag::IsSetEnumFlags(fileType, Alt::Platform::FileTypeBits::Pipe));

		EXPECT_TRUE(Alt::BitFlag::IsSetEnumFlags(fileType, Alt::Platform::FileTypeBits::Directory));

		EXPECT_FALSE(Alt::BitFlag::IsSetEnumFlags(fileType, Alt::Platform::FileTypeBits::SymLink));
		EXPECT_FALSE(Alt::BitFlag::IsSetEnumFlags(fileType, Alt::Platform::FileTypeBits::Junction));
	}

	currentPath.FormatAppend("{}{}{}text.txt", ALT_DS, testDir, ALT_DS) iferr_return;

	{
		Alt::Platform::Handle hFile = Alt::Platform::FileOpenOrCreate(
			currentPath,
			Alt::Platform::FileOpenOrCreateBits::DeleteByHandle,
			Alt::Platform::FileShareBits::Read,
			Alt::Platform::FileAccessBits::Write,
			Alt::Platform::FileHintBits::Temporary
		) iferr_return_with(err);

		{
			const bool isExist = Alt::Platform::FileExistAny(currentPath) iferr_return_with(err);
			EXPECT_TRUE(isExist);
		}

		{
			const bool isSame = Alt::Platform::FileSame(currentPath, currentPath) iferr_return_with(err);
			EXPECT_TRUE(isSame);
		}

		{
			Alt::String canonicalPath = Alt::Platform::FilePathCanonical(currentPath) iferr_return_with(err);
			EXPECT_EQ(canonicalPath, currentPath);
		}

		{
			usize sizeBytes = Alt::Platform::FileSize(hFile) iferr_return_with(err);
			EXPECT_EQ(sizeBytes, 0);

			Alt::Platform::FileResize(hFile, 32) iferr_return_with(err);

			sizeBytes = Alt::Platform::FileSize(hFile) iferr_return_with(err);
			EXPECT_EQ(sizeBytes, 32);
		}

		{
			const Alt::Platform::FileTypeBits fileType = Alt::Platform::FileGetFileType(hFile);
			EXPECT_TRUE(Alt::BitFlag::IsSetEnumFlags(fileType, Alt::Platform::FileTypeBits::File));
			EXPECT_TRUE(Alt::BitFlag::IsSetEnumFlags(fileType, Alt::Platform::FileTypeBits::RegularFile));
			EXPECT_FALSE(Alt::BitFlag::IsSetEnumFlags(fileType, Alt::Platform::FileTypeBits::Char));
			EXPECT_FALSE(Alt::BitFlag::IsSetEnumFlags(fileType, Alt::Platform::FileTypeBits::Pipe));
		}

		{
			const Alt::Platform::FileTypeBits fileType = Alt::Platform::FileGetType(hFile, true) iferr_return_with(err);
			EXPECT_TRUE(Alt::BitFlag::IsSetEnumFlags(fileType, Alt::Platform::FileTypeBits::File));
			EXPECT_TRUE(Alt::BitFlag::IsSetEnumFlags(fileType, Alt::Platform::FileTypeBits::RegularFile));
			EXPECT_FALSE(Alt::BitFlag::IsSetEnumFlags(fileType, Alt::Platform::FileTypeBits::Char));
			EXPECT_FALSE(Alt::BitFlag::IsSetEnumFlags(fileType, Alt::Platform::FileTypeBits::Pipe));

			EXPECT_FALSE(Alt::BitFlag::IsSetEnumFlags(fileType, Alt::Platform::FileTypeBits::Directory));
			EXPECT_FALSE(Alt::BitFlag::IsSetEnumFlags(fileType, Alt::Platform::FileTypeBits::SymLink));
			EXPECT_FALSE(Alt::BitFlag::IsSetEnumFlags(fileType, Alt::Platform::FileTypeBits::Junction));
		}

		Alt::Platform::FileDelete(hFile) iferr_return_with(err);
		Alt::Platform::FileClose(hFile) iferr_return_with(err);

		{
			isExist = Alt::Platform::FileExistAny(currentPath) iferr_return_with(err);
			EXPECT_FALSE(isExist);
		}
	}
}

TEST(Platform, FileSystem_WriteReadFiles) {

	Alt::Platform::Error err;
	iferr_scope_on_err {

		if (err.IsErr()) ALT_PRINTLN("{}", err);
		ALT_BREAKPOINT;

		FAIL();
	};

	const Alt::Slice<const char> testDir = "test\\FileSystem_WriteReadFiles"_s;
	Alt::String currentPath = Alt::Platform::FileGetCurrentDirectory() iferr_return_with(err);

	Alt::String directory = currentPath.Clone() iferr_return;
	directory.FormatAppend("{}{}{}", ALT_DS, testDir, ALT_DS) iferr_return;

	Alt::Platform::FileDirectoryCreate(directory) iferr_return_with(err);
	defer {
		iferr_scope;

		Alt::String dir = Alt::Platform::FileGetCurrentDirectory() iferr_return;
		dir.Append("\\test") iferr_return;

		Alt::Platform::FileRemoveAll(dir, Alt::Platform::FileRemoveAllType::All) iferr_return;
	};

	currentPath.FormatAppend("{}{}{}text.txt", ALT_DS, testDir, ALT_DS) iferr_return;

	{
		Alt::Platform::Handle hFile = Alt::Platform::FileOpenOrCreate(
			currentPath,
			Alt::Platform::FileOpenOrCreateBits::None,
			Alt::Platform::FileShareBits::Read,
			Alt::Platform::FileAccessBits::Write | Alt::Platform::FileAccessBits::Read,
			Alt::Platform::FileHintBits::Temporary
		) iferr_return_with(err);

		defer {
			Alt::Platform::FileClose(hFile) iferr_return_with(err);
		};

		const Alt::Slice<const char> data = "Hello from Alt"_s;
		Alt::Platform::FileWrite(hFile, data.CastToBytes().Data(), data.GetSize()) iferr_return_with(err);

		Alt::Vector<u8> dataReadBytes = Alt::Vector<u8>::Allocate(256) iferr_return;

		ALT_MAYBE_UNUSED usize seekBytes = Alt::Platform::FileSeek(hFile, Alt::Platform::FileSeekOpt::Begin) iferr_return_with(err);
		usize readBytes = Alt::Platform::FileRead(hFile, dataReadBytes.Data(), 1) iferr_return_with(err);
		EXPECT_EQ(readBytes, 1);

		seekBytes = Alt::Platform::FileSeek(hFile, Alt::Platform::FileSeekOpt::Begin) iferr_return_with(err);
		readBytes = Alt::Platform::FileRead(hFile, dataReadBytes.Data(), dataReadBytes.GetSize()) iferr_return_with(err);
		EXPECT_EQ(readBytes, data.GetSize());

		const Alt::Slice<const char> dataRead{ reinterpret_cast<const char*>(dataReadBytes.Data()), readBytes };
		EXPECT_EQ(dataRead, data);

		usize sizeBytes = Alt::Platform::FileSize(hFile) iferr_return_with(err);
		EXPECT_EQ(sizeBytes, data.GetSizeBytes());

		Alt::Platform::FileClear(hFile) iferr_return_with(err);

		sizeBytes = Alt::Platform::FileSize(hFile) iferr_return_with(err);
		EXPECT_EQ(sizeBytes, 0);
	}
}

TEST(Platform, FileSystem_FileMapMemory) {

	Alt::Platform::Error err;
	iferr_scope_on_err {

		if (err.IsErr()) ALT_PRINTLN("{}", err);
		ALT_BREAKPOINT;

		FAIL();
	};

	defer {
		iferr_scope;

		Alt::String path = Alt::Platform::FileGetCurrentDirectory() iferr_return_with(err);
		path.Append("\\test") iferr_return;

		Alt::Platform::FileRemoveAll(path, Alt::Platform::FileRemoveAllType::All)
			iferr_panic("Failed to remove \"test\"");
	};

	Alt::String path = Alt::Platform::FileGetCurrentDirectory() iferr_return_with(err);
	path.Append("\\test\\FileSystem_FileMapMemory\\file.txt") iferr_return;

	const Alt::Slice<const char> data = "Hello from Alt - File Map"_s;

	{
		Alt::Platform::Handle hFile = Alt::Platform::FileOpenOrCreate(
			path,
			Alt::Platform::FileOpenOrCreateBits::None,
			Alt::Platform::FileShareBits::Read,
			Alt::Platform::FileAccessBits::Write,
			Alt::Platform::FileHintBits::Temporary | Alt::Platform::FileHintBits::SequentialAccess
		) iferr_return_with(err);

		defer {
			Alt::Platform::FileClose(hFile) iferr_return_with(err);
		};

		Alt::Platform::FileWrite(hFile, data.CastToBytes().Data(), data.CastToBytes().GetSize()) iferr_return_with(err);
	}

	{
		Alt::Platform::Handle hFile = Alt::Platform::FileOpenOrCreate(
			path,
			Alt::Platform::FileOpenOrCreateBits::Open,
			Alt::Platform::FileShareBits::Read,
			Alt::Platform::FileAccessBits::Read,
			Alt::Platform::FileHintBits::SequentialAccess
		) iferr_return_with(err);

		defer {
			Alt::Platform::FileClose(hFile) iferr_return_with(err);
		};

		Alt::Platform::Handle hMap = Alt::Platform::FileCreateMapMemoryObject(hFile, Alt::Platform::FileCreateMapMemoryObjectBits::Read, 0)
			iferr_return_with(err);

		defer {
			Alt::Platform::FileClose(hMap) iferr_return_with(err);
		};

		const usize offset = 1;
		u8* pData = Alt::Platform::FileMapMemory(hMap, Alt::Platform::FileMapMemoryBits::Read, offset, 0) iferr_return_with(err);

		const usize sizeInBytes = Alt::Platform::FileSize(hFile) iferr_return_with(err);

		const Alt::Slice<const char> str{ reinterpret_cast<const char*>(pData), sizeInBytes - offset };
		ALT_PRINTLN("{}", str);
		EXPECT_EQ(str, data.SubSlice(offset));

		Alt::Platform::FileUnmapMemory(pData) iferr_return_with(err);
	}
}

TEST(Platform, FileSystem_PrintLogicalDrivers) {

	{
		Alt::Platform::FileLogicalDriversIterator iterator = Alt::Platform::FileGetLogicalDriversIterator();
		while (Alt::Optional<char> driver = iterator.Next()) {

			ALT_PRINTLN("{}:\\", *driver);
		}
	}

	{
		Alt::Platform::FileLogicalDriversIterator iterator = Alt::Platform::FileGetLogicalDriversIterator();
		iterator.Advance(2);

		while (Alt::Optional<char> driver = iterator.Next()) {

			ALT_PRINTLN("{}:\\", *driver);
		}
	}

	{
		Alt::Platform::FileLogicalDriversIterator iterator = Alt::Platform::FileGetLogicalDriversIterator();
		iterator.AdvanceBack(2);

		while (Alt::Optional<char> driver = iterator.Next()) {

			ALT_PRINTLN("{}:\\", *driver);
		}
	}

	{
		Alt::Platform::FileLogicalDriversIterator iterator = Alt::Platform::FileGetLogicalDriversIterator();
		iterator.AdvanceBack(1);

		while (Alt::Optional<char> driver = iterator.NextBack()) {

			ALT_PRINTLN("{}:\\", *driver);
		}
	}
}

TEST(Platform, FileSystem_FilePath) {

	{
		EXPECT_EQ(Alt::Platform::FilePathSplit("/var/tmp/example.txt"_s).parentPath, "/var/tmp"_s);
		EXPECT_EQ(Alt::Platform::FilePathSplit("/"_s).parentPath, "/"_s);
		EXPECT_EQ(Alt::Platform::FilePathSplit("/var/tmp/."_s).parentPath, "/var/tmp"_s);
	}

	{
		EXPECT_EQ(Alt::Platform::FilePathSplit("/foo/bar.txt"_s).filename, "bar.txt"_s);
		EXPECT_EQ(Alt::Platform::FilePathSplit("/foo/.bar"_s).filename, ".bar"_s);
		EXPECT_EQ(Alt::Platform::FilePathSplit("/foo/bar/"_s).filename, ""_s);
		EXPECT_EQ(Alt::Platform::FilePathSplit("/foo/."_s).filename, "."_s);
		EXPECT_EQ(Alt::Platform::FilePathSplit("/foo/.."_s).filename, ".."_s);
		EXPECT_EQ(Alt::Platform::FilePathSplit("."_s).filename, "."_s);
		EXPECT_EQ(Alt::Platform::FilePathSplit(".."_s).filename, ".."_s);
		EXPECT_EQ(Alt::Platform::FilePathSplit("/"_s).filename, ""_s);
		EXPECT_EQ(Alt::Platform::FilePathSplit("//host"_s).filename, "host"_s);
	}

	{
		EXPECT_EQ(Alt::Platform::FilePathSplit("/foo/bar.txt"_s).stem, "bar"_s);
		EXPECT_EQ(Alt::Platform::FilePathSplit("/foo/.bar"_s).stem, ".bar"_s);
		EXPECT_EQ(Alt::Platform::FilePathSplit("foo.bar.baz.tar"_s).stem, "foo.bar.baz"_s);

		EXPECT_EQ(Alt::Platform::FilePathSplit("foo.bar.baz.tar"_s).stem, "foo.bar.baz"_s);
		EXPECT_EQ(Alt::Platform::FilePathSplit("foo.bar.baz.tar"_s).extension, ".tar"_s);

		EXPECT_EQ(Alt::Platform::FilePathSplit("foo.bar.baz"_s).stem, "foo.bar"_s);
		EXPECT_EQ(Alt::Platform::FilePathSplit("foo.bar.baz"_s).extension, ".baz"_s);

		EXPECT_EQ(Alt::Platform::FilePathSplit("foo.bar"_s).stem, "foo"_s);
		EXPECT_EQ(Alt::Platform::FilePathSplit("foo.bar"_s).extension, ".bar"_s);

		EXPECT_EQ(Alt::Platform::FilePathSplit("foo"_s).stem, "foo"_s);
		EXPECT_EQ(Alt::Platform::FilePathSplit("foo"_s).extension, ""_s);

		EXPECT_EQ(Alt::Platform::FilePathSplit("."_s).stem, "."_s);
		EXPECT_EQ(Alt::Platform::FilePathSplit("."_s).extension, ""_s);

		EXPECT_EQ(Alt::Platform::FilePathSplit(".."_s).stem, ".."_s);
		EXPECT_EQ(Alt::Platform::FilePathSplit(".."_s).extension, ""_s);

		EXPECT_EQ(Alt::Platform::FilePathSplit("..."_s).stem, ".."_s);
		EXPECT_EQ(Alt::Platform::FilePathSplit("..."_s).extension, "."_s);

		EXPECT_EQ(Alt::Platform::FilePathSplit("...."_s).stem, "..."_s);
		EXPECT_EQ(Alt::Platform::FilePathSplit("...."_s).extension, "."_s);
	}

	{
		EXPECT_EQ(Alt::Platform::FilePathSplit("/foo/.hidden"_s).extension, ""_s);
		EXPECT_EQ(Alt::Platform::FilePathSplit("/foo/..bar"_s).extension, ".bar"_s);
		EXPECT_EQ(Alt::Platform::FilePathSplit("/foo/bar"_s).extension, ""_s);
		EXPECT_EQ(Alt::Platform::FilePathSplit("/foo/bar."_s).extension, "."_s);
	}

	{
		EXPECT_TRUE(Alt::Platform::FilePathIsAbsolute("C:/"_s));
		EXPECT_TRUE(Alt::Platform::FilePathIsAbsolute("\\\\?\\C:/"_s));
		EXPECT_FALSE(Alt::Platform::FilePathIsAbsolute("foo/bar"_s));
		EXPECT_FALSE(Alt::Platform::FilePathIsAbsolute("\\\\?\\foo/bar"_s));
	}
}

namespace {

static Alt::Result<void, Alt::Platform::Error> DirectoryEntry(
	Alt::String& path,
	const Alt::Optional<Alt::Platform::PathOffset>& pathOffset,
	const usize deep
) noexcept {

	const auto func = [&](const Alt::Platform::FileDirectoryEntry& entry) {

		for (usize i = 0; i < deep; ++i) {
			ALT_WRITE_BUFFERED_CONSOLE("    ");
		}

		if (entry.isDir) {

			ALT_WRITE_BUFFERED_CONSOLE("{} - {}\\\n", *entry.filename, Alt::DateTime::FromTime(entry.lastChangeTime));

			if (deep < 4) {

				DirectoryEntry(path, entry.filepath, deep + 1);
			}
		}
		else {

			ALT_WRITE_BUFFERED_CONSOLE("{} - {}\n", *entry.filename, Alt::DateTime::FromTime(entry.lastChangeTime));
		}
	};

	Alt::Platform::FileForEachDirectoryEntryDesc desc{};

	if (!pathOffset) {
		return Alt::Platform::FileForEachDirectoryEntry(path, path, func, desc);
	}
	else {
		return Alt::Platform::FileForEachDirectoryEntry(*pathOffset, path, func, desc);
	}
}

}

TEST(Platform, FileSystem_FilePathDirectoryIterator) {

	Alt::Platform::Error err;
	iferr_scope_on_err {

		if (err.IsErr()) ALT_PRINTLN("{}", err);
		ALT_BREAKPOINT;

		FAIL();
	};

	Alt::String path;

	Alt::Platform::FileLogicalDriversIterator iterator = Alt::Platform::FileGetLogicalDriversIterator();
	while (Alt::Optional<char> driver = iterator.Next()) {

		path.Format("{}:\\", *driver) iferr_return;
		ALT_WRITE_BUFFERED_CONSOLE(path);
		ALT_WRITE_BUFFERED_CONSOLE("\n");

		DirectoryEntry(path, Alt::Nullopt, 1) iferr_return_with(err);
	}

	ALT_FLUSH_BUFFERED_CONSOLE();
}

//TEST(Platform, FileSystem_FileDialogOpen) {
//
//	Alt::Platform::Error err;
//	iferr_scope_on_err {
//
//		if (err.IsErr()) ALT_PRINTLN("{}", err);
//		ALT_BREAKPOINT;
//
//		FAIL();
//	};
//
//	const auto filters = Alt::MakeArray(
//
//		Alt::Platform::FileDialogOpenDesc::Filter {
//			"C++ .cpp"_s, { "cpp"_s }
//		},
//
//		Alt::Platform::FileDialogOpenDesc::Filter {
//			"C++ .h"_s, { "h"_s }
//		},
//
//		Alt::Platform::FileDialogOpenDesc::Filter {
//			"C++"_s, { "cpp"_s, "h"_s }
//		},
//
//		Alt::Platform::FileDialogOpenDesc::ALL_FILES_FILTER
//	);
//
//	Alt::Platform::FileDialogOpenDesc desc{};
//	desc.filters = filters; 
//
//	Alt::String filepathInit = Alt::Platform::FileGetCurrentDirectory() iferr_return_with(err);
//	Alt::Optional<Alt::String> path = Alt::Platform::FileDialogOpen(filepathInit, Alt::Platform::FileDialogOpenType::Open, desc)
//		iferr_return_with(err);
//
//	ALT_PRINTLN("{}", path);
//}

//TEST(Platform, ReadConsole) {
//
//	Alt::Platform::Error err;
//	iferr_scope_on_err {
//
//		if (err.IsErr()) ALT_PRINTLN("{}", err);
//		ALT_BREAKPOINT;
//
//		FAIL();
//	};
//
//	Alt::String str = Alt::Platform::ConsoleRead() iferr_return_with(err);
//
//	ALT_PRINTLN(str);
//}

TEST(Platform, ConsoleWidthInChar) {

	Alt::Platform::Error err;
	iferr_scope_on_err {

		if (err.IsErr()) ALT_PRINTLN("{}", err);
		ALT_BREAKPOINT;

		FAIL();
	};

	ALT_PRINTLN("{cm=begin}{}{cm=end;cm=left}{}{cm=down_begin}", ALT_VTS, "|>", ALT_VTS, ">|", ALT_VTS);
}

TEST(Platform, EnvironmentVariable) {

	Alt::Platform::Error err;
	iferr_scope_on_err {

		if (err.IsErr()) ALT_PRINTLN("{}", err);
		ALT_BREAKPOINT;

		FAIL();
	};

	Alt::String strTemp = Alt::Platform::GetEnvironmentVariable("TEMP"_s) iferr_return_with(err);
	ALT_PRINTLN(strTemp);

	Alt::String strPath = Alt::Platform::GetEnvironmentVariable("PATH"_s) iferr_return_with(err);
	ALT_PRINTLN(strPath);
}

TEST(Platform, VTS) {

	ALT_PRINT("{save;cm=up;cl=whole_line;load;}", ALT_VTS);
	ALT_PRINTLN("{save;fg=green_bold;bg=0,0,255}Text{load}", ALT_VTS, ALT_VTS);
}

TEST(Platform, Toy) {

	Alt::Platform::Error err;
	iferr_scope_on_err {

		if (err.IsErr()) ALT_PRINTLN("{}", err);
		ALT_BREAKPOINT;

		FAIL();
	};

	constexpr static usize TICKS = 128;
	constexpr static usize ROWS = 4;
	constexpr static usize COLS = 32;

	usize startRow = 0;
	usize startCol = 0;
	Alt::String buffer;

	const Vec3f32 c0{ 255.0f, 147.0f, 15.0f };
	const Vec3f32 c1{ 255.0f, 249.0f, 91.0f };

	ALT_PRINT("{save;cs=hide;new}", ALT_VTS);

	for (usize iTick = 0; iTick < TICKS; ++iTick) {

		const usize freq = 16;
		const f32 tbias = (f32)(iTick % freq) / (f32)(freq - 1);

		{
			const f32 t = Alt::Math::Mod(0.0f + tbias, 1.0f);
			const Vec3u8 c = c0.Lerp(c1, t).Floor().Cast<u8>();

			buffer.Format(

				"{{fg={},{},{}}"
				"┌────────────────────────────────┐"
				"{default}\n",

				c.x, c.y, c.z
			) iferr_return;

			ALT_WRITE_BUFFERED_CONSOLE(buffer, ALT_VTS, ALT_VTS);
		}

		for (usize i = 0; i < startRow; ++i) {

			const f32 t = Alt::Math::Mod((f32)(i + 1) / (f32)(ROWS + 2) + tbias, 1.0f);
			const Vec3u8 c = c0.Lerp(c1, t).Floor().Cast<u8>();

			buffer.Format(

				"{{fg={},{},{}}│{{default}"
				"{{cm=right{};fg=green}|{{cm=right{};default}"
				"{{fg={},{},{}}│{default;cm=down_begin}",

				c.x, c.y, c.z,
				startCol, COLS - startCol - 1,
				c.x, c.y, c.z

			) iferr_return;

			ALT_WRITE_BUFFERED_CONSOLE(buffer, ALT_VTS, ALT_VTS, ALT_VTS, ALT_VTS, ALT_VTS, ALT_VTS);
		}

		{
			const f32 t = Alt::Math::Mod((f32)(startRow + 1) / (f32)(ROWS + 2) + tbias, 1.0f);
			const Vec3u8 c = c0.Lerp(c1, t).Floor().Cast<u8>();

			buffer.Format(

				"{{fg={},{},{}}│"
				"{{fg=red}~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{{default}"
				"{{fg={},{},{}}│{default}\n",

				c.x, c.y, c.z,
				c.x, c.y, c.z

			) iferr_return;

			ALT_WRITE_BUFFERED_CONSOLE(buffer, ALT_VTS, ALT_VTS, ALT_VTS, ALT_VTS, ALT_VTS);
		}

		for (usize i = startRow + 1; i < ROWS; ++i) {

			const f32 t = Alt::Math::Mod((f32)(i + 1) / (f32)(ROWS + 2) + tbias, 1.0f);
			const Vec3u8 c = c0.Lerp(c1, t).Floor().Cast<u8>();

			buffer.Format(

				"{{fg={},{},{}}│{{default}"
				"{{cm=right{};fg=green}|{{cm=right{};default}"
				"{{fg={},{},{}}│{default;cm=down_begin}",

				c.x, c.y, c.z,
				startCol, COLS - startCol - 1,
				c.x, c.y, c.z

			) iferr_return;

			ALT_WRITE_BUFFERED_CONSOLE(buffer, ALT_VTS, ALT_VTS, ALT_VTS, ALT_VTS, ALT_VTS, ALT_VTS);
		}

		{
			const f32 t = tbias;
			const Vec3u8 c = c0.Lerp(c1, t).Floor().Cast<u8>();

			buffer.Format(

				"{{fg={},{},{}}"
				"└────────────────────────────────┘"
				"{default}\n",

				c.x, c.y, c.z

			) iferr_return;

			ALT_WRITE_BUFFERED_CONSOLE(buffer, ALT_VTS, ALT_VTS);
		}

		ALT_FLUSH_BUFFERED_CONSOLE();

		if (iTick + 1 == TICKS) {
			break;
		}

		buffer.Format("{{cm=up{}}", ROWS + 2) iferr_return;
		ALT_WRITE_BUFFERED_CONSOLE(buffer, ALT_VTS);

		for (usize i = 0; i < ROWS + 2; ++i) {

			ALT_WRITE_BUFFERED_CONSOLE("{cl=whole_line;cm=down}", ALT_VTS);
		}

		buffer.Format("{{cm=up{}}", ROWS + 2) iferr_return;
		ALT_WRITE_BUFFERED_CONSOLE(buffer, ALT_VTS);

		startRow = (startRow + 1) % ROWS;
		startCol = (startCol + 1) % COLS;

		Alt::Platform::SleepMs(32);
	}

	buffer.Format("{{load;cm=top;cm=down{}}", ROWS + 2) iferr_return;
	ALT_PRINT(buffer, ALT_VTS);
}

TEST(Platform, Shadertoy) {

	// https://www.shadertoy.com/view/Xs3GWj
	class FragmentShader {
	public:

		FragmentShader(const f32 time) noexcept
			: iTime(time)
		{}

		Vec4f32 Run(const Vec2f32& fragCoord, const Vec2f32& iResolution) noexcept {

			f32 t = Alt::Math::Mod(iTime, 10.0f);
			Vec3f32 color = Vec3f32(0.0f);
			for(f32 i = 0.; i < AA()*AA() - 0.5; i += 1.) {
				Vec2f32 uv = (fragCoord.xy() + Vec2f32(Alt::Math::Floor(i/AA()), Alt::Math::Mod(i,AA()))/AA()) / iResolution.xy();
				Vec2f32 crtUV = CRTCurveUV( uv );
				if ( crtUV.x < 0.0f || crtUV.x > 1.0f || crtUV.y < 0.0f || crtUV.y > 1.0f ) continue;
				Vec2f32 p = 50.0f * crtUV - 25.0f;
				p *= 0.75f + 0.05f*Alt::Math::Mod(iTime, 10.f);
				p += Alt::Math::Mod(iTime, 10.f) - 5.f;
				p.x *= iResolution.x / iResolution.y;
				if (t < 2.0f || 8.0f < t) {
					f32 fade = Alt::Math::Smoothstep(0.0f, 2.0f, t) - Alt::Math::Smoothstep(8.0f, 10.0f, t);
					f32 scale = iResolution.y/50.9f * f32(AA())*fade + 1.0f;
					p = (p.xy()*scale).Floor()/scale;
				}

				Vec3f32 c;
				if      (Alt::Math::Mod(0.1f * iTime, 5.0f) < 1.0f) c = margarita(p);
				else if (Alt::Math::Mod(0.1f * iTime, 5.0f) < 2.0f) c = plaid_meltdown(p);
				else if (Alt::Math::Mod(0.1f * iTime, 5.0f) < 3.0f) c = sunlight_revealed(p);
				else if (Alt::Math::Mod(0.1f * iTime, 5.0f) < 4.0f) c = threesome(p);
				else if (Alt::Math::Mod(0.1f * iTime, 5.0f) < 5.0f) c = digital_bacteria(p);

				DrawVignette( c, crtUV );
				DrawScanline( c, uv );
				color += c / f32(AA()*AA());
			}

			return Vec4f32{ color, 1.0f };
		}

	private:

		static f32 AA() noexcept {
			return 4.0f;
		}

		static f32 PI() noexcept {
			return (f32)ALT_MATH_PI;
		}

		Vec2f32 CRTCurveUV(Vec2f32 uv) const noexcept {

			uv = uv * 2.0f - 1.0f;
			Vec2f32 offset = uv.Abs().yx() / Vec2f32( 6.0f, 4.0f );
			uv = uv + uv * offset * offset;
			uv = uv * 0.5 + 0.5;
			return uv;
		}

		void DrawVignette(Vec3f32& color, Vec2f32 uv) const noexcept {
			f32 vignette = uv.x * uv.y * ( 1.0f - uv.x ) * ( 1.0f - uv.y );
			vignette = Alt::Math::Clamp( Alt::Math::Pow( 16.0f * vignette, 0.3f ), 0.0f, 1.0f );
			color *= vignette;
		}

		void DrawScanline(Vec3f32& color, Vec2f32 uv) const noexcept {
			f32 scanline      = Alt::Math::Clamp( 0.95f + 0.05f * Alt::Math::Cos( 3.14f * ( uv.y + 0.008f * iTime ) * 240.0f * 1.0f ), 0.0f, 1.0f );
			f32 grille        = 0.85f + 0.15f * Alt::Math::Clamp( 1.5f * Alt::Math::Cos( 3.14f * uv.x * 640.0f * 1.0f ), 0.0f, 1.0f );
			color *= scanline * grille * 1.2f;
		}

		f32 atanp(Vec2f32 p) const noexcept { return Alt::Math::Atan2( p.y, p.x ); }
		f32 cube_root(f32 x) const noexcept { return Alt::Math::IsSignPositive( x ) ? Alt::Math::Pow( Alt::Math::Abs( x ), 1.0f / 3.0f ) : 0.0f; }
		f32 sq(f32 x) const noexcept { return x * x; }

		Vec3f32 margarita(Vec2f32 p) const noexcept {
			f32 z = p.Length() - 3.5f * atanp(p) + Alt::Math::Sin(p.x) + Alt::Math::Cos(p.y);
			if (Alt::Math::Mod(z, 7.0f * PI()) < PI() / 2.0f) return Vec3f32(1.0f, 0.0f, 0.0f);
			if (Alt::Math::Mod(z, 1.0f * PI()) < PI() / 2.0f) return Vec3f32(0.0f);
			return Vec3f32(1.0f);
		}

		Vec3f32 digital_bacteria(Vec2f32 p) const noexcept {
			p /= 4.0f;
			f32 x = sq(Alt::Math::Sin(p.x)+p.y) + sq(Alt::Math::Cos(p.y)+p.x);
			f32 y = Alt::Math::Cos(10.0f*p.x) + Alt::Math::Cos(10.0f*p.y) - Alt::Math::Sin(p.x*p.y);
			f32 z = sq(Alt::Math::Sin(Alt::Math::Floor(p.x))+Alt::Math::Floor(p.y)) + sq(Alt::Math::Cos(Alt::Math::Floor(p.y))+Alt::Math::Floor(p.x));
			if (17.0f < x && x < 21.0f && 17.0f < z && z < 21.0f && y < 0.0f)
				return Vec3f32(1.0f, 1.0f, 85.0f / 256.0f);
			if (17.0f < z && z < 21.0f) return Vec3f32(85.0f / 256.0f, 0.0f, 0.0f);
			if (17.0f < x && x < 21.0f) return Vec3f32(170.0f/256.0f, 170.0f / 256.0f, 0.0f);
			return Vec3f32(85.0f / 256.0f, 85.0f / 256.0f, 0.0f);
		}

		Vec3f32 threesome(Vec2f32 p) const noexcept {
			p /= 3.;
			f32 z = 1.;
			z *= Alt::Math::Sin((p + Vec2f32(5.0f, 0.0f)).Length()) * Alt::Math::Cos(8.0f * atanp(p + Vec2f32(5.0f, 0.0)));
			z *= Alt::Math::Sin((p - Vec2f32(5.0f, 5.0f)).Length()) * Alt::Math::Cos(8.0f * atanp(p - Vec2f32(5.0f, 5.0)));
			z *= Alt::Math::Sin((p + Vec2f32(0.0f, 5.0f)).Length()) * Alt::Math::Cos(8.0f * atanp(p + Vec2f32(0.0f, 5.0)));
			if ((-0.1f < z && z < 0.0f) || 0.2f < z) return Vec3f32(0.0f);
			return Vec3f32(1.0f);
		}

		Vec3f32 plaid_meltdown(Vec2f32 p) const noexcept {
			p /= 15.0f;
			p += 7.0f;
			f32 a = 2.0f * Alt::Math::Sin(p.x * Alt::Math::Sin(p.y) + p.y * Alt::Math::Sin(p.x));
			f32 b = cube_root(Alt::Math::Sin(2.5f * Alt::Math::Sqrt(2.0f) * (p.x - p.y)));
			f32 c = cube_root(Alt::Math::Sin(2.5f * Alt::Math::Sqrt(2.0f) * (p.x + p.y)));
			f32 d = Alt::Math::Sin(80.0f * p.x) + Alt::Math::Sin(80.0f * p.y);
			if (0.25f * (a + b + c) > 0.5f * d) return Vec3f32(0.0f);
			return Vec3f32(1.0f);
		}

		Vec3f32 sunlight_revealed(Vec2f32 p) const noexcept {
			p /= 6.;
			p.x += 2.;
			f32 a = Vec2f32(3.0f - p.x, p.y).Length() + Alt::Math::Abs(p.y) + Alt::Math::Abs(1.0f - p.x);
			f32 f = Alt::Math::Atan2(p.y, p.x - 1.0f);
			f32 c = Alt::Math::Atan2(p.y, p.x - 3.0f);
			f32 R = sq(p.x - 1.0f) + sq(p.y);
			Vec3f32 col = Vec3f32(0);
			bool mix = false;
			if (5.0f < a && a < 7.0f && Alt::Math::Mod(f, PI() / 7.0f) < PI() / 14.0f) {
				col += Vec3f32(0.0f, 82.0f / 256.0f, 173.0f / 256.0f);
				if(mix) col /= 2.0f;
				mix = true;
			}
			if (5.0f < a && a < 7.0f && Alt::Math::Mod(c, PI() / 9.0f) < PI() / 18.0f) {
				col += Vec3f32(1.0f, 0.0f, 0.0f);
				if (mix) col /= 2.0f;
				mix = true;
			}
			if (5.0f < a && a < 7.0f && Alt::Math::Mod(f, PI() / 8.0f) < PI() / 16.0f) {
				col += Vec3f32(1.0f, 1.0f, 0.0f);
				if (mix) col /= 2.0f;
				mix = true;
			}
			if ((45.0f - 3.0f * p.x) * PI() / 180.0f < f && f < (47.0f - p.x) * PI() / 180.0f && p.y > 0.1f * p.x
			   && Alt::Math::Mod(Alt::Math::Log(R) / Alt::Math::Log(f), 2.0f) < 1.0f) {
				col += Vec3f32(1.0f);
				if(mix) col /= 2.0f;
			}
			return col;
		}

	private:

		f32 iTime;
	};

	Alt::Platform::Error err;
	iferr_scope_on_err {

		if (err.IsErr()) ALT_PRINTLN("{}", err);
		ALT_BREAKPOINT;

		FAIL();
	};

	constexpr static usize TICKS = 500;

	Alt::String buffer;

	ALT_PRINT("{save;cs=hide;new}", ALT_VTS);

	for (usize iTick = 0; iTick < TICKS; ++iTick) {

		const Alt::Pair<usize, usize> resolution = Alt::Platform::ConsoleGetSizeInChars() iferr_return_with(err);
		const Vec2f32 consoleSize{ (f32)resolution.f(), (f32)resolution.s() };

		buffer.Clear();
		const f32 time = (f32)Alt::Platform::GetTicksSeconds();

		for (usize y = 0; y < resolution.s(); ++y) {

			for (usize x = 0; x < resolution.f(); ++x) {

				const Vec2f32 fragCoord{ (f32)x, (f32)y };

				FragmentShader shader{ time };
				const Vec4f32 color = shader.Run(fragCoord, consoleSize);

				const Vec3u8 consoleColor = (color.xyz() * 255.0f)
					.Ceil()
					.Min(255.0f, 255.0f, 255.0f)
					.Cast<u8>();

				buffer.Format("{{bg={},{},{}} ", consoleColor.x, consoleColor.y, consoleColor.z) iferr_return;
				ALT_WRITE_BUFFERED_CONSOLE(buffer, ALT_VTS);
			}

			buffer.Format("{cm=down_begin}") iferr_return;
			ALT_WRITE_BUFFERED_CONSOLE(buffer, ALT_VTS);
		}

		buffer.Format("{cm=top_begin}") iferr_return;
		ALT_WRITE_BUFFERED_CONSOLE(buffer, ALT_VTS);

		ALT_FLUSH_BUFFERED_CONSOLE();

		Alt::Platform::SleepMs(32);
	}

	ALT_PRINTLN("{load;cm=bottom}", ALT_VTS);
}

//#include "windows.h"

TEST(Platform, VirtualMemory) {

	/*
	 * On ALT_PLATFORM_X86_32 all 'VirtualAlloc' fails. Test with Allocators also fails.
	 * Commented code which replicates 'Alt::Platform' usage.
	 */

	//u8* pMemory = reinterpret_cast<u8*>(VirtualAlloc(nullptr, Alt::Memory::GB(2), MEM_RESERVE, PAGE_READWRITE));
	//ASSERT_TRUE(pMemory);

	//defer {

	//	VirtualFree(pMemory, 0, MEM_RELEASE);
	//};

	//const usize sizeInBytesToAlloc = Alt::Memory::MB(16);

	//for (usize i = 0; i < 5; ++i) {

	//	u8* pMemoryNew = reinterpret_cast<u8*>(VirtualAlloc(
	//		pMemory + sizeInBytesToAlloc * i,
	//		sizeInBytesToAlloc,
	//		MEM_COMMIT,
	//		PAGE_READWRITE
	//	));

	//	EXPECT_EQ(pMemoryNew, pMemory + sizeInBytesToAlloc * i);
	//}

	u8* pMemory = Alt::Platform::VirtualMemoryAllocateBytes(
		nullptr,
		ALT_PLATFORM_32BIT_BOOL ? Alt::Memory::GB(2) : Alt::Memory::TB(1),
		Alt::Platform::VirtualMemoryAllocateBytesBits::Reserve,
		Alt::Platform::VirtualMemoryAccessBits::ReadWrite
	);

	ASSERT_TRUE(pMemory);

	defer {

		Alt::Platform::VirtualMemoryDeallocateBytes(pMemory, 0, Alt::Platform::VirtualMemoryDeallocateBytesBits::Release);
	};

	const usize sizeInBytesToAlloc = Alt::Memory::MB(16);

	for (usize i = 0; i < 5; ++i) {

		u8* pMemoryNew = Alt::Platform::VirtualMemoryAllocateBytes(
			pMemory + sizeInBytesToAlloc * i,
			sizeInBytesToAlloc,
			Alt::Platform::VirtualMemoryAllocateBytesBits::Commit,
			Alt::Platform::VirtualMemoryAccessBits::ReadWrite
		);

		EXPECT_EQ(pMemoryNew, pMemory + sizeInBytesToAlloc * i);
	}
}

TEST(Platform, RoundToPageSize) {

	constexpr static usize ITERS = 1'000'000;

	const usize pageSize = Alt::Platform::GetPageSizeInBytes();
	Alt::Xoshiro256StarStar rnd = Alt::Xoshiro256StarStar::CreateSeededByTime();

	for (usize i = 0; i < ITERS; ++i) {

		const u64 num = rnd.GenerateU64();

		EXPECT_EQ(Alt::Platform::MemoryRoundUpToPageSizeWrap(num), Alt::Num::RoundUpWrap(num, pageSize)) << num;
		EXPECT_EQ(Alt::Platform::MemoryRoundUpToPageSizeWrapPreserveZero(num), Alt::Num::RoundUpWrapPreserveZero(num, pageSize)) << num;
	}

	EXPECT_EQ(Alt::Platform::MemoryRoundUpToPageSizeWrap((usize)0), pageSize);
	EXPECT_EQ(Alt::Platform::MemoryRoundUpToPageSizeWrapPreserveZero((usize)0), 0);

	for (usize i = 0; i < ITERS; ++i) {

		const u64 num = rnd.GenerateU64();

		EXPECT_EQ(Alt::Platform::MemoryRoundDownToPageSize(num), Alt::Num::RoundDown(num, pageSize)) << num;
	}
}
