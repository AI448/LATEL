#ifndef LATEL_VECTORPRODUCTION_HPP_
#define LATEL_VECTORPRODUCTION_HPP_


#include "LATEL_common.hpp"


namespace LATEL
{


template<random_accessible_vector_concept LHSVectorT, vector_concept RHSVectorT>
requires(!random_accessible_vector_concept<RHSVectorT>)
decltype(auto) operator*(LHSVectorT&& lhs_vector, RHSVectorT&& rhs_vector)
{
  using result_type = std::common_type_t<
    typename std::remove_reference_t<LHSVectorT>::value_type,
    typename std::remove_reference_t<RHSVectorT>::value_type
  >;
  result_type result = 0;
  for(auto&& [i, r]: rhs_vector){
    result += lhs_vector[i] * r;
  }
  return result;
}


template<vector_concept LHSVectorT, random_accessible_vector_concept RHSVectorT>
requires(!random_accessible_vector_concept<LHSVectorT>)
decltype(auto) operator*(LHSVectorT&& lhs_vector, RHSVectorT&& rhs_vector)
{
  using result_type = std::common_type_t<
    typename std::remove_reference_t<LHSVectorT>::value_type,
    typename std::remove_reference_t<RHSVectorT>::value_type
  >;
  result_type result = 0;
  for(auto&& [i, l]: lhs_vector){
    result += l * rhs_vector[i];
  }
  return result;
}


template<random_accessible_vector_concept LHSVectorT, random_accessible_vector_concept RHSVectorT>
decltype(auto) operator*(LHSVectorT&& lhs_vector, RHSVectorT&& rhs_vector)
{
  using result_type = std::common_type_t<
    typename std::remove_reference_t<LHSVectorT>::value_type,
    typename std::remove_reference_t<RHSVectorT>::value_type
  >;
  result_type result = 0;
  if(lhs_vector.size() < rhs_vector.size()){
    for(auto&& [i, l]: lhs_vector){
      result += l * rhs_vector[i];
    }
  }else{
    for(auto&& [i, r]: rhs_vector){
      result += lhs_vector[i] * r;
    }
  }
  return result;
}


}


#endif
