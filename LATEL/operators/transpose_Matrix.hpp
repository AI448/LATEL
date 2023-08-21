#ifndef LATEL_OPERATORS_TRANSPOSE_MATRIX_HPP_
#define LATEL_OPERATORS_TRANSPOSE_MATRIX_HPP_


#include "../common.hpp"


namespace LATEL
{


namespace _transpose_Matrix
{


template<class MatrixType>
class TransposedMatrixView
{
public:

  using matrix_category = std::conditional_t<
    LATEL::bidirectional_matrix_concept<MatrixType>,
    LATEL::bidirectional_matrix_tag,
    std::conditional_t<
      LATEL::row_matrix_concept<MatrixType>,
      LATEL::column_matrix_tag,
      std::conditional_t<
        LATEL::column_matrix_concept<MatrixType>,
        LATEL::row_matrix_tag,
        LATEL::sequential_access_matrix_tag
      >
    >
  >;

  using index_type = std::remove_reference_t<MatrixType>::index_type;

  using value_type = std::remove_reference_t<MatrixType>::value_type;

private:

  MatrixType _matrix;

public:

  explicit TransposedMatrixView(MatrixType&& matrix):
    _matrix(std::forward<MatrixType>(matrix))
  {}

  decltype(auto) row_dimension() const noexcept
  {
    return _matrix.column_dimension();
  }

  decltype(auto) column_dimension() const noexcept
  {
    return _matrix.row_dimension();
  }

  auto row(const std::integral auto& row_index) const noexcept requires(LATEL::column_matrix_concept<MatrixType>)
  {
    return _matrix.column(row_index);
  }

  auto column(const std::integral auto& column_index) const noexcept requires(LATEL::row_matrix_concept<MatrixType>)
  {
    return _matrix.row(column_index);
  }

private:

  struct Transpose
  {
    decltype(auto) operator()(auto&& trio) const noexcept
    {
      return ACCBOOST2::capture_as_tuple(
        ACCBOOST2::get<1>(std::forward<decltype(trio)>(trio)),
        ACCBOOST2::get<0>(std::forward<decltype(trio)>(trio)),
        ACCBOOST2::get<2>(std::forward<decltype(trio)>(trio))
      );
    }
  };

public:

  decltype(auto) begin() const noexcept
  {
    return ACCBOOST2::make_map_iterator(Transpose{}, _matrix.begin());
  }

  decltype(auto) end() const noexcept
  {
    return ACCBOOST2::make_map_iterator_or_sentinel(Transpose{}, _matrix.end());
  }

};


}


template<LATEL::sequential_access_matrix_concept MatrixType>
decltype(auto) transpose(MatrixType&& matrix) noexcept
{
  return _transpose_Matrix::TransposedMatrixView<MatrixType>(std::forward<MatrixType>(matrix));
}


}


#endif
