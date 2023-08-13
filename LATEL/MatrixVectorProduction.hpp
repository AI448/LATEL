#ifndef LATEL_MATRIXVECTORPRODUCTION_HPP_
#define LATEL_MATRIXVECTORPRODUCTION_HPP_


#include "LATEL_common.hpp"


namespace LATEL
{


template<class MatrixType, class VectorType>
class RowMajorMatrixVectorProductionView
{
public:

  using index_type = typename MatrixType::index_type;

  using value_type = typename VectorType::value_type;

private:

  MatrixType _matrix;
  VectorType _vector;

public:

  RowMajorMatrixVectorProductionView(MatrixType&& matrix, VectorType&& vector):
    _matrix(std::forward<MatrixType>(matrix)), _vector(std::forward<VectorType>(vector))
  {}

  RowMajorMatrixVectorProductionView(RowMajorMatrixVectorProductionView&&) = default;

  template<vector_concept ResultVectorType>
  void add_to(ResultVectorType& result)
  {
    auto m = _matrix.row_dimension();
    for(auto&& i: ACCBOOST2::range(m)){
      value_type x = 0;
      for(auto&& [j, a]: _matrix.row(i)){
        x += a * _vector[j]
      }
      result[i] += x;
    }
  }

  template<vector_concept ResultVectorType>
  void subtract_from(ResultVectorType& result)
  {
    auto m = _matrix.row_dimension();
    for(auto&& i: ACCBOOST2::range(m)){
      value_type x = 0;
      for(auto&& [j, a]: _matrix.row(i)){
        x += a * _vector[j]
      }
      result[i] -= x;
    }
  }

};


template<row_major_matrix_concept MatrixType, vector_concept VectorType>
decltype(auto) operator*(MatrixType&& matrix, VectorType&& vector)
{
  return RowMajorMatrixVectorProductionView<MatrixType, VectorType>(std::forward<MatrixType>(matrix), std::forward<VectorType>(vector));
}


}


#endif
