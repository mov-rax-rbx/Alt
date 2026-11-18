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

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static Time FromSince1Jan1970NS(const u64 timeSince1Jan1970NS) noexcept {

        Time time{};
        time.m_TimeSince1Jan1970NS = timeSince1Jan1970NS;

        return time;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static Time FromUnixNS(const u64 timeUnixNS) noexcept {

        Time time{};
        time.m_TimeSince1Jan1970NS = timeUnixNS;

        return time;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static Time FromWin32FileTime(const u64 win32FileTime) noexcept {

        // 1 filetime = 100ns
        // 100 ns units between 1601-01-01 and 1970-01-01, 11'644'473'600 seconds
        constexpr u64 TIME_SINCE_1601_100NS = static_cast<u64>(11'644'473'600) * static_cast<u64>(10'000'000);

        Time time{};
        time.m_TimeSince1Jan1970NS = (Max(win32FileTime, TIME_SINCE_1601_100NS) - TIME_SINCE_1601_100NS) * 100;

        return time;
    }

    ALT_FORCE_INLINE constexpr Time() noexcept = default;

    ALT_NODISCARD ALT_FORCE_INLINE constexpr u64 GetSince1Jan1970NS() const noexcept {
        return m_TimeSince1Jan1970NS;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr u64 GetSince1Jan1970MS() const noexcept {
        return m_TimeSince1Jan1970NS / MS_TO_NS;
    }

private:

    constexpr static u64 MS_TO_NS = 1'000'000;

    // 2554-07-21 23:34:33 - Desc<u64>::Max()
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

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static DateTime From(const Desc& desc) noexcept {

        DateTime dateTime{};

        dateTime.m_Year = static_cast<u16>(desc.year);
        dateTime.m_Month = static_cast<u8>(desc.month);
        dateTime.m_Day = static_cast<u8>(desc.day);
        dateTime.m_Hour = static_cast<u8>(desc.hour);
        dateTime.m_Minute = static_cast<u8>(desc.minute);
        dateTime.m_Second = static_cast<u8>(desc.second);
        dateTime.m_Nanosecond = static_cast<u32>(desc.nanosecond);

        const u32 y = static_cast<u32>(desc.year);
        const u32 m = static_cast<u32>(desc.month);
        const u32 d = static_cast<u32>(desc.day);

        const u64 days = TotalDaysFromYMD(y, m, d);

        dateTime.m_DayOfWeek = DayOfWeekFromTotalDays(days);

        return dateTime;
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr static DateTime FromTime(const Time time) noexcept {

        /*
         * https://howardhinnant.github.io/date_algorithms.html
         * 
         * https://github.com/benjoffe/fast-world-calendars/blob/main/time/algorithms/benjoffe.hpp
         */

        const u64 totalNS = time.GetSince1Jan1970NS();
        const u64 days = totalNS / 86'400'000'000'000;
        const u64 ns = totalNS % 86'400'000'000'000;

        DateTime dateTime{};
        //dateTime.m_Hour = static_cast<u8>(ns / 3'600'000'000'000);
        //dateTime.m_Minute = static_cast<u8>((ns / 60'000'000'000) % 60);
        //dateTime.m_Second = static_cast<u8>((ns / 1'000'000'000) % 60);
        //dateTime.m_Nanosecond = static_cast<u32>(ns % 1'000'000'000);

        const u64 low = ns >> 9;
        const u64 time_ = (low * 36028797 + ((low * 39) >> 11)) >> 46;
        const u64 prd0  = time_ * 1193047;
        const u64 prd1  = time_ * 71582789;

        dateTime.m_Hour = static_cast<u8>(prd0 >> 32);
        dateTime.m_Minute = static_cast<u8>((static_cast<u64>(static_cast<u32>(prd0)) * 60) >> 32);
        dateTime.m_Second = static_cast<u8>((static_cast<u64>(static_cast<u32>(prd1)) * 60) >> 32);
        dateTime.m_Nanosecond = static_cast<u32>(ns - time_ * 1'000'000'000);

        YMDFromTotalDays(days, dateTime.m_Year, dateTime.m_Month, dateTime.m_Day);

        dateTime.m_DayOfWeek = DayOfWeekFromTotalDays(days);

        return dateTime;
    }

    ALT_FORCE_INLINE constexpr DateTime() noexcept = default;

    ALT_NODISCARD ALT_FORCE_INLINE constexpr usize GetYear() const noexcept {
        return static_cast<usize>(m_Year);
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr usize GetMonth() const noexcept {
        return static_cast<usize>(m_Month);
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr usize GetDay() const noexcept {
        return static_cast<usize>(m_Day);
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr usize GetHour() const noexcept {
        return static_cast<usize>(m_Hour);
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr usize GetMinute() const noexcept {
        return static_cast<usize>(m_Minute);
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr usize GetSecond() const noexcept {
        return static_cast<usize>(m_Second);
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr usize GetMillisecond() const noexcept {
        return static_cast<usize>(m_Nanosecond / MS_TO_NS);
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr usize GetNanosecond() const noexcept {
        return static_cast<usize>(m_Nanosecond);
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr usize GetDayOfWeek() const noexcept {
        return static_cast<usize>(m_DayOfWeek);
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr u64 GetSince1Jan1970NS() const noexcept {

        const u32 y = static_cast<u32>(m_Year);
        const u32 m = static_cast<u32>(m_Month);
        const u32 d = static_cast<u32>(m_Day);

        const u64 days = TotalDaysFromYMD(y, m, d);

        return
            static_cast<u64>(days) * static_cast<u64>(86'400'000'000'000) +
            static_cast<u64>(m_Hour) * static_cast<u64>(3'600'000'000'000) +
            static_cast<u64>(m_Minute) * static_cast<u64>(60'000'000'000) +
            static_cast<u64>(m_Second) * static_cast<u64>(1'000'000'000) +
            static_cast<u64>(m_Nanosecond);
    }

    ALT_NODISCARD ALT_FORCE_INLINE constexpr u64 GetSince1Jan1970MS() const noexcept {

        const u32 y = static_cast<u32>(m_Year);
        const u32 m = static_cast<u32>(m_Month);
        const u32 d = static_cast<u32>(m_Day);

        const u64 days = TotalDaysFromYMD(y, m, d);

        return
            static_cast<u64>(days) * static_cast<u64>(86'400'000) +
            static_cast<u64>(m_Hour) * static_cast<u64>(3'600'000) +
            static_cast<u64>(m_Minute) * static_cast<u64>(60'000) +
            static_cast<u64>(m_Second) * static_cast<u64>(1'000) +
            static_cast<u64>(m_Nanosecond) / MS_TO_NS;
    }

private:

    ALT_FORCE_INLINE constexpr static void YMDFromTotalDays(const u64 totalDays, u16& y_, u8& m_, u8& d_) noexcept {

        const u64 z = totalDays + 719468;
        const u64 era = z / 146097;
        const u32 doe = static_cast<u32>(z - era * 146097);               // [0, 146096]
        const u32 yoe = (doe - doe/1460 + doe/36524 - doe/146096) / 365;  // [0, 399]
        const u64 y = static_cast<u64>(yoe) + era * 400;
        const u32 doy = doe - (365*yoe + yoe/4 - yoe/100);                // [0, 365]
        const u32 mp = (5*doy + 2)/153;                                   // [0, 11]
        const u32 d = doy - (153*mp+2)/5 + 1;                             // [1, 31]
        const u32 m = mp < 10 ? mp+3 : mp-9;                              // [1, 12]

        y_ = static_cast<u16>(y + (m <= 2 ? 1 : 0));
        m_ = static_cast<u8>(m);
        d_ = static_cast<u8>(d);
    }

    ALT_FORCE_INLINE constexpr static u64 TotalDaysFromYMD(u32 y, const u32 m, const u32 d) noexcept {

        //y -= m <= 2;
        //const u32 era = y / 400;
        //const u32 yoe = static_cast<u32>(y - era * 400);           // [0, 399]
        //const u32 doy = (153*(m > 2 ? m-3 : m+9) + 2)/5 + d-1;     // [0, 365]
        //const u32 doe = yoe * 365 + yoe/4 - yoe/100 + doy;         // [0, 146096]
        //return static_cast<u64>(era * 146097 + doe - 719468);

        /*
         * https://github.com/benjoffe/fast-date-benchmarks/blob/main/algorithms/benjoffe_fast64.hpp
         */
        const u32 bump = m <= 2;
        const u32 yrs = (y + 5880000) - bump;
        const u32 cen = yrs / 100;
        const s32 shift = bump ? 8829 : -2919;

        const u32 yearDays = yrs * 365 + yrs / 4 - cen + cen / 4;
        const u32 monthDays = static_cast<u32>(979 * static_cast<s32>(m) + shift) / 32;

        return static_cast<u64>(yearDays + monthDays + d - 2148345369u);
    }

    ALT_FORCE_INLINE constexpr static u8 DayOfWeekFromTotalDays(const u64 totalDays) noexcept {

        //return static_cast<u8>((totalDays + 4) % 7);

        /*
         * https://www.benjoffe.com/fast-day-of-week#fn=u64unix
         */
        const u64 M = Num::Desc<u64>::Max() / 7;
        const u64 Z = 0x9400ull << 48;        // 10,664,523,917,613,334,528
        const u64 a = totalDays * M + Z;
        const u64 b = (totalDays >> 2) + (totalDays >> 5);

        return static_cast<u8>((a + b) >> 61); // top 3 bits
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