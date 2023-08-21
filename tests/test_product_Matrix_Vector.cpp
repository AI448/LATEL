#include "operators/product_Matrix_Vector.hpp"
#include "SparseVector.hpp"
#include "DenseVector.hpp"
#include "CompressedMatrix.hpp"
#include "VectorView.hpp"
#include "MatrixView.hpp"


int main()
{
  auto stdout_stream = ACCBOOST2::IO::make_stdout_stream<char8_t>();

  ACCBOOST2::Array<std::tuple<std::uint32_t, std::uint32_t, double>> data;
  for(std::uint32_t i = 0; i < 3; i += 1){
    for(std::uint32_t j = 0; j < 3; j += 1){
      data.push_back(i, j, i + 0.1 * j);
    }
  }

  LATEL::CompressedColumnMatrix<std::uint32_t, double> column_matrix(LATEL::make_MatrixView(3, 3, data));
  LATEL::CompressedRowMatrix<std::uint32_t, double> row_matrix(LATEL::make_MatrixView(3, 3, data));
  LATEL::CompressedMatrix<std::uint32_t, double> bidirectional_matrix(LATEL::make_MatrixView(3, 3, data));
  auto sparse_vector = LATEL::make_VectorView(3, ACCBOOST2::enumerate(ACCBOOST2::range(3)));
  LATEL::DenseVector<std::uint32_t, double> dense_vector(sparse_vector);

  LATEL::SparseVector<std::uint32_t, double> x;

  static_assert(LATEL::lazy_evaluation_vector_concept<decltype(column_matrix * sparse_vector)>);
  x = column_matrix * sparse_vector;
  LATEL::serialize(stdout_stream, x); stdout_stream("\n");

  static_assert(LATEL::lazy_evaluation_vector_concept<decltype(row_matrix * dense_vector)>);
  x = row_matrix * dense_vector;
  LATEL::serialize(stdout_stream, x); stdout_stream("\n");

  static_assert(LATEL::lazy_evaluation_vector_concept<decltype(bidirectional_matrix * dense_vector)>);
  x = bidirectional_matrix * dense_vector;
  LATEL::serialize(stdout_stream, x); stdout_stream("\n");


}
