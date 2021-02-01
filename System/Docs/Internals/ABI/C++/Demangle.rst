========
Demangle
========

The Demangle library provides facilities for demangling C++ symbol and type
names.


Itanium ABI
-----------

The Itanium ABI requires a general purpose demangling function to exist. It
takes a name that could be either a type name (e.g. from a ``std::type_info``
object) or a symbol and attempts to demangle it. Because of this dual purpose,
it is not possible for it to reliably distinguish between a mangled type name or
an unmangled symbol. For example, the string ``"i"`` could either be a global
variable of name ``i`` or the type ``int``; this function will always demangle
it as the latter.

__cxa_demangle
**************

.. doxygenfunction:: __cxxabiv1::__cxa_demangle


XVI extensions
--------------

As a non-standard extension, two more functions are provided. These are broadly
similar to ``__cxa_demangle`` but have been split in order to remove the
type-vs-symbol ambiguity.

System.ABI.C++.DemangleSymbol
*****************************

.. doxygenfunction:: System::ABI::Cxx::DemangleSymbol

System.ABI.C++.DemangleType
***************************

.. doxygenfunction:: System::ABI::Cxx::DemangleType
