#ifndef LATEL_OPERATORS_PRODUCT_MATRIX_VECTOR_HPP_
#define LATEL_OPERATORS_PRODUCT_MATRIX_VECTOR_HPP_


#include "../common.hpp"
#include "product_Scalar_Vector.hpp"
#include "product_Vectors.hpp"


namespace LATEL
{


namespace _product_Matrix_Vector
{


template<class MatrixType, class VectorType>
class MatrixProductedVectorView
{
public:

  using vector_category = LATEL::lazy_evaluation_vector_tag;
  using index_type = std::remove_cv_t<
    std::remove_reference_t<
      std::common_type_t<
        typename std::remove_reference_t<MatrixType>::index_type,
        typename std::remove_reference_t<VectorType>::index_type
      >
    >
  >;
  using value_type = std::remove_cv_t<
    std::remove_reference_t<
      std::common_type_t<
        typename std::remove_reference_t<MatrixType>::value_type,
        typename std::remove_reference_t<VectorType>::value_type
      >
    >
  >;

private:

  MatrixType _matrix;
  VectorType _vector;

public:

  explicit MatrixProductedVectorView(MatrixType&& matrix, VectorType&& vector):
    _matrix(std::forward<MatrixType>(matrix)), _vector(std::forward<VectorType>(vector))
  {
    assert(_matrix.column_dimension() == _vector.dimension());
  }

  decltype(auto) dimension() const noexcept
  {
    return _matrix.row_dimension();
  }

private:

  template<class LHSVectorType>
  void add_to_as_row_matrix_and_dense_vector(LHSVectorType& lhs_vector) const
  {
    assert(lhs_vector.dimension() == _matrix.row_dimension());
    for(auto&& i: ACCBOOST2::range(_matrix.row_dimension())){
      lhs_vector[i] += _matrix.row(i) * _vector;
    }
  }

  template<class LHSVectorType>
  void add_to_as_column_matrix_and_sparse_vector(LHSVectorType& lhs_vector) const
  {
    assert(lhs_vector.dimension() == _matrix.row_dimension());
    for(auto&& [j, v]: _vector){
      if(v == 0) continue;
      lhs_vector += _matrix.column(j) * v;
    }
  }

  template<class LHSVectorType>
  void add_to_as_coordinate_matrix_and_dense_vector(LHSVectorType& lhs_vector) const
  {
    assert(lhs_vector.dimension() == _matrix.row_dimension());
    for(auto&& [i, j, a]: _matrix){
      lhs_vector[i] += a * _vector[j];
    }
  }

  template<class LHSVectorType>
  void subtract_from_as_row_matrix_and_dense_vector(LHSVectorType& lhs_vector) const
  {
    assert(lhs_vector.dimension() == _matrix.row_dimension());
    for(auto&& i: ACCBOOST2::range(_matrix.row_dimension())){
      lhs_vector[i] -= _matrix.row(i) * _vector;
    }
  }

  template<class LHSVectorType>
  void subtract_from_as_column_matrix_and_sparse_vector(LHSVectorType& lhs_vector) const
  {
    assert(lhs_vector.dimension() == _matrix.row_dimension());
    for(auto&& [j, v]: _vector){
      if(v == 0) continue;
      lhs_vector -= _matrix.column(j) * v;
    }
  }

  template<class LHSVectorType>
  void subtract_from_as_coordinate_matrix_and_dense_vector(LHSVectorType& lhs_vector) const
  {
    assert(lhs_vector.dimension() == _matrix.row_dimension());
    for(auto&& [i, j, a]: _matrix){
      lhs_vector[i] -= a * _vector[j];
    }
  }

  template<class LHSVectorType>
  void assign_to_as_row_matrix_and_dense_vector(LHSVectorType& lhs_vector) const
  {
    lhs_vector.zero_clear(_matrix.row_dimension());
    for(auto&& i: ACCBOOST2::range(_matrix.row_dimension())){
      lhs_vector[i] = _matrix.row(i) * _vector;
    }
  }

  template<class LHSVectorType>
  void assign_to_as_column_matrix_and_sparse_vector(LHSVectorType& lhs_vector) const
  {
    lhs_vector.zero_clear(_matrix.row_dimension());
    add_to_as_column_matrix_and_sparse_vector(lhs_vector);
  }

  template<class LHSVectorType>
  void assign_to_as_coordinate_matrix_and_dense_vector(LHSVectorType& lhs_vector) const
  {
    lhs_vector.zero_clear(_matrix.row_dimension());
    add_to_as_coordinate_matrix_and_dense_vector(lhs_vector);
  }

public:

  template<class LHSVectorType>
  void assign_to(LHSVectorType& lhs_vector) const
  {
    if constexpr (LATEL::bidirectional_matrix_concept<MatrixType> && LATEL::random_access_vector_concept<VectorType>){
      if(_vector.upper_of_nonzeros() >= _vector.dimension() / 2){
        assign_to_as_row_matrix_and_dense_vector(lhs_vector);
      }else{
        assign_to_as_column_matrix_and_sparse_vector(lhs_vector);
      }
    }else if constexpr (LATEL::row_matrix_concept<MatrixType> && LATEL::random_access_vector_concept<VectorType>){
      assign_to_as_row_matrix_and_dense_vector(lhs_vector);
    }else if constexpr (LATEL::column_matrix_concept<MatrixType> && LATEL::sequential_access_vector_concept<VectorType>){
      assign_to_as_column_matrix_and_sparse_vector(lhs_vector);
    }else{
      assign_to_as_coordinate_matrix_and_dense_vector(lhs_vector);
    }
  }

  template<class LHSVectorType>
  void add_to(LHSVectorType& lhs_vector) const
  {
    if constexpr (LATEL::bidirectional_matrix_concept<MatrixType> && LATEL::random_access_vector_concept<VectorType>){
      if(_vector.upper_of_nonzeros() >= _vector.dimsnion() / 2){
        add_to_as_row_matrix_and_dense_vector(lhs_vector);
      }else{
        add_to_as_column_matrix_and_sparse_vector(lhs_vector);
      }
    }else if constexpr (LATEL::row_matrix_concept<MatrixType> && LATEL::random_access_vector_concept<VectorType>){
      add_to_as_row_matrix_and_dense_vector(lhs_vector);
    }else if constexpr (LATEL::column_matrix_concept<MatrixType> && LATEL::sequential_access_vector_concept<VectorType>){
      add_to_as_column_matrix_and_sparse_vector(lhs_vector);
    }else{
      add_to_as_coordinate_matrix_and_dense_vector(lhs_vector);
    }
  }

  template<class LHSVectorType>
  void subtract_from(LHSVectorType& lhs_vector) const
  {
    if constexpr (LATEL::bidirectional_matrix_concept<MatrixType> && LATEL::random_access_vector_concept<VectorType>){
      if(_vector.upper_of_nonzeros() >= _vector.dimsnion() / 2){
        subtract_from_as_row_matrix_and_dense_vector(lhs_vector);
      }else{
        subtract_from_as_column_matrix_and_sparse_vector(lhs_vector);
      }
    }else if constexpr (LATEL::row_matrix_concept<MatrixType> && LATEL::random_access_vector_concept<VectorType>){
      subtract_from_as_row_matrix_and_dense_vector(lhs_vector);
    }else if constexpr (LATEL::column_matrix_concept<MatrixType> && LATEL::sequential_access_vector_concept<VectorType>){
      subtract_from_as_column_matrix_and_sparse_vector(lhs_vector);
    }else{
      subtract_from_as_coordinate_matrix_and_dense_vector(lhs_vector);
    }
  }

};


}



template<LATEL::sequential_access_matrix_concept MatrixType, LATEL::sequential_access_vector_concept VectorType>
decltype(auto) operator*(MatrixType&& matrix, VectorType&& vector)
{
  static_assert(LATEL::column_matrix_concept<MatrixType> || LATEL::random_access_vector_concept<VectorType>);
  return _product_Matrix_Vector::MatrixProductedVectorView<MatrixType, VectorType>(std::forward<MatrixType>(matrix), std::forward<VectorType>(vector));
}


}


#endif
