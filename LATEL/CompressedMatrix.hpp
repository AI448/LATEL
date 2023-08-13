#ifndef LATEL_COMPRESSEDMATRIX_HPP_
#define LATEL_COMPRESSEDMATRIX_HPP_



#include "ACCBOOST2/container.hpp"
#include "common.hpp"
#include "MatixRangePolicy.hpp"
#include "VectorView.hpp"
#include "TransposedMatrixView.hpp"


namespace LATEL
{


template<class IndexType, class ValueType>
class CompressedMatrix: public RowMatrixRangePolicy<CompressedMatrix<IndexType, ValueType>>
{
public:

  using matrix_category = LATEL::bidirectional_matrix_tag;

  using index_type = IndexType;

  using value_type = ValueType;

private:

  struct CompressedStorage
  {
    ACCBOOST2::Array<std::size_t> _positions;
    ACCBOOST2::ZippedArray<index_type, value_type> _data;

    void assign(const auto& dimension, const auto& range)
    {
      assert(dimension < std::numeric_limits<index_type>::max());

      _positions.clear();
      _data.clear();
      _positions.resize(dimension + 1, 0);
      std::size_t nonzeros = 0;
      for(auto&& [i, j, a]: range){
        assert(i + 1 < _positions.size());
        if(i + 2 < _positions.size()){
          _positions[i + 2] += 1;
        }
        nonzeros += 1;
      }
      if(_positions.size() >= 2){
        for(auto&& i: ACCBOOST2::range(2, _positions.size())){
          _positions[i] += _positions[i - 1];
        }
      }
      _data.resize(nonzeros);
      for(auto&& [i, j, a]: range){
        _data[_positions[i + 1]] = std::forward_as_tuple(std::forward<decltype(j)>(j), std::forward<decltype(a)>(a));
        _positions[i + 1] += 1;
      }
      assert(_positions[0] == 0);
      assert(_positions[_positions.size() - 1] == nonzeros);
    }

    decltype(auto) dimension() const noexcept
    {
      assert(_positions.size() - 1 <= std::numeric_limits<index_type>::max());
      return _positions.size() != 0 ? _positions.size() - 1 : 0;      
    }

    decltype(auto) data(const auto& index) const noexcept
    {
      assert(index + 1 < _positions.size());
      return ACCBOOST2::slice(
        _positions[index],
        _positions[index + 1],
        _data
      );
    }

  };

  CompressedStorage _row_storage;
  CompressedStorage _column_storage;

public:

  CompressedMatrix() = default;
  CompressedMatrix(CompressedMatrix&&) = default;
  CompressedMatrix(const CompressedMatrix&) = default;
  CompressedMatrix& operator=(CompressedMatrix&&) = default;
  CompressedMatrix& operator=(const CompressedMatrix&) = default;

  decltype(auto) row_dimension() const noexcept
  {
    return _row_storage.dimension();
  }

  decltype(auto) column_dimension() const noexcept
  {
    return _column_storage.dimension();
  }

  decltype(auto) row(const std::integral auto& row_index) const noexcept
  {
    return LATEL::make_VectorView(_column_storage.dimension(), _row_storage.data(row_index));
  }

  decltype(auto) column(const std::integral auto& column_index) const noexcept
  {
    return LATEL::make_VectorView(_row_storage.dimension(), _column_storage.data(column_index));
  }

  explicit CompressedMatrix(const eager_evaluation_matrix_concept auto& matrix):
    CompressedMatrix()
  {
    _row_storage.assign(matrix.row_dimension(), matrix);
    _column_storage.assign(matrix.column_dimension(), LATEL::transpose(matrix));
  }

  CompressedMatrix& operator=(const eager_evaluation_matrix_concept auto& matrix)
  {
    _row_storage.assign(matrix.row_dimension(), matrix);
    _column_storage.assign(matrix.column_dimension(), LATEL::transpose(matrix));
    return *this;
  }

};


}


#endif