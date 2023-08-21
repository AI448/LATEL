#ifndef LATEL_OPERATORS_PRODUCT_SCALAR_VECTOR_HPP_
#define LATEL_OPERATORS_PRODUCT_SCALAR_VECTOR_HPP_


#include "../common.hpp"
#include "map_Vector.hpp"


namespace LATEL
{


template<LATEL::scalar_concept ScalarType, LATEL::sequential_access_vector_concept VectorType>
decltype(auto) operator*(ScalarType&& scalar, VectorType&& vector)
{
  using R = std::ranges::range_reference_t<const VectorType&>;
  static_assert(ACCBOOST2::is_tuple<R>);

  struct F
  {
    std::common_type_t<ScalarType&&, std::remove_cv_t<std::remove_reference_t<typename std::remove_reference_t<VectorType>::value_type>>> scalar;
    decltype(auto) operator()(R&& pair) const noexcept
    {
      return ACCBOOST2::capture_as_tuple(
        ACCBOOST2::get<0>(std::forward<R>(pair)),
        scalar * ACCBOOST2::get<1>(std::forward<R>(pair))
      );
    }
  };

  return LATEL::map(F{std::forward<ScalarType>(scalar)}, std::forward<VectorType>(vector));
}


template<LATEL::sequential_access_vector_concept VectorType, LATEL::scalar_concept ScalarType>
decltype(auto) operator*(VectorType&& vector, ScalarType&& scalar)
{
  using R = std::ranges::range_reference_t<const VectorType&>;
  static_assert(ACCBOOST2::is_tuple<R>);

  struct F
  {
    std::common_type_t<ScalarType&&, std::remove_cv_t<std::remove_reference_t<typename std::remove_reference_t<VectorType>::value_type>>> scalar;
    decltype(auto) operator()(R&& pair) const noexcept
    {
      return ACCBOOST2::capture_as_tuple(
        ACCBOOST2::get<0>(std::forward<R>(pair)),
        ACCBOOST2::get<1>(std::forward<R>(pair)) * scalar
      );
    }
  };

  return LATEL::map(F{std::forward<ScalarType>(scalar)}, std::forward<VectorType>(vector));
}


template<LATEL::sequential_access_vector_concept VectorType, LATEL::scalar_concept ScalarType>
decltype(auto) operator/(VectorType&& vector, ScalarType&& scalar)
{
  using R = std::ranges::range_reference_t<const VectorType&>;
  static_assert(ACCBOOST2::is_tuple<R>);

  struct F
  {
    std::common_type_t<ScalarType&&, std::remove_cv_t<std::remove_reference_t<typename std::remove_reference_t<VectorType>::value_type>>> scalar;    
    decltype(auto) operator()(R&& pair) const noexcept
    {
      return ACCBOOST2::capture_as_tuple(
        ACCBOOST2::get<0>(std::forward<R>(pair)),
        ACCBOOST2::get<1>(std::forward<R>(pair)) / scalar
      );
    }
  };

  return LATEL::map(F{std::forward<ScalarType>(scalar)}, std::forward<VectorType>(vector));
}


template<LATEL::sequential_access_vector_concept VectorType>
VectorType operator+(VectorType&& vector)
{
  return std::forward<VectorType>(vector);
}


template<LATEL::sequential_access_vector_concept VectorType>
decltype(auto) operator-(VectorType&& vector)
{
  using R = std::ranges::range_reference_t<const VectorType&>;
  static_assert(ACCBOOST2::is_tuple<R>);

  struct F
  {
    decltype(auto) operator()(R&& pair) const noexcept
    {
      return ACCBOOST2::capture_as_tuple(
        ACCBOOST2::get<0>(std::forward<R>(pair)),
        -ACCBOOST2::get<1>(std::forward<R>(pair))
      );
    }
  };

  return LATEL::map(F{}, std::forward<VectorType>(vector));
}


}


#endif
