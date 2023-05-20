#ifndef LATEL_TRANSPOSEDMATRIXVIEW_HPP_
#define LATEL_TRANSPOSEDMATRIXVIEW_HPP_


#include "foundations.hpp"


namespace LATEL
{


template<class MatrixT>
class TransposedMatrixView
{
public:

  using index_type = std::remove_reference_t<MatrixT>::index_type;

  using value_type = std::remove_reference_t<MatrixT>::value_type;

private:

  MatrixT _matrix;

public:

  explicit TransposedMatrixView(MatrixT&& matrix):
    _matrix(std::forward<MatrixT>(matrix))
  {}

  decltype(auto) row_dimension() const noexcept
  {
    return _matrix.column_dimension();
  }

  decltype(auto) column_dimension() const noexcept
  {
    return _matrix.row_dimension();
  }

  template<std::integral I, class X = MatrixT>
  auto row(const I& row_index) const noexcept -> decltype(std::declval<const X&>().column(row_index))
//  decltype(auto) row(const I& row_index) const noexcept
  {
    return _matrix.column(row_index);
  }

  template<std::integral I, class X = MatrixT>
  auto column(const I& column_index) const noexcept -> decltype(std::declval<const X&>().row(column_index))
//  auto column(const I& column_index) const noexcept -> decltype(std::declval<const MatrixT&>().row(column_index))
  {
    return _matrix.row(column_index);
  }

private:

  template<class IteratorT>
  decltype(auto) make_transposed_iterator(IteratorT&& iterator) const noexcept
  {
    return ACCBOOST2::make_map_iterator(
      [](auto&& trio)
      {
        return ACCBOOST2::capture_as_tuple(
          ACCBOOST2::get<1>(trio),
          ACCBOOST2::get<0>(trio),
          ACCBOOST2::get<2>(trio)
        );
      },
      std::forward<IteratorT>(iterator)
    );
  }

public:

  decltype(auto) begin() const noexcept
  {
    return make_transposed_iterator(_matrix.begin());
  }

  decltype(auto) end() const noexcept
  {
    return make_transposed_iterator(_matrix.end());
  }

};


template<matrix_concept MatrixT>
decltype(auto) transpose(MatrixT&& matrix) noexcept
{
  return TransposedMatrixView<MatrixT>(std::forward<MatrixT>(matrix));
}


}


#endif
