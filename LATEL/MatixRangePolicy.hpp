#ifndef LATEL_MATRIXRANGE_POLICY_HPP_
#define LATEL_MATRIXRANGE_POLICY_HPP_


#include "common.hpp"


namespace LATEL
{


template<class DerivativeMatrixT>
class RowMatrixRangePolicy
{
protected:

  RowMatrixRangePolicy() = default;
  RowMatrixRangePolicy(RowMatrixRangePolicy&&) = default;
  RowMatrixRangePolicy(const RowMatrixRangePolicy&) = default;
  ~RowMatrixRangePolicy() = default;
  RowMatrixRangePolicy& operator=(RowMatrixRangePolicy&&) = default;
  RowMatrixRangePolicy& operator=(const RowMatrixRangePolicy&) = default;

private:

  template<class IndexType>
  decltype(auto) make_iterable_iterator(const IndexType& first_row_index) const noexcept
  {
    return ACCBOOST2::make_map_iterator(
      [&](auto&& row_index)
      {
        return ACCBOOST2::map(
          [row_index=row_index](auto&& pair) -> decltype(auto)
          {
            return std::tuple<IndexType, decltype(ACCBOOST2::get<0>(pair)), decltype(ACCBOOST2::get<1>(pair))>(row_index, ACCBOOST2::get<0>(pair), ACCBOOST2::get<1>(pair));
          },
          static_cast<const DerivativeMatrixT&>(*this).row(row_index)
        );
      },
      ACCBOOST2::make_integer_iterator(first_row_index)
    );
  }

public:

  decltype(auto) begin() const noexcept
  {
    auto m = static_cast<const DerivativeMatrixT&>(*this).row_dimension();
    return ACCBOOST2::make_chain_from_iterable_iterator(make_iterable_iterator(static_cast<decltype(m)>(0)), make_iterable_iterator(m));
  }

  decltype(auto) end() const noexcept
  {
    auto m = static_cast<const DerivativeMatrixT&>(*this).row_dimension();
    return ACCBOOST2::make_chain_from_iterable_sentinel(make_iterable_iterator(m));
  }

};


template<class DerivativeMatrixT>
class ColumnMatrixRangePolicy
{
protected:

  ColumnMatrixRangePolicy() = default;
  ColumnMatrixRangePolicy(ColumnMatrixRangePolicy&&) = default;
  ColumnMatrixRangePolicy(const ColumnMatrixRangePolicy&) = default;
  ~ColumnMatrixRangePolicy() = default;
  ColumnMatrixRangePolicy& operator=(ColumnMatrixRangePolicy&&) = default;
  ColumnMatrixRangePolicy& operator=(const ColumnMatrixRangePolicy&) = default;

private:

  template<class IndexType>
  decltype(auto) make_iterable_iterator(const IndexType& first_column_index) const noexcept
  {
    return ACCBOOST2::make_map_iterator(
      [&](auto&& column_index)
      {
        return ACCBOOST2::map(
          [column_index=column_index](auto&& pair) -> decltype(auto)
          {
            return std::tuple<decltype(ACCBOOST2::get<0>(pair)), IndexType, decltype(ACCBOOST2::get<1>(pair))>(ACCBOOST2::get<0>(pair), column_index, ACCBOOST2::get<1>(pair));
          },
          static_cast<const DerivativeMatrixT&>(*this).column(column_index)
        );
      },
      ACCBOOST2::make_integer_iterator(first_column_index)
    );
  }

public:

  decltype(auto) begin() const noexcept
  {
    auto n = static_cast<const DerivativeMatrixT&>(*this).column_dimension();
    return ACCBOOST2::make_chain_from_iterable_iterator(make_iterable_iterator(static_cast<decltype(n)>(0)), make_iterable_iterator(n));
  }

  decltype(auto) end() const noexcept
  {
    auto n = static_cast<const DerivativeMatrixT&>(*this).column_dimension();
    return ACCBOOST2::make_chain_from_iterable_sentinel(make_iterable_iterator(n));
  }

};


}


#endif
