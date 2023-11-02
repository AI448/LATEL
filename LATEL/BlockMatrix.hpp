#ifndef LATEL_BLOCKMATRIX_HPP_
#define LATEL_BLOCKMATRIX_HPP_

#include "ACCBOOST2/container.hpp"
#include "common.hpp"
#include "MatixRangePolicy.hpp"
#include "VectorView.hpp"


namespace LATEL
{


template<class IndexType, class ValueType>
class BlockMatrix: public RowMatrixRangePolicy<BlockMatrix<IndexType, ValueType>>
{
public:

  using matrix_category = LATEL::bidirectional_matrix_tag;
  using index_type = IndexType;
  using value_type = ValueType;

private:

  struct Data
  {
    index_type _block_size = 0;
    ACCBOOST2::Array<index_type> _block_indices;
    ACCBOOST2::Array<std::size_t> _positions;
    ACCBOOST2::Array<std::size_t> _another_positions;
    ACCBOOST2::Array<index_type> _indices;
    ACCBOOST2::Array<value_type> _values;
  };

  std::array<Data, 2> _data;

  void assign(const sequential_access_matrix_concept auto& matrix)
  {
    std::array<index_type, 2> block_size = {_data[Axis::ROW]._block_size, _data[Axis::COLUMN]._block_size};
    auto m = matrix.row_dimension();
    auto n = matrix.column_dimension();
    assert(block_size[Axis::ROW] > 0);
    assert(block_size[Axis::COLUMN] > 0);
    assert(m > 0);
    assert(n > 0);
    //
    _data[Axis::ROW]._block_indices.clear();
    _data[Axis::ROW]._block_indices.resize(m, 0);
    _data[Axis::ROW]._positions.clear();
    _data[Axis::ROW]._positions.resize(m * block_size[Axis::COLUMN] + 1, 0);
    _data[Axis::COLUMN]._block_indices.clear();
    _data[Axis::COLUMN]._block_indices.resize(n, 0);
    _data[Axis::COLUMN]._positions.clear();    
    _data[Axis::COLUMN]._positions.resize(n * block_size[Axis::ROW] + 1, 0);

    std::size_t size = 0;
    // 行・列データの開始位置を算出
    // i 行目のデータの開始位置を _positions[i + 1] に格納している
    for(auto&& [i, j, a]: matrix){
      if(i + 2 <= m){
        _data[Axis::ROW]._positions[(i + 2) * block_size[Axis::COLUMN]] += 1;
      }
      if(j + 2 <= n){
        _data[Axis::COLUMN]._positions[(j + 2) * block_size[Axis::ROW]] += 1;
      }
      size += 1;
    }
    if(m > 2){
      for(auto&& i: ACCBOOST2::range(2, m)){
        _data[Axis::ROW]._positions[(i + 1) * block_size[Axis::COLUMN]] += _data[Axis::ROW]._positions[i * block_size[Axis::COLUMN]];
      }
    }
    if(n > 2){
      for(auto&& j: ACCBOOST2::range(2, n)){
        _data[Axis::COLUMN]._positions[(j + 1) * block_size[Axis::ROW]] += _data[Axis::COLUMN]._positions[j * block_size[Axis::ROW]];
      }
    }
    // データを配置
    _data[Axis::ROW]._indices.resize(size);
    _data[Axis::ROW]._values.resize(size);
    _data[Axis::ROW]._another_positions.resize(size);    
    _data[Axis::COLUMN]._indices.resize(size);
    _data[Axis::COLUMN]._values.resize(size);
    _data[Axis::COLUMN]._another_positions.resize(size);    
    for(auto&& [i, j, a]: matrix){
      // std::array<std::size_t, 2> position = {_data[Axis::ROW]._positions[(i + 1) * block_size[Axis::COLUMN]], _data[Axis::COLUMN]._positions[(j + 1) * block_size[Axis::ROW]]};
      std::size_t position[2] = {_data[Axis::ROW]._positions[(i + 1) * block_size[Axis::COLUMN]], _data[Axis::COLUMN]._positions[(j + 1) * block_size[Axis::ROW]]};      
      _data[Axis::ROW]._indices[position[Axis::ROW]] = j;
      _data[Axis::ROW]._values[position[Axis::ROW]] = a;
      _data[Axis::ROW]._another_positions[position[Axis::ROW]] = position[Axis::COLUMN];
      _data[Axis::COLUMN]._indices[position[Axis::COLUMN]] = i;
      _data[Axis::COLUMN]._values[position[Axis::COLUMN]] = a;
      _data[Axis::COLUMN]._another_positions[position[Axis::COLUMN]] = position[Axis::ROW];
      //
      _data[Axis::ROW]._positions[(i + 1) * block_size[Axis::COLUMN]] += 1;
      _data[Axis::COLUMN]._positions[(j + 1) * block_size[Axis::ROW]] += 1;
    }
    assert(_data[Axis::ROW]._positions[m * block_size[Axis::COLUMN]] == size);
    assert(_data[Axis::COLUMN]._positions[n * block_size[Axis::ROW]] == size);
    for(auto&& i: ACCBOOST2::range(m)){
      for(auto&& k: ACCBOOST2::range(1, block_size[Axis::COLUMN])){
        _data[Axis::ROW]._positions[i * block_size[Axis::COLUMN] + k] = _data[Axis::ROW]._positions[(i + 1) * block_size[Axis::COLUMN]];
      }
    }
    for(auto&& j: ACCBOOST2::range(n)){
      for(auto&& l: ACCBOOST2::range(1, block_size[Axis::ROW])){
        _data[Axis::COLUMN]._positions[j * block_size[Axis::ROW] + l] = _data[Axis::COLUMN]._positions[(j + 1) * block_size[Axis::ROW]];
      }
    }
  }

  void assign(const std::integral auto& row_block_size, const std::integral auto& column_block_size, const sequential_access_matrix_concept auto& matrix)
  {
    assert(row_block_size != 0);
    assert(column_block_size != 0);
    _data[Axis::ROW]._block_size = row_block_size;
    _data[Axis::COLUMN]._block_size = column_block_size;
    assign(matrix);
  }


public:

  BlockMatrix() = default;
  BlockMatrix(BlockMatrix&&) = default;
  BlockMatrix(const BlockMatrix&) = default;

  BlockMatrix(const std::integral auto& row_block_size, const std::integral auto& column_block_size):
    _data()
  {
    assert(row_block_size != 0);
    assert(column_block_size != 0);
    _data[Axis::ROW]._block_size = row_block_size;
    _data[Axis::COLUMN]._block_size = column_block_size;
  }

  // BlockMatrix(const std::integral auto& row_block_size, const std::integral auto& column_block_size, const std::integral auto& row_dimension, const std::integral auto& column_dimension):
  //   _data()
  // {
  //   assert(row_block_size != 0);
  //   assert(column_block_size != 0);
  //   assert(row_dimension != 0);
  //   assert(column_dimension != 0);
  //   _data[Axis::ROW]._block_size = row_block_size;
  //   _data[Axis::ROW]._block_indices.resize(row_dimension, 0);
  //   _data[Axis::ROW]._positions.resize((row_dimension + 1) * column_block_size, 0);
  //   _data[Axis::COLUMN]._block_size = column_block_size;
  //   _data[Axis::COLUMN]._block_indices.resize(column_dimension, 0);
  //   _data[Axis::COLUMN]._positions.resize((column_dimension + 1) * row_block_size, 0);
  // }

  BlockMatrix(const std::integral auto& row_block_size, const std::integral auto& column_block_size, const sequential_access_matrix_concept auto& matrix):
    BlockMatrix()
  {
    assign(row_block_size, column_block_size, matrix);
  }

  explicit BlockMatrix(const sequential_access_matrix_concept auto& matrix):
    BlockMatrix()
  {
    assign(matrix);
  }

  BlockMatrix& operator=(BlockMatrix&&) = default;
  BlockMatrix& operator=(const BlockMatrix&) = default;

  BlockMatrix& operator=(const sequential_access_matrix_concept auto& matrix)
  {
    assign(matrix);
    return *this;
  }

private:

  template<Axis A>
  index_type dimension() const noexcept
  {
    return _data[A]._block_indices.size();
  }

public:

  decltype(auto) row_dimension() const noexcept
  {
    return dimension<Axis::ROW>();
  }

  decltype(auto) column_dimension() const noexcept
  {
    return dimension<Axis::COLUMN>();
  }

private:

  template<Axis A>
  decltype(auto) vector(const std::integral auto& index) const noexcept
  {
    auto first_position = (_data[A]._positions.size() != 0 ? _data[A]._positions[index * _data[1 - A]._block_size] : 0);
    auto last_position = (_data[A]._positions.size() != 0 ? _data[A]._positions[(index + 1) * _data[1 - A]._block_size] : 0);
    return LATEL::make_VectorView(
      dimension<Axis(1 - A)>(),
      ACCBOOST2::map(
        [&](auto&& p) -> decltype(auto)
        {
          return std::forward_as_tuple(_data[A]._indices[p], _data[A]._values[p]);
        },
        ACCBOOST2::range(first_position, last_position)
      )
    );
  }

public:

  decltype(auto) row(const std::integral auto& row_index) const noexcept
  {
    return vector<Axis::ROW>(row_index);
  }

  decltype(auto) column(const std::integral auto& column_index) const noexcept
  {
    return vector<Axis::COLUMN>(column_index);
  }

  template<Axis A>
  decltype(auto) get_block_index(const std::integral auto& index) const noexcept
  {
    return _data[A]._block_indices[index];
  }

  decltype(auto) get_row_block_index(const std::integral auto& row_index) const noexcept
  {
    return get_block_index<Axis::ROW>(row_index);
  }

  decltype(auto) get_column_block_index(const std::integral auto& column_index) const noexcept
  {
    return get_block_index<Axis::COLUMN>(column_index);
  }

  template<Axis A>
  void set_block_index(const std::integral auto& index, const std::integral auto& block_index)
  {
    auto major_axis = A;
    auto minor_axis = 1 - A;
    std::array<index_type, 2> block_size = {_data[Axis::ROW]._block_size, _data[Axis::COLUMN]._block_size};
    index_type major_index = index;
    assert(index < dimension<A>());
    assert(block_index < block_size[major_axis]);

    if(block_index > _data[major_axis]._block_indices[major_index]){
      for(auto&& major_position: ACCBOOST2::range(
        _data[major_axis]._positions[major_index * block_size[minor_axis]],
        _data[major_axis]._positions[(major_index + 1) * block_size[minor_axis]]
      )){
        assert(major_position < _data[major_axis]._another_positions.size());
        auto&& minor_position = _data[major_axis]._another_positions[major_position];
        auto&& minor_index = _data[major_axis]._indices[major_position];
        assert(_data[minor_axis]._another_positions[minor_position] == major_position);
        assert(_data[minor_axis]._indices[minor_position] == major_index);
        assert(_data[minor_axis]._values[minor_position] == _data[major_axis]._values[major_position]);
        // minor_axis 方向のデータの移動
        value_type minor_value = _data[minor_axis]._values[minor_position];
        auto p = minor_position;
        // for(auto k = 0U; k < _data[major_axis]._block_size; ++k){
        //   std::cerr << _data[minor_axis]._positions[minor_index * block_size[major_axis] + k] << ", ";
        // }
        // std::cerr << std::endl;
        for(auto k = _data[major_axis]._block_indices[major_index]; k < block_index; ++k){
          assert(_data[minor_axis]._positions[minor_index * block_size[major_axis] + k] <= p);
          // k 番目のブロックの末尾位置を取得
          auto q = _data[minor_axis]._positions[minor_index * block_size[major_axis] + k + 1] - 1;
          assert(p <= q);
          assert(q < _data[minor_axis]._indices.size());
          assert(q < _data[minor_axis]._values.size());
          assert(q < _data[minor_axis]._another_positions.size());
          if(p != q){
            // q のデータを p に移動
            auto& r = _data[major_axis]._another_positions[_data[minor_axis]._another_positions[q]];
            assert(r == q);
            r = p;
            _data[minor_axis]._indices[p] = std::move(_data[minor_axis]._indices[q]);
            _data[minor_axis]._values[p] = std::move(_data[minor_axis]._values[q]);
            _data[minor_axis]._another_positions[p] = std::move(_data[minor_axis]._another_positions[q]);
            p = q;
          }
          // k 番目のブロックの末尾位置をデクリメント
          _data[minor_axis]._positions[minor_index * block_size[major_axis] + k + 1] -= 1;
        }
        if(p != minor_position){
          _data[major_axis]._another_positions[major_position] = p;
          _data[minor_axis]._indices[p] = major_index;
          _data[minor_axis]._values[p] = std::move(minor_value);
          _data[minor_axis]._another_positions[p] = major_position;
        }
      }
      _data[major_axis]._block_indices[major_index] = block_index;
    }else if(block_index < _data[major_axis]._block_indices[index]){
      for(auto&& major_position: ACCBOOST2::range(
        _data[major_axis]._positions[index * block_size[minor_axis]],
        _data[major_axis]._positions[(index + 1) * block_size[minor_axis]]
      )){
        auto&& minor_position = _data[major_axis]._another_positions[major_position];
        auto&& minor_index = _data[major_axis]._indices[major_position];
        assert(_data[minor_axis]._another_positions[minor_position] == major_position);
        assert(_data[minor_axis]._indices[minor_position] == major_index);
        assert(_data[minor_axis]._values[minor_position] == _data[major_axis]._values[major_position]);
        // minor_axis 方向のデータの移動
        value_type minor_value = _data[minor_axis]._values[minor_position];
        auto p = minor_position;
        for(auto k = _data[major_axis]._block_indices[major_index]; k > block_index; --k){
          // k 番目のブロックの開始位置を取得
          auto q = _data[minor_axis]._positions[minor_index * block_size[major_axis] + k];
          assert(q <= p);
          if(q != p){
            // q のデータを p に移動
            auto& r = _data[major_axis]._another_positions[_data[minor_axis]._another_positions[q]];
            assert(r == q);
            r = p;
            _data[minor_axis]._indices[p] = std::move(_data[minor_axis]._indices[q]);
            _data[minor_axis]._values[p] = std::move(_data[minor_axis]._values[q]);
            _data[minor_axis]._another_positions[p] = std::move(_data[minor_axis]._another_positions[q]);
            p = q;
          }
          // k 番目のブロックの開始位置をインクリメント
          _data[minor_axis]._positions[minor_index * block_size[major_axis] + k] += 1;
        }
        if(p != minor_position){
          _data[major_axis]._another_positions[major_position] = p;
          _data[minor_axis]._indices[p] = major_index;
          _data[minor_axis]._values[p] = std::move(minor_value);
          _data[minor_axis]._another_positions[p] = major_position;
        }
      }
      _data[major_axis]._block_indices[major_index] = block_index;
    }
  }

  void set_row_block_index(const std::integral auto& index, const std::integral auto& block_index)
  {
    set_block_index<Axis::ROW>(index, block_index);
  }

  void set_column_block_index(const std::integral auto& index, const std::integral auto& block_index)
  {
    set_block_index<Axis::COLUMN>(index, block_index);
  }

// private:

  class SubMatrixView: public RowMatrixRangePolicy<SubMatrixView>
  {
  public:

    using matrix_category = LATEL::bidirectional_matrix_tag;
    using index_type = typename BlockMatrix::index_type;
    using value_type = typename BlockMatrix::value_type;

  private:

    const BlockMatrix& _block_matrix;
    std::array<index_type, 2> _block_index;

  public:

    SubMatrixView(const BlockMatrix& block_matrix, const std::integral auto& row_block_index, const std::integral auto& column_block_index) noexcept:
      _block_matrix(block_matrix), _block_index{static_cast<index_type>(row_block_index), static_cast<index_type>(column_block_index)}
    {}

  private:

    template<Axis A>
    decltype(auto) dimension() const noexcept
    {
      return _block_matrix. template dimension<A>();
    }

  public:

    decltype(auto) row_dimension() const noexcept
    {
      return _block_matrix.row_dimension();
    }

    decltype(auto) column_dimension() const noexcept
    {
      return _block_matrix.column_dimension();
    }

  private:

    template<Axis A>
    decltype(auto) vector(const index_type& index) const noexcept
    {
      assert(index < _block_matrix.dimension<A>());
      std::array<index_type, 2> block_size = {_block_matrix._data[Axis::ROW]._block_size, _block_matrix._data[Axis::COLUMN]._block_size};
      auto first_position = (
        _block_index[A] == _block_matrix._data[A]._block_indices[index]
        ? _block_matrix._data[A]._positions[index * block_size[1 - A] + _block_index[1 - A]]
        : 0
      );
      assert(first_position <= _block_matrix._data[A]._indices.size());
      auto last_position = (
        _block_index[A] == _block_matrix._data[A]._block_indices[index]
        ? _block_matrix._data[A]._positions[index * block_size[1 - A] + _block_index[1 - A] + 1]
        : 0
      );
      assert(last_position <= _block_matrix._data[A]._indices.size());
      assert(first_position <= last_position);
      return LATEL::VectorView(
        _block_matrix. template dimension<static_cast<Axis>(1 - A)>(),
        ACCBOOST2::map(
          [&](auto&& p) -> decltype(auto)
          {
            return std::forward_as_tuple(_block_matrix._data[A]._indices[p], _block_matrix._data[A]._values[p]);
          },
          ACCBOOST2::range(first_position, last_position)
        )
      );
    }

  public:

    decltype(auto) row(const std::integral auto& row_index) const noexcept
    {
      return vector<Axis::ROW>(row_index);
    }

    decltype(auto) column(const std::integral auto& column_index) const noexcept
    {
      return vector<Axis::COLUMN>(column_index);
    }

  };

public:

  decltype(auto) make_sub_matrix_view(const std::integral auto& row_block_index, const std::integral auto& column_block_index) const noexcept
  {
    assert(row_block_index < _data[Axis::ROW]._block_size);
    assert(column_block_index < _data[Axis::COLUMN]._block_size);
    return SubMatrixView(*this, row_block_index, column_block_index);
  }

};


}


#endif

/*
ACCBOOST2::_utility_iterator_make_map_iterator::MapIterator<
  const PROTOPT::LATEL::BlockMatrix<1, 1, long unsigned int, double>::make_iterator(const long unsigned int&) const::<lambda(auto:78&&)>&,
  ACCBOOST2::_utility_iterator_make_integer_iterator::IntegerIterator<long unsigned int>
>::MapIterator(
  ACCBOOST2::_utility_iterator_make_map_iterator::MapIterator<
    PROTOPT::LATEL::BlockMatrix<1, 1, long unsigned int, double>::make_iterator(const long unsigned int&) const::<lambda(auto:78&&)>&,
    ACCBOOST2::_utility_iterator_make_integer_iterator::IntegerIterator<long unsigned int>
  >
)
*/