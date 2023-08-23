#ifndef LATEL_OPERATORS_PRODUCT_PERMUTATOR_VECTOR_HPP_
#define LATEL_OPERATORS_PRODUCT_PERMUTATOR_VECTOR_HPP_


#include "../common.hpp"
#include "map_Vector.hpp"
#include "filter_Vector.hpp"


namespace LATEL
{


namespace _product_Permutator_Vector
{


template<class PermutatorType, class VectorType>
class PermutatedVectorView
{
public:

  using vector_category = LATEL::sequential_access_vector_tag;
  using index_type = std::common_type_t<typename std::remove_reference_t<PermutatorType>::index_type, typename std::remove_reference_t<VectorType>::index_type>;
  using value_type = typename std::remove_reference_t<VectorType>::value_type;

private:

  PermutatorType _permutator;
  VectorType _vector;

public:

  explicit PermutatedVectorView(PermutatorType&& permutator, VectorType&& vector):
    _permutator(std::forward<PermutatorType>(permutator)), _vector(std::forward<VectorType>(vector))
  {
    assert(_permutator.column_dimension() == _vector.dimension());
  }

  decltype(auto) dimension() const noexcept
  {
    return _permutator.row_dimension();
  }

  decltype(auto) upper_of_nonzeros() const noexcept
  {
    return std::min<index_type>(_permutator.row_dimension(), _vector.upper_of_nonzeros());
  }

private:

  struct Permutator
  {
    const PermutatedVectorView* p = nullptr;
    decltype(auto) operator()(auto&& pair) const noexcept
    {
      assert(p != nullptr);
      return ACCBOOST2::capture_as_tuple(
        p->_permutator.permutate(ACCBOOST2::get<0>(std::forward<decltype(pair)>(pair))),
        ACCBOOST2::get<1>(std::forward<decltype(pair)>(pair))
      );
    }
  };

  struct Filter
  {
    decltype(auto) operator()(auto&& pair) const noexcept
    {
      return ACCBOOST2::get<0>(pair) != std::remove_reference_t<PermutatorType>::null_index;
    }
  };

public:

  decltype(auto) begin() const noexcept
  {
    return ACCBOOST2::make_filter_iterator(Filter{}, ACCBOOST2::make_map_iterator(Permutator{this}, _vector.begin()), ACCBOOST2::make_map_iterator_or_sentinel(Permutator{this}, _vector.end()));
  }

  decltype(auto) end() const noexcept
  {
    return ACCBOOST2::make_filter_sentinel(ACCBOOST2::make_map_iterator_or_sentinel(Permutator{this}, _vector.end()));
  }

};


}


template<LATEL::permutator_concept PermutatorType, LATEL::sequential_access_vector_concept VectorType>
decltype(auto) operator*(PermutatorType&& permutator, VectorType&& vector)
{
  assert(permutator.column_dimension() == vector.dimension());

  return _product_Permutator_Vector::PermutatedVectorView<PermutatorType, VectorType>(std::forward<PermutatorType>(permutator), std::forward<VectorType>(vector));
  
  // using R = std::ranges::range_reference_t<const VectorType&>;
  // static_assert(ACCBOOST2::is_tuple<R>);

  // struct F
  // {
  //   PermutatorType permutator;
  //   decltype(auto) operator()(R&& pair) const noexcept
  //   {
  //     return ACCBOOST2::capture_as_tuple(
  //       permutator.permutate(ACCBOOST2::get<0>(std::forward<R>(pair))),
  //       ACCBOOST2::get<1>(std::forward<R>(pair))
  //     );
  //   }
  // };

  // return LATEL::filter(
  //   [null_index = std::remove_reference_t<PermutatorType>::null_index](auto&& pair) -> decltype(auto)
  //   {
  //     return ACCBOOST2::get<0>(pair) != null_index;
  //   },
  //   LATEL::map(
  //     F{std::forward<PermutatorType>(permutator)},
  //     std::forward<VectorType>(vector)
  //   )
  // );
}


}


#endif
