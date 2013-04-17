find_path(Iberty_INCLUDE_DIR demangle.h)

find_library(Iberty_LIBRARY NAMES iberty)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Iberty DEFAULT_MSG Iberty_LIBRARY Iberty_INCLUDE_DIR)

include(FeatureSummary)
set_package_properties(binutils-devel PROPERTIES URL http://www.gcc.org/
  DESCRIPTION "Development files of binutils.")

