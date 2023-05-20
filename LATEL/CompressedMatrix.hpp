#ifndef LATEL_COMPRESSEDMATRIX_HPP_
#define LATEL_COMPRESSEDMATRIX_HPP_



#include "ACCBOOST2/container.hpp"
#include "foundations.hpp"
#include "MatixRangePolicy.hpp"
#include "VectorView.hpp"
#include "TransposedMatrixView.hpp"


namespace LATEL
{


template<class IndexT, class ValueT>
class CompressedMatrix: public RowMatrixRangePolicy<CompressedMatrix<IndexT, ValueT>>
{
public:

  using index_type = IndexT;

  using value_type = ValueT;

private:

  struct CompressedStorage
  {
    ACCBOOST2::Array<std::size_t> _positions;
    ACCBOOST2::ZippedArray<index_type, value_type> _data;

    template<std::integral I, class RangeT>
    void assign(const I& dimension, const RangeT& range)
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

    index_type dimension() const noexcept
    {
      assert(_positions.size() - 1 <= std::numeric_limits<index_type>::max());
      return _positions.size() != 0 ? _positions.size() - 1 : 0;      
    }

    template<std::integral I>
    decltype(auto) data(const I& index) const noexcept
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

  template<std::integral I>
  decltype(auto) row(const I& row_index) const noexcept
  {
    return _row_storage.data(row_index);
  }

  template<std::integral J>
  decltype(auto) column(const J& column_index) const noexcept
  {
    return _column_storage.data(column_index);
  }

  template<matrix_concept MatrixT>
  explicit CompressedMatrix(const MatrixT& matrix):
    CompressedMatrix()
  {
    _row_storage.assign(matrix.row_dimension(), matrix);
    _column_storage.assign(matrix.column_dimension(), transpose(matrix));
  }

  template<matrix_concept MatrixT>
  CompressedMatrix& operator=(const MatrixT& matrix)
  {
    if(std::addressof(matrix) != this){
      _row_storage.assign(matrix.row_dimension(), matrix);
      _column_storage.assign(matrix.column_dimension(), transpose(matrix));
    }
    return *this;
  }

};


}


#endif