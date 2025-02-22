# TODO turn this in a FindBinutils with components
include(CheckCSourceCompiles)
include(FindPackageHandleStandardArgs)

find_path(Iberty_INCLUDE_DIR demangle.h PATH_SUFFIXES libiberty)
find_library(Iberty_LIBRARY NAMES iberty)

find_package_handle_standard_args(Iberty DEFAULT_MSG Iberty_LIBRARY Iberty_INCLUDE_DIR)

# TODO improve this version check
set(Binutils_VERSION_MAJOR 2)
set(Binutils_VERSION_MINOR 22)

set(_old_requires ${CMAKE_REQUIRED_INCLUDES})
list(APPEND CMAKE_REQUIRED_INCLUDES ${Iberty_INCLUDE_DIR})

check_c_source_compiles("
    #include <demangle.h>
    int main(int argc, char **argv) {
        return DEMANGLE_COMPONENT_INITIALIZER_LIST;
    }"
    Binutils_HAVE_DEMANGLE_COMPONENT_INITIALIZER_LIST
)
if(Binutils_HAVE_DEMANGLE_COMPONENT_INITIALIZER_LIST)
  set(Binutils_VERSION_MINOR 23)
endif()

check_c_source_compiles("
    #include <demangle.h>
    int main(int argc, char **argv) {
        return DEMANGLE_COMPONENT_REFERENCE_THIS;
    }"
    Binutils_HAVE_DEMANGLE_COMPONENT_REFERENCE_THIS
)
if(Binutils_HAVE_DEMANGLE_COMPONENT_REFERENCE_THIS)
  set(Binutils_VERSION_MINOR 24)
endif()

if(IBERTY_FOUND AND NOT TARGET Binutils::Iberty)
    add_library(Binutils::Iberty UNKNOWN IMPORTED)
    set_target_properties(Binutils::Iberty PROPERTIES
        IMPORTED_LOCATION "${Iberty_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${Iberty_INCLUDE_DIR}"
    )
endif()

mark_as_advanced(Iberty_LIBRARY Iberty_INCLUDE_DIR)


find_path(Opcodes_INCLUDE_DIR dis-asm.h)
find_library(Opcodes_LIBRARY NAMES opcodes)

find_package_handle_standard_args(Opcodes DEFAULT_MSG Opcodes_INCLUDE_DIR Opcodes_LIBRARY)

if(OPCODES_FOUND AND NOT TARGET Binutils::Opcodes)
    add_library(Binutils::Opcodes UNKNOWN IMPORTED)
    set_target_properties(Binutils::Opcodes PROPERTIES
        IMPORTED_LOCATION "${Opcodes_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${Opcodes_INCLUDE_DIR}"
    )
endif()


set(CMAKE_REQUIRED_LIBRARIES ${Opcodes_LIBRARY})
check_c_source_compiles("
    #include <dis-asm.h>
    int main(int argc, char **argv) {
        disassembler_ftype ftype = print_insn_i386;
        return 0;
     }"
     Binutils_HAVE_PRINT_INSN_I386
)
if(NOT Binutils_HAVE_PRINT_INSN_I386)
  set(Binutils_VERSION_MINOR 29)
endif()

mark_as_advanced(Opcodes_LIBRARY Opcodes_INCLUDE_DIR)

check_c_source_compiles("
    #include <demangle.h>
    int main(int argc, char **argv) {
        return DEMANGLE_COMPONENT_TPARM_OBJ;
    }"
    Binutils_HAVE_DEMANGLE_COMPONENT_TPARM_OBJ
)
if(Binutils_HAVE_DEMANGLE_COMPONENT_TPARM_OBJ)
  set(Binutils_VERSION_MINOR 32)
endif()

 # we don't really care about that particular define, but about a behavior
 # difference in demangling, but that is much harder to check for here
check_c_source_compiles("
    #include <ctf.h>
    int main(int argc, char **argv) {
        return CTF_F_NEWFUNCINFO;
    }"
    Binutils_HAVE_CTF_F_NEWFUNCINFO
)
if(Binutils_HAVE_CTF_F_NEWFUNCINFO)
  set(Binutils_VERSION_MINOR 36)
endif()

check_c_source_compiles("
#define PACKAGE
#define PACKAGE_VERSION
    #include <dis-asm.h>
    int main(int, char**) {
        init_disassemble_info(NULL, NULL, NULL, NULL);
        return 0;
    }"
    Binutils_HAVE_4ARG_INTI_DISASSEMBLE_INFO
)
if (Binutils_HAVE_4ARG_INTI_DISASSEMBLE_INFO)
    set(Binutils_VERSION_MINOR 39)
endif()

check_c_source_compiles("
    #include <demangle.h>
    int main(int argc, char **argv) {
        return DEMANGLE_COMPONENT_STRUCTURED_BINDING;
    }"
    Binutils_HAVE_DEMANGLE_COMPONENT_STRUCTURED_BINDING
)
if(Binutils_HAVE_DEMANGLE_COMPONENT_STRUCTURED_BINDING)
  set(Binutils_VERSION_MINOR 40)
endif()

check_c_source_compiles("
    #include <demangle.h>
    int main(int argc, char **argv) {
        return DEMANGLE_COMPONENT_EXTENDED_BUILTIN_TYPE;
    }"
    Binutils_HAVE_DEMANGLE_COMPONENT_EXTENDED_BUILTIN_TYPE
)
if(Binutils_HAVE_DEMANGLE_COMPONENT_EXTENDED_BUILTIN_TYPE)
  set(Binutils_VERSION_MINOR 41)
endif()

check_c_source_compiles("
    #include <demangle.h>
    int main(int argc, char **argv) {
        return DEMANGLE_COMPONENT_CONSTRAINTS;
    }"
    Binutils_HAVE_DEMANGLE_COMPONENT_CONSTRAINTS
)
if(Binutils_HAVE_DEMANGLE_COMPONENT_CONSTRAINTS)
  set(Binutils_VERSION_MINOR 42)
endif()

if(TARGET Binutils::Iberty)
   target_link_libraries(Binutils::Iberty INTERFACE Binutils::Opcodes)
endif()
if(TARGET Binutils::Opcodes)
   target_link_libraries(Binutils::Opcodes INTERFACE Binutils::Iberty)
endif()

include(FeatureSummary)
set_package_properties(binutils-devel PROPERTIES URL https://www.gcc.org/
  DESCRIPTION "Development files of binutils.")

set(CMAKE_REQUIRED_INCLUDES ${_old_requires})
