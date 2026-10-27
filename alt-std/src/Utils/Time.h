#pragma once

#ifndef __ALT_STD_TIME__
#define __ALT_STD_TIME__

#include "../TypeTraits.h"

#include "Num.h"

#include "../String/Format.h"

namespace Alt {

class Time;
class DateTime;

namespace TypeTraits {

template <>
struct __is_trivially_reallocatable__<Time>
    : std::true_type
{};

template <>
struct __is_trivially_reallocatable__<DateTime>
    : std::true_type
{};

} // namespace TypeTraits

class Time {
public:

    ALT_NODISCARD ALT_FORCE_INLINE static Time FromSince1Jan1970NS(const u64 timeSince1Jan1970NS) noexcept {

        Time time{};
        time.m_TimeSince1Jan1970NS = timeSince1Jan1970NS;

        return time;
    }

    ALT_NODISCARD ALT_FORCE_INLINE static Time FromUnixNS(const u64 timeUnixNS) noexcept {

        Time time{};
        time.m_TimeSince1Jan1970NS = timeUnixNS;

        return time;
    }

    ALT_NODISCARD ALT_FORCE_INLINE static Time FromWin32FileTime(const u64 win32FileTime) noexcept {

        // 1 filetime = 100ns
        // 100 ns units between 1601-01-01 and 1970-01-01, 11'644'473'600 seconds
        constexpr static u64 TIME_SINCE_1601_100NS = static_cast<u64>(11'644'473'600) * static_cast<u64>(10'000'000);

        Time time{};
        time.m_TimeSince1Jan1970NS = (Num::Max(win32FileTime, TIME_SINCE_1601_100NS) - TIME_SINCE_1601_100NS) * 100;

        return time;
    }

    ALT_FORCE_INLINE Time() noexcept = default;

    ALT_NODISCARD ALT_FORCE_INLINE u64 GetSince1Jan1970NS() const noexcept {
        return m_TimeSince1Jan1970NS;
    }

    ALT_NODISCARD ALT_FORCE_INLINE u64 GetSince1Jan1970MS() const noexcept {
        return m_TimeSince1Jan1970NS / MS_TO_NS;
    }

private:

    constexpr static u64 MS_TO_NS = 1'000'000;

    // 2554-07-21 23:34:33 - Num::Desc<u64>::Max()
    u64 m_TimeSince1Jan1970NS = 0;
};

class DateTime {
public:

    struct Desc {

        // [1970, ...
        usize year = 1970;

        // [1, 12]
        usize month = 1;

        // [1, 31]
        usize day = 1;

        // [0, 24)
        usize hour = 0;

        // [0, 60)
        usize minute = 0;

        // [0, 60)
        usize second = 0;

        // [0, 1'000'000'000)
        usize nanosecond = 0;
    };

    ALT_NODISCARD ALT_FORCE_INLINE static DateTime From(const Desc& desc) noexcept {

        DateTime dateTime{};

        dateTime.m_Year = static_cast<u16>(desc.year);
        dateTime.m_Month = static_cast<u8>(desc.month);
        dateTime.m_Day = static_cast<u8>(desc.day);
        dateTime.m_Hour = static_cast<u8>(desc.hour);
        dateTime.m_Minute = static_cast<u8>(desc.minute);
        dateTime.m_Second = static_cast<u8>(desc.second);
        dateTime.m_Nanosecond = static_cast<u32>(desc.nanosecond);

        u32 y = static_cast<u32>(dateTime.m_Year);
        u32 m = static_cast<u32>(dateTime.m_Month);
        u32 d = static_cast<u32>(dateTime.m_Day);

        y -= m <= 2;
        const u32 era = y / 400;
        const u32 yoe = static_cast<u32>(y - era * 400);           // [0, 399]
        const u32 doy = (153*(m > 2 ? m-3 : m+9) + 2)/5 + d-1;     // [0, 365]
        const u32 doe = yoe * 365 + yoe/4 - yoe/100 + doy;         // [0, 146096]
        const u32 days = era * 146097 + static_cast<u32>(doe) - 719468;

        dateTime.m_DayOfWeek = static_cast<u8>((days + 4) % 7);

        return dateTime;
    }

    ALT_NODISCARD ALT_FORCE_INLINE static DateTime FromTime(const Time time) noexcept {

        /*
         * https://howardhinnant.github.io/date_algorithms.html
         */

        const u64 totalNS = time.GetSince1Jan1970NS();
        const u64 days = totalNS / 86'400'000'000'000;
        const u64 ns = totalNS % 86'400'000'000'000;

        DateTime dateTime{};
        dateTime.m_Hour = static_cast<u8>(ns / 3'600'000'000'000);
        dateTime.m_Minute = static_cast<u8>((ns / 60'000'000'000) % 60);
        dateTime.m_Second = static_cast<u8>((ns / 1'000'000'000) % 60);
        dateTime.m_Nanosecond = static_cast<u32>(ns % 1'000'000'000);

        const u64 z = days + 719468;
        const u64 era = z / 146097;
        const u32 doe = static_cast<u32>(z - era * 146097);               // [0, 146096]
        const u32 yoe = (doe - doe/1460 + doe/36524 - doe/146096) / 365;  // [0, 399]
        const u64 y = static_cast<u64>(yoe) + era * 400;
        const u32 doy = doe - (365*yoe + yoe/4 - yoe/100);                // [0, 365]
        const u32 mp = (5*doy + 2)/153;                                   // [0, 11]
        const u32 d = doy - (153*mp+2)/5 + 1;                             // [1, 31]
        const u32 m = mp < 10 ? mp+3 : mp-9;                              // [1, 12]

        dateTime.m_Year = static_cast<u16>(y + (m <= 2 ? 1 : 0));
        dateTime.m_Month = static_cast<u8>(m);
        dateTime.m_Day = static_cast<u8>(d);

        dateTime.m_DayOfWeek = static_cast<u8>((days + 4) % 7);

        return dateTime;
    }

    ALT_FORCE_INLINE DateTime() noexcept = default;

    ALT_NODISCARD ALT_FORCE_INLINE usize GetYear() const noexcept {
        return static_cast<usize>(m_Year);
    }

    ALT_NODISCARD ALT_FORCE_INLINE usize GetMonth() const noexcept {
        return static_cast<usize>(m_Month);
    }

    ALT_NODISCARD ALT_FORCE_INLINE usize GetDay() const noexcept {
        return static_cast<usize>(m_Day);
    }

    ALT_NODISCARD ALT_FORCE_INLINE usize GetHour() const noexcept {
        return static_cast<usize>(m_Hour);
    }

    ALT_NODISCARD ALT_FORCE_INLINE usize GetMinute() const noexcept {
        return static_cast<usize>(m_Minute);
    }

    ALT_NODISCARD ALT_FORCE_INLINE usize GetSecond() const noexcept {
        return static_cast<usize>(m_Second);
    }

    ALT_NODISCARD ALT_FORCE_INLINE usize GetMillisecond() const noexcept {
        return static_cast<usize>(m_Nanosecond / MS_TO_NS);
    }

    ALT_NODISCARD ALT_FORCE_INLINE usize GetNanosecond() const noexcept {
        return static_cast<usize>(m_Nanosecond);
    }

    ALT_NODISCARD ALT_FORCE_INLINE usize GetDayOfWeek() const noexcept {
        return static_cast<usize>(m_DayOfWeek);
    }

    ALT_NODISCARD ALT_FORCE_INLINE u64 GetSince1Jan1970NS() const noexcept {

        u32 y = static_cast<u32>(m_Year);
        u32 m = static_cast<u32>(m_Month);
        u32 d = static_cast<u32>(m_Day);

        y -= m <= 2;
        const u32 era = y / 400;
        const u32 yoe = static_cast<u32>(y - era * 400);           // [0, 399]
        const u32 doy = (153*(m > 2 ? m-3 : m+9) + 2)/5 + d-1;     // [0, 365]
        const u32 doe = yoe * 365 + yoe/4 - yoe/100 + doy;         // [0, 146096]
        const u32 days = era * 146097 + static_cast<u32>(doe) - 719468;

        return
            static_cast<u64>(days) * static_cast<u64>(86'400'000'000'000) +
            static_cast<u64>(m_Hour) * static_cast<u64>(3'600'000'000'000) +
            static_cast<u64>(m_Minute) * static_cast<u64>(60'000'000'000) +
            static_cast<u64>(m_Second) * static_cast<u64>(1'000'000'000) +
            static_cast<u64>(m_Nanosecond);
    }

    ALT_NODISCARD ALT_FORCE_INLINE u64 GetSince1Jan1970MS() const noexcept {

        u32 y = static_cast<u32>(m_Year);
        u32 m = static_cast<u32>(m_Month);
        u32 d = static_cast<u32>(m_Day);

        y -= m <= 2;
        const u32 era = y / 400;
        const u32 yoe = static_cast<u32>(y - era * 400);           // [0, 399]
        const u32 doy = (153*(m > 2 ? m-3 : m+9) + 2)/5 + d-1;     // [0, 365]
        const u32 doe = yoe * 365 + yoe/4 - yoe/100 + doy;         // [0, 146096]
        const u32 days = era * 146097 + static_cast<u32>(doe) - 719468;

        return
            static_cast<u64>(days) * static_cast<u64>(86'400'000) +
            static_cast<u64>(m_Hour) * static_cast<u64>(3'600'000) +
            static_cast<u64>(m_Minute) * static_cast<u64>(60'000) +
            static_cast<u64>(m_Second) * static_cast<u64>(1'000) +
            static_cast<u64>(m_Nanosecond) / MS_TO_NS;
    }

private:

    constexpr static u64 MS_TO_NS = 1'000'000;

    // see. Desc
    u32 m_Nanosecond = 0;

    u16 m_Year = 1970;
    u8 m_Month = 1;
    u8 m_Day = 1;
    u8 m_Hour = 0;
    u8 m_Minute = 0;
    u8 m_Second = 0;
    u8 m_DayOfWeek = 0;
};

template <>
class Format<DateTime> {
public:

    ALT_FORCE_INLINE static Optional<EstimateExactSizeResult> EstimateExactSize(
        FormatEstimator&, const Slice<const char> parameters, const DateTime&
    ) noexcept {

        // 2026-09-01 14:35:42

        if (parameters.IsPopulated()) {
            return Nullopt;
        }

        EstimateExactSizeResult result{};
        result.size += 4 + 1;
        result.size += 2 + 1;
        result.size += 2;
        result.size += 1;
        result.size += 2 + 1;
        result.size += 2 + 1;
        result.size += 2;

        return result;
    }

    template <typename FormatWriterT>
    ALT_FORCE_INLINE static bool WriteAppend(
        FormatWriter<FormatWriterT>& writer, const Slice<const char> parameters, const DateTime& dataTime
    ) noexcept {
        iferr_scope;

        if (parameters.IsPopulated()) {
            return false;
        }

        Format<usize>::WriteAppend(writer, ""_s, dataTime.GetYear()) iferr_return false;
        writer.Append('-') iferr_return false;

        const usize month = dataTime.GetMonth();
        if (month < 10) {
            writer.Append('0') iferr_return false;
        }

        Format<usize>::WriteAppend(writer, ""_s, month) iferr_return false;
        writer.Append('-') iferr_return false;

        const usize day = dataTime.GetDay();
        if (day < 10) {
            writer.Append('0') iferr_return false;
        }
 
        Format<usize>::WriteAppend(writer, ""_s, day) iferr_return false;

        writer.Append(' ') iferr_return false;

        const usize hour = dataTime.GetHour();
        if (hour < 10) {
            writer.Append('0') iferr_return false;
        }

        Format<usize>::WriteAppend(writer, ""_s, hour) iferr_return false;
        writer.Append(':') iferr_return false;

        const usize minute = dataTime.GetMinute();
        if (minute < 10) {
            writer.Append('0') iferr_return false;
        }
 
        Format<usize>::WriteAppend(writer, ""_s, minute) iferr_return false;
        writer.Append(':') iferr_return false;

        
        const usize second = dataTime.GetSecond();
        if (second < 10) {
            writer.Append('0') iferr_return false;
        }

        Format<usize>::WriteAppend(writer, ""_s, second) iferr_return false;

        return true;
    }
};

} // namespace Alt

#endif // __ALT_STD_TIME__