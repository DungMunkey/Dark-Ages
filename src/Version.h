#ifndef _VERSION_H
#define _VERSION_H

//DA_VERSION_BASE is the version being worked toward; bump it on dev right after each release so development builds
//don't label themselves as the release that just shipped. The automated dev builds add DA_VERSION_SUFFIX (for
//example "-dev.47+a1b2c3d", passed through msbuild's /p:DAVersionSuffix=...); a release or local build has none.
//The whole string has to fit the credits line: at most 30 characters (see the static_asserts next to credits()).
#define DA_VERSION_BASE "2.0.0-alpha.3"
#ifndef DA_VERSION_SUFFIX
#define DA_VERSION_SUFFIX ""
#endif
#define DA_VERSION   DA_VERSION_BASE DA_VERSION_SUFFIX

//The copyright is kept in two pieces because the end credits show them on separate lines (the credits font is
//16 reference pixels per character and lines aren't wrapped, so a credits line can hold at most 38 characters -
//see the static_asserts next to credits() in CDarkages.cpp). DA_COPYRIGHT below joins them back into the single
//full string that the title screen uses.
#define DA_COPYRIGHT_YEARS "Copyright (C) 1997-2026"
#define DA_COPYRIGHT_OWNER "Michael Hoopmann, Dark Knight Software"
#define DA_COPYRIGHT DA_COPYRIGHT_YEARS ", " DA_COPYRIGHT_OWNER

#endif
