#ifndef _VERSION_H
#define _VERSION_H

#define DA_VERSION   "2.0 alpha 3"

//The copyright is kept in two pieces because the end credits show them on separate lines (the credits font is
//16 reference pixels per character and lines aren't wrapped, so a credits line can hold at most 38 characters -
//see the static_asserts next to credits() in CDarkages.cpp). DA_COPYRIGHT below joins them back into the single
//full string that the title screen uses.
#define DA_COPYRIGHT_YEARS "Copyright (C) 1997-2026"
#define DA_COPYRIGHT_OWNER "Michael Hoopmann, Dark Knight Software"
#define DA_COPYRIGHT DA_COPYRIGHT_YEARS ", " DA_COPYRIGHT_OWNER

#endif
