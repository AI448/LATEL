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
  static_assert(!std::is_reference_v<ValueType>);
  static_assert(std::copyable<ValueType>);

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

  decltype(auto) upper_of_nonzeros() const noexcept
  {
    return _values.size();
  }

  void zero_clear(const std::integral auto& dimension)
  {
    assert(dimension <= std::numeric_limits<index_type>::max());
    _values.clear();
    _values.resize(dimension, value_type(0));
  }

  void zero_clear() noexcept(std::is_nothrow_assignable_v<ValueType>)
  {
    for(auto& x: _values){
      x = 0;
    }
  }

  decltype(auto) operator[](const std::integral auto& index) const noexcept
  {
    return _values[index];
  }

  decltype(auto) operator[](const std::integral auto& index) noexcept
  {
    return _values[index];
  }

public:

  decltype(auto) begin() const noexcept
  {
    return ACCBOOST2::make_random_access_enumerate_iterator(0, _values.begin());
  }

  decltype(auto) end() const noexcept
  {
    return ACCBOOST2::make_random_access_enumerate_iterator(_values.size(), _values.end());
  }

  explicit DenseVector(const sequential_access_vector_concept auto& vector):
    DenseVector()
  {
    operator=(vector);
  }

  DenseVector& operator=(const sequential_access_vector_concept auto& vector)
  {
    _values.resize(vector.dimension(), 0);
    {
      auto&& p = _values.begin();
      for(auto&& [i, x]: vector){
        p[i] = x;
      }
    }
    return *this;
  }

  DenseVector& operator+=(const sequential_access_vector_concept auto& vector)
  {
    {
      auto&& p = _values.begin();
      for(auto&& [i, x]: vector){
        p[i] += x;
      }
    }
    return *this;
  }

  DenseVector& operator-=(const sequential_access_vector_concept auto& vector)
  {
    {
      auto&& p = _values.begin();
      for(auto&& [i, x]: vector){
        p[i] -= x;
      }
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

  DenseVector& operator=(const LATEL::lazy_evaluation_vector_concept auto& rhs)
  {
    rhs.assign_to(*this);
    return *this;
  }

  DenseVector& operator+=(const LATEL::lazy_evaluation_vector_concept auto& rhs)
  {
    rhs.add_to(*this);
    return *this;
  }

  DenseVector& operator-=(const LATEL::lazy_evaluation_vector_concept auto& rhs)
  {
    rhs.subtract_from(*this);
    return *this;
  }

};


}


#endif
