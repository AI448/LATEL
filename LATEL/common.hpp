#ifndef LATEL_COMMON_HPP_
#define LATEL_COMMON_HPP_


#include <limits>
#include "ACCBOOST2/utility.hpp"
#include "ACCBOOST2/IO.hpp"


namespace LATEL
{


enum Axis: std::size_t {ROW = 0, COLUMN = 1};


struct vector_tag {};

struct eager_evaluation_vector_tag: vector_tag {};

struct random_access_vector_tag: eager_evaluation_vector_tag {};

struct lazy_evaluation_vector_tag: vector_tag {};


template<class V>
concept vector_concept = 
  requires (const V& x)
  {
    typename std::remove_reference_t<V>::vector_category;
    typename std::remove_reference_t<V>::index_type;
    typename std::remove_reference_t<V>::value_type;
    {x.dimension()} -> std::convertible_to<typename std::remove_reference_t<V>::index_type>;
  } &&
  std::is_base_of_v<vector_tag, typename std::remove_reference_t<V>::vector_category>  
;


template<class V>
concept eager_evaluation_vector_concept = (
  vector_concept<V> &&
  std::is_base_of_v<eager_evaluation_vector_tag, typename std::remove_reference_t<V>::vector_category> &&
  std::ranges::range<const V&> &&
  std::convertible_to<
    std::ranges::range_reference_t<const V&>,
    std::tuple<typename std::remove_reference_t<V>::index_type, typename std::remove_reference_t<V>::value_type>
  >
);


template<class V>
concept random_access_vector_concept = (
  eager_evaluation_vector_concept<V> &&
  std::is_base_of_v<random_access_vector_tag, typename std::remove_reference_t<V>::vector_category> &&
  requires (const V& x)
  {
    {x[std::declval<typename std::remove_reference_t<V>::index_type>()]} -> std::convertible_to<typename std::remove_reference_t<V>::value_type>;
  }
);


struct matrix_tag {};

struct eager_evaluation_matrix_tag: matrix_tag {};

struct row_matrix_tag: eager_evaluation_matrix_tag {};

struct column_matrix_tag: eager_evaluation_matrix_tag {};

struct bidirectional_matrix_tag: row_matrix_tag, column_matrix_tag {};


template<class M>
concept matrix_concept = (
  requires (const M& x)
  {
    typename std::remove_reference_t<M>::matrix_category;
    typename std::remove_reference_t<M>::index_type;
    typename std::remove_reference_t<M>::value_type;
    {x.row_dimension()} -> std::convertible_to<typename std::remove_reference_t<M>::index_type>;
    {x.column_dimension()} -> std::convertible_to<typename std::remove_reference_t<M>::index_type>;
  } &&
  std::is_base_of_v<LATEL::matrix_tag, typename std::remove_reference_t<M>::matrix_category>
);


template<class M>
concept eager_evaluation_matrix_concept = (
  matrix_concept<M> &&
  std::is_base_of_v<LATEL::eager_evaluation_matrix_tag, typename std::remove_reference_t<M>::matrix_category> &&
  std::ranges::range<M> &&
  std::convertible_to<
    std::ranges::range_reference_t<const M&>,
    std::tuple<typename std::remove_reference_t<M>::index_type, typename std::remove_reference_t<M>::index_type, typename std::remove_reference_t<M>::value_type>
  >
);


template<class M>
concept row_matrix_concept = (
  eager_evaluation_matrix_concept<M> &&
  std::is_base_of_v<LATEL::row_matrix_tag, typename std::remove_reference_t<M>::matrix_category> &&
  requires (M& m)
  {
    {m.row(std::declval<typename std::remove_reference_t<M>::index_type>())} -> eager_evaluation_vector_concept;
  }
);


template<class M>
concept column_matrix_concept = (
  eager_evaluation_matrix_concept<M> &&
  std::is_base_of_v<LATEL::column_matrix_tag, typename std::remove_reference_t<M>::matrix_category> &&
  requires (M& m)
  {
    {m.column(std::declval<typename std::remove_reference_t<M>::index_type>())} -> eager_evaluation_vector_concept;
  }
);

template<class M>
concept bidirectional_matrix_concept = (
  row_matrix_concept<M> &&
  column_matrix_concept<M>
);


// template<class M>
// concept row_major_matrix_concept = 
//   matrix_concept<M> &&
//   requires (const M& x)
//   {
//     {x.row(std::declval<typename std::remove_reference_t<M>::index_type>())} -> vector_concept;
//   };


// template<class P>
// concept permutator_concept = requires (const P& x)
// {
//   typename std::remove_reference_t<P>::index_type;
//   {x.row_dimension()} -> std::convertible_to<typename std::remove_reference_t<P>::index_type>;
//   {x.column_dimension()} -> std::convertible_to<typename std::remove_reference_t<P>::index_type>;
//   {x.permutate(std::declval<const typename std::remove_reference_t<P>::index_type&>())} -> std::convertible_to<typename std::remove_reference_t<P>::index_type>;
//   {x.unpermutate(std::declval<const typename std::remove_reference_t<P>::index_type&>())} -> std::convertible_to<typename std::remove_reference_t<P>::index_type>;
// };

template<class CharT>
void serialize(ACCBOOST2::IO::OutputStream<CharT>& ostream, const eager_evaluation_vector_concept auto& vector)
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

template<class CharT>
void serialize(ACCBOOST2::IO::OutputStream<CharT>& ostream, const eager_evaluation_matrix_concept auto& matrix)
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
