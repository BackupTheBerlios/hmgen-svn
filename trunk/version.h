#ifndef VERSION_H
#define VERSION_H

#define HMG_STRINGIFY(s)    HMG_TOSTRING(s)
#define HMG_TOSTRING(s)     #s

#define HMG_VERSION_MAJOR       0
#define HMG_VERSION_MINOR       9
#define HMG_VERSION_MICRO       0
#define HMG_VERSION_REV         SVN_REVISION
#define HMG_VERSION_INT         ((HMG_VERSION_MAJOR<<24) + \
                                 (HMG_VERSION_MINOR<<20) + \
                                 (HMG_VERSION_MICRO<<16) + \
                                  HMG_VERSION_REV)
#if HMG_VERSION_REV > 100
#define HMG_VERSION_STRING      "Version " HMG_STRINGIFY(HMG_VERSION_MAJOR) \
                                "."        HMG_STRINGIFY(HMG_VERSION_MINOR) \
                                "."        HMG_STRINGIFY(HMG_VERSION_MICRO) \
                                "r"        HMG_STRINGIFY(HMG_VERSION_REV)
#else
#define HMG_VERSION_STRING      "Version " HMG_STRINGIFY(HMG_VERSION_MAJOR) \
                                "."        HMG_STRINGIFY(HMG_VERSION_MINOR) \
                                "."        HMG_STRINGIFY(HMG_VERSION_MICRO)
#endif
#define HMG_COPYRIGHT_STRING    "Copyright (C) 2007 Ivo van Poorten"

#endif
