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

namespace params14 {

template<class T, class C, C... cs>
struct NamedParam {
  T value;
};

template<class C, C... cs>
struct Param {
  template<class T>
    NamedParam<T, C, cs...> operator=(T&& val) {
      return {std::forward<T>(val)};
    }
};

template<class C, C... cs>
void pick(Param<C, cs...>) {
  static_assert(C(0), "keyword not found");
}

template<class C, C... cs, class T, class... Args>
T pick(Param<C, cs...>, NamedParam<T, C, cs...> p, Args&&... args) {
  return std::move(p.value);
}

template<class C, C... cs, class T, class Cp, C... csp, class... Args>
auto pick(Param<C, cs...> seed, NamedParam<T, Cp, csp...> p, Args&&... args) {
  return pick(seed, std::forward<Args>(args)...);
}

template<class C, C... cs, class D>
D pick_default(Param<C, cs...>, D&& d) {
  return std::forward<D>(d);
}

template<class C, C... cs, class D, class T, class... Args>
T pick_default(Param<C, cs...>, D&& d, NamedParam<T, C, cs...> p, Args&&... args) {
  return std::move(p.value);
}

template<class C, C... cs, class D, class T, class Cp, C... csp, class... Args>
auto pick_default(Param<C, cs...> seed, D&& d, NamedParam<T, Cp, csp...> p, Args&&... args) {
  return pick_default(seed, std::forward<D>(d), std::forward<Args>(args)...);
}

namespace literals {
template<class C, C... cs>
constexpr Param<C, cs...> operator""_kw() {
  return {};
}

}

}
#endif
