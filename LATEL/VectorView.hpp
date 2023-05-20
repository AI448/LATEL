#ifndef LATEL_VECTORVIEW_HPP_
#define LATEL_VECTORVIEW_HPP_


#include "foundations.hpp"


namespace LATEL
{


template<class RangeT>
class VectorView
{
public:

  using index_type = std::size_t;

  using value_type = decltype(*std::declval<const RangeT&>().begin()); // TODO utility にしたい

private:

  index_type _dimension;
  RangeT _range;

public:

  VectorView(const index_type& dimension, RangeT&& range):
    _dimension(dimension), _range(std::forward<RangeT>(range))
  {}

  decltype(auto) dimension() const noexcept
  {
    return _dimension;
  }

  decltype(auto) begin() const noexcept
  {
    return _range.begin();
  }

  decltype(auto) end() const noexcept
  {
    return _range.end();
  }

  template<class R = RangeT>
  requires(
    ACCBOOST2::META::is_valid_to_size_v<const R&>
  )
  decltype(auto) size() const noexcept
  {
    return _range.size();
  }

};


template<class RangeT>
decltype(auto) make_VectorView(const std::size_t& dimension, RangeT&& range)
{
  return VectorView<RangeT>(dimension, std::forward<RangeT>(range));
}


}


#endif
