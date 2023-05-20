#ifndef LATEL_RANDOMACCESSIBLESPARSEMATRIX_HPP_
#define LATEL_RANDOMACCESSIBLESPARSEMATRIX_HPP_


#include "ACCBOOST2/container.hpp"
#include "foundations.hpp"
#include "MatixRangePolicy.hpp"
#include "VectorView.hpp"


namespace LATEL
{


  template<class IndexT, class ValueT>
  class RandomAccessibleSparseMatrix: public RowMatrixRangePolicy<RandomAccessibleSparseMatrix<IndexT, ValueT>>
  {
  public:

    using index_type = IndexT;

    using value_type = ValueT;

  private:

    static inline const value_type _zero = value_type(0);

    ACCBOOST2::Sparse2DArray<value_type> _sparse_2d_array; // TODO index_type 追加

  public:

    RandomAccessibleSparseMatrix() = default;
    RandomAccessibleSparseMatrix(RandomAccessibleSparseMatrix&&) = default;
    RandomAccessibleSparseMatrix(const RandomAccessibleSparseMatrix&) = default;

    template<std::integral I, std::integral J>
    RandomAccessibleSparseMatrix(const I& row_dimension, const J& column_dimension):
      _sparse_2d_array(row_dimension, column_dimension)
    {
      assert(row_dimension <= std::numeric_limits<index_type>::max());
      assert(column_dimension <= std::numeric_limits<index_type>::max());
    }

    template<axis_type axis>
    decltype(auto) dimension() const noexcept
    {
      if constexpr (axis == 0){
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

    template<axis_type axis>
    class Vector
    {
    public:

      using index_type = RandomAccessibleSparseMatrix::index_type;
      using value_type = RandomAccessibleSparseMatrix::value_type;

    private:

      const RandomAccessibleSparseMatrix& _matrix;
      index_type _index;

    public:

      decltype(auto) dimension() const noexcept
      {
        if constexpr (axis == ROW){
          return _matrix._sparse_2d_array.row_size();
        }else{
          return _matrix._sparse_2d_array.column_size();
        }
      }

      decltype(auto) size() const noexcept
      {
        if constexpr (axis == ROW){
          return _matrix._sparse_2d_array.row(_index).size();
        }else{
          return _matrix._sparse_2d_array.column(_index).size();
        }
      }

      Vector(const RandomAccessibleSparseMatrix& matrix, const index_type& index):
        _matrix(matrix), _index(index)
      {}

    private:

      template<class IteratorT>
      static decltype(auto) make_iterator(IteratorT&& iterator)
      {
        if constexpr (axis == ROW){
          return ACCBOOST2::make_map_iterator(
            [](auto&& trio)
            {
              return ACCBOOST2::capture_as_tuple(
                ACCBOOST2::get<1>(std::forward<decltype(trio)>(trio)),
                ACCBOOST2::get<2>(std::forward<decltype(trio)>(trio))
              );
            },
            std::forward<IteratorT>(iterator)
          );
        }else{
          return ACCBOOST2::make_map_iterator(
            [](auto&& trio)
            {
              return ACCBOOST2::capture_as_tuple(
                ACCBOOST2::get<0>(std::forward<decltype(trio)>(trio)),
                ACCBOOST2::get<2>(std::forward<decltype(trio)>(trio))
              );
            },
            std::forward<IteratorT>(iterator)
          );
        }
      }

    public:

      decltype(auto) begin() const noexcept
      {
        if constexpr (axis == ROW){
          return make_iterator(_matrix._sparse_2d_array.row(_index).begin());
        }else{
          return make_iterator(_matrix._sparse_2d_array.column(_index).begin());
        }
      }

      decltype(auto) end() const noexcept
      {
        if constexpr (axis == ROW){
          return make_iterator(_matrix._sparse_2d_array.row(_index).end());
        }else{
          return make_iterator(_matrix._sparse_2d_array.column(_index).end());
        }
      }

    };

  public:

    decltype(auto) row(const std::size_t& row_index) const noexcept
    {
      return Vector<ROW>(*this, row_index);
    }

    decltype(auto) column(const std::size_t& column_index) const noexcept
    {
      return Vector<COLUMN>(*this, column_index);
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
      std::size_t _row_index;
      std::size_t _column_index;

      Proxy(RandomAccessibleSparseMatrix& matrix, const std::size_t& row_index, const std::size_t& column_index) noexcept:
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

    decltype(auto) operator[](const std::array<std::size_t, 2>& index_pair) const noexcept
    {
      return _sparse_2d_array.get(index_pair[0], index_pair[1], _zero);
    }

    decltype(auto) operator[](const std::array<std::size_t, 2>& index_pair) noexcept
    {
      return Proxy(*this, index_pair[0], index_pair[1]);
    }

    template<matrix_concept MatrixT>
    explicit RandomAccessibleSparseMatrix(const MatrixT& matrix):
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
