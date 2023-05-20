#ifndef LATEL_COORDINATEMATRIX_HPP_
#define LATEL_COORDINATEMATRIX_HPP_


#include "ACCBOOST2/container.hpp"
#include "foundations.hpp"


namespace LATEL
{


template<class IndexT, class ValueT>
class CoordinateMatrix
{
public:

  using index_type = IndexT;

  using value_type = ValueT;

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

  template<matrix_concept MatrixT>
  explicit CoordinateMatrix(const MatrixT& matrix):
    _m(matrix.row_dimension()), _n(matrix.column_dimension()), _data(matrix)
  {
/*
    if constexpr (ACCBOOST2::is_random_access_range<const MatrixT&>){
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
*/    
  }

  template<matrix_concept MatrixT>
  CoordinateMatrix& operator=(const MatrixT& matrix)
  {
    _m = matrix.row_dimension();
    _n = matrix.column_dimension();
    _data = matrix;
    return *this;
  }

};


}


#endif
