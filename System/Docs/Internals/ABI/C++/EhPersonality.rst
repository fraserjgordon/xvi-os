=============
EhPersonality
=============

The EhPersonality library contains personality routines used for exception
handling in C++ code. These routines are called per stack frame to examine the
exception and determine whether it will be caught in this frame. Additionally,
the personality routine is responsible for setting up cleanup actions during
stack unwinding.

Personality routines are compiler-specific: they depend on how the exception
handling and cleanup code is generated by the compiler.


Itanium ABI
-----------

The Itanium ABI defines the signature for personality routines but neither
specifies the name nor the internals of how the personality routines work. As
such, this library conforms to the Itanium ABI (by exporting personality
routines that conform to the ABI specification) but the routines themselves do
not form part of that ABI.

The language- and personality-specific data for a frame is referred to as the
language-specific data area (LSDA). A pointer to the LSDA forms part of the
unwind information that the unwind library retrieves and the appropriate pointer
is passed to each frame's personality routine so that it can find its data. This
allows one personality routine to handle multiple frames (varying only in the
LSDA pointer) but each frame can also have a different personality routine -- as
long as the Itanium ABI is followed for these routines, exceptions can be thrown
and caught between different languages and runtimes.

This library currently provides personality routines for C and C++ code from
the GCC and Clang compilers.

_Unwind_Personality_Fn
**********************

.. doxygentypedef:: _Unwind_Personality_Fn


GCC/Clang ABI
-------------

The GCC and Clang compilers generate interoperable exception handling code and
language-specific data for C++. This personality routine is also used when
compiling C code with exception support (as compiler extensions allow cleanup
actions to be used in C code). For both compilers, the personality routine is
called ``__gxx_personality_v0`` (as it originated in GCC and was adopted by
Clang as a compatibility measure).

__gxx_personality_v0
********************

This function is an implementation of :cpp:type:`_Unwind_Personality_Fn`.

.. doxygenfunction:: __gxx_personality_v0

Language-specific data
~~~~~~~~~~~~~~~~~~~~~~

The language-specific data for ``__gxx_personality_v0`` consists of three
sub-tables describing call sites, types and actions. Before these, there is a
header describing how to decode the data in the tables.

The data in these tables is encoded using
:ref:`DWARF pointer encodings <Internals/ABI/Dwarf/Encodings:Pointers>`. The
appropriate base addresses for relative pointers can be retrieved from the
unwind context:

    - text-relative: :cpp:func:`_Unwind_GetTextRelBase`
    - data-relative: :cpp:func:`_Unwind_GetDataRelBase`
    - function-relative: :cpp:func:`_Unwind_GetRegionStart`

The base address for function-relative addresses may be overridden by the LSDA
header to use a different base address; the others are fixed.

Header
++++++

The header of the LDSA describes how the other tables are encoded. In order, it
contains:

    - a byte describing the encoding of the next field (it may indicate that the
      field is omitted).
    - an encoded value representing the base address for function-relative
      encoded values. If omitted, the function start address will be used for
      this.
    - a byte describing the encoding of entries in the type table. If it
      indicates that the entries are omitted, there is no type table for this
      function.
    - a |ULEB128|-encoded offset of the type table. The offset is relative to 
      the byte immediately after this encoded offset. If there is no type table,
      this field is not present.
    - a byte describing the encoding of entries in the call site table. The call
      site table is always present so this must not indicate that the entries
      are omitted\ [#callsiteomit]_. This will always be an absolute encoding as
      it is used for sizes as well as addresses.
    - a |ULEB128|-encoded value giving the size of the call site table (in 
      bytes, not the number of entries).

.. [#callsiteomit]      If a function requires no call site entries, it does not
                        catch exceptions nor have cleanup code so the entire
                        LSDA can be omitted.

Immediately after the header is the call site table, the size and encoding of
which were specified by the header.

Call site table
+++++++++++++++

The call site table consists of a number of tuples describing a range of code
and giving the address of the :term:`landingpad` that will handle any actions
required when an exception is thrown from within that code. The encoding for the
contents of this table is given in the LSDA header; each field of the tuple uses
the same encoding. The address fields are implicitly treated as being function-\
relative.

Each entry has the following form:

    - an encoded value giving the relative address of the start of the code
      range.
    - an encoded value giving the size (in bytes) of the code range.
    - an encoded value giving the relative address of the landingpad for this
      range of code.
    - a |ULEB128|-encoded value giving the ID of the first entry in the action
      chain for the code range.

The call site table is sorted by starting address of each code range. As each
entry is variable size\ [#callsiteencoding]_, a linear scan through the table is
needed in order to find the landingpad and action for a particular address.

.. [#callsiteencoding]  This is true regardless of the encoding chosen for the
                        address and size fields: the presence of a ULEB-encoded
                        field makes is variable-length in all cases.

Action table
++++++++++++

The action table begins immediately after the end of the call site table (the
header contains the size of the call site table, making it easy to locate).
Actions in the table are arranged in chains: for each action, there is a
condition to check and, if that condition does not match, the next action in the
chain is consulted. This continues until a condition matches or the chain ends.

Actions are identified by their ID. This ID is the byte offset of the action
entry in the table plus one (leaving ID 0 reserved to indicate an empty action
chain). Call sites that specify action ID 0 as the head of their action chains
have cleanups to perform on unwinding but will not catch any exceptions. The
action ID is significant and is passed to the landingpad to indicate which
action matched the current exception so that it can, for example, jump to the
code for the appropriate catch clause.

Each entry has a pair of values:

    - a |SLEB128|-encoded value giving the type ID of a type to filter against. 
      Whether this is positive, zero or negative determines how the type is
      tested against the exception type.
    - a |SLEB128|-encoded value giving the self-relative (i.e relative to the
      first byte of this encoded offset) offset of the next action in the chain.
      If the offset is zero, it indicates that there are no more entries in the
      chain.

The type filter is interpreted as follows:

    - positive: this type represents the type in a catch clause.
    - negative: this type represents a type in an exception specification.
    - zero: there is no associated type; this is a cleanup action.

In the non-zero cases, the ID is an index into the type table that describes
the corresponding type.
    

Type table
++++++++++

The type table is a table of encoded pointers to :cpp:type:`std::type_info`
objects. Because it is indexed, the only valid encodings for entries in this
table are fixed size encodings (i.e they must not be SLEB/ULEB encodings).

Indexing into the table is slightly unusual -- the address of the table is given
in the LSDA header but this address is in the interior of the table. Positive
(catch clause) type IDs are at negative offsets from this address while negative
(exception specification) type IDs are at positive offsets from this address.

Personality routine behaviour
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Search phase
++++++++++++

Unwind phase
++++++++++++


ARM EHABI
---------

The ARM exception handling ABI (ARM EHABI) uses a different set of personality
routines to the GCC ABI. These routines are only exported for ARM builds of the
library.


XVI extensions
--------------

There are currently no XVI extensions provided by this library.


Glossary
--------

.. glossary::

    landingpad
        A block of code to be executed as part of handling an exception. It may
        be a catch statement (also known as a "catchpad") or some cleanup code
        to execute while unwinding through this function.


.. |SLEB128| replace:: :ref:`SLEB128 <Internal/ABI/Dwarf/Encodings:SLEB128>`

.. |ULEB128| replace:: :ref:`ULEB128 <Internal/ABI/Dwarf/Encodings:ULEB128>`
