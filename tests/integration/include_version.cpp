/**
 * \file            include_version.cpp
 * \brief           Independent public-header compilation
 */

#include <xgen/bytes/version.h>

static_assert(XGB_VERSION_MAJOR == 0 && XGB_ABI_VERSION == 1,
              "Package identity");
