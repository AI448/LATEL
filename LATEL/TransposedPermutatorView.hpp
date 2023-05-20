#ifndef LATEL_TRANSPOSEDPERMUTATORVIEW_HPP_
#define LATEL_TRANSPOSEDPERMUTATORVIEW_HPP_


#include "foundations.hpp"


namespace LATEL
{


template<class PermutatorT>
class TransposedPermutatorView
{
public:

  using index_type = std::remove_reference_t<PermutatorT>::index_type;

private:

  PermutatorT _permutator;

public:

  explicit TransposedPermutatorView(PermutatorT&& permutator):
    _permutator(std::forward<PermutatorT>(permutator))
  {}

  decltype(auto) row_dimension() const noexcept
  {
    return _permutator.column_dimension();
  }

  decltype(auto) column_dimension() const noexcept
  {
    return _permutator.row_dimension();
  }

  template<std::integral I>
  decltype(auto) permutate(const I& i) const noexcept
  {
    return _permutator.unpermutate(i);
  }

  template<std::integral I>
  decltype(auto) unpermutate(const I& i) const noexcept
  {
    return _permutator.permutate(i);
  }

};


template<permutator_concept PermutatorT>
decltype(auto) transpose(PermutatorT&& P)
{
  return TransposedPermutatorView<PermutatorT>(std::forward<PermutatorT>(P));
}


template<permutator_concept PermutatorT>
decltype(auto) inverse(PermutatorT&& P)
{
  return TransposedPermutatorView<PermutatorT>(std::forward<PermutatorT>(P));
}


}



#endif
