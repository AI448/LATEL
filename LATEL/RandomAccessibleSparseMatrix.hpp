#ifndef LATEL_RANDOMACCESSIBLESPARSEMATRIX_HPP_
#define LATEL_RANDOMACCESSIBLESPARSEMATRIX_HPP_


#include "ACCBOOST2/container.hpp"
#include "common.hpp"
#include "MatixRangePolicy.hpp"
#include "VectorView.hpp"


namespace LATEL
{


  template<class IndexType, class ValueType>
  class RandomAccessibleSparseMatrix: public RowMatrixRangePolicy<RandomAccessibleSparseMatrix<IndexType, ValueType>>
  {
  public:

    using matrix_category = LATEL::bidirectional_matrix_tag;

    using index_type = IndexType;

    using value_type = ValueType;

  private:

    static inline constexpr value_type _zero = 0;

    ACCBOOST2::Sparse2DArray<value_type> _sparse_2d_array; // TODO index_type 追加

  public:

    RandomAccessibleSparseMatrix() = default;
    RandomAccessibleSparseMatrix(RandomAccessibleSparseMatrix&&) = default;
    RandomAccessibleSparseMatrix(const RandomAccessibleSparseMatrix&) = default;

    RandomAccessibleSparseMatrix(const std::integral auto& row_dimension, const std::integral auto& column_dimension):
      _sparse_2d_array(row_dimension, column_dimension)
    {
      assert(row_dimension <= std::numeric_limits<index_type>::max());
      assert(column_dimension <= std::numeric_limits<index_type>::max());
    }

    RandomAccessibleSparseMatrix& operator=(RandomAccessibleSparseMatrix&&) = default;
    RandomAccessibleSparseMatrix& operator=(const RandomAccessibleSparseMatrix&) = default;

    template<Axis A>
    decltype(auto) dimension() const noexcept
    {
      if constexpr (A == Axis::ROW){
        return _sparse_2d_array.row_size();
      }else{
        return _sparse_2d_array.column_size();
      }
    }

    decltype(auto) row_dimension() const noexcept
    {
      return _sparse_2d_array.row_size();
    }

    decltype(auto) column_dimension() const noexcept
    {
      return _sparse_2d_array.column_size();
    }

  private:

    template<Axis A>
    class Vector
    {
    public:

      using vector_category = LATEL::sequential_access_vector_tag;
      using index_type = RandomAccessibleSparseMatrix::index_type;
      using value_type = RandomAccessibleSparseMatrix::value_type;

    private:

      const RandomAccessibleSparseMatrix& _matrix;
      index_type _index;

    public:

      Vector(const RandomAccessibleSparseMatrix& matrix, const std::integral auto& index):
        _matrix(matrix), _index(index)
      {}

      decltype(auto) dimension() const noexcept
      {
        if constexpr (A == Axis::ROW){
          return _matrix._sparse_2d_array.row_size();
        }else{
          return _matrix._sparse_2d_array.column_size();
        }
      }

      decltype(auto) size() const noexcept
      {
        if constexpr (A == Axis::ROW){
          return _matrix._sparse_2d_array.row(_index).size();
        }else{
          return _matrix._sparse_2d_array.column(_index).size();
        }
      }

    private:

      template<class IteratorT>
      static decltype(auto) make_iterator(IteratorT&& iterator)
      {
        return ACCBOOST2::make_map_iterator(
          [](auto&& trio) -> decltype(auto)
          {
            return ACCBOOST2::capture_as_tuple(
              ACCBOOST2::get<1 - A>(std::forward<decltype(trio)>(trio)),
              ACCBOOST2::get<2>(std::forward<decltype(trio)>(trio))
            );
          },
          std::forward<IteratorT>(iterator)
        );
      }

    public:

      decltype(auto) begin() const noexcept
      {
        if constexpr (A == ROW){
          return make_iterator(_matrix._sparse_2d_array.row(_index).begin());
        }else{
          return make_iterator(_matrix._sparse_2d_array.column(_index).begin());
        }
      }

      decltype(auto) end() const noexcept
      {
        if constexpr (A == ROW){
          return make_iterator(_matrix._sparse_2d_array.row(_index).end());
        }else{
          return make_iterator(_matrix._sparse_2d_array.column(_index).end());
        }
      }

    };

  public:

    decltype(auto) row(const std::size_t& row_index) const noexcept
    {
      return Vector<Axis::ROW>(*this, row_index);
    }

    decltype(auto) column(const std::size_t& column_index) const noexcept
    {
      return Vector<Axis::COLUMN>(*this, column_index);
    }

    // template<std::integral I>
    // void redimension_row(const I& row_dimension)
    // {
    //   assert(row_dimension <= std::numeric_limits<index_type>::max());
    //   _sparse_2d_array.resize_row(row_dimension);
    // }

    // template<std::integral I>
    // void redimension_column(const I& column_dimension)
    // {
    //   assert(column_dimension <= std::numeric_limits<index_type>::max());
    //   _sparse_2d_array.resize_column(column_dimension);
    // }

    // template<std::integral I, std::integral J>
    // void redimension(const I& row_dimension, const J& column_dimension)
    // {
    //   redimension_row(row_dimension);
    //   redimension_column(column_dimension);
    // }

  private:

    class Proxy
    {
      friend class RandomAccessibleSparseMatrix;

    private:

      // TODO Sparse2DArray の方をプロキシを使った実装にして，プロキシのプロキシにする
      RandomAccessibleSparseMatrix& _matrix;
      index_type _row_index;
      index_type _column_index;

      Proxy(RandomAccessibleSparseMatrix& matrix, const std::integral auto& row_index, const std::integral auto& column_index) noexcept:
        _matrix(matrix), _row_index(row_index), _column_index(column_index)
      {}

    public:

      operator const value_type& () const noexcept
      {
        return _matrix._sparse_2d_array.get(_row_index, _column_index, _zero);
      }

      template<class V>
      requires(
        std::is_constructible_v<value_type, V&&>
      )
      Proxy& operator=(V&& rhs)
      {
        if(rhs != _zero){
          _matrix._sparse_2d_array.emplace(_row_index, _column_index, std::forward<V>(rhs));
        }else{
          _matrix._sparse_2d_array.erase(_row_index, _column_index);
        }
        return *this;
      }

    #define OPT_LATEL_SPARSEMATRIX_PROXY_OPERATOR(OP) \
      template<class V> \
      requires( \
        std::is_constructible_v<value_type, V&&> \
      ) \
      Proxy& operator OP (V&& rhs) \
      { \
        value_type tmp = _matrix._sparse_2d_array.get(_row_index, _column_index, _zero); \
        tmp OP std::forward<V>(rhs); \
        return operator=(std::move(tmp)); \
      }

    OPT_LATEL_SPARSEMATRIX_PROXY_OPERATOR(+=)
    OPT_LATEL_SPARSEMATRIX_PROXY_OPERATOR(-=)
    OPT_LATEL_SPARSEMATRIX_PROXY_OPERATOR(*=)
    OPT_LATEL_SPARSEMATRIX_PROXY_OPERATOR(/=)

    };

  public:

    decltype(auto) operator[](const std::tuple<index_type, index_type>& index_pair) const noexcept
    {
      return _sparse_2d_array.get(std::get<0>(index_pair), std::get<1>(index_pair), _zero);
    }

    decltype(auto) operator[](const std::tuple<index_type, index_type>& index_pair) noexcept
    {
      return Proxy(*this, std::get<0>(index_pair), std::get<1>(index_pair));
    }

    explicit RandomAccessibleSparseMatrix(const LATEL::sequential_access_matrix_concept auto& matrix):
      _sparse_2d_array(matrix.row_dimension(), matrix.column_dimension())
    {
      for(auto&& [i, j, a]: matrix){
        _sparse_2d_array.emplace(i, j, a);
      }
    }

    // template<matrix_concept MatrixT>
    // RandomAccessibleSparseMatrix& operator=(const MatrixT& matrix)
    // {
    //   _sparse_2d_array.resize_row(matrix.row_dimension());
    //   _sparse_2d_array.resize_column(matrix.column_dimension());
    //   for(auto&& [i, j, a]: matrix){
    //     _sparse_2d_array.emplace(i, j, a);
    //   }
    //   return *this;
    // }


  };



}


#endif
