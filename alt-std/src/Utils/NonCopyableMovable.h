#pragma once

#ifndef __ALT_STD_NON_COPYABLE_MOVABLE__
#define __ALT_STD_NON_COPYABLE_MOVABLE__

/**
 * @brief Prevents a class from being copied.
 * 
 * @param class_ The name of the class to which the non-copyable restriction will be applied.
 * @example
 * 
 * class NonCopy {
 * 
 *     ALT_MAKE_CLASS_NON_COPYABLE(NonCopy)
 * };
 * 
 * NonCopy res;
 * // NonCopy copy = res; // Compilation Error: Use of deleted function
 */
#define ALT_MAKE_CLASS_NON_COPYABLE(class_) \
    class_(const class_&) = delete; \
    class_& operator=(const class_&) = delete;

/**
 * @brief Prevents a class from being moved.
 * 
 * @param class_ The name of the class to which the non-movable restriction will be applied.
 * @example
 * 
 * class NonMove {
 * 
 *     ALT_MAKE_CLASS_NON_MOVABLE(NonMove)
 * };
 * 
 * NonMove obj;
 * // NonMove moved = std::move(obj); // Compilation Error: Use of deleted function
 */
#define ALT_MAKE_CLASS_NON_MOVABLE(class_) \
    class_(class_&&) noexcept = delete; \
    class_& operator=(class_&&) noexcept = delete;

#endif // __ALT_STD_NON_COPYABLE_MOVABLE__