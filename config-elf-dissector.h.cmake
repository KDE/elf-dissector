#ifndef CONFIG_ELFDISSECTOR_H
#define CONFIG_ELFDISSECTOR_H

// TODO actually detect the binutils version...
#define BINUTILS_MAJOR_VERSION 2
#define BINUTILS_MINOR_VERSION 24

#define BINUTILS_VERSION ((BINUTILS_MAJOR_VERSION << 8) | BINUTILS_MINOR_VERSION)
#define BINUTILS_VERSION_CHECK(maj, min) ((maj << 8) | min)

#endif
