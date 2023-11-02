#ifndef LATEL_UNITVECTOR_HPP_
#define LATEL_UNITVECTOR_HPP_


#include "common.hpp"


namespace LATEL
{


template<class IndexType, class ValueType>
class UnitVector
{
public:

  using vector_category = LATEL::random_access_vector_tag;
  using index_type = IndexType;
  using value_type = ValueType;

private:

  index_type _dimension;
  index_type _index;

public:

  explicit UnitVector(const index_type& dimension, const index_type& index) noexcept:
    _dimension(dimension), _index(index)
  {}

  UnitVector(UnitVector&&) = default;
  UnitVector(const UnitVector&) = default;

  UnitVector& operator=(UnitVector&&) = default;
  UnitVector& operator=(const UnitVector&) = default;

  decltype(auto) dimension() const noexcept
  {
    return (_dimension);
  }

  decltype(auto) upper_of_nonzeros() const noexcept
  {
    return 1;
  }

private:

  struct MakePair
  {
    const UnitVector* p = nullptr;
    decltype(auto) operator()(auto&&) const noexcept
    {
      assert(p != nullptr);
      return ACCBOOST2::capture_as_tuple(p->_index, 1);
    }
  };

public:

  decltype(auto) begin() const noexcept
  {
    return ACCBOOST2::make_map_iterator(MakePair{this}, ACCBOOST2::make_integer_iterator(0));
  }

  decltype(auto) end() const noexcept
  {
    return ACCBOOST2::make_map_iterator_or_sentinel(MakePair{this}, ACCBOOST2::make_integer_iterator(1));
  }

  value_type operator[](const index_type& index) const noexcept
  {
    assert(index < _dimension);
    if(index != _index){
      return 0;
    }else{
      return 1;
    }
  }

// deleted:

  UnitVector() = delete;

};


}


#endif
