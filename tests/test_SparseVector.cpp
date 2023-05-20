#include "SparseVector.hpp"
#include "VectorView.hpp"



int main()
{
  auto stdout_stream = ACCBOOST2::IO::make_stdout_stream<char8_t>();

  LATEL::SparseVector<std::uint32_t, double> v(LATEL::VectorView(10, ACCBOOST2::enumerate(ACCBOOST2::range(10))));

  LATEL::serialize(stdout_stream, v);

  return 0;
}
