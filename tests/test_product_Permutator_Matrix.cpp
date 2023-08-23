#include "operators/product_Permutator_Matrix.hpp"
#include "Permutator.hpp"
#include "CompressedMatrix.hpp"
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

  {
    LATEL::Permutator<std::uint32_t> p(3, 3);
    p.set(0, 1);
    p.set(1, 2);

    LATEL::CompressedMatrix<std::uint32_t, double> m(LATEL::make_MatrixView(3, 3, data));
    serialize(stdout_stream, m); stdout_stream("\n");
    static_assert(LATEL::bidirectional_matrix_concept<decltype(p * m)>);
    serialize(stdout_stream, p * m); stdout_stream("\n");
    static_assert(LATEL::bidirectional_matrix_concept<decltype(m * p)>);
    serialize(stdout_stream, m * p); stdout_stream("\n");

  }

}