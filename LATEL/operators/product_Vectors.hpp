#ifndef LATEL_OPERATORS_PRODUCT_VECTORS_HPP_
#define LATEL_OPERATORS_PRODUCT_VECTORS_HPP_


#include "../common.hpp"


namespace LATEL
{


template<LATEL::random_access_vector_concept LHSVectorType, LATEL::vector_concept RHSVectorType>
requires(
  !LATEL::random_access_vector_concept<RHSVectorType>
)
decltype(auto) operator*(const LHSVectorType& lhs_vector, const RHSVectorType& rhs_vector)
{
  using result_type = std::common_type_t<
    typename std::remove_reference_t<LHSVectorType>::value_type,
    typename std::remove_reference_t<RHSVectorType>::value_type
  >;
  result_type result = 0;
  for(auto&& [i, r]: rhs_vector){
//    result += lhs_vector[i] * r;    
    result = std::fma(lhs_vector[i], r, result);
  }
  return result;
}


template<LATEL::vector_concept LHSVectorType, LATEL::random_access_vector_concept RHSVectorType>
requires(
  !LATEL::random_access_vector_concept<LHSVectorType>
)
decltype(auto) operator*(const LHSVectorType& lhs_vector, const RHSVectorType& rhs_vector)
{
  using result_type = std::common_type_t<
    typename std::remove_reference_t<LHSVectorType>::value_type,
    typename std::remove_reference_t<RHSVectorType>::value_type
  >;
  result_type result = 0;
  for(auto&& [i, l]: lhs_vector){
    // result += l * rhs_vector[i];
    result = std::fma(l, rhs_vector[i], result);
  }
  return result;
}


template<LATEL::random_access_vector_concept LHSVectorType, LATEL::random_access_vector_concept RHSVectorType>
decltype(auto) operator*(const LHSVectorType& lhs_vector, const RHSVectorType& rhs_vector)
{
  using result_type = std::common_type_t<
    typename std::remove_reference_t<LHSVectorType>::value_type,
    typename std::remove_reference_t<RHSVectorType>::value_type
  >;
  result_type result = 0;
  if(lhs_vector.upper_of_nonzeros() <= rhs_vector.upper_of_nonzeros()){
    for(auto&& [i, l]: lhs_vector){
      // result += l * rhs_vector[i];
      result = std::fma(l, rhs_vector[i], result);
    }
  }else{
    for(auto&& [i, r]: rhs_vector){
      // result += lhs_vector[i] * r;
      result = std::fma(lhs_vector[i], r, result);      
    }
  }
  return result;
}


}


#endif
