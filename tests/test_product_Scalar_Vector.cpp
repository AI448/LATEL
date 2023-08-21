#include "operators/product_Scalar_Vector.hpp"
#include "VectorView.hpp"
#include "SparseVector.hpp"
#include "DenseVector.hpp"


int main()
{
  auto stdout_stream = ACCBOOST2::IO::make_stdout_stream<char8_t>();

  {
    LATEL::SparseVector<std::uint32_t, double> v(LATEL::make_VectorView(10, ACCBOOST2::enumerate(ACCBOOST2::range(10))));
    LATEL::serialize(stdout_stream, 3.0 * v); stdout_stream("\n");
    LATEL::serialize(stdout_stream, v * 3.0); stdout_stream("\n");
    LATEL::serialize(stdout_stream, v / 2.0); stdout_stream("\n");
    LATEL::serialize(stdout_stream, (3.0 * v) / 3.0); stdout_stream("\n");
  }

  {
    LATEL::DenseVector<std::uint32_t, double> v(LATEL::VectorView(10, ACCBOOST2::enumerate(ACCBOOST2::range(10))));
    LATEL::serialize(stdout_stream, 3.0 * v); stdout_stream("\n");
    LATEL::serialize(stdout_stream, v * 3.0); stdout_stream("\n");
    LATEL::serialize(stdout_stream, v / 2.0); stdout_stream("\n");
    LATEL::serialize(stdout_stream, (3.0 * v) / 3.0); stdout_stream("\n");    
  }

}
