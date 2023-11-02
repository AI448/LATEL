#ifndef LATEL_OPERATORS_FILTER_MATRIX_HPP_
#define LATEL_OPERATORS_FILTER_MATRIX_HPP_


#include "../common.hpp"


namespace LATEL
{

namespace _filter_Matrix
{


template<class FunctorType, class MatrixType>
class FilteredMatrixView
{
  static_assert(std::invocable<const FunctorType&, std::ranges::range_reference_t<const MatrixType&>>);
  static_assert(std::convertible_to<std::invoke_result_t<const FunctorType&, std::ranges::range_reference_t<const MatrixType&>>, bool>);
  using result_type = std::ranges::range_reference_t<const MatrixType&>;
  static_assert(ACCBOOST2::is_tuple<std::remove_reference_t<result_type>>);

public:

  using matrix_category = LATEL::sequential_access_matrix_tag;
  using index_type = typename std::remove_reference_t<MatrixType>::index_type;
  using value_type = typename std::remove_reference_t<MatrixType>::value_type;

private:

  FunctorType _functor;
  MatrixType _matrix;

public:

  explicit FilteredMatrixView(FunctorType&& functor, MatrixType&& matrix):
    _functor(std::forward<FunctorType>(functor)), _matrix(std::forward<MatrixType>(matrix))
  {}

  decltype(auto) row_dimension() const noexcept
  {
    return _matrix.row_dimension();
  }

  decltype(auto) column_dimension() const noexcept
  {
    return _matrix.column_dimension();
  }

  decltype(auto) upper_of_nonzeros() const noexcept
  {
    return _matrix.upper_of_nonzeros();
  }

  decltype(auto) begin() const noexcept
  {
    return ACCBOOST2::make_filter_iterator(_functor, _matrix.begin(), _matrix.end());
  }

  decltype(auto) end() const noexcept
  {
    return ACCBOOST2::make_filter_sentinel(_matrix.end());
  }

};


}


template<class FunctorType, LATEL::sequential_access_matrix_concept MatrixType>
decltype(auto) filter(FunctorType&& functor, MatrixType&& matrix)
{
  return _filter_Matrix::FilteredMatrixView<FunctorType, MatrixType>(std::forward<FunctorType>(functor), std::forward<MatrixType>(matrix));
}


}


#endif
