#ifndef LATEL_OPERATORS_PRODUCT_PERMUTATOR_MATRIX_HPP_
#define LATEL_OPERATORS_PRODUCT_PERMUTATOR_MATRIX_HPP_


#include "../common.hpp"
#include "product_Permutator_Vector.hpp"
#include "transpose_Permutator.hpp"
#include "transpose_Matrix.hpp"


namespace LATEL
{


namespace _product_Permutator_Matrix
{


template<class PermutatorType, class MatrixType>
class RowPermutatedMatrixView
{
public:

  using matrix_category = typename std::remove_reference_t<MatrixType>::matrix_category;
  using index_type = std::common_type_t<typename std::remove_reference_t<PermutatorType>::index_type, typename std::remove_reference_t<MatrixType>::index_type>;
  using value_type = typename std::remove_reference_t<MatrixType>::value_type;

private:

  PermutatorType _permutator;
  MatrixType _matrix;

public:

  explicit RowPermutatedMatrixView(PermutatorType&& permutator, MatrixType&& matrix):
    _permutator(std::forward<PermutatorType>(permutator)), _matrix(std::forward<MatrixType>(matrix))
  {}

  decltype(auto) row_dimension() const noexcept
  {
    return _permutator.row_dimension();
  }

  decltype(auto) column_dimension() const noexcept
  {
    return _matrix.column_dimension();
  }

  decltype(auto) row(const std::unsigned_integral auto& row_index) const noexcept requires(LATEL::row_matrix_concept<const MatrixType&>)
  {
    auto&& i = _permutator.unpermutate(row_index);
    assert(i < _matrix.row_dimension());
    return _matrix.row(i);
  }

  decltype(auto) column(const std::unsigned_integral auto& column_index) const noexcept requires(LATEL::column_matrix_concept<const MatrixType&>)
  {
    return _permutator * _matrix.column(column_index);
  }

private:

  struct Permutator
  {
    const RowPermutatedMatrixView* p = nullptr;
    decltype(auto) operator()(auto&& trio) const noexcept
    {
      assert(p != nullptr);
      assert(ACCBOOST2::get<0>(std::forward<decltype(trio)>(trio)) < p->_matrix.row_dimension());
      return ACCBOOST2::capture_as_tuple(
        p->_permutator.permutate(ACCBOOST2::get<0>(std::forward<decltype(trio)>(trio))),
        ACCBOOST2::get<1>(std::forward<decltype(trio)>(trio)),
        ACCBOOST2::get<2>(std::forward<decltype(trio)>(trio))
      );
    }
  };

  struct Filter
  {
    decltype(auto) operator()(auto&& trio) const noexcept
    {
      return ACCBOOST2::get<0>(trio) != std::remove_reference_t<PermutatorType>::null_index;
    }
  };

public:

  decltype(auto) begin() const noexcept
  {
    return ACCBOOST2::make_filter_iterator(Filter{}, ACCBOOST2::make_map_iterator(Permutator{this}, _matrix.begin()), ACCBOOST2::make_map_iterator_or_sentinel(Permutator{nullptr}, _matrix.end()));
    // return ACCBOOST2::make_map_iterator(Permutator{this}, _matrix.begin());
  }

  decltype(auto) end() const noexcept
  {
    return ACCBOOST2::make_filter_sentinel(ACCBOOST2::make_map_iterator_or_sentinel(Permutator{nullptr}, _matrix.end()));
//    return ACCBOOST2::make_map_iterator_or_sentinel(Permutator{nullptr}, _matrix.end());
  }

};


}


template<LATEL::permutator_concept PermutatorType, LATEL::sequential_access_matrix_concept MatrixType>
decltype(auto) operator*(PermutatorType&& permutator, MatrixType&& matrix)
{
  assert(permutator.column_dimension() == matrix.row_dimension());
  return _product_Permutator_Matrix::RowPermutatedMatrixView<PermutatorType, MatrixType>(std::forward<PermutatorType>(permutator), std::forward<MatrixType>(matrix));
}


template<LATEL::sequential_access_matrix_concept MatrixType, LATEL::permutator_concept PermutatorType>
decltype(auto) operator*(MatrixType&& matrix, PermutatorType&& permutator)
{
  assert(matrix.column_dimension() == permutator.row_dimension());
  return transpose(transpose(permutator) * transpose(matrix));
}


}


#endif
