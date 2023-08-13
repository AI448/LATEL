#ifndef LATEL_VECTORVIEW_HPP_
#define LATEL_VECTORVIEW_HPP_


#include "common.hpp"


namespace LATEL
{


template<class RangeType>
class VectorView
{

  static_assert(std::ranges::range<RangeType>);
  static_assert(ACCBOOST2::is_tuple<std::remove_reference_t<std::ranges::range_rvalue_reference_t<RangeType>>>);
  static_assert(std::integral<std::remove_reference_t<std::tuple_element_t<0, std::ranges::range_reference_t<RangeType>>>>);

public:

  using vector_category = eager_evaluation_vector_tag;

  using index_type = std::remove_cv_t<std::remove_reference_t<std::tuple_element_t<0, std::ranges::range_reference_t<RangeType>>>>;

  using value_type = std::remove_cv_t<std::remove_reference_t<std::tuple_element_t<1, std::ranges::range_reference_t<RangeType>>>>;

private:

  index_type _dimension;
  RangeType _range;

public:

  VectorView(const std::integral auto& dimension, RangeType&& range):
    _dimension(dimension), _range(std::forward<RangeType>(range))
  {}

  decltype(auto) dimension() const noexcept
  {
    return _dimension;
  }

  decltype(auto) size() const noexcept
  {
    if constexpr (std::ranges::random_access_range<RangeType>){
      return index_type(_range.end() - _range.begin());
    }else{
      return _dimension;
    }
  }

  decltype(auto) begin() const noexcept
  {
    return _range.begin();
  }

  decltype(auto) end() const noexcept
  {
    return _range.end();
  }

};


template<class RangeType>
decltype(auto) make_VectorView(const std::integral auto& dimension, RangeType&& range)
{
  return VectorView<RangeType>(dimension, std::forward<RangeType>(range));
}


}


#endif
