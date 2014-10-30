#ifndef FUNC_NAME_H
#define FUNC_NAME_H

#include <iostream>

#ifdef __GNUC__
#define func_name __PRETTY_FUNCTION__
#endif

#ifdef __MWERKS__
#define func_name __PRETTY_FUNCTION__
#endif

#ifdef __CWCC__
#define func_name __PRETTY_FUNCTION__
#endif

#ifdef __DMC__
#define func_name __PRETTY_FUNCTION__
#endif

#ifdef __ICC
#define func_name __PRETTY_FUNCTION__
#endif

#ifdef __MINGW32__
#define func_name __PRETTY_FUNCTION__
#endif

#ifdef __MINGW64__
#define func_name __PRETTY_FUNCTION__
#endif

#ifdef __INTEL_COMPILER
#define func_name __FUNCTION__
#endif

#ifdef __xlc__
#define func_name __FUNCTION__
#endif

#ifdef __xlC__
#define func_name __FUNCTION__
#endif

#ifdef __BORLANDC__
#define func_name __FUNC__
#endif

#ifdef __HP_cc
#define func_name __func__
#endif

#endif // FUNC_NAME_H
