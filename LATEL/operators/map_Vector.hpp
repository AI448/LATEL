#ifndef LATEL_OPERATORS_MAP_VECTOR_HPP_
#define LATEL_OPERATORS_MAP_VECTOR_HPP_


#include "../common.hpp"
#include "../VectorView.hpp"


namespace LATEL
{


namespace _map_Vector
{


template<class FunctorType, class VectorType>
class MappedVectorView
{
  static_assert(std::invocable<const FunctorType&, std::ranges::range_reference_t<const VectorType&>>);
  using result_type = std::invoke_result_t<const FunctorType&, std::ranges::range_reference_t<const VectorType&>>;
  static_assert(ACCBOOST2::is_tuple<std::remove_reference_t<result_type>>);

public:

  using vector_category = LATEL::sequential_access_vector_tag;
  using index_type = std::remove_cv_t<std::remove_reference_t<std::tuple_element_t<0, std::remove_reference_t<result_type>>>>;
  using value_type = std::remove_cv_t<std::remove_reference_t<std::tuple_element_t<1, std::remove_reference_t<result_type>>>>;

private:

  FunctorType _functor;
  VectorType _vector;

public:

  explicit MappedVectorView(FunctorType&& functor, VectorType&& vector):
    _functor(std::forward<FunctorType>(functor)), _vector(std::forward<VectorType>(vector))
  {}

  decltype(auto) dimension() const noexcept
  {
    return _vector.dimension();
  }

  decltype(auto) upper_of_nonzeros() const noexcept
  {
    return _vector.upper_of_nonzeros();
  }

  decltype(auto) begin() const noexcept
  {
    return ACCBOOST2::make_map_iterator(_functor, _vector.begin());
  }

  decltype(auto) end() const noexcept
  {
    return ACCBOOST2::make_map_iterator_or_sentinel(_functor, _vector.end());
  }

};


}


template<class FunctorType, LATEL::sequential_access_vector_concept VectorType>
decltype(auto) map(FunctorType&& functor, VectorType&& vector)
{
  return _map_Vector::MappedVectorView<FunctorType, VectorType>(std::forward<FunctorType>(functor), std::forward<VectorType>(vector));
}


}


#endif
