#ifndef LATEL_COMMON_HPP_
#define LATEL_COMMON_HPP_


#include <limits>
#include "ACCBOOST2/utility.hpp"
#include "ACCBOOST2/IO.hpp"


namespace LATEL
{


enum Axis: std::size_t {ROW = 0, COLUMN = 1};


struct vector_tag {};
struct sequential_access_vector_tag: vector_tag {};
struct random_access_vector_tag: sequential_access_vector_tag {};
struct lazy_evaluation_vector_tag: vector_tag {};

/// ベクターであることを表すコンセプト
template<class V>
concept vector_concept = (
  requires (const V& x)
  {
    typename std::remove_reference_t<V>::vector_category;
    typename std::remove_reference_t<V>::index_type;
    typename std::remove_reference_t<V>::value_type;
    {x.dimension()} -> std::convertible_to<typename std::remove_reference_t<V>::index_type>;
  } &&
  std::is_base_of_v<vector_tag, typename std::remove_reference_t<V>::vector_category>
);

/// シーケンシャルアクセス可能なベクターであることを表すコンセプト
template<class V>
concept sequential_access_vector_concept = (
  vector_concept<V> &&
  std::is_base_of_v<sequential_access_vector_tag, typename std::remove_reference_t<V>::vector_category> &&
  std::ranges::range<const V&> &&
  std::convertible_to<std::ranges::range_reference_t<const V&>, std::tuple<typename std::remove_reference_t<V>::index_type, typename std::remove_reference_t<V>::value_type>> &&
  requires (const V& x)
  {
    {x.upper_of_nonzeros()} -> std::convertible_to<typename std::remove_reference_t<V>::index_type>;
  }
);

/// ランダムアクセス可能なベクターであることを表すコンセプト
template<class V>
concept random_access_vector_concept = (
  sequential_access_vector_concept<V> &&
  std::is_base_of_v<random_access_vector_tag, typename std::remove_reference_t<V>::vector_category> &&
  requires (const V& x)
  {
    {x[std::declval<typename std::remove_reference_t<V>::index_type>()]} -> std::convertible_to<typename std::remove_reference_t<V>::value_type>;
  }
);

/// イテレータによる即時評価が不可能なベクターであることを表すコンセプト
template<class X>
concept lazy_evaluation_vector_concept = (
  vector_concept<X> &&
  std::is_base_of_v<lazy_evaluation_vector_tag, typename std::remove_reference_t<X>::vector_category>
);


struct matrix_tag {};
struct sequential_access_matrix_tag: matrix_tag {};
struct row_matrix_tag: sequential_access_matrix_tag {};
struct column_matrix_tag: sequential_access_matrix_tag {};
struct bidirectional_matrix_tag: row_matrix_tag, column_matrix_tag {};


/// 行列であることを表すコンセプト
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

/// シーケンシャルアクセス可能な行列であることを表すコンセプト
template<class M>
concept sequential_access_matrix_concept = (
  matrix_concept<M> &&
  std::is_base_of_v<LATEL::sequential_access_matrix_tag, typename std::remove_reference_t<M>::matrix_category> &&
  std::ranges::range<M> &&
  std::convertible_to<
    std::ranges::range_reference_t<const M&>,
    std::tuple<typename std::remove_reference_t<M>::index_type, typename std::remove_reference_t<M>::index_type, typename std::remove_reference_t<M>::value_type>
  >
);

/// 行に対してランダムアクセス可能な行列であることを表すコンセプト
template<class M>
concept row_matrix_concept = (
  sequential_access_matrix_concept<M> &&
  std::is_base_of_v<LATEL::row_matrix_tag, typename std::remove_reference_t<M>::matrix_category> &&
  requires (M& m)
  {
    {m.row(std::declval<typename std::remove_reference_t<M>::index_type>())} -> sequential_access_vector_concept;
  }
);

/// 列に対してランダムアクセス可能な行列であることを表すコンセプト
template<class M>
concept column_matrix_concept = (
  sequential_access_matrix_concept<M> &&
  std::is_base_of_v<LATEL::column_matrix_tag, typename std::remove_reference_t<M>::matrix_category> &&
  requires (M& m)
  {
    {m.column(std::declval<typename std::remove_reference_t<M>::index_type>())} -> sequential_access_vector_concept;
  }
);

/// 行と列に対してランダムアクセス可能な matrix であることを表すコンセプト
template<class M>
concept bidirectional_matrix_concept = (
  row_matrix_concept<M> &&
  column_matrix_concept<M>
);


struct permutator_tag {};

/// 置換であることを表すコンセプト
template<class X>
concept permutator_concept = (
  requires (const X& x)
  {
    typename std::remove_reference_t<X>::permutator_category;
    typename std::remove_reference_t<X>::index_type;
    {std::remove_reference_t<X>::null_index} -> std::convertible_to<typename std::remove_reference_t<X>::index_type>;
    {x.row_dimension()} -> std::convertible_to<typename std::remove_reference_t<X>::index_type>;
    {x.column_dimension()} -> std::convertible_to<typename std::remove_reference_t<X>::index_type>;
    {x.permutate(std::declval<typename std::remove_reference_t<X>::index_type>())} -> std::convertible_to<typename std::remove_reference_t<X>::index_type>;
    {x.unpermutate(std::declval<typename std::remove_reference_t<X>::index_type>())} -> std::convertible_to<typename std::remove_reference_t<X>::index_type>;
  } &&
  std::is_base_of_v<LATEL::permutator_tag, typename std::remove_reference_t<X>::permutator_category>
);


/// ベクトル・行列・置換ではないことを表すコンセプト
template<class X>
concept scalar_concept = (
  !LATEL::vector_concept<X> &&
  !LATEL::matrix_concept<X> &&
  !LATEL::permutator_concept<X>
);


template<class CharT>
void serialize(ACCBOOST2::IO::OutputStream<CharT>& ostream, const sequential_access_vector_concept auto& vector)
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
void serialize(ACCBOOST2::IO::OutputStream<CharT>& ostream, const sequential_access_matrix_concept auto& matrix)
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
