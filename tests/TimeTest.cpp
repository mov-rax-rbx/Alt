#include "pch.h"

#define ALT_USE_SCALAR_TYPES_IN_GLOBAL_NAMESPACE

#include "Utils.h"

#include "../alt-std/src/Random/Xoshiro256.h"
#include "../alt-std/src/Utils/Time.h"

TEST(Time, TimeTest) {

    for (usize i = 0; i < Alt::Desc<u16>::Max(); ++i) {

        const Alt::Time time = Alt::Time::FromSince1Jan1970NS(i);
        const Alt::DateTime dateTime = Alt::DateTime::FromTime(time);

        EXPECT_EQ(time.GetSince1Jan1970NS(), dateTime.GetSince1Jan1970NS());
        EXPECT_EQ(time.GetSince1Jan1970MS(), dateTime.GetSince1Jan1970MS());
    }

    constexpr static usize NUM_ITERS = 1'000'000;

    Alt::Xoshiro256StarStar rnd = Alt::Xoshiro256StarStar::CreateSeededByTime();

    //ALT_TIME_BENCH("Time", 1'000,
        for (usize i = 0; i < NUM_ITERS; ++i) {

            const u64 t = rnd.GenerateU64((u64)Alt::Desc<u16>::Max() + 1, Alt::Desc<u64>::Max());

            const Alt::Time time = Alt::Time::FromSince1Jan1970NS(t);
            const Alt::DateTime dateTime = Alt::DateTime::FromTime(time);

            EXPECT_EQ(time.GetSince1Jan1970NS(), dateTime.GetSince1Jan1970NS());
            EXPECT_EQ(time.GetSince1Jan1970MS(), dateTime.GetSince1Jan1970MS());
        }
    //);

    Alt::String str;

    {
        const Alt::Time time = Alt::Time::FromSince1Jan1970NS(0);
        const Alt::DateTime dateTime = Alt::DateTime::FromTime(time);

        str.Format("{}", dateTime);
        EXPECT_EQ(str, "1970-01-01 00:00:00");

        EXPECT_EQ(time.GetSince1Jan1970NS(), dateTime.GetSince1Jan1970NS());
        EXPECT_EQ(time.GetSince1Jan1970MS(), dateTime.GetSince1Jan1970MS());
    }

    {
        const Alt::Time time = Alt::Time::FromSince1Jan1970NS(Alt::Desc<u32>::Max());
        const Alt::DateTime dateTime = Alt::DateTime::FromTime(time);

        str.Format("{}", dateTime);
        EXPECT_EQ(str, "1970-01-01 00:00:04");

        EXPECT_EQ(time.GetSince1Jan1970NS(), dateTime.GetSince1Jan1970NS());
        EXPECT_EQ(time.GetSince1Jan1970MS(), dateTime.GetSince1Jan1970MS());
    }

    {
        const Alt::Time time = Alt::Time::FromSince1Jan1970NS(Alt::Desc<u64>::Max());
        const Alt::DateTime dateTime = Alt::DateTime::FromTime(time);

        str.Format("{}", dateTime);
        EXPECT_EQ(str, "2554-07-21 23:34:33") << str.CStr();

        EXPECT_EQ(time.GetSince1Jan1970NS(), dateTime.GetSince1Jan1970NS());
        EXPECT_EQ(time.GetSince1Jan1970MS(), dateTime.GetSince1Jan1970MS());
    }
}
