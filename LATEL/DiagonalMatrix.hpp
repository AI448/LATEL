#ifndef LATEL_DIAGONALMATRIX_HPP_
#define LATEL_DIAGONALMATRIX_HPP_


#include "common.hpp"
#include "UnitVector.hpp"
#include "ACCBOOST2/container.hpp"


namespace LATEL
{


template<class IndexType, class ValueType>
class DiagonalMatrix
{
public:

  using matrix_category = LATEL::sequential_access_matrix_tag;
  using index_type = IndexType;
  using value_type = ValueType;

private:

  ACCBOOST2::Array<value_type> _values;

public:

  DiagonalMatrix() = default;
  DiagonalMatrix(DiagonalMatrix&&) = default;
  DiagonalMatrix(const DiagonalMatrix&) = default;

  explicit DiagonalMatrix(const std::unsigned_integral auto& dimension):
    _values(dimension, value_type(0))
  {}

  explicit DiagonalMatrix(const LATEL::sequential_access_matrix_concept auto& matrix):
    DiagonalMatrix()
  {
    operator=(matrix);
  }

  DiagonalMatrix& operator=(DiagonalMatrix&&) = default;
  DiagonalMatrix& operator=(const DiagonalMatrix&) = default;

  DiagonalMatrix& operator=(const LATEL::sequential_access_matrix_concept auto& matrix)
  {
    assert(matrix.row_dimension() == matrix.column_dimension());    
    _values.clear();
    _values.resize(matrix.row_dimension(), value_type(0));
    for(auto&& [i, j, a]: matrix){
      assert(i == j);
      _values[i] = a;
    }
    return *this;
  }

  decltype(auto) row_dimension() const noexcept
  {
    return _values.size();
  }

  decltype(auto) column_dimension() const noexcept
  {
    return _values.size();
  }

  decltype(auto) operator[](const std::unsigned_integral auto& index) noexcept
  {
    return _values[index];
  }

  decltype(auto) operator[](const std::unsigned_integral auto& index) const noexcept
  {
    return _values[index];
  }

private:

  struct CloneIndex
  {
    decltype(auto) operator()(auto&& pair) const noexcept
    {
      return std::tuple<
        std::tuple_element_t<0, std::remove_reference_t<decltype(pair)>>,
        std::tuple_element_t<0, std::remove_reference_t<decltype(pair)>>,
        std::tuple_element_t<1, std::remove_reference_t<decltype(pair)>>
      >(
        ACCBOOST2::get<0>(pair),
        ACCBOOST2::get<0>(pair),
        ACCBOOST2::get<1>(std::forward<decltype(pair)>(pair))
      );
    }
  };

public:

  decltype(auto) begin() const noexcept
  {
    return ACCBOOST2::make_map_iterator(
      CloneIndex{},
      ACCBOOST2::make_random_access_enumerate_iterator(
        static_cast<index_type>(0), _values.begin()
      )
    );
  }

  decltype(auto) end() const noexcept
  {
    return ACCBOOST2::make_map_iterator_or_sentinel(
      CloneIndex{},
      ACCBOOST2::make_random_access_enumerate_iterator(
        static_cast<index_type>(_values.size()), _values.end()
      )
    );
  }

};


}


#endif
