/*
 * Params14
 * Copyright 2020 Serge Guelton
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#ifndef PARAMS14_H
#define PARAMS14_H

#include <utility>
#include <tuple>

namespace params14 {

template<class T, class C, C... cs>
struct NamedParam {
  T value;
  static constexpr C keyword[] = {cs..., 0};
};
template<class T, class C, C... cs>
constexpr C NamedParam<T, C, cs...>::keyword[];

template<class C, C... cs>
struct Param {
  template<class T>
    NamedParam<T, C, cs...> operator=(T&& val) {
      return {std::forward<T>(val)};
    }
  static constexpr C keyword[] = {cs..., 0};
};
template<class C, C... cs>
constexpr C Param<C, cs...>::keyword[];

template<class... P>
class KeywordArguments {
  std::tuple<P...> kw_args;

  template<class T, class C, C... cs>
  constexpr void unique_key(NamedParam<T, C, cs...>* key) {
  }

  template<class T, class C, C... cs, class... Args>
  constexpr void unique_key(NamedParam<T, C, cs...>* key, NamedParam<T, C, cs...>* other, Args... args) {
    static_assert(C(0), "keyword specified twice");
  }
  template<class T, class C, C... cs, class Tp, class Cp, Cp... csp, class... Args>
  constexpr void unique_key(NamedParam<T, C, cs...>* key, NamedParam<Tp, Cp, csp...>* other, Args... args) {
    unique_key(key, args...);
  }

  constexpr void unique_keys() {
  }

  template<class T, class C, C... cs, class... Args>
  constexpr void unique_keys(NamedParam<T, C, cs...>* key, Args... args) {
    // FIXME: O(n^2) check, done at compile time
    unique_key(key, args...);
    unique_keys(args...);
  }

  public:
  template<class... Args>
  KeywordArguments(Args&&... args) : kw_args(std::forward<Args>(args)...) {
    unique_keys(static_cast<P*>(nullptr)...);
  }

  class KeySet {
    public:
    template<class C, C... cs>
    static constexpr bool contains(Param<C, cs...> kw) {
      return contains_(kw, static_cast<P*>(nullptr)...);
    }
    private:
    // contains helpers
    template<class C, C... cs>
    static constexpr bool contains_(Param<C, cs...>) {
      return false;
    }

    template<class C, C... cs, class T, class... Args>
    static constexpr bool contains_(Param<C, cs...>, NamedParam<T, C, cs...>*, Args... args) {
      return true;
    }

    template<class C, C... cs, class T, class Cp, C... csp, class... Args>
    static constexpr bool contains_(Param<C, cs...> kw, NamedParam<T, Cp, csp...>*, Args... args) {
      return contains_(kw, args...);
    }
  };

  static constexpr KeySet keys = {};

  constexpr std::size_t size() const { return sizeof...(P); }


  template<class C, C... cs>
  auto get(Param<C, cs...> kw) {
    return fwd_get_simple(kw, std::make_index_sequence<sizeof...(P)>());
  }

  template<class C, C... cs, class D>
  auto get(Param<C, cs...> kw, D&& default_) {
    return fwd_get_default(kw, std::forward<D>(default_), std::make_index_sequence<sizeof...(P)>());
  }

  template<class V>
  auto visit(V&& visitor) {
    return fwd_visit(std::forward<V>(visitor), std::make_index_sequence<sizeof...(P)>());
  }


  protected:

  // get helpers
  template<class C, C... cs>
  void get_simple(Param<C, cs...>) {
    static_assert(C(0), "keyword not found");
  }

  template<class C, C... cs, class T, class... Args>
  auto& get_simple(Param<C, cs...>, NamedParam<T, C, cs...> &p, Args&... args) {
    return p.value;
  }

  template<class C, C... cs, class T, class Cp, C... csp, class... Args>
  auto& get_simple(Param<C, cs...> kw, NamedParam<T, Cp, csp...> &p, Args&... args) {
    return get_simple(kw, args...);
  }
  template<class C, C... cs, std::size_t... Is>
  auto& fwd_get_simple(Param<C, cs...> kw, std::index_sequence<Is...>) {
    return get_simple(kw, std::get<Is>(kw_args)...);
  }

  // get with default helpers
  template<class C, C... cs, class D>
  D get_default(Param<C, cs...>, D&& default_) {
    return std::forward<D>(default_);
  }

  template<class C, C... cs, class D, class T, class... Args>
  auto & get_default(Param<C, cs...>, D&&, NamedParam<T, C, cs...> &p, Args&... args) {
    return p.value;
  }

  template<class C, C... cs, class D, class T, class Cp, C... csp, class... Args>
  auto get_default(Param<C, cs...> kw, D&& default_, NamedParam<T, Cp, csp...> &p, Args&... args) {
    return get_default(kw, std::forward<D>(default_), args...);
  }

  template<class C, C... cs, class D, std::size_t... Is>
  auto fwd_get_default(Param<C, cs...> kw, D&& default_, std::index_sequence<Is...>) {
    return get_default(kw, std::forward<D>(default_), std::get<Is>(kw_args)...);
  }

  // visit helper

  template<class V, std::size_t... Is>
  void fwd_visit(V&& visitor, std::index_sequence<Is...>) {
    std::initializer_list<bool> _ = {(visitor(std::tuple_element<Is, decltype(kw_args)>::type::keyword, std::get<Is>(kw_args).value), true)...};
    (void)_;
  }

};

template<class... KWArgs>
KeywordArguments<KWArgs...> parse(KWArgs&&... kwargs) {
  return {std::forward<KWArgs>(kwargs)...};
}

namespace literals {
template<class C, C... cs>
constexpr Param<C, cs...> operator""_kw() {
  return {};
}

}

}
#endif
