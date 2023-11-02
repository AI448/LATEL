#ifndef LATEL_COMPRESSEDMATRIX_HPP_
#define LATEL_COMPRESSEDMATRIX_HPP_


#include "ACCBOOST2/container.hpp"
#include "common.hpp"
#include "MatixRangePolicy.hpp"
#include "VectorView.hpp"
#include "operators/transpose_Matrix.hpp"


namespace LATEL
{


template<class IndexType, class ValueType>
class CompressedRowMatrix: public RowMatrixRangePolicy<CompressedRowMatrix<IndexType, ValueType>>
{
public:

  using matrix_category = LATEL::row_matrix_tag;
  using index_type = IndexType;
  using value_type = ValueType;

private:

  index_type _column_dimension;
  ACCBOOST2::Array<std::size_t> _positions;
  ACCBOOST2::ZippedArray<index_type, value_type> _data;

  void assign(const auto& matrix)
  {
    assert(matrix.row_dimension() < std::numeric_limits<index_type>::max());
    _column_dimension = matrix.column_dimension();
    _positions.clear();
    _data.clear();
    _positions.resize(matrix.row_dimension() + 1, 0);
    // 非ゼロ要素数
    std::size_t nonzeros = 0;
    // _positions[i + 1] が i 行目のデータの個数となるように
    for(auto&& [i, j, a]: matrix){
      assert(i >= 0); assert(i < matrix.row_dimension()); assert(j >= 0); assert(j < matrix.column_dimension());
      assert(i + 1 < _positions.size());
      if(a == 0) continue;
      if(i + 2 < _positions.size()){
        _positions[i + 2] += 1;
      }
      nonzeros += 1;
    }
    // _positions[i + 1] が i 行目のデータの開始位置となるように
    if(_positions.size() >= 2){
      for(auto&& i: ACCBOOST2::range(2, _positions.size())){
        _positions[i] += _positions[i - 1];
      }
    }
    // データを配置
    _data.resize(nonzeros);
    for(auto&& [i, j, a]: matrix){
      if(a == 0) continue;
      assert(nonzeros != 0);
      nonzeros -= 1;
      // データを配置
      _data[_positions[i + 1]] = std::forward_as_tuple(std::forward<decltype(j)>(j), std::forward<decltype(a)>(a));
      // i 行目のデータの開始位置をインクリメント
      _positions[i + 1] += 1;
    }
    assert(nonzeros == 0);
    assert(_positions[0] == 0);
    assert(_positions[_positions.size() - 1] == _data.size());
    // [_positions[i] , _positions[i + 1]) が i 行目のデータの範囲となる
  }

public:

  CompressedRowMatrix() = default;
  CompressedRowMatrix(CompressedRowMatrix&&) = default;
  CompressedRowMatrix(const CompressedRowMatrix&) = default;

  template<LATEL::sequential_access_matrix_concept M>
  CompressedRowMatrix(const M& other):
    CompressedRowMatrix()
  {
    assign(other);
  }

  CompressedRowMatrix& operator=(CompressedRowMatrix&&) = default;
  CompressedRowMatrix& operator=(const CompressedRowMatrix&) = default;

  template<LATEL::sequential_access_matrix_concept M>
  CompressedRowMatrix& operator=(const M& other)
  {
    assign(other);
    return *this;
  }

  decltype(auto) row_dimension() const noexcept
  {
    return _positions.size() != 0 ? _positions.size() - 1 : 0;
  }

  decltype(auto) column_dimension() const noexcept
  {
    return (_column_dimension);
  }

  decltype(auto) row(const std::integral auto& row_index) const noexcept
  {
    assert(row_index + 1 < _positions.size());
    return LATEL::make_VectorView(_column_dimension, ACCBOOST2::slice(_positions[row_index], _positions[row_index + 1], _data));
  }

};


template<class IndexType, class ValueType>
class CompressedColumnMatrix: public ColumnMatrixRangePolicy<CompressedColumnMatrix<IndexType, ValueType>>
{
public:

  using matrix_category = LATEL::column_matrix_tag;
  using index_type = IndexType;
  using value_type = ValueType;

private:

  CompressedRowMatrix<index_type, value_type> _row_matrix;

public:

  CompressedColumnMatrix() = default;
  CompressedColumnMatrix(CompressedColumnMatrix&&) = default;
  CompressedColumnMatrix(const CompressedColumnMatrix&) = default;

  template<LATEL::sequential_access_matrix_concept M>
  CompressedColumnMatrix(const M& other):
    _row_matrix(LATEL::transpose(other))
  {}

  CompressedColumnMatrix& operator=(CompressedColumnMatrix&&) = default;
  CompressedColumnMatrix& operator=(const CompressedColumnMatrix&) = default;

  template<LATEL::sequential_access_matrix_concept M>
  CompressedColumnMatrix& operator=(const M& other)
  {
    _row_matrix = LATEL::transpose(other);
    return *this;
  }

  decltype(auto) row_dimension() const noexcept
  {
    return _row_matrix.column_dimension();
  }

  decltype(auto) column_dimension() const noexcept
  {
    return _row_matrix.row_dimension();
  }

  decltype(auto) column(const std::integral auto& column_index) const noexcept
  {
    return _row_matrix.row(column_index);
  }

};


template<class IndexType, class ValueType>
class CompressedMatrix: public RowMatrixRangePolicy<CompressedMatrix<IndexType, ValueType>>
{
public:

  using matrix_category = LATEL::bidirectional_matrix_tag;
  using index_type = IndexType;
  using value_type = ValueType;

private:

  CompressedRowMatrix<index_type, value_type> _row_matrix;
  CompressedColumnMatrix<index_type, value_type> _column_matrix;

public:

  CompressedMatrix() = default;
  CompressedMatrix(CompressedMatrix&&) = default;
  CompressedMatrix(const CompressedMatrix&) = default;

  template<LATEL::sequential_access_matrix_concept M>
  CompressedMatrix(const M& other):
    _row_matrix(other), _column_matrix(other)
  {}

  CompressedMatrix& operator=(CompressedMatrix&&) = default;
  CompressedMatrix& operator=(const CompressedMatrix&) = default;

  template<LATEL::sequential_access_matrix_concept M>
  CompressedMatrix& operator=(const M& other)
  {
    _row_matrix = other;
    _column_matrix = other;
    return *this;
  }

  decltype(auto) row_dimension() const noexcept
  {
    return _column_matrix.row_dimension();
  }

  decltype(auto) column_dimension() const noexcept
  {
    return _row_matrix.column_dimension();
  }

  decltype(auto) row(const std::integral auto& row_index) const noexcept
  {
    return _row_matrix.row(row_index);
  }

  decltype(auto) column(const std::integral auto& column_index) const noexcept
  {
    return _column_matrix.column(column_index);
  }

};


}


#endif