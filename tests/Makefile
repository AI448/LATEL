TESTS=test_SparseVector test_DenseVector \
test_product_Scalar_Vector \
test_CompressedMatrix \
test_product_Permutator_Vector test_product_Matrix_Vector
# test_CoordinateMatrix test_CompressedMatrix test_BlockMatrix test_ScalarVectorProduction test_RandomAccessibleSparseMatrix \
# test_transpose_matrix


RESULTS=$(patsubst %, %.result, $(TESTS))
OUTS=$(patsubst %, %.out, $(TESTS))
DEPENDS=$(patsubst %, %.d, $(TESTS))

CXXFLAGS=-std=c++20 -W -Wall -fconcepts-diagnostics-depth=10 -g -O0 -I../../ACCBOOST2 -I../LATEL


all: $(RESULTS)

clean:
	rm -f $(RESULTS) $(OUTS) $(DEPENDS)

.PRECIOUS: $(OUTS) $(DEPENDS)

-include $(DEPENDS)

%.result: %.out
	valgrind --tool=memcheck --leak-check=full ./$< >$@
	cat $@

%.out: %.cpp
	$(CXX) $< $(CXXFLAGS) -o $@
	$(CXX) -MM $< $(CXXFLAGS) | sed 's%^.*\.o%$@%g' >$(patsubst %.out, %.d, $@)
