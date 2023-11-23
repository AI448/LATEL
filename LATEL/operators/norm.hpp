#ifndef LATEL_OPERATORS_NORM_HPP_
#define LATEL_OPERATORS_NORM_HPP_


#include "../common.hpp"


namespace LATEL
{


decltype(auto) norm2(const LATEL::sequential_access_vector_concept auto& vector)
{
  typename std::remove_reference_t<decltype(vector)>::value_type sum_of_squares = 0;
  for(auto&& [j, v]: vector){
    // sum_of_squares += v * v;
    sum_of_squares = std::fma(v, v, sum_of_squares);
  }
  return sum_of_squares;
}


decltype(auto) norm(const LATEL::sequential_access_vector_concept auto& vector)
{
  using std::sqrt;
  return sqrt(norm2(vector));
}


}


#endif
