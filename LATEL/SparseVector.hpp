#ifndef LATEL_SPARSEVECTOR_HPP_
#define LATEL_SPARSEVECTOR_HPP_


#include "ACCBOOST2/container.hpp"
#include "common.hpp"


namespace LATEL
{

template<class IndexType, class ValueType>
class SparseVector
{
public:

  using vector_category = LATEL::random_access_vector_tag;
  using index_type = IndexType;
  using value_type = ValueType;

private:

  static inline constexpr value_type _zero = ValueType(0);

  /// 非ゼロ要素の添字の配列
  ACCBOOST2::Array<index_type> _indices;

  /// フラグと値の組の配列
  ACCBOOST2::ZippedArray<bool, value_type> _flag_values;

public:

  SparseVector() = default;
  SparseVector(SparseVector&&) = default;

  SparseVector(const SparseVector& other):
    SparseVector()
  {
    _indices.reserve(other.dimension());
    _indices = other._indices;
    _flag_values = other._flag_values;
  }

  /// 次元を指定して構築
  explicit SparseVector(const std::integral auto& dimension):
    _indices(), _flag_values()
  {
    _indices.reserve(dimension);
    _flag_values.resize(dimension, false, _zero);
  }

  SparseVector& operator=(SparseVector&&) = default;

  /// @brief 次元
  /// @return 次元
  decltype(auto) dimension() const noexcept
  {
    return _flag_values.size();
  }

  decltype(auto) upper_of_nonzeros() const noexcept
  {
    return _indices.size();
  }

  void zero_clear() noexcept
  {
    for(auto&& i: _indices){
      _flag_values[i] = std::forward_as_tuple(false, _zero);
    }
    _indices.clear();
  }

  /// @brief 次元を指定してゼロクリア
  /// @param dimension 次元
  void zero_clear(const std::unsigned_integral auto& dimension)
  {
    assert(dimension <= std::numeric_limits<index_type>::max());
    zero_clear();
    _flag_values.resize(dimension, false, _zero);
    _indices.reserve(dimension);
  }

  /// @brief 
  /// @param index 
  /// @return 
  decltype(auto) operator[](const std::integral auto& index) const noexcept
  {
    assert(index < _flag_values.size());
    return ACCBOOST2::get<1>(_flag_values[index]);
  }

private:

  class Proxy
  {
    friend class SparseVector;

  private:

    SparseVector& _sparse_vector;
    index_type _index;

    Proxy(SparseVector& sparse_vector, const std::integral auto& index) noexcept:
      _sparse_vector(sparse_vector), _index(index)
    {}

  public:

    Proxy(Proxy&&) = default;
    Proxy(const Proxy&) = default;

    operator const value_type& () const noexcept
    {
      assert(_index < _sparse_vector._flag_values.size());
      return ACCBOOST2::get<1>(_sparse_vector._flag_values[_index]);
    }

#define LATEL_SPARSEVECTOR_PROXY_ASSIGNMENT(OP) \
    template<class V>\
    Proxy& operator OP (V&& value)\
    {\
      assert(_index < _sparse_vector._flag_values.size());\
      auto&& [flag_reference, value_reference] = _sparse_vector._flag_values[_index];\
      static_assert(std::is_reference_v<decltype(flag_reference)>);\
      static_assert(std::is_reference_v<decltype(value_reference)>);\
      value_reference OP std::forward<V>(value);\
      if(value_reference != 0) [[likely]] {\
        if(flag_reference == false){\
          flag_reference = true;\
          _sparse_vector._indexes.push_back_without_allocation(_index);\
        }\
      }\
      return *this;\
    }

    LATEL_SPARSEVECTOR_PROXY_ASSIGNMENT(=)
    LATEL_SPARSEVECTOR_PROXY_ASSIGNMENT(+=)
    LATEL_SPARSEVECTOR_PROXY_ASSIGNMENT(-=)
    LATEL_SPARSEVECTOR_PROXY_ASSIGNMENT(*=)
    LATEL_SPARSEVECTOR_PROXY_ASSIGNMENT(/=)

  // deleted:

    Proxy() = delete;

  };

public:

  decltype(auto) operator[](const std::integral auto& index) noexcept
  {
    assert(index < _flag_values.size());
    return Proxy(*this, index);
  }

private:

  // struct Functor
  // {
  //   const SparseVector* p = nullptr;
  //   decltype(auto) operator()(const auto& index) const noexcept
  //   {
  //     assert(p != nullptr);
  //     return std::forward_as_tuple(index, ACCBOOST2::get<1>(p->_flag_values[index]));
  //   }
  // };
  struct Functor
  {
    const ValueType* p = nullptr;
    decltype(auto) operator()(const auto& index) const noexcept
    {
      assert(p != nullptr);
      return std::forward_as_tuple(index, p[index]);
    }
  };

public:

  decltype(auto) begin() const noexcept
  {
    return ACCBOOST2::make_map_iterator(Functor{_flag_values.size() != 0 ? std::addressof(std::get<1>(_flag_values[0])) : nullptr}, _indices.begin());
  }

  decltype(auto) end() const noexcept
  {
    return ACCBOOST2::make_map_iterator_or_sentinel(Functor{_flag_values.size() != 0 ? std::addressof(std::get<1>(_flag_values[0])) : nullptr}, _indices.end());    
  }

private:

  void _assign(const sequential_access_vector_concept auto& vector)
  {
    zero_clear(vector.dimension());
    for(auto&& [i, v]: vector){
      _indices.push_back_without_allocation(i);
      _flag_values[i] = std::forward_as_tuple(true, std::forward<decltype(v)>(v));
    }
  }

public:

  SparseVector& operator=(const SparseVector& rhs)
  {
    _assign(rhs);
    return *this;
  }

  explicit SparseVector(const sequential_access_vector_concept auto& vector):
    SparseVector()
  {
    _assign(vector);
  }

  SparseVector& operator=(const sequential_access_vector_concept auto& vector)
  {
    _assign(vector);
    return *this;
  }

  SparseVector& operator+=(const sequential_access_vector_concept auto& vector)
  {
    assert(vector.dimension() == dimension());
    for(auto&& [i, x]: vector){
      operator[](i) += x;
    }
    return *this;
  }

  SparseVector& operator-=(const sequential_access_vector_concept auto& vector)
  {
    assert(vector.dimension() == dimension());
    for(auto&& [i, x]: vector){
      operator[](i) -= x;
    }
    return *this;
  }

  SparseVector& operator*=(const std::convertible_to<value_type> auto& scalar)
  {
    // TODO 正しくやるなら scalar が finite であるかをチェックする必要がある
    for(auto&& i: _indices){
      ACCBOOST2::get<1>(_flag_values[i]) *= scalar;
    }
    return *this;
  }

  SparseVector& operator/=(const std::convertible_to<value_type> auto& scalar)
  {
    // TODO 正しくやるなら scalar が finite であるかをチェックする必要がある    
    for(auto&& i: _indices){
      ACCBOOST2::get<1>(_flag_values[i]) /= scalar;
    }
    return *this;
  }

  explicit SparseVector(const lazy_evaluation_vector_concept auto& vector):
    SparseVector()
  {
    vector.assign_to(*this);
  }

  SparseVector& operator=(const lazy_evaluation_vector_concept auto& vector)
  {
    vector.assign_to(*this);
    return *this;
  }

  SparseVector& operator+=(const lazy_evaluation_vector_concept auto& vector)
  {
    vector.add_to(*this);
    return *this;
  }

  SparseVector& operator-=(const lazy_evaluation_vector_concept auto& vector)
  {
    vector.subtract_from(*this);
    return *this;
  }

};


}



#endif
