#ifndef LATEL_MATRIXVIEW_HPP_
#define LATEL_MATRIXVIEW_HPP_


#include "foundations.hpp"


namespace LATEL
{


template<class RangeT>
class MatrixView
{
public:

  using index_type = std::size_t;

  using value_type = decltype(ACCBOOST2::get<2>(*(std::declval<const RangeT&>().begin())));

private:

  index_type _row_dimension;
  index_type _column_dimension;
  RangeT _range;

public:

  template<std::integral I, std::integral J>
  MatrixView(const I& row_dimension, const J& column_dimension, RangeT&& range):
    _row_dimension(row_dimension), _column_dimension(column_dimension), _range(std::forward<RangeT>(range))
  {}

  decltype(auto) row_dimension() const noexcept
  {
    return _row_dimension;
  }

  decltype(auto) column_dimension() const noexcept
  {
    return _column_dimension;
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


template<std::integral I, std::integral J, class RangeT>
decltype(auto) make_MatrixView(const I& row_dimension, const J& column_dimension, RangeT&& range)
{
  return MatrixView<RangeT>(row_dimension, column_dimension, std::forward<RangeT>(range));
}


}


#endif
