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

  static inline constexpr std::size_t _null_position = std::numeric_limits<std::size_t>::max();

  static inline constexpr value_type _zero = ValueType(0);

  /// 非ゼロ要素の添字の配列
  ACCBOOST2::Array<index_type> _indexes;

  /// 添字の格納位置と値の組の配列
  ACCBOOST2::ZippedArray<std::size_t, value_type> _position_values;

public:

  SparseVector() = default;
  SparseVector(SparseVector&&) = default;
  SparseVector(const SparseVector&) = default;

  /// 次元を指定して構築
  explicit SparseVector(const std::integral auto& dimension):
    _indexes(), _position_values()
  {
    _indexes.reserve(dimension);
    _position_values.resize(dimension, _null_position, _zero);
  }

  SparseVector& operator=(SparseVector&&) = default;
  SparseVector& operator=(const SparseVector&) = default;

  /// @brief 次元
  /// @return 次元
  decltype(auto) dimension() const noexcept
  {
    return _position_values.size();
  }

  decltype(auto) upper_of_nonzeros() const noexcept
  {
    return _indexes.size();
  }

  /// @brief 次元を指定してゼロクリア
  /// @param dimension 次元
  void zero_clear(const std::integral auto& dimension)
  {
    assert(dimension <= std::numeric_limits<index_type>::max());
    for(auto&& i: _indexes){
      if(i < _position_values.size()){
        _position_values[i] = std::make_tuple(_null_position, _zero);
      }
    }
    _indexes.clear();
    _position_values.resize(dimension, _null_position, _zero);
    _indexes.reserve(dimension);
  }

  void zero_clear() noexcept
  {
    for(auto&& i: _indexes){
      _position_values[i] = std::make_tuple(_null_position, _zero);
    }
    _indexes.clear();
  }

  /// @brief 
  /// @param index 
  /// @return 
  decltype(auto) operator[](const std::integral auto& index) const noexcept
  {
    assert(index < _position_values.size());
    return ACCBOOST2::get<1>(_position_values[index]);
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
      assert(_index < _sparse_vector._position_values.size());
      return ACCBOOST2::get<1>(_sparse_vector._position_values[_index]);
    }

#define LATEL_SPARSEVECTOR_PROXY_ASSIGNMENT(OP) \
    template<class V>\
    Proxy& operator OP (V&& value)\
    {\
      assert(_index < _sparse_vector._position_values.size());\
      auto&& [position_reference, value_reference] = _sparse_vector._position_values[_index];\
      static_assert(std::is_reference_v<decltype(position_reference)>);\
      static_assert(std::is_reference_v<decltype(value_reference)>);\
      value_reference OP std::forward<V>(value);\
      if(value_reference != 0){\
        if(position_reference == _null_position){\
          position_reference = _sparse_vector._indexes.size();\
          _sparse_vector._indexes.push_back(_index);\
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
    assert(index < _position_values.size());
    return Proxy(*this, index);
  }

private:

  struct Functor
  {
    const SparseVector* p = nullptr;
    decltype(auto) operator()(const auto& index) const noexcept
    {
      assert(p != nullptr);
      return std::forward_as_tuple(index, ACCBOOST2::get<1>(p->_position_values[index]));
    }
  };

public:

  decltype(auto) begin() const noexcept
  {
    return ACCBOOST2::make_map_iterator(Functor{this}, _indexes.begin());
  }

  decltype(auto) end() const noexcept
  {
    return ACCBOOST2::make_map_iterator_or_sentinel(Functor{this}, _indexes.end());    
  }

  explicit SparseVector(const sequential_access_vector_concept auto& vector):
    SparseVector()
  {
    operator=(vector);
  }

  SparseVector& operator=(const sequential_access_vector_concept auto& vector)
  {
    // _indexes.clear();
    // _position_values.clear();
    // _indexes.reserve(vector.upper_of_nonzeros());
    // _position_values.resize(vector.dimension(), _null_position, _zero);
    // for(auto&& [i, x]: vector){
    //   _position_values[i] = std::forward_as_tuple(_indexes.size(), std::forward<decltype(x)>(x));
    //   _indexes.push_back_without_allocation(std::forward<decltype(i)>(i));      
    // }
    zero_clear(vector.dimension());
    for(auto&& [i, x]: vector){
      operator[](i) += x;
    }
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
    for(auto&& i: _indexes){
      ACCBOOST2::get<1>(_position_values[i]) *= scalar;
    }
    return *this;
  }

  SparseVector& operator/=(const std::convertible_to<value_type> auto& scalar)
  {
    for(auto&& i: _indexes){
      ACCBOOST2::get<1>(_position_values[i]) /= scalar;
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

};


}



#endif
