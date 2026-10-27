#pragma once

#ifndef __ALT_STD_MATH_PRE_INCLUDE__
#define __ALT_STD_MATH_PRE_INCLUDE__

#include <math.h>

#include "../../TypeTraits.h"
#include "../../Memory/Impl/MemoryPreInclude.h"

#include "../../Utils/Num.h"

namespace Alt {
namespace _MathDetails {

ALT_NODISCARD ALT_FORCE_INLINE f32 fmod(const f32 x, const f32 y) noexcept {
    return fmodf(x, y);
}

ALT_NODISCARD ALT_FORCE_INLINE f64 fmod(const f64 x, const f64 y) noexcept {
    return ::fmod(x, y);
}

} // namespace _MathDetails

namespace Math {

#define ALT_MATH_E 2.71828182845904523536 // e
#define ALT_MATH_PI 3.14159265358979323846 // pi

template <typename T,
    std::enable_if_t<std::is_scalar_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr bool IsSignPositive(const T x) noexcept {
    return Num::IsSignPositive(x);
}

template <typename T,
    std::enable_if_t<std::is_scalar_v<T>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr T Abs(const T x) noexcept {
    return Num::Abs(x);
}

template <typename T, typename U,
    std::enable_if_t<std::is_scalar_v<T> && std::is_convertible_v<U, T> && !Num::Desc<T>::IsFloat()>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr T Mod(const T x, const U y) noexcept {
    return Num::Mod(x, y);
}

template <typename T, typename U,
    std::enable_if_t<std::is_scalar_v<T> && std::is_convertible_v<U, T> && Num::Desc<T>::IsFloat()>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr T Mod(const T x, const U y_) noexcept {

    const T y = static_cast<T>(y_);
    return _MathDetails::fmod(x, y);

    // const f32 c = Frac(Abs(x / y)) * Abs(y);
    // return IsSignPositive(x) ? c : -c;
}

template <typename T, typename ...ArgsT,
    std::enable_if_t<std::is_scalar_v<T> && std::conjunction_v<std::is_convertible<ArgsT, T>...>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr T Max(const T x, const ArgsT... args) noexcept {
    return Num::Max(x, args...);
}

template <typename T, typename ...ArgsT,
    std::enable_if_t<std::is_scalar_v<T> && std::conjunction_v<std::is_convertible<ArgsT, T>...>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr T Min(const T x, const ArgsT... args) noexcept {
    return Num::Min(x, args...);
}

template <typename T0, typename T1, typename T2,
    std::enable_if_t<std::is_scalar_v<T0> && std::is_convertible_v<T1, T0> && std::is_convertible_v<T2, T0>>* = nullptr>
ALT_NODISCARD ALT_FORCE_INLINE constexpr T0 Clamp(const T0 val, const T1 from, const T2 to) noexcept {
    return Num::Clamp(val, from, to);
}

ALT_NODISCARD ALT_FORCE_INLINE constexpr bool IsEqFuzzy(const f32 x, const f32 y, const f32 eps = Num::Desc<f32>::Eps()) noexcept {

    const f32 norm = Min(Abs(x) + Abs(y), Num::Desc<f32>::Max());
    return Abs(x - y) <= Max(Num::Desc<f32>::Min(), eps * norm);
}

ALT_NODISCARD ALT_FORCE_INLINE constexpr bool IsEqFuzzy(const f64 x, const f64 y, const f64 eps = Num::Desc<f64>::Eps()) noexcept {

    const f64 norm = Min(Abs(x) + Abs(y), Num::Desc<f64>::Max());
    return Abs(x - y) <= Max(Num::Desc<f64>::Min(), eps * norm);
}

ALT_NODISCARD ALT_FORCE_INLINE constexpr ssize CompareFuzzy(const f32 x, const f32 y, const f32 eps = Num::Desc<f32>::Eps()) noexcept {

    if (IsEqFuzzy(x, y, eps)) {
        return 0;
    }

    return x < y ? -1 : 1;
}

ALT_NODISCARD ALT_FORCE_INLINE constexpr ssize CompareFuzzy(const f64 x, const f64 y, const f64 eps = Num::Desc<f64>::Eps()) noexcept {

    if (IsEqFuzzy(x, y, eps)) {
        return 0;
    }

    return x < y ? -1 : 1;
}

#ifdef ALT_COMPILER_MSVC
#pragma warning(push)
#pragma warning(disable: 4723 /* division by 0 */)
#endif // ALT_COMPILER_MSVC

ALT_NODISCARD ALT_FORCE_INLINE constexpr f32 SafeInv(const f32 x, const f32 defaultVal = 0.0f) noexcept {
    return IsEqFuzzy(x, 0.0f) ? defaultVal : (1.0f / x);
}

ALT_NODISCARD ALT_FORCE_INLINE constexpr f64 SafeInv(const f64 x, const f64 defaultVal = 0.0) noexcept {
    return IsEqFuzzy(x, 0.0) ? defaultVal : (1.0 / x);
}

#ifdef ALT_COMPILER_MSVC
#pragma warning(pop)
#endif // ALT_COMPILER_MSVC

ALT_FORCE_INLINE constexpr f32 DegToRad(const f32 deg) noexcept {
    return deg / 180.0f * static_cast<f32>(ALT_MATH_PI);
}

ALT_FORCE_INLINE constexpr f32 RadToDeg(const f32 rad) noexcept {
    return rad / static_cast<f32>(ALT_MATH_PI) * 180.0f;
}

ALT_FORCE_INLINE constexpr f64 DegToRad(const f64 deg) noexcept {
    return deg / 180.0 * static_cast<f64>(ALT_MATH_PI);
}

ALT_FORCE_INLINE constexpr f64 RadToDeg(const f64 rad) noexcept {
    return rad / static_cast<f64>(ALT_MATH_PI) * 180.0;
}

ALT_NODISCARD ALT_FORCE_INLINE constexpr f32 Lerp(const f32 from, const f32 to, const f32 t) noexcept {
    return (1.0f - t) * from + to * t;
}

ALT_NODISCARD ALT_FORCE_INLINE constexpr f64 Lerp(const f64 from, const f64 to, const f64 t) noexcept {
    return (1.0 - t) * from + to * t;
}

ALT_NODISCARD ALT_FORCE_INLINE constexpr f32 Smoothstep(const f32 from, const f32 to, const f32 x) noexcept {

    const f32 t = Num::Clamp((x - from) / (to - from), 0.0f, 1.0f);
    return t * t * (3.0f - 2.0f * t);
}

ALT_NODISCARD ALT_FORCE_INLINE constexpr f64 Smoothstep(const f64 from, const f64 to, const f64 x) noexcept {

    const f64 t = Num::Clamp((x - from) / (to - from), 0.0, 1.0);
    return t * t * (3.0 - 2.0 * t);
}

ALT_NODISCARD ALT_FORCE_INLINE f32 Floor(const f32 x) noexcept {
    return floorf(x);
}

ALT_NODISCARD ALT_FORCE_INLINE f64 Floor(const f64 x) noexcept {
    return ::floor(x);
}

ALT_NODISCARD ALT_FORCE_INLINE f32 Ceil(const f32 x) noexcept {
    return ceilf(x);
}

ALT_NODISCARD ALT_FORCE_INLINE f64 Ceil(const f64 x) noexcept {
    return ::ceil(x);
}

ALT_NODISCARD ALT_FORCE_INLINE f32 Trunc(const f32 x) noexcept {
    return truncf(x);
}

ALT_NODISCARD ALT_FORCE_INLINE f64 Trunc(const f64 x) noexcept {
    return ::trunc(x);
}

ALT_NODISCARD ALT_FORCE_INLINE f32 Frac(const f32 x) noexcept {
    return x - floorf(x);
}

ALT_NODISCARD ALT_FORCE_INLINE f64 Frac(const f64 x) noexcept {
    return x - floor(x);
}

ALT_NODISCARD ALT_FORCE_INLINE f32 Round(const f32 x) noexcept {
    return roundf(x);
}

ALT_NODISCARD ALT_FORCE_INLINE f64 Round(const f64 x) noexcept {
    return ::round(x);
}

ALT_NODISCARD ALT_FORCE_INLINE f32 Sqrt(const f32 x) noexcept {
    return sqrtf(x);
}

ALT_NODISCARD ALT_FORCE_INLINE f64 Sqrt(const f64 x) noexcept {
    return ::sqrt(x);
}

ALT_NODISCARD ALT_FORCE_INLINE f32 Rsqrt(f32 x, const f32 defaultVal = 0.0f) noexcept {

    return SafeInv(sqrtf(x), defaultVal);

//#ifndef ALT_USE_CUSTOM_FAST_MATH
//
//	return SafeInv(sqrtf(x));
//#else
//
//	// https://0x5f37642f.com/documents/RobertsonBriefHistory.pdf
//
//	f32 xhalf = 0.5f*x;
//	u32 i = Memory::Transmute<f32, u32>(x); // get bits for floating value
//	i = (u32)0x5f375a86 - (i>>1); // gives initial guess y0
//	x = Memory::Transmute<u32, f32>(i); // convert bits back to float
//	x = x*(1.5f-xhalf*x*x); // Newton step, repeating increases accuracy return x;
//	x = x*(1.5f-xhalf*x*x); // 2nd
//
//	return x;
//#endif // ALT_USE_CUSTOM_FAST_MATH
}

ALT_NODISCARD ALT_FORCE_INLINE f64 Rsqrt(f64 x, const f64 defaultVal = 0.0) noexcept {

    return SafeInv(::sqrt(x), defaultVal);

//#ifndef ALT_USE_CUSTOM_FAST_MATH
//
//	return SafeInv(::sqrt(x));
//#else
//
//	// https://0x5f37642f.com/documents/RobertsonBriefHistory.pdf
//
//	f64 xhalf = 0.5*x;
//	u64 i = Memory::Transmute<f64, u64>(x); // get bits for floating value
//	i = (u64)0x5fe6eb50c7b537a9 - (i>>1); // gives initial guess y0
//	x = Memory::Transmute<u64, f64>(i); // convert bits back to float
//	x = x*(1.5-xhalf*x*x); // Newton step, repeating increases accuracy return x;
//	x = x*(1.5-xhalf*x*x); // 2nd
//
//	return x;
//#endif // ALT_USE_CUSTOM_FAST_MATH
}

ALT_NODISCARD ALT_FORCE_INLINE f32 Cbrt(const f32 x) noexcept {
    return cbrtf(x);
}

ALT_NODISCARD ALT_FORCE_INLINE f64 Cbrt(const f64 x) noexcept {
    return ::cbrt(x);
}

ALT_NODISCARD ALT_FORCE_INLINE f32 Acos(const f32 x) noexcept {
    return acosf(x);
}

ALT_NODISCARD ALT_FORCE_INLINE f64 Acos(const f64 x) noexcept {
    return ::acos(x);
}

ALT_NODISCARD ALT_FORCE_INLINE f32 Asin(f32 x) noexcept {
#ifndef ALT_USE_CUSTOM_FAST_MATH

    return asinf(x);
#else

    // https://developer.download.nvidia.com/cg/Cg_3.1/Cg-3.1_April2012_ReferenceManual.pdf
    // p. 689

    using U = typename Num::Desc<f32>::Unsigned;

    const U signBit = IsSignPositive(x) ? 0 : 1;
    const U mask = signBit << (Num::Desc<U>::Bits() - 1);

    x = Abs(x);

    f32 ret = -0.0187293f;
    ret *= x;
    ret += 0.0742610f;
    ret *= x;
    ret -= 0.2121144f;
    ret *= x;
    ret += 1.5707288f;
    ret = (f32)ALT_MATH_PI * 0.5f - Sqrt(1.0f - x) * ret;

    return Memory::Transmute<U, f32>(Memory::Transmute<f32, U>(ret) | mask);
#endif // ALT_USE_CUSTOM_FAST_MATH
}

ALT_NODISCARD ALT_FORCE_INLINE f64 Asin(f64 x) noexcept {
#ifndef ALT_USE_CUSTOM_FAST_MATH

    return ::asin(x);
#else

    // https://developer.download.nvidia.com/cg/Cg_3.1/Cg-3.1_April2012_ReferenceManual.pdf
    // p. 689

    using U = typename Num::Desc<f64>::Unsigned;

    const U signBit = IsSignPositive(x) ? 0 : 1;
    const U mask = signBit << (Num::Desc<U>::Bits() - 1);

    x = Abs(x);

    f64 ret = -0.0187293;
    ret *= x;
    ret += 0.0742610;
    ret *= x;
    ret -= 0.2121144;
    ret *= x;
    ret += 1.5707288;
    ret = (f64)ALT_MATH_PI * 0.5f - Sqrt(1.0 - x) * ret;

    return Memory::Transmute<U, f64>(Memory::Transmute<f64, U>(ret) | mask);
#endif // ALT_USE_CUSTOM_FAST_MATH
}

ALT_NODISCARD ALT_FORCE_INLINE f32 Atan(const f32 x) noexcept {
#ifndef ALT_USE_CUSTOM_FAST_MATH

    return atanf(x);
#else

    // https://developer.download.nvidia.com/cg/Cg_3.1/Cg-3.1_April2012_ReferenceManual.pdf
    // p. 692

    f32 t0, t1,/* t2,*/ t3, t4;

    t3 = 1.0f;
    t1 = Abs(x);

    t0 = Max(t3, t1);
    t1 = Min(t3, t1);
    t3 = 1.0f / t0;
    t3 = t1 * t3;

    t4 = t3 * t3;
    t0 =         - 0.013480470f;
    t0 = t0 * t4 + 0.057477314f;
    t0 = t0 * t4 - 0.121239071f;
    t0 = t0 * t4 + 0.195635925f;
    t0 = t0 * t4 - 0.332994597f;
    t0 = t0 * t4 + 0.999995630f;
    t3 = t0 * t3;

    t3 = Abs(x) > 1.0f ? 1.570796327f - t3 : t3;
    t3 = IsSignPositive(x) ? t3 : -t3;

    return t3;
#endif // ALT_USE_CUSTOM_FAST_MATH
}

ALT_NODISCARD ALT_FORCE_INLINE f64 Atan(const f64 x) noexcept {
#ifndef ALT_USE_CUSTOM_FAST_MATH

    return ::atan(x);
#else

    // https://developer.download.nvidia.com/cg/Cg_3.1/Cg-3.1_April2012_ReferenceManual.pdf
    // p. 692

    f64 t0, t1,/* t2,*/ t3, t4;

    t3 = 1.0;
    t1 = Abs(x);

    t0 = Max(t3, t1);
    t1 = Min(t3, t1);
    t3 = 1.0 / t0;
    t3 = t1 * t3;

    t4 = t3 * t3;
    t0 =         - 0.013480470;
    t0 = t0 * t4 + 0.057477314;
    t0 = t0 * t4 - 0.121239071;
    t0 = t0 * t4 + 0.195635925;
    t0 = t0 * t4 - 0.332994597;
    t0 = t0 * t4 + 0.999995630;
    t3 = t0 * t3;

    t3 = Abs(x) > 1.0 ? 1.570796327 - t3 : t3;
    t3 = IsSignPositive(x) ? t3 : -t3;

    return t3;
#endif // ALT_USE_CUSTOM_FAST_MATH
}

ALT_NODISCARD ALT_FORCE_INLINE f32 Atan2(const f32 y, const f32 x) noexcept {
#ifndef ALT_USE_CUSTOM_FAST_MATH

    return atan2f(y, x);
#else

    // https://developer.download.nvidia.com/cg/Cg_3.1/Cg-3.1_April2012_ReferenceManual.pdf
    // p. 690-691

    f32 t0, t1,/* t2,*/ t3, t4;

    t3 = Abs(x);
    t1 = Abs(y);

    t0 = Max(t3, t1);
    t1 = Min(t3, t1);
    t3 = 1.0f / t0;
    t3 = t1 * t3;

    t4 = t3 * t3;
    t0 =         - 0.013480470f;
    t0 = t0 * t4 + 0.057477314f;
    t0 = t0 * t4 - 0.121239071f;
    t0 = t0 * t4 + 0.195635925f;
    t0 = t0 * t4 - 0.332994597f;
    t0 = t0 * t4 + 0.999995630f;
    t3 = t0 * t3;

    t3 = Abs(y) > Abs(x) ? 1.570796327f - t3 : t3;
    t3 = IsSignPositive(x) ? t3 : 3.141592654f - t3;
    t3 = IsSignPositive(y) ? t3 : -t3;

    return t3;
#endif // ALT_USE_CUSTOM_FAST_MATH
}

ALT_NODISCARD ALT_FORCE_INLINE f64 Atan2(const f64 y, const f64 x) noexcept {
#ifndef ALT_USE_CUSTOM_FAST_MATH

    return ::atan2(y, x);
#else

    // https://developer.download.nvidia.com/cg/Cg_3.1/Cg-3.1_April2012_ReferenceManual.pdf
    // p. 690-691

    f64 t0, t1,/* t2,*/ t3, t4;

    t3 = Abs(x);
    t1 = Abs(y);

    t0 = Max(t3, t1);
    t1 = Min(t3, t1);
    t3 = 1.0 / t0;
    t3 = t1 * t3;

    t4 = t3 * t3;
    t0 =         - 0.013480470;
    t0 = t0 * t4 + 0.057477314;
    t0 = t0 * t4 - 0.121239071;
    t0 = t0 * t4 + 0.195635925;
    t0 = t0 * t4 - 0.332994597;
    t0 = t0 * t4 + 0.999995630;
    t3 = t0 * t3;

    t3 = Abs(y) > Abs(x) ? 1.570796327 - t3 : t3;
    t3 = IsSignPositive(x) ? t3 : 3.141592654 - t3;
    t3 = IsSignPositive(y) ? t3 : -t3;

    return t3;
#endif // ALT_USE_CUSTOM_FAST_MATH
}

ALT_NODISCARD ALT_FORCE_INLINE f32 Cos(const f32 x) noexcept {
    return cosf(x);
}

ALT_NODISCARD ALT_FORCE_INLINE f64 Cos(const f64 x) noexcept {
    return ::cos(x);
}

ALT_NODISCARD ALT_FORCE_INLINE f32 Cosh(const f32 x) noexcept {
    return coshf(x);
}

ALT_NODISCARD ALT_FORCE_INLINE f64 Cosh(const f64 x) noexcept {
    return ::cosh(x);
}

ALT_NODISCARD ALT_FORCE_INLINE f32 Sin(const f32 x) noexcept {
    return sinf(x);
}

ALT_NODISCARD ALT_FORCE_INLINE f64 Sin(const f64 x) noexcept {
    return ::sin(x);
}

ALT_NODISCARD ALT_FORCE_INLINE f32 Sinh(const f32 x) noexcept {
    return sinhf(x);
}

ALT_NODISCARD ALT_FORCE_INLINE f64 Sinh(const f64 x) noexcept {
    return ::sinh(x);
}

ALT_NODISCARD ALT_FORCE_INLINE f32 Tan(const f32 x) noexcept {
    return tanf(x);
}

ALT_NODISCARD ALT_FORCE_INLINE f64 Tan(const f64 x) noexcept {
    return ::tan(x);
}

ALT_NODISCARD ALT_FORCE_INLINE f32 Tanh(const f32 x) noexcept {
    return tanhf(x);
}

ALT_NODISCARD ALT_FORCE_INLINE f64 Tanh(const f64 x) noexcept {
    return ::tanh(x);
}

ALT_NODISCARD ALT_FORCE_INLINE f32 Exp(const f32 x) noexcept {
    return expf(x);
}

ALT_NODISCARD ALT_FORCE_INLINE f64 Exp(const f64 x) noexcept {
    return ::exp(x);
}

ALT_NODISCARD ALT_FORCE_INLINE f32 Exp2(const f32 x) noexcept {
    return exp2f(x);
}

ALT_NODISCARD ALT_FORCE_INLINE f64 Exp2(const f64 x) noexcept {
    return ::exp2(x);
}

ALT_NODISCARD ALT_FORCE_INLINE f32 Log(const f32 x) noexcept {
    return logf(x);
}

ALT_NODISCARD ALT_FORCE_INLINE f64 Log(const f64 x) noexcept {
    return ::log(x);
}

ALT_NODISCARD ALT_FORCE_INLINE f32 Log10(const f32 x) noexcept {
    return log10f(x);
}

ALT_NODISCARD ALT_FORCE_INLINE f64 Log10(const f64 x) noexcept {
    return ::log10(x);
}

ALT_NODISCARD ALT_FORCE_INLINE f32 Pow(const f32 x, const f32 pow) noexcept {
    return powf(x, pow);
}

ALT_NODISCARD ALT_FORCE_INLINE f64 Pow(const f64 x, const f64 pow) noexcept {
    return ::pow(x, pow);
}

} // namespace Math
} // namespace Alt

#endif // __ALT_STD_MATH_PRE_INCLUDE__