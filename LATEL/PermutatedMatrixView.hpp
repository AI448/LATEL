#ifndef LATEL_PERMUTATORMATRIXVIEW_HPP_
#define LATEL_PERMUTATORMATRIXVIEW_HPP_


#include "LATEL_common.hpp"


namespace LATEL
{


template<class PermutatorT, class MatrixType>
class RowPermutatedMatrix
{
public:

  using index_type = std::common_type_t<typename std::remove_reference_t<PermutatorT>::index_type, typename std::remove_reference_t<MatrixT>::index_type>;

  using value_type = std::remove_reference_t<MatrixT>::value_type;

private:

  PermutatorT _permutator;
  MatrixType _matrix;

public:

  RowPermutatedMatrix(PermutatorT&& permutator, MatrixType&& matrix) noexcept:
    _permutator(std::forward<PermutatorT>(permutator)), _matrix(std::forward<MatrixType>(matrix))
  {
    assert(_permutator.column_dimension() == _matrix.row_dimension());
  }

  decltype(auto) row_dimension() const noexcept
  {
    return _permutator.row_dimension();
  }

  decltype(auto) column_dimension() const noexcept
  {
    return _matrix.column_dimension();
  }

private:

  template<class IteratorT>
  decltype(auto) make_iterator(IteratorT&& iterator) const noexcept
  {
    return ACCBOOST2::make_map_iterator(
      [&](auto&& i_j_a) -> decltype(auto)
      {
        return std::forward_as_tuple(_permutator.permutate(ACCBOOST2::get<0>(i_j_a)), ACCBOOST2::get<1>(i_j_a), ACCBOOST2::get<2>(i_j_a));
      },
      iterator
    );
  }

public:

  decltype(auto) begin() const noexcept
  {
    return make_iterator(_matrix.begin());
  }

  decltype(auto) end() const noexcept
  {
    return make_iterator(_matrix.end());
  }

  template<class M = MatrixType>
  auto size() const noexcept -> decltype(std::declval<M>().size())
  {
    return _matrix.size();
  }

};


template<class MatrixType, class PermutatorT>
class ColumnPermutatedMatrix
{
public:

  using index_type = std::common_type_t<typename std::remove_reference_t<MatrixT>::index_type, typename std::remove_reference_t<PermutatorT>::index_type>;

  using value_type = MatrixType::value_type;

private:

  MatrixType _matrix;
  PermutatorT _permutator;

public:

  ColumnPermutatedMatrix(MatrixType&& matrix, PermutatorT&& permutator) noexcept:
    _matrix(std::forward<MatrixType>(matrix)), _permutator(std::forward<PermutatorT>(permutator))
  {
    assert(matrix.column_dimension() == permutator.row_dimension());
  }

  decltype(auto) row_dimension() const noexcept
  {
    return _matrix.row_dimension();
  }

  decltype(auto) column_dimension() const noexcept
  {
    return _permutator.column_dimension();
  }

private:

  template<class IteratorT>
  decltype(auto) make_iterator(IteratorT&& iterator) const noexcept
  {
    return ACCBOOST2::make_map_iterator(
      [&](auto&& i_j_a) -> decltype(auto)
      {
        return std::forward_as_tuple(ACCBOOST2::get<0>(i_j_a), _permutator.unpermutate(ACCBOOST2::get<1>(i_j_a)), ACCBOOST2::get<2>(i_j_a));
      },
      iterator
    );
  }

public:

  decltype(auto) begin() const noexcept
  {
    return make_iterator(_matrix.begin());
  }

  decltype(auto) end() const noexcept
  {
    return make_iterator(_matrix.end());
  }

  template<class M = MatrixType>
  auto size() const noexcept ->decltype(std::declval<M>().size())
  {
    return _matrix.size();
  }

};


template<permutator_concept PermutatorT, matrix_concept MatrixT>
decltype(auto) operator*(PermutatorT&& permutator, MatrixT&& matrix)
{
  return RowPermutatedMatrix<PermutatorT, MatrixT>(std::forward<PermutatorT>(permutator), std::forward<MatrixT>(matrix));
}


template<matrix_concept MatrixT, permutator_concept PermutatorT>
decltype(auto) operator*(MatrixT&& matrix, PermutatorT&& permutator)
{
  return ColumnPermutatedMatrix<MatrixT, PermutatorT>(std::forward<MatrixT>(matrix), std::forward<PermutatorT>(permutator));
}


}


#endif
