#ifndef LATEL_FOUNDATIONS_HPP_
#define LATEL_FOUNDATIONS_HPP_


#include <limits>
#include "ACCBOOST2/utility.hpp"
#include "ACCBOOST2/IO.hpp"


namespace LATEL
{


using axis_type = enum {ROW = 0, COLUMN = 1};


template<class V>
concept vector_concept = requires (const V& x)
{
  typename std::remove_reference_t<V>::index_type;
  typename std::remove_reference_t<V>::value_type;
  {x.dimension()} -> std::convertible_to<typename std::remove_reference_t<V>::index_type>;
  requires ACCBOOST2::is_range<const V&>;
};


template<class M>
concept matrix_concept = requires (const M& x)
{
  typename std::remove_reference_t<M>::index_type;
  typename std::remove_reference_t<M>::value_type;
  {x.row_dimension()} -> std::convertible_to<typename std::remove_reference_t<M>::index_type>;
  {x.column_dimension()} -> std::convertible_to<typename std::remove_reference_t<M>::index_type>;
  requires ACCBOOST2::is_range<M>;
};


template<class P>
concept permutator_concept = requires (const P& x)
{
  typename std::remove_reference_t<P>::index_type;
  {x.row_dimension()} -> std::convertible_to<typename std::remove_reference_t<P>::index_type>;
  {x.column_dimension()} -> std::convertible_to<typename std::remove_reference_t<P>::index_type>;
  {x.permutate(std::declval<const typename std::remove_reference_t<P>::index_type&>())} -> std::convertible_to<typename std::remove_reference_t<P>::index_type>;
  {x.unpermutate(std::declval<const typename std::remove_reference_t<P>::index_type&>())} -> std::convertible_to<typename std::remove_reference_t<P>::index_type>;
};

template<class CharT, vector_concept VectorT>
void serialize(ACCBOOST2::IO::OutputStream<CharT>& ostream, const VectorT& vector)
{
  ostream("[");
  bool first = true;
  for(auto&& [i, x]: vector){
    if(first){
      first = false;
    }else{
      ostream(", ");
    }
    ostream("[", i, ", ", x, "]");
  }
  ostream("]");
}

template<class CharT, matrix_concept MatrixT>
void serialize(ACCBOOST2::IO::OutputStream<CharT>& ostream, const MatrixT& matrix)
{
  ostream("[");
  bool first = true;
  for(auto&& [i, j, a]: matrix){
    if(first){
      first = false;
    }else{
      ostream(", ");
    }
    ostream("[", i, ", ", j, ", ", a, "]");
  }
  ostream("]");
}


}


#endif
