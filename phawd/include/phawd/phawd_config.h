/*!
 * PHAWD - Parameters Handler and Waveform Display
 * Licensed under the GNU GPLv3 license. See LICENSE for more details.
 * @author HuNing-He
 * @date 2022-2-24
 * @version 0.3
 * @email 2689112371@qq.com
 * @copyright (c) 2022 HuNing-He
 * @file phawd_config.h
 * @brief definition of socket communication in phawd
 */

#ifndef PHAWD_CONFIG_H
#define PHAWD_CONFIG_H

#ifdef _WIN32
#pragma warning(disable:4251) /* No DLL interface for type of member of exported class */
#pragma warning(disable:4275) /* No DLL interface for base class of exported class */
#pragma warning(disable:4267) /* Don't warn about conversion from size_t to int */
#pragma warning(disable:4996) /* Don't warn about windows socket deprecated */
#pragma warning(disable:4200) /* Don't warn about flexible array in C */
#pragma warning(disable:4819) /* Encoding warning */
#pragma warning(disable:4101) /* Unreferenced local variables */
#endif

// Handle portable symbol export.
// Defining manually which symbol should be exported is required
// under Windows whether MinGW or MSVC is used.
//
// The headers then have to be able to work in two different modes:
// - dllexport when one is building the library,
// - dllimport for clients using the library.
//
// On Linux, set the visibility accordingly. If C++ symbol visibility
// is handled by the compiler, see: http://gcc.gnu.org/wiki/Visibility
# if defined _WIN32 || defined __CYGWIN__
// On Microsoft Windows, use dllimport and dllexport to tag symbols.
  # ifdef __GNUC__
    # define PHAWD_DLLIMPORT __attribute__ ((dllimport))
    # define PHAWD_DLLEXPORT __attribute__ ((dllexport))
    # define PHAWD_DLLLOCAL
  # else
    # define PHAWD_DLLIMPORT __declspec(dllimport)
    # define PHAWD_DLLEXPORT __declspec(dllexport)
    # define PHAWD_DLLLOCAL
  # endif // __GNUC__
# else
// On Linux, for GCC >= 4, tag symbols using GCC extension.
#  if __GNUC__ >= 4
#   define PHAWD_DLLIMPORT __attribute__ ((visibility("default")))
#   define PHAWD_DLLEXPORT __attribute__ ((visibility("default")))
#   define PHAWD_DLLLOCAL  __attribute__ ((visibility("hidden")))
#  else
// Otherwise (GCC < 4 or another compiler is used), export everything.
#   define PHAWD_DLLIMPORT
#   define PHAWD_DLLEXPORT
#   define PHAWD_DLLLOCAL
#  endif // __GNUC__ >= 4
# endif // defined _WIN32 || defined __CYGWIN__

# ifdef PHAWD_STATIC
// If one is using the library statically, get rid of extra information.
#  define PHAWD_DLLAPI
#  define PHAWD_LOCAL
# else
// Depending on whether one is building or using the library define DLLAPI to import or export.
#  ifdef PHAWD_EXPORTS
#   define PHAWD_DLLAPI PHAWD_DLLEXPORT
#  else
#   define PHAWD_DLLAPI PHAWD_DLLIMPORT
#  endif // PHAWD_EXPORTS
#  define PHAWD_LOCAL PHAWD_DLLLOCAL
# endif // PHAWD_STATIC

#endif //PHAWD_CONFIG_H
