#include <iostream>
#include "CompressedMatrix.hpp"
#include "MatrixView.hpp"



using namespace LATEL;


int main()
{
  auto stdout_stream = ACCBOOST2::IO::make_stdout_stream<char8_t>();

  ACCBOOST2::Array<std::tuple<std::uint32_t, std::uint32_t, double>> data;
  for(std::uint32_t i = 0; i < 3; i += 1){
    for(std::uint32_t j = 0; j < 3; j += 1){
      data.push_back(i, j, i + 0.1 * j);
    }
  }

  {
    using M = CompressedRowMatrix<std::uint32_t, double>;
    static_assert(LATEL::row_matrix_concept<M>);
    M m(LATEL::make_MatrixView(3, 3, data));
    serialize(stdout_stream, m); stdout_stream("\n");
    serialize(stdout_stream, M(LATEL::make_MatrixView(3, 3, data))); stdout_stream("\n");
  }

  {
    using M = CompressedColumnMatrix<std::uint32_t, double>;
    static_assert(LATEL::column_matrix_concept<M>);
    M m(LATEL::make_MatrixView(3, 3, data));
    serialize(stdout_stream, m); stdout_stream("\n");
    serialize(stdout_stream, M(LATEL::make_MatrixView(3, 3, data))); stdout_stream("\n");
  }

  {
    using M = CompressedMatrix<std::uint32_t, double>;
    static_assert(LATEL::bidirectional_matrix_concept<M>);
    M m(LATEL::make_MatrixView(3, 3, data));
    serialize(stdout_stream, m); stdout_stream("\n");
    serialize(stdout_stream, M(LATEL::make_MatrixView(3, 3, data))); stdout_stream("\n");
  }

  return 0;
}
