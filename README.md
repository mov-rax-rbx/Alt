# Alternative C++ Standard Library

This library exists because I want to improve the C++ standard library. It is not intended as a replacement for the standard library.

## Example

```cpp
#include "alt-std/Math/Math.h"

#include "alt-std/Collections/StaticVector.h"
#include "alt-std/Collections/StaticBitSet.h"

int main(int argc, char** argv) {
    iferr_scope;

    constexpr usize TOTAL_NUM = 4096;

    Alt::StaticBitSet<TOTAL_NUM> primes{ true };

    primes[0] = false;
    primes[1] = false;

    const usize numIters = (usize)Alt::Math::Sqrt((f32)primes.GetSize());
    for (usize i = 2; i <= numIters; ++i) {

        if (!primes[i]) {
            continue;
        }

        for (usize k = 2;; ++k) {

            const usize num = Alt::Num::MulOverflowCheck(i, k) iferr {
                break;
            }

            if (num >= primes.GetSize()) {
                break;
            }

            primes[num] = false;
        }
    }

    const usize total = primes
        .GetIterator()
        .Fold((usize)0, [](const usize acc, const bool val) {
            return acc + (val ? 1 : 0);
        });

    ALT_PRINTLN("Total {}:\n{indices}", total, primes);

    // or ...

    Alt::StaticVector<usize, TOTAL_NUM> prms = primes
        .GetIterator()
        .Enumerate()
        .Filter([](const Alt::Pair<usize, bool>& val) {
            return val.s();
        })
        .Map([](const Alt::Pair<usize, bool>& val) {
            return val.f();
        })
        .Collect<Alt::StaticVector<usize, TOTAL_NUM>>() iferr_panic_only_dev("Never panic because reuses 'TOTAL_NUM'");

    ALT_PRINTLN("Total {}:\n{}", prms.GetSize(), prms);
}
```

## Details

This is not intended as a replacement for the standard library, but rather as a better alternative that should simplify programs written using C++. For now, the target platform is Windows only, and the target compilers are LLVM Clang and MSVC. This is my cummulative stuff which I'm using on internal projects.

## 3rd

- [xxHash](https://github.com/Cyan4973/xxHash/tree/v0.8.4) - hash
- [fast_float](https://github.com/fastfloat/fast_float/tree/v8.2.10) - string to f32/f64
- [zmij](https://github.com/vitaut/zmij/tree/v1.2) - f32/f64 to string
- [blqs](https://github.com/chkas/blqsort) - sorting algorithm
