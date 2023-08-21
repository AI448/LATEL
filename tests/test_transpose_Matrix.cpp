#include "CompressedMatrix.hpp"
#include "MatrixView.hpp"
#include "TransposedMatrixView.hpp"


int main()
{
  auto stdout_stream = ACCBOOST2::IO::make_stdout_stream<char8_t>();
  
  ACCBOOST2::Array<std::tuple<std::uint32_t, std::uint32_t, double>> data;
  for(std::uint32_t i = 0; i < 3; i += 1){
    for(std::uint32_t j = 0; j < 3; j += 1){
      data.push_back(i, j, i + 0.1 * j);
    }
  }


}