#include "operators/product_Permutator_Vector.hpp"
#include "operators/transpose_Permutator.hpp"
#include "Permutator.hpp"
#include "DenseVector.hpp"


int main()
{
  auto stdout_stream = ACCBOOST2::IO::make_stdout_stream<char8_t>();

  {
    LATEL::Permutator<std::uint32_t> p(3, 3);
    p.set(0, 1);
    p.set(1, 2);

    LATEL::DenseVector<std::uint32_t, double> v(3);
    v[0] = 0;
    v[1] = 1;
    v[2] = 2;

    LATEL::serialize(stdout_stream, p * v); stdout_stream("\n");
    LATEL::serialize(stdout_stream, transpose(p) * (p * v)); stdout_stream("\n");
  }

  {
    LATEL::Permutator<std::uint32_t> p(1, 3);
    p.set(2, 0);
  
    LATEL::DenseVector<std::uint32_t, double> v(3);
    v[0] = 0;
    v[1] = 1;
    v[2] = 2;

    LATEL::serialize(stdout_stream, p * v); stdout_stream("\n");
  }



}