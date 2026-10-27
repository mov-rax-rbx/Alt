#pragma once

#include "Impl/MathPreInclude.h"

#include "Impl/Vec2.h"
#include "Impl/Vec3.h"
#include "Impl/Vec4.h"
#include "Impl/Point3.h"

#include "Impl/Quat.h"
#include "Impl/DualQuat.h"

#include "Impl/Matrix44.h"

#include "Impl/AABB2.h"
#include "Impl/AABB3.h"

#ifdef ALT_USE_MATH_TYPES_IN_GLOBAL_NAMESPACE

template <typename T>
using Vec2 = Alt::Math::Vec2<T>;

using Vec2u8 = Alt::Math::Vec2<u8>;
using Vec2u16 = Alt::Math::Vec2<u16>;
using Vec2u32 = Alt::Math::Vec2<u32>;
using Vec2u64 = Alt::Math::Vec2<u64>;
using Vec2usize = Alt::Math::Vec2<usize>;

using Vec2s8 = Alt::Math::Vec2<s8>;
using Vec2s16 = Alt::Math::Vec2<s16>;
using Vec2s32 = Alt::Math::Vec2<s32>;
using Vec2s64 = Alt::Math::Vec2<s64>;
using Vec2ssize = Alt::Math::Vec2<ssize>;

using Vec2f32 = Alt::Math::Vec2<f32>;
using Vec2f64 = Alt::Math::Vec2<f64>;

template <typename T>
using Vec3 = Alt::Math::Vec3<T>;

using Vec3u8 = Alt::Math::Vec3<u8>;
using Vec3u16 = Alt::Math::Vec3<u16>;
using Vec3u32 = Alt::Math::Vec3<u32>;
using Vec3u64 = Alt::Math::Vec3<u64>;
using Vec3usize = Alt::Math::Vec3<usize>;

using Vec3s8 = Alt::Math::Vec3<s8>;
using Vec3s16 = Alt::Math::Vec3<s16>;
using Vec3s32 = Alt::Math::Vec3<s32>;
using Vec3s64 = Alt::Math::Vec3<s64>;
using Vec3ssize = Alt::Math::Vec3<ssize>;

using Vec3f32 = Alt::Math::Vec3<f32>;
using Vec3f64 = Alt::Math::Vec3<f64>;

template <typename T>
using Point3 = Alt::Math::Point3<T>;

using Point3f32 = Alt::Math::Point3f32;
using Point3f64 = Alt::Math::Point3f64;

template <typename T>
using Vec4 = Alt::Math::Vec4<T>;

using Vec4u8 = Alt::Math::Vec4<u8>;
using Vec4u16 = Alt::Math::Vec4<u16>;
using Vec4u32 = Alt::Math::Vec4<u32>;
using Vec4u64 = Alt::Math::Vec4<u64>;
using Vec4usize = Alt::Math::Vec4<usize>;

using Vec4s8 = Alt::Math::Vec4<s8>;
using Vec4s16 = Alt::Math::Vec4<s16>;
using Vec4s32 = Alt::Math::Vec4<s32>;
using Vec4s64 = Alt::Math::Vec4<s64>;
using Vec4ssize = Alt::Math::Vec4<ssize>;

using Vec4f32 = Alt::Math::Vec4<f32>;
using Vec4f64 = Alt::Math::Vec4<f64>;

template <typename T>
using Quat = Alt::Math::Quat<T>;

using Quatf32 = Alt::Math::Quatf32;
using Quatf64 = Alt::Math::Quatf64;

template <typename T>
using DualQuat = Alt::Math::DualQuat<T>;

using DualQuatf32 = Alt::Math::DualQuatf32;
using DualQuatf64 = Alt::Math::DualQuatf64;

template <typename T>
using Matrix44 = Alt::Math::Matrix44<T>;

using Matrix44f32 = Alt::Math::Matrix44f32;
using Matrix44f64 = Alt::Math::Matrix44f64;

template <typename T>
using AABB2 = Alt::Math::AABB2<T>;

using AABB2f32 = Alt::Math::AABB2f32;
using AABB2f64 = Alt::Math::AABB2f64;

template <typename T>
using AABB3 = Alt::Math::AABB3<T>;

using AABB3f32 = Alt::Math::AABB3f32;
using AABB3f64 = Alt::Math::AABB3f64;

#endif // ALT_USE_MATH_TYPES_IN_GLOBAL_NAMESPACE