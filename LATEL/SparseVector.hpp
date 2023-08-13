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
  explicit SparseVector(std::integral auto&& dimension):
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

  decltype(auto) size() const noexcept
  {
    return _indexes.size();
  }

  /// @brief 次元を指定してゼロクリア
  /// @param dimension 次元
  void zero_clear(std::integral auto&& dimension)
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
  decltype(auto) operator[](std::integral auto&& index) const noexcept
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

  template<class IndexIteratorT>
  decltype(auto) make_iterator(IndexIteratorT&& index_iterator) const noexcept
  {
    return ACCBOOST2::make_map_iterator(
      [&](const auto& index)
      {
        return std::forward_as_tuple(index, ACCBOOST2::get<1>(_position_values[index]));
      },
      std::forward<IndexIteratorT>(index_iterator)
    );
  }

public:

  decltype(auto) begin() const noexcept
  {
    return make_iterator(_indexes.begin());
  }

  decltype(auto) end() const noexcept
  {
    return make_iterator(_indexes.end());
  }

  explicit SparseVector(const eager_evaluation_vector_concept auto& vector):
    SparseVector(vector.dimension())
  {
    for(auto&& [i, x]: vector){
      operator[](i) = x;
    }
  }

  SparseVector& operator=(const eager_evaluation_vector_concept auto& vector)
  {
    zero_clear(vector.dimension());
    for(auto&& [i, x]: vector){
      operator[](i) = x;
    }
    return *this;
  }

  SparseVector& operator+=(const eager_evaluation_vector_concept auto& vector)
  {
    assert(vector.dimension() == dimension());
    for(auto&& [i, x]: vector){
      operator[](i) += x;
    }
    return *this;
  }

  SparseVector& operator-=(const eager_evaluation_vector_concept auto& vector)
  {
    assert(vector.dimension() == dimension());
    for(auto&& [i, x]: vector){
      operator[](i) -= x;
    }
    return *this;
  }

  SparseVector& operator*=(const std::convertible_to<index_type> auto& scalar)
  {
    for(auto&& i: _indexes){
      ACCBOOST2::get<1>(_position_values[i]) *= scalar;
    }
    return *this;
  }

  SparseVector& operator/=(const std::convertible_to<index_type> auto& scalar)
  {
    for(auto&& i: _indexes){
      ACCBOOST2::get<1>(_position_values[i]) /= scalar;
    }
    return *this;
  }

};


}



#endif
