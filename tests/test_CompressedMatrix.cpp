#include <iostream>
#include "CompressedMatrix.hpp"
#include "MatrixView.hpp"



using namespace LATEL;


int main()
{
  ACCBOOST2::Array<std::tuple<std::uint32_t, std::uint32_t, double>> data;
  for(std::uint32_t i = 0; i < 3; i += 2){
    for(std::uint32_t j = 0; j < 3; j += 2){
      data.push_back(i, j, i + 0.1 * j);
    }
  }

  CompressedMatrix<std::uint32_t, double> matrix(
    make_MatrixView(3, 3, data)
  );
  
  for(std::uint32_t i = 0; i < 3; i += 2){
    for(auto&& [j, a]: matrix.row(i)){
      std::cout << "[" << i << "," << j << "," << a << "] ";
    }
    std::cout << std::endl;
  }

  return 0;
}