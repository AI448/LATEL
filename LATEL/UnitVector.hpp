#ifndef LATEL_UNITVECTOR_HPP_
#define LATEL_UNITVECTOR_HPP_


#include "LATEL_common.hpp"


namespace LATEL
{


template<class IndexType, class ValueType>
class UnitVector
{
public:

  using index_type = IndexType;
  using value_type = ValueType;

private:

  index_type _dimension;
  index_type _index;

public:

  UnitVector(const index_type& dimension, const index_type& index) noexcept:
    _dimension(dimension), _index(index)
  {}

  UnitVector(UnitVector&&) = default;
  UnitVector(const UnitVector&) = default;

  UnitVector& operator=(UnitVector&&) = default;
  UnitVector& operator=(const UnitVector&) = default;

  decltype(auto) dimension() const noexcept
  {
    return _dimension;
  }

  decltype(auto) size() const noexcept
  {
    return 1;
  }

  decltype(auto) make_iterator(const std::size_t i) const noexcept
  {
    return ACCBOOST2::make_map_iterator(
      [&](auto&&)
      {
        return ACCBOOST2::capture_as_tuple(_index, 1);
      },
      ACCBOOST2::make_integer_iterator(i)
    );
  }

  decltype(auto) begin() const noexcept
  {
    return make_iterator(0);
  }

  decltype(auto) end() const noexcept
  {
    return make_iterator(1);
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
