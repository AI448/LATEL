#ifndef LATEL_OPERATORS_DROP_HPP_
#define LATEL_OPERATORS_DROP_HPP_


#include "filter_Vector.hpp"


namespace LATEL
{


template<LATEL::sequential_access_vector_concept VectorType, class ValueType>
decltype(auto) drop(VectorType&& vector, ValueType&& tolerance)
{
  using std::abs;
  return LATEL::filter(
    [tolerance=std::forward<ValueType>(tolerance)](const auto& pair){
      return abs(std::get<1>(pair)) > tolerance;
    },
    std::forward<VectorType>(vector)
  );
}


}


#endif
