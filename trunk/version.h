#ifndef VERSION_H
#define VERSION_H

#define HMG_STRINGIFY(s)    HMG_TOSTRING(s)
#define HMG_TOSTRING(s)     #s

#define HMG_VERSION_MAJOR       0
#define HMG_VERSION_MINOR       8
#define HMG_VERSION_MICRO       SVN_REVISION
#define HMG_VERSION_INT         ((HMG_VERSION_MAJOR<<8) + \
                                 (HMG_VERSION_MINOR<<4) + \
                                  HMG_VERSION_MICRO)
#define HMG_VERSION_STRING      "Version " HMG_STRINGIFY(HMG_VERSION_MAJOR) \
                                "."        HMG_STRINGIFY(HMG_VERSION_MINOR) \
                                "."        HMG_STRINGIFY(HMG_VERSION_MICRO)
#define HMG_COPYRIGHT_STRING    "Copyright (C) 2007 Ivo van Poorten"

#endif
