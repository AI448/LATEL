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

  index_type _m;
  index_type _n;
  ACCBOOST2::ZippedArray<index_type, index_type, value_type> _data;

public:

  CoordinateMatrix() noexcept:
    _m(0), _n(0), _data()
  {}

  CoordinateMatrix(const std::size_t& m, const std::size_t& n) noexcept:
    _m(m), _n(n), _data()
  {}

  CoordinateMatrix(CoordinateMatrix&&) = default;
  CoordinateMatrix(const CoordinateMatrix&) = default;
  CoordinateMatrix& operator=(CoordinateMatrix&&) = default;
  CoordinateMatrix& operator=(const CoordinateMatrix&) = default;

  decltype(auto) row_dimension() const noexcept
  {
    return _m;
  }

  decltype(auto) column_dimension() const noexcept
  {
    return _n;
  }

  decltype(auto) begin() const noexcept
  {
    return _data.begin();
  }

  decltype(auto) end() const noexcept
  {
    return _data.end();
  }

  decltype(auto) size() const noexcept
  {
    return _data.size();
  }

  template<class I, class J, class V>
  void append(I&& i, J&& j, V&& v)
  {
    assert(std::size_t(i) < _m);
    assert(std::size_t(j) < _n);
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
    _m = row_dimension;
    _n = column_dimension;
    _data.clear();
  }

  void zero_clear() noexcept
  {
    _data.clear();
  }

  explicit CoordinateMatrix(const sequential_access_matrix_concept auto& matrix):
    _m(matrix.row_dimension()), _n(matrix.column_dimension()), _data(matrix)
  {

    if constexpr (std::ranges::random_access_range<decltype(matrix)>){
      _data.reserve(std::size(matrix));
      for(auto&& [i, j, v]: matrix){
        assert(std::size_t(i) < _m);
        assert(std::size_t(j) < _n);
        _data.push_back_without_allocation(
          std::forward<decltype(i)>(i),
          std::forward<decltype(j)>(j),
          std::forward<decltype(v)>(v)
        );
      }
    }else{
      for(auto&& [i, j, v]: matrix){
        assert(std::size_t(i) < _m);
        assert(std::size_t(j) < _n);
        _data.push_back(
          std::forward<decltype(i)>(i),
          std::forward<decltype(j)>(j),
          std::forward<decltype(v)>(v)
        );
      }
    }

  }

  CoordinateMatrix& operator=(const sequential_access_matrix_concept auto& matrix)
  {
    _m = matrix.row_dimension();
    _n = matrix.column_dimension();
    _data = matrix;
    return *this;
  }

};


}


#endif
