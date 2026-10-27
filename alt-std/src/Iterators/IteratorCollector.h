#pragma once

#ifndef __ALT_STD_ITERATOR_COLLECTOR__
#define __ALT_STD_ITERATOR_COLLECTOR__

/*
 * Implementation for 'Collect...(...)'
 * 
 * Example of usage:
 * 
 *     template <typename T>
 *     class IteratorCollector<Vector<T>> {
 *     public:
 * 
 *         using SelfT = Vector<T>;
 * 
 *         template <typename U = T,
 *             std::enable_if_t<std::is_assignable_v<T&, U>>* = nullptr>
 *         static bool ExtendMove(SelfT& self, IteratorInterface<U, IIteratorForwardTrait> iter) noexcept {
 *             iferr_scope;
 * 
 *             const usize reserve = Num::AddWrap(self.GetSize(), iter.GetSizeHint());
 *             self.Reserve(reserve) iferr_return false;
 * 
 *             while (Optional<U> val = iter.Next()) {
 * 
 *                 self.Push(std::move(val.GetValue())) iferr_return false;
 *             }
 * 
 *             return true;
 *         }
 *     };
 */
namespace Alt {

template <typename ContainerSelfT>
class IteratorCollector;

} // namespace Alt

#endif // __ALT_STD_ITERATOR_COLLECTOR__