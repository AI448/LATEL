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

  struct AddRowIndex
  {
    DerivativeMatrixT::index_type i;

    decltype(auto) operator()(auto&& pair) const noexcept
    {
      return std::tuple<
        typename DerivativeMatrixT::index_type,
        decltype(ACCBOOST2::get<0>(std::forward<decltype(pair)>(pair))),
        decltype(ACCBOOST2::get<1>(std::forward<decltype(pair)>(pair)))
      >(
        i, ACCBOOST2::get<0>(std::forward<decltype(pair)>(pair)), ACCBOOST2::get<1>(std::forward<decltype(pair)>(pair))
      );
    }
  };

  struct MakeRowRange
  {
    const RowMatrixRangePolicy* p;
    decltype(auto) operator()(auto&& i) const noexcept
    {
      assert(i < static_cast<const DerivativeMatrixT*>(p)->row_dimension());
      return ACCBOOST2::map(AddRowIndex{i}, static_cast<const DerivativeMatrixT*>(p)->row(i));
    }
  };

  decltype(auto) make_rows_iterator(const auto& first_row_index) const noexcept
  {
    return ACCBOOST2::make_map_iterator(MakeRowRange{this}, ACCBOOST2::make_integer_iterator(static_cast<typename DerivativeMatrixT::index_type>(first_row_index)));
  }

public:

  decltype(auto) begin() const noexcept
  {
    auto m = static_cast<const DerivativeMatrixT&>(*this).row_dimension();
    return ACCBOOST2::make_chain_from_iterable_iterator(make_rows_iterator(0), make_rows_iterator(m));
  }

  decltype(auto) end() const noexcept
  {
    auto m = static_cast<const DerivativeMatrixT&>(*this).row_dimension();
    return ACCBOOST2::make_chain_from_iterable_sentinel(make_rows_iterator(m));
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

  struct AddColumnIndex
  {
    DerivativeMatrixT::index_type j;
    decltype(auto) operator()(auto&& pair) const noexcept
    {
      return std::tuple<
        decltype(ACCBOOST2::get<0>(pair)), typename DerivativeMatrixT::index_type, decltype(ACCBOOST2::get<1>(pair))
      >(
        ACCBOOST2::get<0>(pair), j, ACCBOOST2::get<1>(pair)
      );
    }
  };

  struct MakeColumnRange
  {
    const ColumnMatrixRangePolicy* p;
    decltype(auto) operator()(const auto&& j) const noexcept
    {
      return ACCBOOST2::map(AddColumnIndex{j}, static_cast<const DerivativeMatrixT*>(p)->column(j));
    }
  };

  decltype(auto) make_columns_iterator(const auto& first_column_index) const noexcept
  {
    return ACCBOOST2::make_map_iterator(MakeColumnRange{this}, ACCBOOST2::make_integer_iterator(static_cast<typename DerivativeMatrixT::index_type>(first_column_index)));
  }

public:

  decltype(auto) begin() const noexcept
  {
    auto n = static_cast<const DerivativeMatrixT&>(*this).column_dimension();
    return ACCBOOST2::make_chain_from_iterable_iterator(make_columns_iterator(0), make_columns_iterator(n));
  }

  decltype(auto) end() const noexcept
  {
    auto n = static_cast<const DerivativeMatrixT&>(*this).column_dimension();
    return ACCBOOST2::make_chain_from_iterable_sentinel(make_columns_iterator(n));
  }

};


}


#endif
