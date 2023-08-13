#ifndef LATEL_PERMUTATEDVECTORVIEW_HPP_
#define LATEL_PERMUTATEDVECTORVIEW_HPP_


#include "LATEL_common.hpp"


namespace LATEL
{


template<class PermutatorT, class VectorT>
class PermutatedVectorView
{
public:

  using index_type = std::common_type_t<typename std::remove_reference_t<PermutatorT>::index_type, typename std::remove_reference_t<VectorT>::index_type>;

  using value_type = std::remove_reference_t<VectorT>::value_type;

private:

  PermutatorT _permutator;
  VectorT _vector;

public:

  PermutatedVectorView(PermutatorT&& permutator, VectorT&& vector):
    _permutator(std::forward<PermutatorT>(permutator)), _vector(std::forward<VectorT>(vector))
  {
    assert(_permutator.column_dimension() == vector.dimension());
  }

  decltype(auto) dimension() const noexcept
  {
    return _permutator.row_dimension();
  }

  decltype(auto) size() const noexcept
  {
    return _vector.size();
  }

private:

  template<class IteratorT>
  decltype(auto) make_permutated_iterator(IteratorT&& iterator) const noexcept
  {
    return ACCBOOST2::make_map_iterator(
      [&](auto&& pair) -> decltype(auto)
      {
        return std::forward_as_tuple(
          _permutator.permutate(ACCBOOST2::get<0>(std::forward<decltype(pair)>(pair))),
          ACCBOOST2::get<1>(std::forward<decltype(pair)>(pair))
        );
      },
      std::forward<IteratorT>(iterator)
    );
  }

public:

  decltype(auto) begin() const noexcept
  {
    return make_permutated_iterator(_vector.begin());
  }

  decltype(auto) end() const noexcept
  {
    return make_permutated_iterator(_vector.end());
  }

};


template<permutator_concept PermutatorT, vector_concept VectorT>
decltype(auto) operator*(PermutatorT&& permutator, VectorT&& vector)
{
  return PermutatedVectorView<PermutatorT, VectorT>(std::forward<PermutatorT>(permutator), std::forward<VectorT>(vector));
}


}


#endif
