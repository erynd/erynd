// unordered_map standard header

// Copyright (c) Microsoft Corporation.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

// Per Apache License, Version 2.0, Section 4, Point b: I (kynex7510) changed this file.

#ifndef _GEODE_UNORDERED_MAP_
#define _GEODE_UNORDERED_MAP_
#include <yvals_core.h>
#if _STL_COMPILER_PREPROCESSOR
#include "xhash.hpp"

#if _HAS_CXX17
#include <xpolymorphic_allocator.h>
#endif // _HAS_CXX17

#pragma pack(push, _CRT_PACKING)
#pragma warning(push, _STL_WARNING_LEVEL)
#pragma warning(disable : _STL_DISABLED_WARNINGS)
_STL_DISABLE_CLANG_WARNINGS
#pragma push_macro("new")
#undef new

namespace geode::stl {

template <class _Kty, // key type
    class _Ty, // mapped type
    class _Tr, // comparator predicate type
    class _Alloc, // actual _STD allocator type (should be value _STD allocator)
    bool _Mfl> // true if multiple equivalent keys are permitted
class _Umap_traits : public _Tr { // traits required to make _Hash behave like a map
public:
    using key_type            = _Kty;
    using value_type          = _STD pair<const _Kty, _Ty>;
    using _Mutable_value_type = _STD pair<_Kty, _Ty>;
    using key_compare         = _Tr;
    using allocator_type      = _Alloc;
#if _HAS_CXX17
    using node_type = _STD _Node_handle<_STD _List_node<value_type, typename _STD allocator_traits<_Alloc>::void_pointer>, _Alloc,
        _STD _Node_handle_map_base, _Kty, _Ty>;
#endif // _HAS_CXX17

    static constexpr bool _Multi    = _Mfl;
    static constexpr bool _Standard = true;

    template <class... _Args>
    using _In_place_key_extractor = _STD _In_place_key_extract_map<_Kty, _Args...>;

    _Umap_traits() = default;

    explicit _Umap_traits(const _Tr& _Traits) noexcept(_STD is_nothrow_copy_constructible_v<_Tr>) : _Tr(_Traits) {}

    using value_compare = void; // TRANSITION, remove when _Standard becomes unconditionally true

    template <class _Ty1, class _Ty2>
    static const _Kty& _Kfn(const _STD pair<_Ty1, _Ty2>& _Val) noexcept { // extract key from element value
        return _Val.first;
    }

    template <class _Ty1, class _Ty2>
    static const _Ty2& _Nonkfn(const _STD pair<_Ty1, _Ty2>& _Val) noexcept { // extract non-key from element value
        return _Val.second;
    }
};

/*_EXPORT_STD*/ template <class _Kty, class _Ty, class _Hasher = _STD hash<_Kty>, class _Keyeq = _STD equal_to<_Kty>,
    class _Alloc = _STD allocator<_STD pair<const _Kty, _Ty>>>
class unordered_map : public _Hash<_Umap_traits<_Kty, _Ty, _STD _Uhash_compare<_Kty, _Hasher, _Keyeq>, _Alloc, false>> {
    // hash table of {key, mapped} values, unique keys
public:
    static_assert(!_ENFORCE_MATCHING_ALLOCATORS || _STD is_same_v<_STD pair<const _Kty, _Ty>, typename _Alloc::value_type>,
        _MISMATCHED_ALLOCATOR_MESSAGE("unordered_map<Key, Value, Hasher, Eq, Allocator>", "_STD pair<const Key, Value>"));
    static_assert(_STD is_object_v<_Kty>, "The C++ Standard forbids containers of non-object types "
                                     "because of [container.requirements].");

private:
    using _Mytraits      = _STD _Uhash_compare<_Kty, _Hasher, _Keyeq>;
    using _Mybase        = _Hash<_Umap_traits<_Kty, _Ty, _Mytraits, _Alloc, false>>;
    using _Alnode        = typename _Mybase::_Alnode;
    using _Alnode_traits = typename _Mybase::_Alnode_traits;
    using _Nodeptr       = typename _Mybase::_Nodeptr;
    using _Key_compare   = typename _Mybase::_Key_compare;

public:
    using hasher      = _Hasher;
    using key_type    = _Kty;
    using mapped_type = _Ty;
    using key_equal   = _Keyeq;

    using value_type      = _STD pair<const _Kty, _Ty>;
    using allocator_type  = typename _Mybase::allocator_type;
    using size_type       = typename _Mybase::size_type;
    using difference_type = typename _Mybase::difference_type;
    using pointer         = typename _Mybase::pointer;
    using const_pointer   = typename _Mybase::const_pointer;
    using reference       = value_type&;
    using const_reference = const value_type&;
    using iterator        = typename _Mybase::iterator;
    using const_iterator  = typename _Mybase::const_iterator;

    using local_iterator       = typename _Mybase::iterator;
    using const_local_iterator = typename _Mybase::const_iterator;

#if _HAS_CXX17
    using insert_return_type = _STD _Insert_return_type<iterator, typename _Mybase::node_type>;
#endif // _HAS_CXX17

    unordered_map() : _Mybase(_Key_compare(), allocator_type()) {}

    explicit unordered_map(const allocator_type& _Al) : _Mybase(_Key_compare(), _Al) {}

    unordered_map(const unordered_map& _Right)
        : _Mybase(_Right, _Alnode_traits::select_on_container_copy_construction(_Right._Getal())) {}

    unordered_map(const unordered_map& _Right, const allocator_type& _Al) : _Mybase(_Right, _Al) {}

    explicit unordered_map(size_type _Buckets) : _Mybase(_Key_compare(), allocator_type()) {
        _Mybase::rehash(_Buckets);
    }

    unordered_map(size_type _Buckets, const allocator_type& _Al) : _Mybase(_Key_compare(), _Al) {
        _Mybase::rehash(_Buckets);
    }

    unordered_map(size_type _Buckets, const hasher& _Hasharg) : _Mybase(_Key_compare(_Hasharg), allocator_type()) {
        _Mybase::rehash(_Buckets);
    }

    unordered_map(size_type _Buckets, const hasher& _Hasharg, const allocator_type& _Al)
        : _Mybase(_Key_compare(_Hasharg), _Al) {
        _Mybase::rehash(_Buckets);
    }

    unordered_map(size_type _Buckets, const hasher& _Hasharg, const _Keyeq& _Keyeqarg)
        : _Mybase(_Key_compare(_Hasharg, _Keyeqarg), allocator_type()) {
        _Mybase::rehash(_Buckets);
    }

    unordered_map(size_type _Buckets, const hasher& _Hasharg, const _Keyeq& _Keyeqarg, const allocator_type& _Al)
        : _Mybase(_Key_compare(_Hasharg, _Keyeqarg), _Al) {
        _Mybase::rehash(_Buckets);
    }

    template <class _Iter>
    unordered_map(_Iter _First, _Iter _Last) : _Mybase(_Key_compare(), allocator_type()) {
        insert(_First, _Last);
    }

    template <class _Iter>
    unordered_map(_Iter _First, _Iter _Last, const allocator_type& _Al) : _Mybase(_Key_compare(), _Al) {
        insert(_First, _Last);
    }

    template <class _Iter>
    unordered_map(_Iter _First, _Iter _Last, size_type _Buckets) : _Mybase(_Key_compare(), allocator_type()) {
        _Mybase::rehash(_Buckets);
        insert(_First, _Last);
    }

    template <class _Iter>
    unordered_map(_Iter _First, _Iter _Last, size_type _Buckets, const allocator_type& _Al)
        : _Mybase(_Key_compare(), _Al) {
        _Mybase::rehash(_Buckets);
        insert(_First, _Last);
    }

    template <class _Iter>
    unordered_map(_Iter _First, _Iter _Last, size_type _Buckets, const hasher& _Hasharg)
        : _Mybase(_Key_compare(_Hasharg), allocator_type()) {
        _Mybase::rehash(_Buckets);
        insert(_First, _Last);
    }

    template <class _Iter>
    unordered_map(_Iter _First, _Iter _Last, size_type _Buckets, const hasher& _Hasharg, const allocator_type& _Al)
        : _Mybase(_Key_compare(_Hasharg), _Al) {
        _Mybase::rehash(_Buckets);
        insert(_First, _Last);
    }

    template <class _Iter>
    unordered_map(_Iter _First, _Iter _Last, size_type _Buckets, const hasher& _Hasharg, const _Keyeq& _Keyeqarg)
        : _Mybase(_Key_compare(_Hasharg, _Keyeqarg), allocator_type()) {
        _Mybase::rehash(_Buckets);
        insert(_First, _Last);
    }

    template <class _Iter>
    unordered_map(_Iter _First, _Iter _Last, size_type _Buckets, const hasher& _Hasharg, const _Keyeq& _Keyeqarg,
        const allocator_type& _Al)
        : _Mybase(_Key_compare(_Hasharg, _Keyeqarg), _Al) {
        _Mybase::rehash(_Buckets);
        insert(_First, _Last);
    }

#if _HAS_CXX23 && defined(__cpp_lib_concepts) // TRANSITION, GH-395
    template <_STD _Container_compatible_range<value_type> _Rng>
    unordered_map(_STD from_range_t, _Rng&& _Range) : _Mybase(_Key_compare(), allocator_type()) {
        this->_Insert_range_unchecked(_RANGES _Ubegin(_Range), _RANGES _Uend(_Range));
    }

    template <_STD _Container_compatible_range<value_type> _Rng>
    unordered_map(_STD from_range_t, _Rng&& _Range, const allocator_type& _Al) : _Mybase(_Key_compare(), _Al) {
        this->_Insert_range_unchecked(_RANGES _Ubegin(_Range), _RANGES _Uend(_Range));
    }

    template <_STD _Container_compatible_range<value_type> _Rng>
    unordered_map(_STD from_range_t, _Rng&& _Range, size_type _Buckets) : _Mybase(_Key_compare(), allocator_type()) {
        _Mybase::rehash(_Buckets);
        this->_Insert_range_unchecked(_RANGES _Ubegin(_Range), _RANGES _Uend(_Range));
    }

    template <_STD _Container_compatible_range<value_type> _Rng>
    unordered_map(_STD from_range_t, _Rng&& _Range, size_type _Buckets, const allocator_type& _Al)
        : _Mybase(_Key_compare(), _Al) {
        _Mybase::rehash(_Buckets);
        this->_Insert_range_unchecked(_RANGES _Ubegin(_Range), _RANGES _Uend(_Range));
    }

    template <_STD _Container_compatible_range<value_type> _Rng>
    unordered_map(_STD from_range_t, _Rng&& _Range, size_type _Buckets, const hasher& _Hasharg)
        : _Mybase(_Key_compare(_Hasharg), allocator_type()) {
        _Mybase::rehash(_Buckets);
        this->_Insert_range_unchecked(_RANGES _Ubegin(_Range), _RANGES _Uend(_Range));
    }

    template <_STD _Container_compatible_range<value_type> _Rng>
    unordered_map(_STD from_range_t, _Rng&& _Range, size_type _Buckets, const hasher& _Hasharg, const allocator_type& _Al)
        : _Mybase(_Key_compare(_Hasharg), _Al) {
        _Mybase::rehash(_Buckets);
        this->_Insert_range_unchecked(_RANGES _Ubegin(_Range), _RANGES _Uend(_Range));
    }

    template <_STD _Container_compatible_range<value_type> _Rng>
    unordered_map(_STD from_range_t, _Rng&& _Range, size_type _Buckets, const hasher& _Hasharg, const _Keyeq& _Keyeqarg)
        : _Mybase(_Key_compare(_Hasharg, _Keyeqarg), allocator_type()) {
        _Mybase::rehash(_Buckets);
        this->_Insert_range_unchecked(_RANGES _Ubegin(_Range), _RANGES _Uend(_Range));
    }

    template <_STD _Container_compatible_range<value_type> _Rng>
    unordered_map(_STD from_range_t, _Rng&& _Range, size_type _Buckets, const hasher& _Hasharg, const _Keyeq& _Keyeqarg,
        const allocator_type& _Al)
        : _Mybase(_Key_compare(_Hasharg, _Keyeqarg), _Al) {
        _Mybase::rehash(_Buckets);
        this->_Insert_range_unchecked(_RANGES _Ubegin(_Range), _RANGES _Uend(_Range));
    }
#endif // _HAS_CXX23 && defined(__cpp_lib_concepts)

    unordered_map& operator=(const unordered_map& _Right) {
        _Mybase::operator=(_Right);
        return *this;
    }

    unordered_map(unordered_map&& _Right) : _Mybase(_STD move(_Right)) {}

    unordered_map(unordered_map&& _Right, const allocator_type& _Al) : _Mybase(_STD move(_Right), _Al) {}

    unordered_map& operator=(unordered_map&& _Right) noexcept(_Alnode_traits::is_always_equal::value&&
            _STD is_nothrow_move_assignable_v<_Hasher>&& _STD is_nothrow_move_assignable_v<_Keyeq>) {
        _Mybase::operator=(_STD move(_Right));
        return *this;
    }

    mapped_type& operator[](key_type&& _Keyval) {
        return this->_Try_emplace(_STD move(_Keyval)).first->_Myval.second;
    }

    void swap(unordered_map& _Right) noexcept(noexcept(_Mybase::swap(_Right))) {
        _Mybase::swap(_Right);
    }

    using _Mybase::insert;

    template <class _Valty, _STD enable_if_t<_STD is_constructible_v<value_type, _Valty>, int> = 0>
    _STD pair<iterator, bool> insert(_Valty&& _Val) {
        return this->emplace(_STD forward<_Valty>(_Val));
    }

    template <class _Valty, _STD enable_if_t<_STD is_constructible_v<value_type, _Valty>, int> = 0>
    iterator insert(const_iterator _Where, _Valty&& _Val) {
        return this->emplace_hint(_Where, _STD forward<_Valty>(_Val));
    }

    template <class... _Mappedty>
    _STD pair<iterator, bool> try_emplace(const key_type& _Keyval, _Mappedty&&... _Mapval) {
        const auto _Result = this->_Try_emplace(_Keyval, _STD forward<_Mappedty>(_Mapval)...);
        return {this->_List._Make_iter(_Result.first), _Result.second};
    }

    template <class... _Mappedty>
    _STD pair<iterator, bool> try_emplace(key_type&& _Keyval, _Mappedty&&... _Mapval) {
        const auto _Result = this->_Try_emplace(_STD move(_Keyval), _STD forward<_Mappedty>(_Mapval)...);
        return {this->_List._Make_iter(_Result.first), _Result.second};
    }

    template <class... _Mappedty>
    iterator try_emplace(const const_iterator _Hint, const key_type& _Keyval, _Mappedty&&... _Mapval) {
        return this->_List._Make_iter(
            this->_Try_emplace_hint(_Hint._Ptr, _Keyval, _STD forward<_Mappedty>(_Mapval)...));
    }

    template <class... _Mappedty>
    iterator try_emplace(const const_iterator _Hint, key_type&& _Keyval, _Mappedty&&... _Mapval) {
        return this->_List._Make_iter(
            this->_Try_emplace_hint(_Hint._Ptr, _STD move(_Keyval), _STD forward<_Mappedty>(_Mapval)...));
    }

private:
    template <class _Keyty, class _Mappedty>
    _STD pair<iterator, bool> _Insert_or_assign(_Keyty&& _Keyval_arg, _Mappedty&& _Mapval) {
        const auto& _Keyval   = _Keyval_arg;
        const size_t _Hashval = this->_Traitsobj(_Keyval);
        auto _Target          = this->_Find_last(_Keyval, _Hashval);
        if (_Target._Duplicate) {
            _Target._Duplicate->_Myval.second = _STD forward<_Mappedty>(_Mapval);
            return {this->_List._Make_iter(_Target._Duplicate), false};
        }

        this->_Check_max_size();
        // invalidates _Keyval:
        _STD _List_node_emplace_op2<_Alnode> _Newnode(
            this->_Getal(), _STD forward<_Keyty>(_Keyval_arg), _STD forward<_Mappedty>(_Mapval));
        if (this->_Check_rehash_required_1()) {
            this->_Rehash_for_1();
            _Target = this->_Find_last(_Newnode._Ptr->_Myval.first, _Hashval);
        }

        return {this->_List._Make_iter(
                    this->_Insert_new_node_before(_Hashval, _Target._Insert_before, _Newnode._Release())),
            true};
    }

    template <class _Keyty, class _Mappedty>
    iterator _Insert_or_assign(const _Nodeptr _Hint, _Keyty&& _Keyval_arg, _Mappedty&& _Mapval) {
        const auto& _Keyval   = _Keyval_arg;
        const size_t _Hashval = this->_Traitsobj(_Keyval);
        auto _Target          = this->_Find_hint(_Hint, _Keyval, _Hashval);
        if (_Target._Duplicate) {
            _Target._Duplicate->_Myval.second = _STD forward<_Mappedty>(_Mapval);
            return this->_List._Make_iter(_Target._Duplicate);
        }

        this->_Check_max_size();
        // invalidates _Keyval:
        _STD _List_node_emplace_op2<_Alnode> _Newnode(
            this->_Getal(), _STD forward<_Keyty>(_Keyval_arg), _STD forward<_Mappedty>(_Mapval));
        if (this->_Check_rehash_required_1()) {
            this->_Rehash_for_1();
            _Target = this->_Find_hint(_Hint, _Newnode._Ptr->_Myval.first, _Hashval);
        }

        return this->_List._Make_iter(
            this->_Insert_new_node_before(_Hashval, _Target._Insert_before, _Newnode._Release()));
    }

public:
    template <class _Mappedty>
    _STD pair<iterator, bool> insert_or_assign(const key_type& _Keyval, _Mappedty&& _Mapval) {
        return _Insert_or_assign(_Keyval, _STD forward<_Mappedty>(_Mapval));
    }

    template <class _Mappedty>
    _STD pair<iterator, bool> insert_or_assign(key_type&& _Keyval, _Mappedty&& _Mapval) {
        return _Insert_or_assign(_STD move(_Keyval), _STD forward<_Mappedty>(_Mapval));
    }

    template <class _Mappedty>
    iterator insert_or_assign(const_iterator _Hint, const key_type& _Keyval, _Mappedty&& _Mapval) {
        return _Insert_or_assign(_Hint._Ptr, _Keyval, _STD forward<_Mappedty>(_Mapval));
    }

    template <class _Mappedty>
    iterator insert_or_assign(const_iterator _Hint, key_type&& _Keyval, _Mappedty&& _Mapval) {
        return _Insert_or_assign(_Hint._Ptr, _STD move(_Keyval), _STD forward<_Mappedty>(_Mapval));
    }

    unordered_map(_STD initializer_list<value_type> _Ilist) : _Mybase(_Key_compare(), allocator_type()) {
        insert(_Ilist);
    }

    unordered_map(_STD initializer_list<value_type> _Ilist, const allocator_type& _Al) : _Mybase(_Key_compare(), _Al) {
        insert(_Ilist);
    }

    unordered_map(_STD initializer_list<value_type> _Ilist, size_type _Buckets) : _Mybase(_Key_compare(), allocator_type()) {
        _Mybase::rehash(_Buckets);
        insert(_Ilist);
    }

    unordered_map(_STD initializer_list<value_type> _Ilist, size_type _Buckets, const allocator_type& _Al)
        : _Mybase(_Key_compare(), _Al) {
        _Mybase::rehash(_Buckets);
        insert(_Ilist);
    }

    unordered_map(_STD initializer_list<value_type> _Ilist, size_type _Buckets, const hasher& _Hasharg)
        : _Mybase(_Key_compare(_Hasharg), allocator_type()) {
        _Mybase::rehash(_Buckets);
        insert(_Ilist);
    }

    unordered_map(
        _STD initializer_list<value_type> _Ilist, size_type _Buckets, const hasher& _Hasharg, const allocator_type& _Al)
        : _Mybase(_Key_compare(_Hasharg), _Al) {
        _Mybase::rehash(_Buckets);
        insert(_Ilist);
    }

    unordered_map(
        _STD initializer_list<value_type> _Ilist, size_type _Buckets, const hasher& _Hasharg, const _Keyeq& _Keyeqarg)
        : _Mybase(_Key_compare(_Hasharg, _Keyeqarg), allocator_type()) {
        _Mybase::rehash(_Buckets);
        insert(_Ilist);
    }

    unordered_map(_STD initializer_list<value_type> _Ilist, size_type _Buckets, const hasher& _Hasharg,
        const _Keyeq& _Keyeqarg, const allocator_type& _Al)
        : _Mybase(_Key_compare(_Hasharg, _Keyeqarg), _Al) {
        _Mybase::rehash(_Buckets);
        insert(_Ilist);
    }

    unordered_map& operator=(_STD initializer_list<value_type> _Ilist) {
        _Mybase::clear();
        insert(_Ilist);
        return *this;
    }

    _NODISCARD hasher hash_function() const {
        return _Mybase::_Traitsobj._Mypair._Get_first();
    }

    _NODISCARD key_equal key_eq() const {
        return _Mybase::_Traitsobj._Mypair._Myval2._Get_first();
    }

    mapped_type& operator[](const key_type& _Keyval) {
        return this->_Try_emplace(_Keyval).first->_Myval.second;
    }

    _NODISCARD mapped_type& at(const key_type& _Keyval) {
        const auto _Target = this->_Find_last(_Keyval, this->_Traitsobj(_Keyval));
        if (_Target._Duplicate) {
            return _Target._Duplicate->_Myval.second;
        }

        _STD _Xout_of_range("invalid unordered_map<K, T> key");
    }

    _NODISCARD const mapped_type& at(const key_type& _Keyval) const {
        const auto _Target = this->_Find_last(_Keyval, this->_Traitsobj(_Keyval));
        if (_Target._Duplicate) {
            return _Target._Duplicate->_Myval.second;
        }

        _STD _Xout_of_range("invalid unordered_map<K, T> key");
    }

    using _Mybase::_Unchecked_begin;
    using _Mybase::_Unchecked_end;
};

#if _HAS_CXX17
template <class _Iter, class _Hasher = _STD hash<_STD _Guide_key_t<_Iter>>, class _Keyeq = _STD equal_to<_STD _Guide_key_t<_Iter>>,
    class _Alloc = _STD allocator<_STD _Guide_pair_t<_Iter>>,
    _STD enable_if_t<
        _STD conjunction_v<_STD _Is_iterator<_Iter>, _Is_hasher<_Hasher>, _STD negation<_STD _Is_allocator<_Keyeq>>, _STD _Is_allocator<_Alloc>>,
        int> = 0>
unordered_map(_Iter, _Iter, _STD _Guide_size_type_t<_Alloc> = 0, _Hasher = _Hasher(), _Keyeq = _Keyeq(), _Alloc = _Alloc())
    -> unordered_map<_STD _Guide_key_t<_Iter>, _STD _Guide_val_t<_Iter>, _Hasher, _Keyeq, _Alloc>;

template <class _Kty, class _Ty, class _Hasher = _STD hash<_Kty>, class _Keyeq = _STD equal_to<_Kty>,
    class _Alloc = _STD allocator<_STD pair<const _Kty, _Ty>>,
    _STD enable_if_t<_STD conjunction_v<_Is_hasher<_Hasher>, _STD negation<_STD _Is_allocator<_Keyeq>>, _STD _Is_allocator<_Alloc>>, int> = 0>
unordered_map(_STD initializer_list<_STD pair<_Kty, _Ty>>, _STD _Guide_size_type_t<_Alloc> = 0, _Hasher = _Hasher(), _Keyeq = _Keyeq(),
    _Alloc = _Alloc()) -> unordered_map<_Kty, _Ty, _Hasher, _Keyeq, _Alloc>;

template <class _Iter, class _Alloc, _STD enable_if_t<_STD conjunction_v<_STD _Is_iterator<_Iter>, _STD _Is_allocator<_Alloc>>, int> = 0>
unordered_map(_Iter, _Iter, _Alloc) -> unordered_map<_STD _Guide_key_t<_Iter>, _STD _Guide_val_t<_Iter>,
    _STD hash<_STD _Guide_key_t<_Iter>>, _STD equal_to<_STD _Guide_key_t<_Iter>>, _Alloc>;

template <class _Iter, class _Alloc, _STD enable_if_t<_STD conjunction_v<_STD _Is_iterator<_Iter>, _STD _Is_allocator<_Alloc>>, int> = 0>
unordered_map(_Iter, _Iter, _STD _Guide_size_type_t<_Alloc>, _Alloc) -> unordered_map<_STD _Guide_key_t<_Iter>,
    _STD _Guide_val_t<_Iter>, _STD hash<_STD _Guide_key_t<_Iter>>, _STD equal_to<_STD _Guide_key_t<_Iter>>, _Alloc>;

template <class _Iter, class _Hasher, class _Alloc,
    _STD enable_if_t<_STD conjunction_v<_STD _Is_iterator<_Iter>, _Is_hasher<_Hasher>, _STD _Is_allocator<_Alloc>>, int> = 0>
unordered_map(_Iter, _Iter, _STD _Guide_size_type_t<_Alloc>, _Hasher, _Alloc)
    -> unordered_map<_STD _Guide_key_t<_Iter>, _STD _Guide_val_t<_Iter>, _Hasher, _STD equal_to<_STD _Guide_key_t<_Iter>>, _Alloc>;

template <class _Kty, class _Ty, class _Alloc, _STD enable_if_t<_STD _Is_allocator<_Alloc>::value, int> = 0>
unordered_map(_STD initializer_list<_STD pair<_Kty, _Ty>>, _Alloc)
    -> unordered_map<_Kty, _Ty, _STD hash<_Kty>, _STD equal_to<_Kty>, _Alloc>;

template <class _Kty, class _Ty, class _Alloc, _STD enable_if_t<_STD _Is_allocator<_Alloc>::value, int> = 0>
unordered_map(_STD initializer_list<_STD pair<_Kty, _Ty>>, _STD _Guide_size_type_t<_Alloc>, _Alloc)
    -> unordered_map<_Kty, _Ty, _STD hash<_Kty>, _STD equal_to<_Kty>, _Alloc>;

template <class _Kty, class _Ty, class _Hasher, class _Alloc,
    _STD enable_if_t<_STD conjunction_v<_Is_hasher<_Hasher>, _STD _Is_allocator<_Alloc>>, int> = 0>
unordered_map(_STD initializer_list<_STD pair<_Kty, _Ty>>, _STD _Guide_size_type_t<_Alloc>, _Hasher, _Alloc)
    -> unordered_map<_Kty, _Ty, _Hasher, _STD equal_to<_Kty>, _Alloc>;

#if _HAS_CXX23 && defined(__cpp_lib_concepts) // TRANSITION, GH-395
template <_RANGES input_range _Rng, class _Hasher = _STD hash<_STD _Range_key_type<_Rng>>,
    class _Keyeq = _STD equal_to<_STD _Range_key_type<_Rng>>, class _Alloc = _STD allocator<_STD _Range_to_alloc_type<_Rng>>,
    _STD enable_if_t<_STD conjunction_v<_Is_hasher<_Hasher>, _STD negation<_STD _Is_allocator<_Keyeq>>, _STD _Is_allocator<_Alloc>>, int> = 0>
unordered_map(_STD from_range_t, _Rng&&, _STD _Guide_size_type_t<_Alloc> = 0, _Hasher = _Hasher(), _Keyeq = _Keyeq(),
    _Alloc = _Alloc()) -> unordered_map<_STD _Range_key_type<_Rng>, _STD _Range_mapped_type<_Rng>, _Hasher, _Keyeq, _Alloc>;

template <_RANGES input_range _Rng, class _Alloc, _STD enable_if_t<_STD _Is_allocator<_Alloc>::value, int> = 0>
unordered_map(_STD from_range_t, _Rng&&, _STD _Guide_size_type_t<_Alloc>, _Alloc) -> unordered_map<_STD _Range_key_type<_Rng>,
    _STD _Range_mapped_type<_Rng>, _STD hash<_STD _Range_key_type<_Rng>>, _STD equal_to<_STD _Range_key_type<_Rng>>, _Alloc>;

template <_RANGES input_range _Rng, class _Alloc, _STD enable_if_t<_STD _Is_allocator<_Alloc>::value, int> = 0>
unordered_map(_STD from_range_t, _Rng&&, _Alloc) -> unordered_map<_STD _Range_key_type<_Rng>, _STD _Range_mapped_type<_Rng>,
    _STD hash<_STD _Range_key_type<_Rng>>, _STD equal_to<_STD _Range_key_type<_Rng>>, _Alloc>;

template <_RANGES input_range _Rng, class _Hasher, class _Alloc,
    _STD enable_if_t<_STD conjunction_v<_Is_hasher<_Hasher>, _STD _Is_allocator<_Alloc>>, int> = 0>
unordered_map(_STD from_range_t, _Rng&&, _STD _Guide_size_type_t<_Alloc>, _Hasher, _Alloc)
    -> unordered_map<_STD _Range_key_type<_Rng>, _STD _Range_mapped_type<_Rng>, _Hasher, _STD equal_to<_STD _Range_key_type<_Rng>>, _Alloc>;
#endif // _HAS_CXX23 && defined(__cpp_lib_concepts)
#endif // _HAS_CXX17

/*_EXPORT_STD*/ template <class _Kty, class _Ty, class _Hasher, class _Keyeq, class _Alloc>
void swap(unordered_map<_Kty, _Ty, _Hasher, _Keyeq, _Alloc>& _Left,
    unordered_map<_Kty, _Ty, _Hasher, _Keyeq, _Alloc>& _Right) noexcept(noexcept(_Left.swap(_Right))) {
    _Left.swap(_Right);
}

#if _HAS_CXX20
/*_EXPORT_STD*/ template <class _Kty, class _Ty, class _Hasher, class _Keyeq, class _Alloc, class _Pr>
unordered_map<_Kty, _Ty, _Hasher, _Keyeq, _Alloc>::size_type erase_if(
    unordered_map<_Kty, _Ty, _Hasher, _Keyeq, _Alloc>& _Cont, _Pr _Pred) {
    return _STD _Erase_nodes_if(_Cont, _STD _Pass_fn(_Pred));
}
#endif // _HAS_CXX20

/*_EXPORT_STD*/ template <class _Kty, class _Ty, class _Hasher, class _Keyeq, class _Alloc>
_NODISCARD bool operator==(const unordered_map<_Kty, _Ty, _Hasher, _Keyeq, _Alloc>& _Left,
    const unordered_map<_Kty, _Ty, _Hasher, _Keyeq, _Alloc>& _Right) {
    return ::geode::stl:: _Hash_equal(_Left, _Right);
}

#if !_HAS_CXX20
template <class _Kty, class _Ty, class _Hasher, class _Keyeq, class _Alloc>
_NODISCARD bool operator!=(const unordered_map<_Kty, _Ty, _Hasher, _Keyeq, _Alloc>& _Left,
    const unordered_map<_Kty, _Ty, _Hasher, _Keyeq, _Alloc>& _Right) {
    return !(_Left == _Right);
}
#endif // !_HAS_CXX20

_STD_END
#pragma pop_macro("new")
_STL_RESTORE_CLANG_WARNINGS
#pragma warning(pop)
#pragma pack(pop)
#endif // _STL_COMPILER_PREPROCESSOR
#endif // _GEODE_UNORDERED_MAP_
