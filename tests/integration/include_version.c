/**
 * \file            include_version.c
 * \brief           Independent public-header compilation
 */

#include <xgen/bytes/version.h>

_Static_assert(XGB_VERSION_MAJOR == 0 && XGB_ABI_VERSION == 1,
               "Package identity");
