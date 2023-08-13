#include "SparseVector.hpp"
#include "VectorView.hpp"


using namespace LATEL;


int main()
{

  auto stdout_stream = ACCBOOST2::IO::make_stdout_stream<char8_t>();

  static_assert(LATEL::random_access_vector_concept<SparseVector<std::uint32_t, double>>);

  SparseVector<std::uint32_t, double> v(LATEL::VectorView(10, ACCBOOST2::enumerate(ACCBOOST2::range(10))));

  stdout_stream(ACCBOOST2::META::to_string<decltype(v.begin())>());

  serialize(stdout_stream, v);

  return 0;
}
