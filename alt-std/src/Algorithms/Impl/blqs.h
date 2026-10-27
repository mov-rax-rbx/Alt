// https://github.com/chkas/blqsort
// SPDX-License-Identifier: MIT
// blqs.h - Branchless Quicksort
// (c) 2026 christof.kaser@gmail.com
//
#pragma once

#ifndef __ALT_STD_SORT__
#define __ALT_STD_SORT__

#include "../../TypeTraits.h"

#include "../../Utils/Num.h"

namespace Alt {
namespace Algorithms {
namespace _SortDetails {

#if defined(__arm64__)
    #define __ALT_STD_INTERNAL_SORT_PREFER_IF__ 1
#else
    #define __ALT_STD_INTERNAL_SORT_PREFER_IF__ 0
#endif

constexpr static usize SMALLPART = 1024;
constexpr static usize SWSZ = 512;
constexpr static usize UNROLL = 16;

constexpr static usize BLSZ = 512;

template <typename T, typename Compare>
ALT_FORCE_INLINE void sort2(T& a, T& b, Compare comp) noexcept {
    T x = a; T y = b;
    bool m = comp(x, y);
    a = m ? x : y; b = m ? y : x;
}

template <typename T, typename Compare>
ALT_FORCE_INLINE void sort3(T& a, T& b, T& c, Compare comp) noexcept {
    sort2(a, b, comp); sort2(b, c, comp); sort2(a, b, comp);
}

template <typename T, typename Compare>
ALT_FORCE_INLINE void sort4(T& a, T& b, T& c, T& d, Compare comp) noexcept {
    sort2(a, b, comp); sort2(c, d, comp); sort2(a, c, comp);
    sort2(b, d, comp); sort2(b, c, comp);
}

template <typename T, typename Compare>
ALT_FORCE_INLINE void sort5(T& a, T& b, T& c, T& d, T& e, Compare comp) noexcept {
    sort2(b, c, comp); sort2(d, e, comp); sort2(b, d, comp);
    sort2(a, c, comp); sort2(a, d, comp); sort2(c, e, comp);
    sort2(a, b, comp); sort2(c, d, comp); sort2(b, c, comp);
}

template <typename T, typename Compare>
ALT_FORCE_INLINE void sort6(T& a, T& b, T& c, T& d, T& e, T& f, Compare comp) noexcept {
    sort2(a, b, comp); sort2(c, d, comp); sort2(e, f, comp);
    sort2(a, c, comp); sort2(b, d, comp); sort2(e, f, comp);
    sort2(a, e, comp); sort2(b, f, comp); sort2(c, e, comp);
    sort2(d, f, comp); sort2(b, c, comp); sort2(d, e, comp);
    sort2(c, d, comp);
}

template <typename T, typename Compare>
ALT_FORCE_INLINE void sort7(T& a, T& b, T& c, T& d, T& e, T& f, T& g, Compare comp) noexcept {
    sort2(a, g, comp); sort2(c, d, comp); sort2(e, f, comp);
    sort2(a, c, comp); sort2(b, e, comp); sort2(d, g, comp);
    sort2(a, b, comp); sort2(c, f, comp); sort2(d, e, comp);
    sort2(b, c, comp); sort2(e, g, comp);
    sort2(c, d, comp); sort2(e, f, comp);
    sort2(b, c, comp); sort2(d, e, comp); sort2(f, g, comp);
}

template <typename T, typename Compare>
ALT_FORCE_INLINE void sort8(T& a, T& b, T& c, T& d, T& e, T& f, T& g, T& h, Compare comp) noexcept {
    sort2(a,b,comp); sort2(c,d,comp); sort2(e,f,comp); sort2(g,h,comp);
    sort2(a,c,comp); sort2(b,d,comp); sort2(e,g,comp); sort2(f,h,comp);
    sort2(b,c,comp); sort2(f,g,comp);
    sort2(a,e,comp); sort2(b,f,comp); sort2(c,g,comp); sort2(d,h,comp);
    sort2(c,e,comp); sort2(d,f,comp);
    sort2(b,c,comp); sort2(d,e,comp); sort2(f,g,comp);
}

template <typename T, typename Compare>
ALT_FORCE_INLINE void sort9(T& a, T& b, T& c, T& d, T& e, T& f, T& g, T& h, T& i, Compare comp) noexcept {
    sort2(a,d,comp); sort2(b,h,comp); sort2(c,f,comp); sort2(e,i,comp);
    sort2(a,h,comp); sort2(c,e,comp); sort2(d,i,comp); sort2(f,g,comp);
    sort2(a,c,comp); sort2(b,d,comp); sort2(e,f,comp); sort2(h,i,comp);
    sort2(b,e,comp); sort2(d,g,comp); sort2(f,h,comp);
    sort2(a,b,comp); sort2(c,e,comp); sort2(d,f,comp); sort2(g,i,comp);
    sort2(c,d,comp); sort2(e,f,comp); sort2(g,h,comp);
    sort2(b,c,comp); sort2(d,e,comp); sort2(f,g,comp);
}

template <typename T, typename Compare>
ALT_FORCE_INLINE void sort10(T& a, T& b, T& c, T& d, T& e, T& f, T& g, T& h, T& i, T& j, Compare comp) noexcept {
    sort2(a,b,comp); sort2(c,f,comp); sort2(d,g,comp); sort2(e,h,comp); sort2(i,j,comp);
    sort2(a,g,comp); sort2(b,i,comp); sort2(c,e,comp); sort2(d,j,comp); sort2(f,h,comp);
    sort2(a,c,comp); sort2(b,d,comp); sort2(e,f,comp); sort2(g,i,comp); sort2(h,j,comp);
    sort2(a,b,comp); sort2(c,h,comp); sort2(d,f,comp); sort2(e,g,comp); sort2(i,j,comp);
    sort2(b,c,comp); sort2(d,e,comp); sort2(f,g,comp); sort2(h,i,comp);
    sort2(b,d,comp); sort2(c,e,comp); sort2(f,h,comp); sort2(g,i,comp);
    sort2(c,d,comp); sort2(e,f,comp); sort2(g,h,comp);
}

template <typename T, typename Compare>
ALT_FORCE_INLINE void sort11(T& a, T& b, T& c, T& d, T& e, T& f, T& g, T& h, T& i, T& j, T& k, Compare comp) noexcept {
    sort2(a,j,comp); sort2(b,g,comp); sort2(c,e,comp); sort2(d,h,comp); sort2(f,i,comp);
    sort2(a,b,comp); sort2(d,f,comp); sort2(e,k,comp); sort2(g,j,comp); sort2(h,i,comp);
    sort2(b,d,comp); sort2(c,f,comp); sort2(e,h,comp); sort2(i,k,comp);
    sort2(a,e,comp); sort2(b,c,comp); sort2(d,h,comp); sort2(f,j,comp); sort2(g,i,comp);
    sort2(a,b,comp); sort2(c,g,comp); sort2(e,f,comp); sort2(h,i,comp);
    sort2(j,k,comp);
    sort2(c,e,comp); sort2(d,g,comp); sort2(f,h,comp); sort2(i,j,comp);
    sort2(b,c,comp); sort2(d,e,comp); sort2(f,g,comp); sort2(h,i,comp);
    sort2(c,d,comp); sort2(e,f,comp); sort2(g,h,comp);
}

template <typename T, typename Compare>
ALT_FORCE_INLINE void sort12(T& a, T& b, T& c, T& d, T& e, T& f, T& g, T& h, T& i, T& j, T& k, T& l, Compare comp) noexcept {
    sort2(a,i,comp); sort2(b,h,comp); sort2(c,g,comp); sort2(d,l,comp); sort2(e,k,comp); sort2(f,j,comp);
    sort2(a,c,comp); sort2(b,e,comp); sort2(d,f,comp); sort2(g,i,comp); sort2(h,k,comp); sort2(j,l,comp);
    sort2(a,b,comp); sort2(c,j,comp); sort2(e,h,comp); sort2(f,g,comp); sort2(k,l,comp);
    sort2(b,d,comp); sort2(c,h,comp); sort2(e,j,comp); sort2(i,k,comp);
    sort2(a,b,comp); sort2(c,d,comp); sort2(e,f,comp); sort2(g,h,comp); sort2(i,j,comp); sort2(k,l,comp);
    sort2(b,c,comp); sort2(d,f,comp); sort2(g,i,comp); sort2(j,k,comp);
    sort2(c,e,comp); sort2(d,g,comp); sort2(f,i,comp); sort2(h,j,comp);
    sort2(b,c,comp); sort2(d,e,comp); sort2(f,g,comp); sort2(h,i,comp); sort2(j,k,comp);
}
template <typename T, typename Compare>
ALT_FORCE_INLINE void sort13(T& a, T& b, T& c, T& d, T& e, T& f, T& g, T& h, T& i, T& j, T& k, T& l, T& m,
                          Compare comp) noexcept {
    sort2(a,m,comp); sort2(b,k,comp); sort2(c,j,comp); sort2(d,h,comp); sort2(f,l,comp); sort2(g,i,comp);
    sort2(b,g,comp); sort2(c,d,comp); sort2(e,l,comp); sort2(h,j,comp); sort2(i,k,comp);
    sort2(a,e,comp); sort2(b,c,comp); sort2(d,g,comp); sort2(h,i,comp); sort2(j,k,comp); sort2(l,m,comp);
    sort2(e,g,comp); sort2(f,j,comp); sort2(i,l,comp); sort2(k,m,comp);
    sort2(a,f,comp); sort2(d,i,comp); sort2(e,h,comp); sort2(g,l,comp); sort2(j,k,comp);
    sort2(a,b,comp); sort2(c,f,comp); sort2(g,j,comp); sort2(h,i,comp); sort2(k,l,comp);
    sort2(b,d,comp); sort2(c,e,comp); sort2(f,g,comp); sort2(j,k,comp);
    sort2(b,c,comp); sort2(d,e,comp); sort2(f,h,comp); sort2(g,i,comp);
    sort2(c,d,comp); sort2(e,f,comp); sort2(g,h,comp); sort2(i,j,comp);
    sort2(d,e,comp); sort2(f,g,comp);
}
template <typename T, typename Compare>
ALT_FORCE_INLINE void sort14(T& a, T& b, T& c, T& d, T& e, T& f, T& g, T& h, T& i, T& j, T& k, T& l,
                        T& m, T& n, Compare comp) noexcept {
    sort2(a,b,comp); sort2(c,d,comp); sort2(e,f,comp); sort2(g,h,comp); sort2(i,j,comp); sort2(k,l,comp); sort2(m,n,comp);
    sort2(a,c,comp); sort2(b,d,comp); sort2(e,i,comp); sort2(f,j,comp); sort2(k,m,comp); sort2(l,n,comp);
    sort2(a,e,comp); sort2(b,c,comp); sort2(d,h,comp); sort2(f,i,comp); sort2(g,k,comp); sort2(j,n,comp); sort2(l,m,comp);
    sort2(a,g,comp); sort2(b,f,comp); sort2(d,j,comp); sort2(e,k,comp); sort2(h,n,comp); sort2(i,m,comp);
    sort2(c,k,comp); sort2(d,l,comp); sort2(e,g,comp); sort2(h,j,comp);
    sort2(b,d,comp); sort2(c,i,comp); sort2(f,l,comp); sort2(g,h,comp); sort2(k,m,comp);
    sort2(b,e,comp); sort2(c,g,comp); sort2(d,f,comp); sort2(h,l,comp); sort2(i,k,comp); sort2(j,m,comp);
    sort2(c,e,comp); sort2(d,g,comp); sort2(f,i,comp); sort2(h,k,comp); sort2(j,l,comp);
    sort2(d,e,comp); sort2(f,g,comp); sort2(h,i,comp); sort2(j,k,comp);
    sort2(g,h,comp);
}
template <typename T, typename Compare>
ALT_FORCE_INLINE void sort15(T& a, T& b, T& c, T& d, T& e, T& f, T& g, T& h, T& i, T& j, T& k, T& l,
                        T& m, T& n, T& o, Compare comp) noexcept {
    sort2(b,c,comp); sort2(d,k,comp); sort2(e,o,comp); sort2(f,i,comp); sort2(g,n,comp); sort2(h,m,comp); sort2(j,l,comp);
    sort2(a,o,comp); sort2(b,f,comp); sort2(c,i,comp); sort2(d,h,comp); sort2(g,j,comp); sort2(k,m,comp); sort2(l,n,comp);
    sort2(a,h,comp); sort2(b,g,comp); sort2(c,j,comp); sort2(e,k,comp); sort2(f,l,comp); sort2(i,n,comp); sort2(m,o,comp);
    sort2(a,g,comp); sort2(c,e,comp); sort2(d,f,comp); sort2(h,l,comp); sort2(i,k,comp); sort2(j,m,comp); sort2(n,o,comp);
    sort2(a,d,comp); sort2(b,c,comp); sort2(e,h,comp); sort2(f,j,comp); sort2(g,i,comp); sort2(k,l,comp); sort2(m,n,comp);
    sort2(a,b,comp); sort2(c,d,comp); sort2(e,g,comp); sort2(h,j,comp); sort2(k,m,comp); sort2(l,n,comp);
    sort2(b,c,comp); sort2(d,f,comp); sort2(i,k,comp); sort2(l,m,comp);
    sort2(d,e,comp); sort2(f,g,comp); sort2(h,i,comp); sort2(j,k,comp);
    sort2(c,d,comp); sort2(e,f,comp); sort2(g,h,comp); sort2(i,j,comp); sort2(k,l,comp);
    sort2(f,g,comp); sort2(h,i,comp);
}
template <typename T, typename Compare>
ALT_FORCE_INLINE void sort16(T& a, T& b, T& c, T& d, T& e, T& f, T& g, T& h, T& i, T& j, T& k,
                            T& l, T& m, T& n, T& o, T& p, Compare comp) noexcept {
    sort2(a,n,comp); sort2(b,m,comp); sort2(c,p,comp); sort2(d,o,comp); sort2(e,i,comp);
    sort2(f,g,comp); sort2(h,l,comp); sort2(j,k,comp);
    sort2(a,f,comp); sort2(b,h,comp); sort2(c,j,comp); sort2(d,e,comp); sort2(g,n,comp);
    sort2(i,o,comp); sort2(k,p,comp); sort2(l,m,comp);
    sort2(a,b,comp); sort2(c,d,comp); sort2(e,f,comp); sort2(g,i,comp); sort2(h,j,comp);
    sort2(k,l,comp); sort2(m,n,comp); sort2(o,p,comp);
    sort2(a,c,comp); sort2(b,d,comp); sort2(e,k,comp); sort2(f,l,comp); sort2(g,h,comp);
    sort2(i,j,comp); sort2(m,o,comp); sort2(n,p,comp);
    sort2(b,c,comp); sort2(d,m,comp); sort2(e,g,comp); sort2(f,h,comp); sort2(i,k,comp);
    sort2(j,l,comp); sort2(n,o,comp);
    sort2(b,e,comp); sort2(c,g,comp); sort2(f,i,comp); sort2(h,k,comp); sort2(j,n,comp);
    sort2(l,o,comp);
    sort2(c,e,comp); sort2(d,g,comp); sort2(j,m,comp); sort2(l,n,comp);
    sort2(d,f,comp); sort2(g,i,comp); sort2(h,j,comp); sort2(k,m,comp);
    sort2(d,e,comp); sort2(f,g,comp); sort2(h,i,comp); sort2(j,k,comp); sort2(l,m,comp);
    sort2(g,h,comp); sort2(i,j,comp);
}

template <typename T, typename Compare>
ALT_FORCE_INLINE void sorting_network(T* l, usize partsz_min1, Compare comp) noexcept {
    switch (partsz_min1) {
    case 0: break;
    case 1:  sort2(l[0], l[1], comp); break;
    case 2:  sort3(l[0], l[1], l[2], comp); break;
    case 3:  sort4(l[0], l[1], l[2], l[3], comp); break;
    case 4:  sort5(l[0], l[1], l[2], l[3], l[4], comp); break;
    case 5:  sort6(l[0], l[1], l[2], l[3], l[4], l[5], comp); break;
    case 6:  sort7(l[0], l[1], l[2], l[3], l[4], l[5], l[6], comp); break;
    case 7:  sort8(l[0], l[1], l[2], l[3], l[4], l[5], l[6], l[7], comp); break;
    case 8:  sort9(l[0], l[1], l[2], l[3], l[4], l[5], l[6], l[7],
        l[8], comp); break;
    case 9:  sort10(l[0], l[1], l[2], l[3], l[4], l[5], l[6], l[7],
         l[8], l[9], comp); break;
    case 10: sort11(l[0], l[1], l[2], l[3], l[4], l[5], l[6], l[7],
        l[8], l[9], l[10], comp); break;
    case 11: sort12(l[0], l[1], l[2], l[3], l[4], l[5], l[6], l[7],
        l[8], l[9], l[10], l[11], comp); break;
    case 12: sort13(l[0], l[1], l[2], l[3], l[4], l[5], l[6], l[7],
        l[8], l[9], l[10], l[11], l[12], comp); break;
    case 13: sort14(l[0], l[1], l[2], l[3], l[4], l[5], l[6], l[7],
        l[8], l[9], l[10], l[11], l[12], l[13], comp); break;
    case 14: sort15(l[0], l[1], l[2], l[3], l[4], l[5], l[6], l[7],
        l[8], l[9], l[10], l[11], l[12], l[13], l[14], comp); break;
    case 15: sort16(l[0], l[1], l[2], l[3], l[4], l[5], l[6], l[7],
        l[8], l[9], l[10], l[11], l[12], l[13], l[14], l[15], comp); break;
    default: break;
    }
}

template <typename T, typename Compare>
ALT_FORCE_INLINE void heap_sift_down(T* left, sptr n, sptr j, T& k, Compare& comp) noexcept {
    while (j * 2 + 1 < n) {
        sptr child = j * 2 + 1;
        if (child + 1 < n && comp(left[child], left[child + 1])) child++;
        if (!comp(k, left[child])) break;
        left[j] = std::move(left[child]);
        j = child;
    }
    left[j] = std::move(k);
}

template <typename T, typename Compare>
ALT_FORCE_INLINE void heap_sort(T* left, T* right, Compare comp) noexcept {
    sptr n = right - left + 1;
    if (n < 2) return;

    for (sptr i = n / 2; ; ) {
        if (i > 0) {
            i--;
            T k = std::move(left[i]);
            heap_sift_down(left, n, i, k, comp);
        }
        else {
            n--;
            if (n == 0) return;
            T k = std::move(left[n]);
            left[n] = std::move(left[0]);
            heap_sift_down(left, n, 0, k, comp);
        }
    }
}
template <typename T, typename Compare>
ALT_FORCE_INLINE void med5(T& a, T& b, T& c, T& d, T& e, Compare comp) noexcept {
    sort2(a, b, comp); sort2(c, d, comp);
    sort2(a, c, comp); sort2(b, d, comp);
    sort2(b, c, comp); sort2(c, e, comp);
    sort2(b, c, comp);
}

template <typename T, typename Compare>
ALT_FORCE_INLINE void med7(T& a, T& b, T& c, T& d, T& e, T& f, T& g, Compare comp) noexcept {
    sort2(a, b, comp); sort2(c, d, comp); sort2(a, c, comp);
    sort2(b, d, comp); sort2(b, c, comp); sort2(e, f, comp);
    sort2(e, g, comp); sort2(a, e, comp);
    sort2(b, f, comp); sort2(c, g, comp); sort2(b, e, comp);
    sort2(c, e, comp);
    sort2(d, f, comp); sort2(d, e, comp);
}

template <typename T, typename Compare>
ALT_FORCE_INLINE T* partition_small(T* left, T* right, Compare comp) noexcept {
    static_assert(std::is_trivially_copyable_v<T>);

    T* outerleft = left;
    T* pivp = left + 8;

    T piv = *pivp;
    T l1 = left[1], l2 = left[2], l3 = left[3];
    T r2 = right[-2], r1 = right[-1], r0 = *right;
    med7(l1, l2, l3, piv, r2, r1, r0, comp);
    left[1] = l1; left[2] = l2; left[3] = l3;
    right[-2] = r2; right[-1] = r1; *right = r0;
    left += 4;
    right -= 3;

    *pivp = *outerleft;
    T swbuf[SMALLPART];
    T *sw = swbuf, *lwr = left;

    while (right - left >= UNROLL) for (ssize i = UNROLL; i--;) {
#if __ALT_STD_INTERNAL_SORT_PREFER_IF__
        T x = *left++;
        if (comp(x, piv)) *lwr++ = x;
        else *sw++ = x;
#else
        bool h = comp(*left, piv);
        *lwr = *sw = *left++;
        lwr += h; sw += !h;
#endif
    }
    while (left <= right) {
#if __ALT_STD_INTERNAL_SORT_PREFER_IF__
        T x = *left++;
        if (comp(x, piv)) *lwr++ = x;
        else *sw++ = x;
#else
        bool h = comp(*left, piv);
        *lwr = *sw = *left++;
        lwr += h; sw += !h;
#endif
    }
    std::memcpy(lwr, swbuf, (sw - swbuf) * sizeof(T));
    lwr -= 1;
    *outerleft = *lwr;
    *lwr = piv;
    return lwr;
}

template <typename T, typename Compare>
ALT_FORCE_INLINE T* partition_large(T* left, T* right, Compare comp) noexcept {
    static_assert(std::is_trivially_copyable_v<T>);

    T* outerleft = left;
    T* pivp = left + (right - left) / 2;
    T piv = *pivp;

    //unsigned r = blqs_rand();
    ssize r = 4;
    ssize j = r & 7;
    med5(left[1 + j], left[2 + j], left[3 + j], left[4 + j], left[5 + j], comp);
    med5(left[11 + j], left[12 + j], left[13 + j], left[14 + j], left[15 + j], comp);
    med5(left[21 + j], left[22 + j], left[23 + j], left[24 + j], left[25 + j], comp);
    med5(pivp[-2], pivp[-1], piv, pivp[1], pivp[2], comp);
    med5(right[-24 - j], right[-23 - j], right[-22 - j], right[-21 - j], right[-20 - j], comp);
    med5(right[-14 - j], right[-13 - j], right[-12 - j], right[-11 - j], right[-10 - j], comp);
    med5(right[-4 - j], right[-3 - j], right[-2 - j], right[-1 - j], right[ 0 - j], comp);

    med7(left[3 + j], left[13 + j], left[23 + j], piv,	right[-22 - j], right[-12 - j],
        right[-2 - j] , comp);

    left += 1;
    *pivp = *outerleft;

    T swbuf[SWSZ];
    T *lwr = left, *rwr = right, *sw = swbuf;

    while (UNROLL < SWSZ - (sw - swbuf) && left < right - UNROLL) {
        sptr avail = Num::Min(right - left, SWSZ - (sw - swbuf));
        T* endp = right - avail;
        while (right > endp + UNROLL) {
            for (ssize i = UNROLL; i--;) {
#if __ALT_STD_INTERNAL_SORT_PREFER_IF__
                T x = *right--;
                if (comp(x, piv)) *sw++ = x;
                else *rwr-- = x;
#else
                bool h = comp(*right, piv);
                *rwr = *sw = *right--;
                rwr -= !h; sw += h;
#endif
            }
        }
    }

    while (right - left >= UNROLL &&
            (rwr - right > UNROLL || left - lwr > UNROLL)) {

        while (rwr - right > UNROLL && right - left >= UNROLL) {
            for (ssize i = UNROLL; i--;) {
#if __ALT_STD_INTERNAL_SORT_PREFER_IF__
                T x = *left++;
                if (comp(x, piv)) *lwr++ = x;
                else *rwr-- = x;
#else
                bool h = comp(*left, piv);
                *lwr = *rwr = *left++;
                lwr += h; rwr -= !h;
#endif
            }
        }
        while (left - lwr > UNROLL && right - left >= UNROLL) {
            for (ssize i = UNROLL; i--;) {
#if __ALT_STD_INTERNAL_SORT_PREFER_IF__
                T x = *right--;
                if (comp(x, piv)) *lwr++ = x;
                else *rwr-- = x;
#else
                bool h = comp(*right, piv);
                *rwr = *lwr = *right--;
                rwr -= !h; lwr += h;
#endif
            }
        }
    }
    while ((lwr < left||rwr > right) && left <= right) {
        while (rwr > right && left <= right) {
            T x = *left++;
            if (comp(x, piv)) *lwr++ = x;
            else *rwr-- = x;
        }
        while (lwr < left && left <= right) {
            T x = *right--;
            if (comp(x, piv)) *lwr++ = x;
            else *rwr-- = x;
        }
    }
    while (left <= right && !comp(*right, piv)) {
        right--;
        rwr--;
    }
    std::memcpy(lwr, swbuf, (sw - swbuf) * sizeof(T));
    *outerleft = *rwr;
    *rwr = piv;
    return rwr;
}

template <typename T, typename Compare>
inline void smallsort(T* left, T* right, Compare comp) noexcept {
    while (right - left > 15) {
        T* mid = partition_small(left, right, comp);
        smallsort(left, mid - 1, comp);
        left = mid + 1;
    }
    sorting_network(left, right - left, comp);
}

template <typename T, typename Compare>
inline void blqsort(T* left, T* right, Compare comp) noexcept {
    while (1) {
        sptr partszm1 = right - left;
        if (partszm1 <= SMALLPART) break;

        T* mid = partition_large(left, right, comp);

        if (mid - left < partszm1 / 16) {
            if (mid > left) blqsort(left, mid - 1, comp);
            T piv = *mid;
            mid += 1;
            // collect duplicates
            for (T* p = mid; p <= right; p++) {
                if (!comp(piv, *p)) {
                    Swap(*mid, *p);
                    mid++;
                }
            }
            left = mid;
            if (right - left < SMALLPART) break;

            // second chance before fallback to heapsort
            mid = partition_large(left, right, comp);
            if (mid - left < partszm1 / 16) {
                if (mid > left) heap_sort(left, mid - 1, comp);
                if (mid < right) heap_sort(mid + 1, right, comp);
                return;
            }
        }
        if (mid - left < right - mid) {
            blqsort(left, mid - 1, comp);
            left = mid + 1;
        }
        else {
            blqsort(mid + 1, right, comp);
            right = mid - 1;
        }
    }
    smallsort(left, right, comp);
}

template <typename T, typename Compare>
ALT_FORCE_INLINE void insert_sort(T* left, T* right, Compare comp) noexcept {
    for (T* i = left + 1; i <= right; i++) {
        T key = std::move(*i);
        T* j = i;
        while (j > left && comp(key, *(j - 1))) {
            *j = std::move(*(j - 1));
            j--;
        }
        *j = std::move(key);
    }
}

template <typename T, typename Compare>
ALT_FORCE_INLINE void med3(T* a, T* b, T* c, Compare comp) noexcept {
    if (comp(*b, *a)) Swap(*a, *b);
    if (comp(*c, *a)) Swap(*a, *c);
    if (comp(*c, *b)) Swap(*b, *c);
}

// Based on BlockQuicksort by Edelkamp and Weiß

template <typename T, typename Compare>
inline void block_qsort(T* left0, T* right0, Compare comp) noexcept {

    while (right0 - left0 > 16) {

        usize sizem1 = right0 - left0;
        T* mid = left0 + sizem1 / 2;

        T* left = left0 + 1;
        T* right = right0;

        if (sizem1 > 64) {
            med3(left, left + 1, left + 2, comp);
            med3(mid - 1, mid, mid + 1, comp);
            med3(right - 2, right - 1, right, comp);
            med3(left + 1, mid, right - 1, comp);
        }
        else {
            med3(left, mid, right, comp);
            left += 1;
            right -= 1;
        }

        T pivot = std::move(*mid);
        *mid = std::move(*left0);

        T* left_p[BLSZ];
        T* right_p[BLSZ];

        ssize nleft = 0;
        ssize nright = 0;
        ssize left_offs = 0, right_offs = 0;	// Initialization to satisfy sanitizers

        while (right >= left) {
            ssize blsz = Num::Min(BLSZ, right - left + 1);
            if (nleft == 0) {
                for (T* endl = left + blsz; left < endl; left++) {
                    left_p[nleft] = left;
                    nleft += !comp(*left, pivot);
                }
                left_offs = 0;
            }
            else {
                for (T* endr = right - blsz; right > endr; right--) {
                    right_p[nright] = right;
                    nright += !comp(pivot, *right);
                }
                right_offs = 0;
            }
            ssize nswaps = Num::Min(nleft, nright);
            for (ssize k = 0; k < nswaps; k++) {
                Swap(*left_p[left_offs], *right_p[right_offs]);
                left_offs++;
                right_offs++;
            }
            nleft -= nswaps;
            nright -= nswaps;
        }
        if (nleft) {
            for (ssize k = left_offs + nleft - 1; k >= left_offs; k--, right--) {
                Swap(*left_p[k], *right);
            }
        }
        else {
            for (ssize k = right_offs + nright - 1; k >= right_offs; k--, left++) {
                Swap(*right_p[k], *left);
            }
            right = left - 1;
        }
        *left0 = std::move(*right);
        *right = std::move(pivot);

        sptr szl = right - left0;
        sptr szr = right0 - right;
        sptr szmin = (szl < szr) ? szl : szr;

        if (szmin < (right0 - left0) / 16) {
            if (right > left0) heap_sort(left0, right - 1, comp);
            if (right < right0) heap_sort(right + 1, right0, comp);
            return;
        }
        if (szl < szr) {
            block_qsort(left0, right - 1, comp);
            left0 = right + 1;
        } else {
            block_qsort(right + 1, right0, comp);
            right0 = right - 1;
        }
    }
    insert_sort(left0, right0, comp);
}

template <typename T, typename Compare/* = std::less<T>*/>
ALT_FORCE_INLINE void sort(T* first, T* last, Compare comp) noexcept {

    if (last - first < 2) return;
    for (T* p = first + 1; p < last; p++) {
        if (comp(*p, *(p - 1))) goto not_asc_sorted;
    }
    return;
not_asc_sorted:
    for (T* p = last - 1; p > first; p--) {
        if (comp(*(p - 1), *p)) goto not_desc_sorted;
    }
    for (sptr i = 0; i < (last - first) / 2; i++) {
        Swap(first[i], *(last - i - 1));
    }
    return;
not_desc_sorted:

    constexpr bool cheap =
        std::is_trivially_copyable_v<T> &&
        std::is_trivially_default_constructible_v<T> &&
        sizeof(T) <= 16;

    if constexpr (cheap) {
        blqsort(first, last - 1, comp);
    }
    else {
        block_qsort(first, last - 1, comp);
    }
}

#undef __ALT_STD_INTERNAL_SORT_PREFER_IF__

} // namespace _SortDetails
} // namespace Algorithms
} // namespace Alt

#endif // __ALT_STD_SORT__