llvm-objcopy - object copying and editing tool
==============================================

.. program:: llvm-objcopy

SYNOPSIS
--------

:program:`llvm-objcopy` [*options*] *input* [*output*]

DESCRIPTION
-----------

:program:`llvm-objcopy` is a tool to copy and manipulate objects. In basic
usage, it makes a semantic copy of the input to the output. If any options are
specified, the output may be modified along the way, e.g. by removing sections.

If no output file is specified, the input file is modified in-place. If "-" is
specified for the input file, the input is read from the program's standard
input stream. If "-" is specified for the output file, the output is written to
the standard output stream of the program.

If the input is an archive, any requested operations will be applied to each
archive member individually.

The tool is still in active development, but in most scenarios it works as a
drop-in replacement for GNU's :program:`objcopy`.

GENERIC AND CROSS-PLATFORM OPTIONS
----------------------------------

The following options are either agnostic of the file format, or apply to
multiple file formats.

.. option:: --add-gnu-debuglink <debug-file>

 Add a .gnu_debuglink section for ``<debug-file>`` to the output.

.. option:: --add-section <section=file>

 Add a section named ``<section>`` with the contents of ``<file>`` to the
 output. For ELF objects the section will be of type `SHT_NOTE`, if the name
 starts with ".note". Otherwise, it will have type `SHT_PROGBITS`. Can be
 specified multiple times to add multiple sections.

.. option:: --binary-architecture <arch>, -B

 Ignored for compatibility.

.. option:: --disable-deterministic-archives, -U

 Use real values for UIDs, GIDs and timestamps when updating archive member
 headers.

.. option:: --discard-all, -x

 Remove most local symbols from the output. Different file formats may limit
 this to a subset of the local symbols. For example, file and section symbols in
 ELF objects will not be discarded.

.. option:: --enable-deterministic-archives, -D

 Enable deterministic mode when copying archives, i.e. use 0 for archive member
 header UIDs, GIDs and timestamp fields. On by default.

.. option:: --help, -h

 Print a summary of command line options.

.. option:: --only-section <section>, -j

 Remove all sections from the output, except for sections named ``<section>``.
 Can be specified multiple times to keep multiple sections.

.. option:: --regex

 If specified, symbol and section names specified by other switches are treated
 as extended POSIX regular expression patterns.

.. option:: --remove-section <section>, -R

 Remove the specified section from the output. Can be specified multiple times
 to remove multiple sections simultaneously.

.. option:: --strip-all-gnu

 Remove all symbols, debug sections and relocations from the output. This option
 is equivalent to GNU :program:`objcopy`'s ``--strip-all`` switch.

.. option:: --strip-all, -S

 For ELF objects, remove from the output all symbols and non-alloc sections not
 within segments, except for .gnu.warning sections and the section name table.

 For COFF objects, remove all symbols, debug sections, and relocations from the
 output.

.. option:: --strip-debug, -g

 Remove all debug sections from the output.

.. option:: --strip-symbol <symbol>, -N

 Remove all symbols named ``<symbol>`` from the output. Can be specified
 multiple times to remove multiple symbols.

.. option:: --strip-symbols <filename>

 Remove all symbols whose names appear in the file ``<filename>``, from the
 output. In the file, each line represents a single symbol name, with leading
 and trailing whitespace ignored, as is anything following a '#'. Can be
 specified multiple times to read names from multiple files.

.. option:: --strip-unneeded-symbol <symbol>

 Remove from the output all symbols named ``<symbol>`` that are local or
 undefined and are not required by any relocation.

.. option:: --strip-unneeded-symbols <filename>

 Remove all symbols whose names appear in the file ``<filename>``, from the
 output, if they are local or undefined and are not required by any relocation.
 In the file, each line represents a single symbol name, with leading and
 trailing whitespace ignored, as is anything following a '#'. Can be specified
 multiple times to read names from multiple files.

.. option:: --strip-unneeded

 Remove from the output all local or undefined symbols that are not required by
 relocations. Also remove all debug sections.

.. option:: --version, -V

  Display the version of this program.

.. option:: @<FILE>

  Read command-line options and commands from response file `<FILE>`.

COFF-SPECIFIC OPTIONS
---------------------

The following options are implemented only for COFF objects. If used with other
objects, :program:`llvm-objcopy` will either emit an error or silently ignore
them.

.. option:: --only-keep-debug

 Remove the contents of non-debug sections from the output, but keep the section
 headers.

ELF-SPECIFIC OPTIONS
--------------------

The following options are implemented only for ELF objects. If used with other
objects, :program:`llvm-objcopy` will either emit an error or silently ignore
them.

.. option:: --add-symbol <name>=[<section>:]<value>[,<flags>]

 Add a new symbol called ``<name>`` to the output symbol table, in the section
 named ``<section>``, with value ``<value>``. If ``<section>`` is not specified,
 the symbol is added as an absolute symbol. The ``<flags>`` affect the symbol
 properties. Accepted values are:

 - `global` = the symbol will have global binding.
 - `local` = the symbol will have local binding.
 - `weak` = the symbol will have weak binding.
 - `default` = the symbol will have default visibility.
 - `hidden` = the symbol will have hidden visibility.
 - `protected` = the symbol will have protected visibility.
 - `file` = the symbol will be an `STT_FILE` symbol.
 - `section` = the symbol will be an `STT_SECTION` symbol.
 - `object` = the symbol will be an `STT_OBJECT` symbol.
 - `function` = the symbol will be an `STT_FUNC` symbol.
 - `indirect-function` = the symbol will be an `STT_GNU_IFUNC` symbol.

 Additionally, the following flags are accepted but ignored: `debug`,
 `constructor`, `warning`, `indirect`, `synthetic`, `unique-object`, `before`.

 Can be specified multiple times to add multiple symbols.

.. option:: --allow-broken-links

 Allow llvm-objcopy to remove sections even if it would leave invalid section
 references. Any invalid sh_link fields will be set to zero.

.. option:: --build-id-link-dir <dir>

 Set the directory used by :option:`--build-id-link-input` and
 :option:`--build-id-link-output`.

.. option:: --build-id-link-input <suffix>

 Hard-link the input to ``<dir>/xx/xxx<suffix>``, where ``<dir>`` is the directory
 specified by :option:`--build-id-link-dir`. The path used is derived from the
 hex build ID.

.. option:: --build-id-link-output <suffix>

 Hard-link the output to ``<dir>/xx/xxx<suffix>``, where ``<dir>`` is the directory
 specified by :option:`--build-id-link-dir`. The path used is derived from the
 hex build ID.

.. option:: --change-start <incr>, --adjust-start

 Add ``<incr>`` to the program's start address. Can be specified multiple
 times, in which case the values will be applied cumulatively.

.. option:: --compress-debug-sections [<style>]

 Compress DWARF debug sections in the output, using the specified style.
 Supported styles are `zlib-gnu` and `zlib`. Defaults to `zlib` if no style is
 specified.

.. option:: --decompress-debug-sections

 Decompress any compressed DWARF debug sections in the output.

.. option:: --discard-locals, -X

 Remove local symbols starting with ".L" from the output.

.. option:: --dump-section <section>=<file>

 Dump the contents of section ``<section>`` into the file ``<file>``. Can be
 specified multiple times to dump multiple sections to different files.
 ``<file>`` is unrelated to the input and output files provided to
 :program:`llvm-objcopy` and as such the normal copying and editing
 operations will still be performed. No operations are performed on the sections
 prior to dumping them.

.. option:: --extract-dwo

 Remove all sections that are not DWARF .dwo sections from the output.

.. option:: --extract-main-partition

 Extract the main partition from the output.

.. option:: --extract-partition <name>

 Extract the named partition from the output.

.. option:: --globalize-symbol <symbol>

 Mark any defined symbols named ``<symbol>`` as global symbols in the output.
 Can be specified multiple times to mark multiple symbols.

.. option:: --globalize-symbols <filename>

 Read a list of names from the file ``<filename>`` and mark defined symbols with
 those names as global in the output. In the file, each line represents a single
 symbol, with leading and trailing whitespace ignored, as is anything following
 a '#'. Can be specified multiple times to read names from multiple files.

.. option:: --input-target <format>, -I

 Read the input as the specified format. See `SUPPORTED FORMATS`_ for a list of
 valid ``<format>`` values. If unspecified, :program:`llvm-objcopy` will attempt
 to determine the format automatically.

.. option:: --keep-file-symbols

 Keep symbols of type `STT_FILE`, even if they would otherwise be stripped.

.. option:: --keep-global-symbol <symbol>

 Make all symbols local in the output, except for symbols with the name
 ``<symbol>``. Can be specified multiple times to ignore multiple symbols.

.. option:: --keep-global-symbols <filename>

 Make all symbols local in the output, except for symbols named in the file
 ``<filename>``. In the file, each line represents a single symbol, with leading
 and trailing whitespace ignored, as is anything following a '#'. Can be
 specified multiple times to read names from multiple files.

.. option:: --keep-section <section>

 When removing sections from the output, do not remove sections named
 ``<section>``. Can be specified multiple times to keep multiple sections.

.. option:: --keep-symbol <symbol>, -K

 When removing symbols from the output, do not remove symbols named
 ``<symbol>``. Can be specified multiple times to keep multiple symbols.

.. option:: --keep-symbols <filename>

 When removing symbols from the output do not remove symbols named in the file
 ``<filename>``. In the file, each line represents a single symbol, with leading
 and trailing whitespace ignored, as is anything following a '#'. Can be
 specified multiple times to read names from multiple files.

.. option:: --localize-hidden

 Make all symbols with hidden or internal visibility local in the output.

.. option:: --localize-symbol <symbol>, -L

 Mark any defined non-common symbol named ``<symbol>`` as a local symbol in the
 output. Can be specified multiple times to mark multiple symbols as local.

.. option:: --localize-symbols <filename>

 Read a list of names from the file ``<filename>`` and mark defined non-common
 symbols with those names as local in the output. In the file, each line
 represents a single symbol, with leading and trailing whitespace ignored, as is
 anything following a '#'. Can be specified multiple times to read names from
 multiple files.
 
.. option:: --new-symbol-visibility <visibility>

 Specify the visibility of the symbols automatically created when using binary
 input or :option:`--add-symbol`. Valid options are:

 - `default`
 - `hidden`
 - `internal`
 - `protected`

 The default is `default`.

.. option:: --output-target <format>, -O

 Write the output as the specified format. See `SUPPORTED FORMATS`_ for a list
 of valid ``<format>`` values. If unspecified, the output format is assumed to
 be the same as the input file's format.

.. option:: --prefix-alloc-sections <prefix>

 Add ``<prefix>`` to the front of the names of all allocatable sections in the
 output.

.. option:: --prefix-symbols <prefix>

 Add ``<prefix>`` to the front of every symbol name in the output.

.. option:: --preserve-dates, -p

 Preserve access and modification timestamps in the output.

.. option:: --redefine-sym <old>=<new>

 Rename symbols called ``<old>`` to ``<new>`` in the output. Can be specified
 multiple times to rename multiple symbols.

.. option:: --redefine-syms <filename>

 Rename symbols in the output as described in the file ``<filename>``. In the
 file, each line represents a single symbol to rename, with the old name and new
 name separated by an equals sign. Leading and trailing whitespace is ignored,
 as is anything following a '#'. Can be specified multiple times to read names
 from multiple files.

.. option:: --rename-section <old>=<new>[,<flag>,...]

 Rename sections called ``<old>`` to ``<new>`` in the output, and apply any
 specified ``<flag>`` values. See :option:`--set-section-flags` for a list of
 supported flags. Can be specified multiple times to rename multiple sections.

.. option:: --set-section-flags <section>=<flag>[,<flag>,...]

 Set section properties in the output of section ``<section>`` based on the
 specified ``<flag>`` values. Can be specified multiple times to update multiple
 sections.

 Following is a list of supported flags and their effects:

 - `alloc` = add the `SHF_ALLOC` flag.
 - `load` = if the section has `SHT_NOBITS` type, mark it as a `SHT_PROGBITS`
   section.
 - `readonly` = if this flag is not specified, add the `SHF_WRITE` flag.
 - `code` = add the `SHF_EXECINSTR` flag.
 - `merge` = add the `SHF_MERGE` flag.
 - `strings` = add the `SHF_STRINGS` flag.
 - `contents` = if the section has `SHT_NOBITS` type, mark it as a `SHT_PROGBITS`
   section.

 The following flags are also accepted, but are ignored for GNU compatibility:
 `noload`, `debug`, `data`, `rom`, `share`.

.. option:: --set-start-addr <addr>

 Set the start address of the output to ``<addr>``. Overrides any previously
 specified :option:`--change-start` or :option:`--adjust-start` options.

.. option:: --split-dwo <dwo-file>

 Equivalent to running :program:`llvm-objcopy` with :option:`--extract-dwo` and
 ``<dwo-file>`` as the output file and no other options, and then with
 :option:`--strip-dwo` on the input file.

.. option:: --strip-dwo

 Remove all DWARF .dwo sections from the output.

.. option:: --strip-non-alloc

 Remove from the output all non-allocatable sections that are not within
 segments.

.. option:: --strip-sections

 Remove from the output all section headers and all section data not within
 segments. Note that many tools will not be able to use an object without
 section headers.

.. option:: --target <format>, -F

 Equivalent to :option:`--input-target` and :option:`--output-target` for the
 specified format. See `SUPPORTED FORMATS`_ for a list of valid ``<format>``
 values.

.. option:: --weaken-symbol <symbol>, -W

 Mark any global symbol named ``<symbol>`` as a weak symbol in the output. Can
 be specified multiple times to mark multiple symbols as weak.

.. option:: --weaken-symbols <filename>

 Read a list of names from the file ``<filename>`` and mark global symbols with
 those names as weak in the output. In the file, each line represents a single
 symbol, with leading and trailing whitespace ignored, as is anything following
 a '#'. Can be specified multiple times to read names from multiple files.

.. option:: --weaken

 Mark all defined global symbols as weak in the output.

SUPPORTED FORMATS
-----------------

The following values are currently supported by :program:`llvm-objcopy` for the
:option:`--input-target`, :option:`--output-target`, and :option:`--target`
options. For GNU :program:`objcopy` compatibility, the values are all bfdnames.

- `binary`
- `ihex`
- `elf32-i386`
- `elf32-x86-64`
- `elf64-x86-64`
- `elf32-iamcu`
- `elf32-littlearm`
- `elf64-aarch64`
- `elf64-littleaarch64`
- `elf32-littleriscv`
- `elf64-littleriscv`
- `elf32-powerpc`
- `elf32-powerpcle`
- `elf64-powerpc`
- `elf64-powerpcle`
- `elf32-bigmips`
- `elf32-ntradbigmips`
- `elf32-ntradlittlemips`
- `elf32-tradbigmips`
- `elf32-tradlittlemips`
- `elf64-tradbigmips`
- `elf64-tradlittlemips`
- `elf32-sparc`
- `elf32-sparcel`

Additionally, all targets except `binary` and `ihex` can have `-freebsd` as a
suffix.

BINARY INPUT AND OUTPUT
-----------------------

If `binary` is used as the value for :option:`--input-target`, the input file
will be embedded as a data section in an ELF relocatable object, with symbols
``_binary_<file_name>_start``, ``_binary_<file_name>_end``, and
``_binary_<file_name>_size`` representing the start, end and size of the data,
where ``<file_name>`` is the path of the input file as specified on the command
line with non-alphanumeric characters converted to ``_``.

If `binary` is used as the value for :option:`--output-target`, the output file
will be a raw binary file, containing the memory image of the input file.
Symbols and relocation information will be discarded. The image will start at
the address of the first loadable section in the output.

EXIT STATUS
-----------

:program:`llvm-objcopy` exits with a non-zero exit code if there is an error.
Otherwise, it exits with code 0.

BUGS
----

To report bugs, please visit <http://llvm.org/bugs/>.

There is a known issue with :option:`--input-target` and :option:`--target`
causing only ``binary`` and ``ihex`` formats to have any effect. Other values
will be ignored and :program:`llvm-objcopy` will attempt to guess the input
format.

SEE ALSO
--------

:manpage:`llvm-strip(1)`
