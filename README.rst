Params14
########

Header-only library that provides keyword arguments with a syntax similar to
Python's keyword arguments.

Installation
------------

Just copy the ``include/params14.hpp`` header somewhere and points to it using the ``-I`` flag.

Requirements
------------

A C++ compiler that supports C++14 and supports *string literal operator templates* (that's a GNU extension).
It works fine with Clang 6.0.1 and GCC 8.2.1.

Usage
-----

Compiled with ``-std=c++14`` flag:

.. code:: C++

    #include <iostream>

    #include "params14.hpp"

    using namespace params14::literals;

    template<typename... KWArgs>
    void greet(std::ostream& stream, KWArgs&&... kwargs) {
      // Initialize the keyword dict. static assert in case of double arguments
      auto args = params14::parse(std::forward<KWArgs>(kwargs)...);

      // some sanity checks
      static_assert(args.keys.contains("greet"_kw), "ok");
      static_assert(!args.keys.contains("gree"_kw), "ok");

      // getter, with or without default
      stream << args.get("greet"_kw) << "\n"
             << args.get("name"_kw, "folks") << "\n";

      // visitor
      args.visit([&stream](char const kw[], auto const& value) {
                   stream << kw << ": " << value << "\n";});

    }

    int main(int argc, char** argv) {
        greet(std::cout, "greet"_kw="Demat",
                         "name"_kw="d'an holl");
        return 0;
    }


Basically ``params14`` provides five entry point:

- a string literal operator ``_kw`` that requires ``using namespace params14::literals;``. It's used to introduce keywords.
- a way to parse arguments into a keyword-value dictionnary-like structure, ``KeywordArguments<...> params14::parse(...)``
- a way to get value from that structure, with or without default value: ``KeywordArguments<...>::get(keyword [, default_value])``.
- a generic way to visit keyword-value pairs: ``KeywordArguments<...>::visit(visitor)``.

And that's it!


Credits
-------

Thanks to the people at `Quantstack <https://quantstack.net/>`_ for triggering this dev.  It turns out `pybind11 <https://pybind11.readthedocs.io>`_ uses a similar technique for `their keyword argument handling <https://pybind11.readthedocs.io/en/stable/advanced/pycpp/object.html#keyword-arguments>`_. I wasn't aware of their implementation until I was pointed at that one. So, yea, no rocket science here :-)

Contact
-------

Serge sans Paille ``<serge.guelton@telecom-bretagne.eu>``

