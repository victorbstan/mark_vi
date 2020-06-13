

// assertions.h -- assertions and major conditionals.  Invoked by core.h, not environment.h


#ifndef __ASSERTIONS_H__
#define __ASSERTIONS_H__

#include <assert.h>

///////////////////////////////////////////////////////////////////////////////
//  Compile Time Assert
///////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG // This is debug only ...
#define	COMPILE_TIME_ASSERT(HINT_ERROR_WORD, CONDITION_THAT_MUST_BE_TRUE)	\
	typedef int dummy_ ## HINT_ERROR_WORD[(CONDITION_THAT_MUST_BE_TRUE) * 2 - 1]
#else
	#define COMPILE_TIME_ASSERT(name, x)	// Nothing!
#endif

COMPILE_TIME_ASSERT(char, sizeof(char) == 1);
COMPILE_TIME_ASSERT(float, sizeof(float) == 4);
COMPILE_TIME_ASSERT(long, sizeof(long) >= 4);
COMPILE_TIME_ASSERT(int, sizeof(int) == 4);
COMPILE_TIME_ASSERT(unsigned, sizeof(unsigned) == 4);
COMPILE_TIME_ASSERT(short, sizeof(short) == 2);

/* make sure enums are the size of ints for structure packing */
typedef enum {
	THE_DUMMY_VALUE
} THE_DUMMY_ENUM;
COMPILE_TIME_ASSERT(enum, sizeof(THE_DUMMY_ENUM) == sizeof(int));

#ifdef _DEBUG // Results in a duplicate case error if the condition is false.
	#define COMPILE_TIME_ASSERT_SIMPLE(CONDITION_THAT_MUST_BE_TRUE) switch(0){case 0:case CONDITION_THAT_MUST_BE_TRUE:;}
#else
	#define COMPILE_TIME_ASSERT_SIMPLE(CONDITION_THAT_MUST_BE_TRUE)
#endif

///////////////////////////////////////////////////////////////////////////////
//  Compile Time Assert - Debug only
///////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG // This is debug only ...
	#define	DEBUG_COMPILE_TIME_ASSERT(HINT_ERROR_WORD, CONDITION_THAT_MUST_BE_TRUE)	\
		typedef int dummy_ ## HINT_ERROR_WORD[(CONDITION_THAT_MUST_BE_TRUE) * 2 - 1]
#else
	#define DEBUG_COMPILE_TIME_ASSERT(HINT_ERROR_WORD, CONDITION_THAT_MUST_BE_TRUE)	// Nothing!
#endif

///////////////////////////////////////////////////////////////////////////////
//  Debug Only Statements
///////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
	#define DEBUG_ONLY(x) x
#else
	#define DEBUG_ONLY(x)
#endif

// Debatable whether or not to have the trailing semi-colon in this macro ... probably doesn't matter.
#define DEBUG_ASSERT(condition) DEBUG_ONLY (if(!(condition)) log_fatal ("Assertion failure: %s", STRINGIFY(condition)));


#define SPRINTSFUNC_ "%s: " // To help standardize "print __func__"
//#define //#define test(...) test_ (&_funcinfo_obj, __VA_ARGS__) WORKS in 2008 just fine!

///////////////////////////////////////////////////////////////////////////////
//  Run-Time Assert - Active in debug build only
///////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG // Heavier.  Kept because it is slightly used.  Not sure the advantage over assert?  Assert doesn't log or say why.
	// This function should be culled by the compiler in any given source file if nothing uses it.
	// Ought to expand to STRINGIFY the condition, but whatever ...
	static void *__passthru (void *v, int set)
	{
		static void *stored;
		if (set)
			stored = v;

		return stored;
	}

	#define OR_DIE(_x, _msg) __passthru( (void *)_x, 1) ? __passthru( NULL, 0) : (void *)log_fatal ("%s\n%s", _msg,  __func__)

	// Won't appear in a release build
	#define c_assert(_x) assert (!!OR_DIE((_x), "Assertion failure"  )) // Generic_StdError_Printf_NoReturn
#else
	#define c_assert(_x) assert (!!OR_DIE((_x), "Assertion failure"  )) // Generic_StdError_Printf_NoReturn
	//c_assert (left != right); //,	"left = right");
#endif


//#define log_fatal_if (PARENS_CONDITION, PRINTFSTRING, __VA_ARGS__) if PARENS_CONDITION log_fatal (SPRINTSFUNC_ PRINTFSTRING, __VA_ARGS__)
#define log_fatal_if(CONDITION) \
	if (CONDITION) log_fatal (SPRINTSFUNC_ "(%s) is true", __func__, STRINGIFY(CONDITION))

#define log_fatal_if_msg(CONDITION, DESCRIPTION) \
	if (CONDITION) log_fatal (SPRINTSFUNC_ DESCRIPTION " (%s) is true", __func__, STRINGIFY(CONDITION))

#define log_fatal_if_msgf(CONDITION, PRINTFSTRING, __VA_ARGS__) \
	if (CONDITION) log_fatal (SPRINTSFUNC_ PRINTFSTRING " (%s) is true", __func__, __VA_ARGS__, STRINGIFY(CONDITION))

//#define test(...) test_ (&_funcinfo_obj, __VA_ARGS__) WORKS in 2008 just fine!


#endif // ! __ASSERTIONS_H__




