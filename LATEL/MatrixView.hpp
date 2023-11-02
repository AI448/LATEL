#ifndef LATEL_MATRIXVIEW_HPP_
#define LATEL_MATRIXVIEW_HPP_


#include "common.hpp"


namespace LATEL
{


template<class RangeType>
class MatrixView
{
public:

  using matrix_category = LATEL::sequential_access_matrix_tag;

  using index_type = std::common_type_t<
    std::remove_cv_t<std::remove_reference_t<std::tuple_element_t<0, std::remove_reference_t<std::ranges::range_reference_t<RangeType>>>>>,
    std::remove_cv_t<std::remove_reference_t<std::tuple_element_t<1, std::remove_reference_t<std::ranges::range_reference_t<RangeType>>>>>
  >;

  using value_type = std::remove_cv_t<std::remove_reference_t<std::tuple_element_t<2, std::remove_reference_t<std::ranges::range_reference_t<RangeType>>>>>;

private:

  index_type _row_dimension;
  index_type _column_dimension;
  RangeType _range;

public:

  MatrixView(const std::integral auto& row_dimension, const std::integral auto& column_dimension, RangeType&& range):
    _row_dimension(row_dimension), _column_dimension(column_dimension), _range(std::forward<RangeType>(range))
  {}

  decltype(auto) row_dimension() const noexcept
  {
    return (_row_dimension);
  }

  decltype(auto) column_dimension() const noexcept
  {
    return (_column_dimension);
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
decltype(auto) make_MatrixView(const std::integral auto& row_dimension, const std::integral auto& column_dimension, RangeType&& range)
{
  return MatrixView<RangeType>(row_dimension, column_dimension, std::forward<RangeType>(range));
}


}


#endif
