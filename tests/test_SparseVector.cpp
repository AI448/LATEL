#include "SparseVector.hpp"
#include "VectorView.hpp"


using namespace LATEL;


int main()
{

  using V = SparseVector<std::uint32_t, double>;

  static_assert(std::ranges::range<V>);
  static_assert(LATEL::sequential_access_vector_concept<V>);
  static_assert(LATEL::random_access_vector_concept<V>);

  auto stdout_stream = ACCBOOST2::IO::make_stdout_stream<char8_t>();

  stdout_stream(ACCBOOST2::META::to_string<std::ranges::range_reference_t<V>>(), "\n");

  V v(LATEL::VectorView(10, ACCBOOST2::enumerate(ACCBOOST2::range(10))));

  serialize(stdout_stream, v);

  return 0;
}
