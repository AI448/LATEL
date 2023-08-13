#ifndef LATEL_SCALARVECTORPRODUCTION_HPP_
#define LATEL_SCALARVECTORPRODUCTION_HPP_


#include "common.hpp"


namespace LATEL
{


template<class ScalarT, class VectorT>
class ScalarVectorProductionView
{
public:

  using vector_category = LATEL::eager_evaluation_vector_tag;
  using index_type = typename std::remove_reference_t<VectorT>::index_type;
  using value_type = typename std::remove_reference_t<VectorT>::value_type;

private:

  ScalarT _scalar;
  VectorT _vector;

public:

  ScalarVectorProductionView(ScalarT&& scalar, VectorT&& vector) noexcept:
    _scalar(std::forward<ScalarT>(scalar)), _vector(std::forward<VectorT>(vector))
  {}

  ScalarVectorProductionView(ScalarVectorProductionView&&) = default;

  decltype(auto) dimension() const noexcept
  {
    return _vector.dimension();
  }

  decltype(auto) size() const noexcept
  {
    return _vector.size();
  }

private:

  template<class IteratorT>
  decltype(auto) make_iterator(IteratorT&& iterator) const noexcept
  {
    return ACCBOOST2::make_map_iterator(
      [&](auto&& pair) -> decltype(auto)
      {
        return ACCBOOST2::capture_as_tuple(
          ACCBOOST2::get<0>(std::forward<decltype(pair)>(pair)),
          _scalar * ACCBOOST2::get<1>(std::forward<decltype(pair)>(pair))
        );
      },
      iterator
    );
  }

public:

  decltype(auto) begin() const noexcept
  {
    return make_iterator(_vector.begin());
  }

  decltype(auto) end() const noexcept
  {
    return make_iterator(_vector.end());
  }

};


template<class VectorT, class ScalarT>
class VectorScalarProductionView
{
public:

  using vector_category = LATEL::eager_evaluation_vector_tag;
  using index_type = typename std::remove_reference_t<VectorT>::index_type;
  using value_type = typename std::remove_reference_t<VectorT>::value_type;

private:

  VectorT _vector;
  ScalarT _scalar;

public:

  VectorScalarProductionView(VectorT&& vector, ScalarT&& scalar) noexcept:
    _vector(std::forward<VectorT>(vector)), _scalar(std::forward<ScalarT>(scalar))
  {}

  VectorScalarProductionView(VectorScalarProductionView&&) = default;

  decltype(auto) dimension() const noexcept
  {
    return _vector.dimension();
  }

  decltype(auto) size() const noexcept
  {
    return _vector.size();
  }

private:

  template<class IteratorT>
  decltype(auto) make_iterator(IteratorT&& iterator) const noexcept
  {
    return ACCBOOST2::make_map_iterator(
      [&](auto&& pair) -> decltype(auto)
      {
        return ACCBOOST2::capture_as_tuple(
          ACCBOOST2::get<0>(std::forward<decltype(pair)>(pair)),
          ACCBOOST2::get<1>(std::forward<decltype(pair)>(pair)) * _scalar
        );
      },
      iterator
    );
  }

public:

  decltype(auto) begin() const noexcept
  {
    return make_iterator(_vector.begin());
  }

  decltype(auto) end() const noexcept
  {
    return make_iterator(_vector.end());
  }

};


template<class VectorT, class ScalarT>
class VectorScalarDivisionView
{
public:

  using vector_category = LATEL::eager_evaluation_vector_tag;
  using index_type = typename std::remove_reference_t<VectorT>::index_type;
  using value_type = typename std::remove_reference_t<VectorT>::value_type;

private:

  VectorT _vector;
  ScalarT _scalar;

public:

  VectorScalarDivisionView(VectorT&& vector, ScalarT&& scalar) noexcept:
    _vector(std::forward<VectorT>(vector)), _scalar(std::forward<ScalarT>(scalar))
  {}

  VectorScalarDivisionView(VectorScalarDivisionView&&) = default;

  decltype(auto) dimension() const noexcept
  {
    return _vector.dimension();
  }

  decltype(auto) size() const noexcept
  {
    return _vector.size();
  }

private:

  template<class IteratorT>
  decltype(auto) make_iterator(IteratorT&& iterator) const noexcept
  {
    return ACCBOOST2::make_map_iterator(
      [&](auto&& pair) -> decltype(auto)
      {
        return ACCBOOST2::capture_as_tuple(
          ACCBOOST2::get<0>(std::forward<decltype(pair)>(pair)),
          ACCBOOST2::get<1>(std::forward<decltype(pair)>(pair)) / _scalar
        );
      },
      iterator
    );
  }

public:

  decltype(auto) begin() const noexcept
  {
    return make_iterator(_vector.begin());
  }

  decltype(auto) end() const noexcept
  {
    return make_iterator(_vector.end());
  }

};


template<class ScalarT, vector_concept VectorT>
requires(
  std::is_convertible_v<const ScalarT&, typename std::remove_reference_t<VectorT>::value_type>
)
decltype(auto) operator*(ScalarT&& scalar, VectorT&& vector)
{
  return ScalarVectorProductionView<ScalarT, VectorT>(
    std::forward<ScalarT>(scalar),
    std::forward<VectorT>(vector)
  );
}

template<vector_concept VectorT, class ScalarT>
requires(
  std::convertible_to<const ScalarT&, typename std::remove_reference_t<VectorT>::value_type>
)
decltype(auto) operator*(VectorT&& vector, ScalarT&& scalar)
{
  return VectorScalarProductionView<VectorT, ScalarT>(
    std::forward<VectorT>(vector),
    std::forward<ScalarT>(scalar)
  );
}

template<vector_concept VectorT, class ScalarT>
requires(
  std::convertible_to<const ScalarT&, typename std::remove_reference_t<VectorT>::value_type>
)
decltype(auto) operator/(VectorT&& vector, ScalarT&& scalar)
{
  return VectorScalarDivisionView<VectorT, ScalarT>(
    std::forward<VectorT>(vector),
    std::forward<ScalarT>(scalar)
  );
}


}


#endif
