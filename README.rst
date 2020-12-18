Params14
########

Header-only library that provides keyword arguments with a syntax similar to
Python's keyword arguments.

Installation
------------

Just copy the ``include/params14.h`` header somewhere and points to it using the ``-I`` flag.

Requirements
------------

A C++ compiler that supports C++14 and supports *string literal operator templates* (that's a GNU extension).
It works fine with Clang 6.0.1 and GCC 8.2.1.

Usage
-----

Compiled with ``-std=c++14`` flag:

.. code:: C++

    #include <iostream>

    #include <params14.h>

    using namespace params14::literals;

    template<typename... KWArgs>
    void dump_keywords(std::ostream& stream, KWArgs&&... kwargs) {
        stream << params14::pick("greet"_kw, std::forward<KWArgs>(kwargs)...) << "\n"
               << params14::pick_default("name"_kw, "folks", std::forward<KWArgs>(kwargs)...) << "\n";

    }

    int main(int argc, char** argv) {
        dump_keywords(std::cout, "greet"_kw="Demat",
                                 "name"_kw="d'an holl");
        return 0;
    }


Basically ``params14`` provides three entry points:

- a string literal operator ``_kw`` that requires ``using namespace params14::literals;``. It's used to introduce keywords.
- a way to extract mandatory keywords from a parameter pack: ``params14::pick("some"_kw, kwargs...)``.
- a way to extract optional keywords with a default value from a parameter pack: ``params14::pick_default("some"_kw, default, kwargs...)``.

And that's it!


Credits
-------

Thanks to the people at `Quantstack <https://quantstack.net/>`_ for triggering this dev.  It turns out `pybind11 <https://pybind11.readthedocs.io>`_ uses a similar technique for `their keyword argument handling <https://pybind11.readthedocs.io/en/stable/advanced/pycpp/object.html#keyword-arguments>`_. I wasn't aware of their implementation until I was pointed at that one. So, yea, no rocket science here :-)

Contact
-------

Serge sans Paille ``<serge.guelton@telecom-bretagne.eu>``

