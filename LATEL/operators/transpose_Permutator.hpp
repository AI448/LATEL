#ifndef LATEL_OPERATORS_TRANSPOSE_PERMUTATOR_HPP_
#define LATEL_OPERATORS_TRANSPOSE_PERMUTATOR_HPP_


#include "../common.hpp"


namespace LATEL
{


namespace _transpose_Permutator
{


template<class PermutatorType>
class TransposedPermutatorView
{
  static_assert(!std::is_rvalue_reference_v<PermutatorType>);
  static_assert(LATEL::permutator_concept<std::remove_reference_t<PermutatorType>>);

public:

  using permutator_category = LATEL::permutator_tag;
  using index_type = typename std::remove_reference_t<PermutatorType>::index_type;
  static inline constexpr index_type null_index = std::remove_reference_t<PermutatorType>::null_index;

private:

  PermutatorType _permutator;

public:

  explicit TransposedPermutatorView(PermutatorType&& permutator):
    _permutator(std::forward<PermutatorType>(permutator))
  {}

  decltype(auto) row_dimension() const noexcept
  {
    return _permutator.column_dimension();
  }

  decltype(auto) column_dimension() const noexcept
  {
    return _permutator.row_dimension();
  }

  decltype(auto) permutate(const std::integral auto& index) const noexcept
  {
    return _permutator.unpermutate(index);
  }

  decltype(auto) unpermutate(const std::integral auto& index) const noexcept
  {
    return _permutator.permutate(index);
  }

};


}


template<LATEL::permutator_concept PermutatorType>
decltype(auto) transpose(PermutatorType&& permutator)
{
  return _transpose_Permutator::TransposedPermutatorView<PermutatorType>(std::forward<PermutatorType>(permutator));
}


}


#endif
