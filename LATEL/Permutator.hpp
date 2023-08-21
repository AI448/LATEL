#ifndef LATEL_PERMUTATOR_HPP_
#define LATEL_PERMUTATOR_HPP_


#include "ACCBOOST2/container.hpp"
#include "common.hpp"


namespace LATEL
{

template<class IndexType>
class Permutator
{
public:

  using permutator_category = LATEL::permutator_tag;
  using index_type = IndexType;

  static constexpr index_type null_index = std::numeric_limits<index_type>::max();

private:

  ACCBOOST2::Array<index_type> _permutation;
  ACCBOOST2::Array<index_type> _unpermutation;

public:

  Permutator() = default;
  Permutator(Permutator&&) = default;
  Permutator(const Permutator&) = default;

  explicit Permutator(const index_type& m, const index_type& n):
    _permutation(), _unpermutation()
  {
    reset(m, n);
  }

  template<permutator_concept PermutatorT>  
  explicit Permutator(const PermutatorT& p):
    Permutator(p.row_dimension())
  {
    for(auto&& i: ACCBOOST2::range(p.row_dimension())){
      set(i, p.permutate(i));
    }
  }

  Permutator& operator=(Permutator&&) = default;

  Permutator& operator=(const Permutator&) = default;

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

  void set(const std::integral auto& from, const std::integral auto& to) noexcept
  {
    assert(from < _permutation.size());
    assert(to < _unpermutation.size());
    index_type t = _permutation[from];
    index_type f = _unpermutation[to];
    _permutation[from] = to;
    _unpermutation[to] = from;
    if(f != null_index){
      _permutation[f] = t;
    }
    if(t != null_index){
      _unpermutation[t] = f;
    }
  }

  void reset(const index_type& m, const index_type& n)
  {
    try{
      _permutation.resize(n);    
      for(auto&& i: ACCBOOST2::range(std::min(m, n))){
        _permutation[i] = i;
      }
      for(auto&& i: ACCBOOST2::range(std::min(m, n), n)){
        _permutation[i] = null_index;
      }
      _unpermutation.resize(m);
      for(auto&& i: ACCBOOST2::range(std::min(m, n))){
        _unpermutation[i] = i;
      }
      for(auto&& i: ACCBOOST2::range(std::min(m, n), m)){
        _unpermutation[i] = null_index;
      }
    }catch(...){
      _permutation.clear();
      _unpermutation.clear();
      throw;
    }
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
