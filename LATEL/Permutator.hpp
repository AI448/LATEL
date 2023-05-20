#ifndef LATEL_PERMUTATOR_HPP_
#define LATEL_PERMUTATOR_HPP_


#include "ACCBOOST2/container.hpp"
#include "foundations.hpp"


namespace LATEL
{

template<class IndexT>
class Permutator
{
public:

  using index_type = IndexT;

private:

  ACCBOOST2::Array<index_type> _permutation;
  ACCBOOST2::Array<index_type> _unpermutation;

public:

  Permutator() = default;
  Permutator(Permutator&&) = default;
  Permutator(const Permutator&) = default;

  template<std::integral D>
  explicit Permutator(const D& m):
    _permutation(m), _unpermutation(m)
  {
    assert(m <= std::numeric_limits<index_type>::max());

    for(auto&& i: ACCBOOST2::range(m)){
      _permutation[i] = i;
      _unpermutation[i] = i;
    }
  }

  template<permutator_concept PermutatorT>  
  explicit Permutator(const PermutatorT& p):
    Permutator(p.row_dimension())
  {
    for(auto&& i: ACCBOOST2::range(p.row_dimension())){
      set(i, p.permutate(i));
    }
  }

  decltype(auto) row_dimension() const noexcept
  {
    return _unpermutation.size();
  }

  decltype(auto) column_dimension() const noexcept
  {
    return _permutation.size();
  }

  template<std::integral I>
  decltype(auto) permutate(const I& from) const noexcept
  {
    assert(from < _permutation.size());
    return _permutation[from];
  }

  template<std::integral I>
  decltype(auto) unpermutate(const I& to) const noexcept
  {
    assert(to < _unpermutation.size());
    return _unpermutation[to];
  }

  template<std::integral I, std::integral J>
  void set(const I& from, const J& to) noexcept
  {
    assert(from < _permutation.size());
    assert(to < _unpermutation.size());
    index_type t = _permutation[from];
    index_type f = _unpermutation[to];
    _permutation[from] = to;
    _unpermutation[to] = from;
    _permutation[f] = t;
    _unpermutation[t] = f;
  }

  decltype(auto) begin() const noexcept
  {
    return ACCBOOST2::make_random_access_enumerate_iterator(0, _permutation.begin());
  }

  decltype(auto) end() const noexcept
  {
    return ACCBOOST2::make_random_access_enumerate_iterator(_permutation.size(), _permutation.end());
  }

  decltype(auto) size() const noexcept
  {
    return _permutation.size();
  }

};



}


#endif
