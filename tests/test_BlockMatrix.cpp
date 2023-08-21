#include "BlockMatrix.hpp"
#include "MatrixView.hpp"


using namespace LATEL;


// template<class IndexType, class ValueType>
// void print(ACCBOOST2::IO::OutputStream<char8_t>& ostream, const BlockMatrix<IndexType, ValueType>& matrix)
// {
//   for(auto&& [i, j, a]: matrix){
//     ostream(i, " ", j, " ", a, "\n");
//   }
//   // ostream("all ");
//   // serialize(ostream, matrix);
//   // ostream("\n");
//   // for(auto k = 0; k < 2; ++k){
//   //   for(auto l = 0; l < 2; ++l){
//   //     ostream(k, " ", l, " ");
//   //     serialize(ostream, matrix.make_sub_matrix_view(k, l));
//   //     ostream("\n");
//   //   }
//   // }
// }

int main()
{
  auto stdout_stream = ACCBOOST2::IO::make_stdout_stream<char8_t>();

  ACCBOOST2::Array<std::tuple<std::uint32_t, std::uint32_t, double>> data;
  for(std::uint32_t i = 0; i < 3; i += 1){
    for(std::uint32_t j = 0; j < 3; j += 1){
      data.push_back(i, j, i + 0.1 * j);
    }
  }

  BlockMatrix<std::uint32_t, double> A(2, 2, make_MatrixView(3, 3, data));

  static_assert(bidirectional_matrix_concept<BlockMatrix<std::uint32_t, double>>);

  auto print = [&](){
    stdout_stream("all "); serialize(stdout_stream, A); stdout_stream("\n");
    for(int i = 0; i < 2; ++i){
      for(int j = 0; j < 2; ++j){
        stdout_stream(i, " ", j, " "); serialize(stdout_stream, A.make_sub_matrix_view(i, j)); stdout_stream("\n");
      }
    }
  };

  print();

  A.set_block_index<ROW>(2, 1);

  print();

  A.set_block_index<COLUMN>(2, 1);

  print();

  A.set_block_index<ROW>(0, 1);

  print();

  A.set_block_index<ROW>(2, 0);
  A.set_block_index<ROW>(0, 0);
  A.set_block_index<COLUMN>(2, 0);

  print();
}