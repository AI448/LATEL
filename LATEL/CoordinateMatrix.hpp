#ifndef LATEL_COORDINATEMATRIX_HPP_
#define LATEL_COORDINATEMATRIX_HPP_


#include "ACCBOOST2/container.hpp"
#include "common.hpp"


namespace LATEL
{


template<class IndexType, class ValueType>
class CoordinateMatrix
{
public:

  using matrix_category = LATEL::sequential_access_matrix_tag;

  using index_type = IndexType;

  using value_type = ValueType;

private:

  index_type _row_dimension = 0;
  index_type _column_dimension = 0;
  ACCBOOST2::ZippedArray<index_type, index_type, value_type> _data;

public:

  CoordinateMatrix() = default;
  CoordinateMatrix(CoordinateMatrix&&) = default;
  CoordinateMatrix(const CoordinateMatrix&) = default;
  CoordinateMatrix& operator=(CoordinateMatrix&&) = default;
  CoordinateMatrix& operator=(const CoordinateMatrix&) = default;

  CoordinateMatrix(const std::unsigned_integral auto& row_dimension, const std::unsigned_integral auto& column_dimension) noexcept:
    _row_dimension(row_dimension), _column_dimension(column_dimension), _data()
  {}

  decltype(auto) row_dimension() const noexcept
  {
    return _row_dimension;
  }

  decltype(auto) column_dimension() const noexcept
  {
    return _column_dimension;
  }

  decltype(auto) begin() const noexcept
  {
    return _data.begin();
  }

  decltype(auto) end() const noexcept
  {
    return _data.end();
  }

  decltype(auto) upper_of_nonzeros() const noexcept
  {
    return _data.size();
  }

  template<class I, class J, class V>
  void append(I&& i, J&& j, V&& v)
  {
    assert(std::size_t(i) < _row_dimension);
    assert(std::size_t(j) < _column_dimension);
    _data.push_back(
      std::forward<I>(i),
      std::forward<J>(j),
      std::forward<V>(v)
    );
  }

  template<std::integral I, std::integral J>
  void zero_clear(const I& row_dimension, const J& column_dimension) noexcept
  {
    assert(row_dimension <= std::numeric_limits<index_type>::max());
    assert(column_dimension <= std::numeric_limits<index_type>::max());
    _row_dimension = row_dimension;
    _column_dimension = column_dimension;
    _data.clear();
  }

  void zero_clear() noexcept
  {
    _data.clear();
  }

  template<class X>
  explicit CoordinateMatrix(const std::unsigned_integral auto& row_dimension, const std::unsigned_integral auto& column_dimension,  const X& data):
    _row_dimension(row_dimension), _column_dimension(column_dimension), _data(data)
  {}

  explicit CoordinateMatrix(const sequential_access_matrix_concept auto& matrix):
    CoordinateMatrix(matrix.row_dimension(), matrix.column_dimension(), matrix)
  {}

  CoordinateMatrix& operator=(const sequential_access_matrix_concept auto& matrix)
  {
    _row_dimension = matrix.row_dimension();
    _column_dimension = matrix.column_dimension();
    _data = matrix;
    return *this;
  }

};


}


#endif
