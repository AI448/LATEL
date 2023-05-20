#ifndef LATEL_DENSEVECTOR_HPP_
#define LATEL_DENSEVECTOR_HPP_


#include "ACCBOOST2/container.hpp"
#include "foundations.hpp"


namespace LATEL
{


template<class IndexT, class ValueT>
class DenseVector
{
public:

  using index_type = IndexT;

  using value_type = ValueT;

private:

  ACCBOOST2::Array<value_type> _values;

public:

  DenseVector() = default;
  DenseVector(DenseVector&&) = default;
  DenseVector(const DenseVector&) = default;

  template<std::integral I>
  explicit DenseVector(const I& dimension, const value_type& initial_value = value_type(0)):
    _values(dimension, initial_value)
  {
    assert(dimension <= std::numeric_limits<index_type>::max());
  }

  DenseVector& operator=(DenseVector&&) = default;
  DenseVector& operator=(const DenseVector&) = default;

  index_type dimension() const noexcept
  {
    return _values.size();
  }

  template<std::integral I>
  void clear(const I& dimension)
  {
    assert(dimension <= std::numeric_limits<index_type>::max());
    _values.clear();
    _values.resize(dimension, value_type(0));
  }

  template<std::integral I>
  decltype(auto) operator[](const I& index) const noexcept
  {
    return _values[index];
  }

  template<std::integral I>
  decltype(auto) operator[](const I& index) noexcept
  {
    return _values[index];
  }

private:

  template<class ValueArrayT>
  static decltype(auto) make_iterator(ValueArrayT&& values, const std::size_t& index) noexcept
  {
    return ACCBOOST2::make_random_access_enumerate_iterator(index, values.begin() + index);
  }

public:

  decltype(auto) begin() const noexcept
  {
    return make_iterator(_values, 0);
  }

  decltype(auto) end() const noexcept
  {
    return make_iterator(_values, _values.size());
  }

  decltype(auto) begin() noexcept
  {
    return make_iterator(_values, 0);
  }

  decltype(auto) end() noexcept
  {
    return make_iterator(_values, _values.size());
  }

  decltype(auto) size() const noexcept
  {
    return _values.size();
  }

  template<vector_concept VectorT>
  explicit DenseVector(const VectorT& vector):
    DenseVector(vector.dimension())
  {
    for(auto&& [i, x]: vector){
      _values[i] = x;
    }
  }

  template<vector_concept VectorT>
  DenseVector& operator=(const VectorT& vector)
  {
    clear(vector.dimension());
    for(auto&& [i, x]: vector){
      _values[i] = x;
    }
    return *this;
  }

  template<vector_concept VectorT>
  DenseVector& operator+=(const VectorT& vector)
  {
    for(auto&& [i, x]: vector){
      _values[i] += x;
    }
    return *this;
  }

  template<vector_concept VectorT>
  DenseVector& operator-=(const VectorT& vector)
  {
    for(auto&& [i, x]: vector){
      _values[i] -= x;
    }
    return *this;
  }

  template<class T>
  DenseVector& operator*=(const T& scalar)
  {
    for(auto& x: _values){
      x *= scalar;
    }
    return *this;
  }

  template<class T>
  DenseVector& operator/=(const T& scalar)
  {
    for(auto& x: _values){
      x /= scalar;
    }
    return *this;
  }


};


}


#endif
