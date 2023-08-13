#ifndef LATEL_DENSEVECTOR_HPP_
#define LATEL_DENSEVECTOR_HPP_


#include "ACCBOOST2/container.hpp"
#include "common.hpp"


namespace LATEL
{


template<class IndexType, class ValueType>
class DenseVector
{
  static_assert(std::integral<IndexType>);

public:

  using vector_category = random_access_vector_tag;

  using index_type = IndexType;

  using value_type = ValueType;

private:

  ACCBOOST2::Array<value_type> _values;

public:

  DenseVector() = default;
  DenseVector(DenseVector&&) = default;
  DenseVector(const DenseVector&) = default;

  explicit DenseVector(const std::integral auto& dimension, const value_type& initial_value = value_type(0)):
    _values(dimension, initial_value)
  {
    assert(dimension <= std::numeric_limits<index_type>::max());
  }

  DenseVector& operator=(DenseVector&&) = default;
  DenseVector& operator=(const DenseVector&) = default;

  decltype(auto) dimension() const noexcept
  {
    return _values.size();
  }

  decltype(auto) size() const noexcept
  {
    return _values.size();
  }

  void clear(const std::integral auto& dimension)
  {
    assert(dimension <= std::numeric_limits<index_type>::max());
    _values.clear();
    _values.resize(dimension, value_type(0));
  }

  decltype(auto) operator[](const std::integral auto& index) const noexcept
  {
    return _values[index];
  }

  decltype(auto) operator[](const std::integral auto& index) noexcept
  {
    return _values[index];
  }

private:

  static decltype(auto) make_iterator(auto&& values, const std::integral auto& index) noexcept
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

  explicit DenseVector(const eager_evaluation_vector_concept auto& vector):
    DenseVector(vector.dimension())
  {
    for(auto&& [i, x]: vector){
      _values[i] = x;
    }
  }

  DenseVector& operator=(const eager_evaluation_vector_concept auto& vector)
  {
    clear(vector.dimension());
    for(auto&& [i, x]: vector){
      _values[i] = x;
    }
    return *this;
  }

  DenseVector& operator+=(const eager_evaluation_vector_concept auto& vector)
  {
    for(auto&& [i, x]: vector){
      _values[i] += x;
    }
    return *this;
  }

  DenseVector& operator-=(const eager_evaluation_vector_concept auto& vector)
  {
    for(auto&& [i, x]: vector){
      _values[i] -= x;
    }
    return *this;
  }

  DenseVector& operator*=(const std::convertible_to<value_type> auto& scalar)
  {
    for(auto& x: _values){
      x *= scalar;
    }
    return *this;
  }

  DenseVector& operator/=(const std::convertible_to<value_type> auto&& scalar)
  {
    for(auto& x: _values){
      x /= scalar;
    }
    return *this;
  }

};


}


#endif
