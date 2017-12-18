/* A Bison parser, made by GNU Bison 2.1.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Written by Richard Stallman by simplifying the original so called
   ``semantic'' parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     STRING = 258,
     ID = 259,
     NUM = 260,
     LBRACK = 261,
     RBRACK = 262,
     ACCELERATOR = 263,
     AREALIGHTSOURCE = 264,
     ATTRIBUTEBEGIN = 265,
     ATTRIBUTEEND = 266,
     CAMERA = 267,
     CONCATTRANSFORM = 268,
     COORDINATESYSTEM = 269,
     COORDSYSTRANSFORM = 270,
     FILM = 271,
     IDENTITY = 272,
     LIGHTSOURCE = 273,
     LOOKAT = 274,
     MATERIAL = 275,
     MAKENAMEDMATERIAL = 276,
     NAMEDMATERIAL = 277,
     OBJECTBEGIN = 278,
     OBJECTEND = 279,
     OBJECTINSTANCE = 280,
     PIXELFILTER = 281,
     REVERSEORIENTATION = 282,
     ROTATE = 283,
     SAMPLER = 284,
     SCALE = 285,
     SEARCHPATH = 286,
     PORTALSHAPE = 287,
     SHAPE = 288,
     SURFACEINTEGRATOR = 289,
     TEXTURE = 290,
     TRANSFORMBEGIN = 291,
     TRANSFORMEND = 292,
     TRANSFORM = 293,
     TRANSLATE = 294,
     VOLUME = 295,
     VOLUMEINTEGRATOR = 296,
     WORLDBEGIN = 297,
     WORLDEND = 298,
     HIGH_PRECEDENCE = 299
   };
#endif
/* Tokens.  */
#define STRING 258
#define ID 259
#define NUM 260
#define LBRACK 261
#define RBRACK 262
#define ACCELERATOR 263
#define AREALIGHTSOURCE 264
#define ATTRIBUTEBEGIN 265
#define ATTRIBUTEEND 266
#define CAMERA 267
#define CONCATTRANSFORM 268
#define COORDINATESYSTEM 269
#define COORDSYSTRANSFORM 270
#define FILM 271
#define IDENTITY 272
#define LIGHTSOURCE 273
#define LOOKAT 274
#define MATERIAL 275
#define MAKENAMEDMATERIAL 276
#define NAMEDMATERIAL 277
#define OBJECTBEGIN 278
#define OBJECTEND 279
#define OBJECTINSTANCE 280
#define PIXELFILTER 281
#define REVERSEORIENTATION 282
#define ROTATE 283
#define SAMPLER 284
#define SCALE 285
#define SEARCHPATH 286
#define PORTALSHAPE 287
#define SHAPE 288
#define SURFACEINTEGRATOR 289
#define TEXTURE 290
#define TRANSFORMBEGIN 291
#define TRANSFORMEND 292
#define TRANSFORM 293
#define TRANSLATE 294
#define VOLUME 295
#define VOLUMEINTEGRATOR 296
#define WORLDBEGIN 297
#define WORLDEND 298
#define HIGH_PRECEDENCE 299




/* Copy the first part of user declarations.  */
#line 2 "e:\\ETRI\\Projects\\Go!Rilla\\RealtimeRendering\\RealtimeRender\\GIRLCore\\core\\girlparse.y"

#include "api.h"
#include "girl.h"
#include "error.h"
#include "paramset.h"
#include "context.h"
#include "memory.h"
#include <stdarg.h>
#include <sstream>

using namespace girl;

extern int yylex( void );
int line_num = 0;
string current_file;

#define YYMAXDEPTH 100000000

void yyerror( const char *str ) {
	std::stringstream ss;
	ss<<"Parsing error: "<<str;
	girlError( GIRL_SYNTAX,GIRL_SEVERE,ss.str().c_str());
	//Severe( "Parsing error: %s", str);
}

/*
void ParseError( const char *format, ... ) PRINTF_FUNC;

void ParseError( const char *format, ... ) {
	char error[4096];
	va_list args;
	va_start( args, format );
	vsnprintf(error, 4096, format, args);
	yyerror(error);
	va_end( args );
}*/

int cur_paramlist_allocated = 0;
int cur_paramlist_size = 0;
const char **cur_paramlist_tokens = NULL;
void **cur_paramlist_args = NULL;
int *cur_paramlist_sizes = NULL;
bool *cur_paramlist_texture_helper = NULL;

#define CPS cur_paramlist_size
#define CPT cur_paramlist_tokens
#define CPA cur_paramlist_args
#define CPTH cur_paramlist_texture_helper
#define CPSZ cur_paramlist_sizes

typedef struct ParamArray {
	int element_size;
	int allocated;
	int nelems;
	void *array;
} ParamArray;

ParamArray *cur_array = NULL;
bool array_is_single_string = false;

#define NA(r) ((float *) r->array)
#define SA(r) ((const char **) r->array)

void AddArrayElement( void *elem ) {
	if (cur_array->nelems >= cur_array->allocated) {
		cur_array->allocated = 2*cur_array->allocated + 1;
		cur_array->array = realloc( cur_array->array,
			cur_array->allocated*cur_array->element_size );
	}
	char *next = ((char *)cur_array->array) + cur_array->nelems *
		cur_array->element_size;
	memcpy( next, elem, cur_array->element_size );
	cur_array->nelems++;
}

ParamArray *ArrayDup( ParamArray *ra )
{
	ParamArray *ret = new ParamArray;
	ret->element_size = ra->element_size;
	ret->allocated = ra->allocated;
	ret->nelems = ra->nelems;
	ret->array = malloc(ra->nelems * ra->element_size);
	memcpy( ret->array, ra->array, ra->nelems * ra->element_size );
	return ret;
}

void ArrayFree( ParamArray *ra )
{
	free(ra->array);
	delete ra;
}

void FreeArgs()
{
	for (int i = 0; i < cur_paramlist_size; ++i) {
		// NOTE - Ratow - freeing up strings inside string type args
		if(memcmp("string", cur_paramlist_tokens[i], 6) == 0 ||
				memcmp("texture", cur_paramlist_tokens[i], 6) == 0) {
			for (int j = 0; j < cur_paramlist_sizes[i]; ++j)
				free(((char **)cur_paramlist_args[i])[j]);
		}
		delete[] ((char *)cur_paramlist_args[i]);
	}
}

static bool VerifyArrayLength( ParamArray *arr, int required,
	const char *command ) {
	if (arr->nelems != required) {
		std::stringstream ss;
		ss<<command<<" requires a(n) "<<required<<" element array!";
		//ParseError( "%s requires a(n) %d element array!", command, required);
		return false;
	}
	return true;
}
enum { PARAM_TYPE_INT, PARAM_TYPE_BOOL, PARAM_TYPE_FLOAT, PARAM_TYPE_POINT,
	PARAM_TYPE_VECTOR, PARAM_TYPE_NORMAL, PARAM_TYPE_COLOR,
	PARAM_TYPE_STRING, PARAM_TYPE_TEXTURE };
static void InitParamSet(ParamSet &ps, int count, const char **tokens,
	void **args, int *sizes, bool *texture_helper);
static bool lookupType(const char *token, int *type, string &name);
#define YYPRINT(file, type, value)  \
{ \
	if ((type) == ID || (type) == STRING) \
		fprintf ((file), " %s", (value).string); \
	else if ((type) == NUM) \
		fprintf ((file), " %f", (value).num); \
}


/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 132 "e:\\ETRI\\Projects\\Go!Rilla\\RealtimeRendering\\RealtimeRender\\GIRLCore\\core\\girlparse.y"
typedef union YYSTYPE {
char string[1024];
float num;
ParamArray *ribarray;
} YYSTYPE;
/* Line 191 of yacc.c.  */
#line 309 "e:\\ETRI\\Projects\\Go!Rilla\\RealtimeRendering\\RealtimeRender\\GIRLCore\\core\\/girlparse.cpp"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 219 of yacc.c.  */
#line 321 "e:\\ETRI\\Projects\\Go!Rilla\\RealtimeRendering\\RealtimeRender\\GIRLCore\\core\\/girlparse.cpp"

#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T) && (defined (__STDC__) || defined (__cplusplus))
# include <stddef.h> /* INFRINGES ON USER NAME SPACE */
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

#if ! defined (yyoverflow) || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if defined (__STDC__) || defined (__cplusplus)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     define YYINCLUDED_STDLIB_H
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2005 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM ((YYSIZE_T) -1)
#  endif
#  ifdef __cplusplus
extern "C" {
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if (! defined (malloc) && ! defined (YYINCLUDED_STDLIB_H) \
	&& (defined (__STDC__) || defined (__cplusplus)))
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if (! defined (free) && ! defined (YYINCLUDED_STDLIB_H) \
	&& (defined (__STDC__) || defined (__cplusplus)))
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifdef __cplusplus
}
#  endif
# endif
#endif /* ! defined (yyoverflow) || YYERROR_VERBOSE */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (defined (YYSTYPE_IS_TRIVIAL) && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short int yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short int) + sizeof (YYSTYPE))			\
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined (__GNUC__) && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif

#if defined (__STDC__) || defined (__cplusplus)
   typedef signed char yysigned_char;
#else
   typedef short int yysigned_char;
#endif

/* YYFINAL -- State number of the termination state. */
#define YYFINAL  71
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   113

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  45
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  22
/* YYNRULES -- Number of rules. */
#define YYNRULES  64
/* YYNRULES -- Number of states. */
#define YYNSTATES  133

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   299

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned char yyprhs[] =
{
       0,     0,     3,     5,     6,     7,     8,    10,    12,    14,
      16,    21,    24,    27,    29,    32,    34,    36,    41,    44,
      47,    49,    52,    55,    56,    59,    60,    63,    66,    68,
      72,    76,    78,    80,    84,    87,    90,    93,    97,    99,
     103,   114,   118,   122,   126,   129,   131,   134,   138,   140,
     146,   150,   155,   158,   162,   166,   170,   176,   178,   180,
     183,   188,   192,   196,   198
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const yysigned_char yyrhs[] =
{
      46,     0,    -1,    65,    -1,    -1,    -1,    -1,    51,    -1,
      56,    -1,    52,    -1,    53,    -1,    47,     6,    54,     7,
      -1,    47,    55,    -1,    54,    55,    -1,    55,    -1,    48,
       3,    -1,    57,    -1,    58,    -1,    47,     6,    59,     7,
      -1,    47,    60,    -1,    59,    60,    -1,    60,    -1,    49,
       5,    -1,    62,    63,    -1,    -1,    64,    63,    -1,    -1,
       3,    50,    -1,    65,    66,    -1,    66,    -1,     8,     3,
      61,    -1,     9,     3,    61,    -1,    10,    -1,    11,    -1,
      12,     3,    61,    -1,    13,    56,    -1,    14,     3,    -1,
      15,     3,    -1,    16,     3,    61,    -1,    17,    -1,    18,
       3,    61,    -1,    19,     5,     5,     5,     5,     5,     5,
       5,     5,     5,    -1,    20,     3,    61,    -1,    21,     3,
      61,    -1,    22,     3,    61,    -1,    23,     3,    -1,    24,
      -1,    25,     3,    -1,    26,     3,    61,    -1,    27,    -1,
      28,     5,     5,     5,     5,    -1,    29,     3,    61,    -1,
      30,     5,     5,     5,    -1,    31,     3,    -1,    33,     3,
      61,    -1,    32,     3,    61,    -1,    34,     3,    61,    -1,
      35,     3,     3,     3,    61,    -1,    36,    -1,    37,    -1,
      38,    57,    -1,    39,     5,     5,     5,    -1,    41,     3,
      61,    -1,    40,     3,    61,    -1,    42,    -1,    43,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short int yyrline[] =
{
       0,   154,   154,   158,   168,   173,   178,   182,   187,   191,
     197,   202,   206,   209,   213,   219,   223,   228,   233,   237,
     240,   244,   250,   254,   259,   263,   266,   284,   287,   291,
     298,   305,   309,   313,   320,   326,   330,   334,   341,   345,
     352,   356,   363,   370,   377,   381,   385,   389,   396,   400,
     404,   411,   415,   419,   426,   433,   440,   447,   451,   455,
     461,   465,   472,   479,   483
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "STRING", "ID", "NUM", "LBRACK",
  "RBRACK", "ACCELERATOR", "AREALIGHTSOURCE", "ATTRIBUTEBEGIN",
  "ATTRIBUTEEND", "CAMERA", "CONCATTRANSFORM", "COORDINATESYSTEM",
  "COORDSYSTRANSFORM", "FILM", "IDENTITY", "LIGHTSOURCE", "LOOKAT",
  "MATERIAL", "MAKENAMEDMATERIAL", "NAMEDMATERIAL", "OBJECTBEGIN",
  "OBJECTEND", "OBJECTINSTANCE", "PIXELFILTER", "REVERSEORIENTATION",
  "ROTATE", "SAMPLER", "SCALE", "SEARCHPATH", "PORTALSHAPE", "SHAPE",
  "SURFACEINTEGRATOR", "TEXTURE", "TRANSFORMBEGIN", "TRANSFORMEND",
  "TRANSFORM", "TRANSLATE", "VOLUME", "VOLUMEINTEGRATOR", "WORLDBEGIN",
  "WORLDEND", "HIGH_PRECEDENCE", "$accept", "start", "array_init",
  "string_array_init", "num_array_init", "array", "string_array",
  "real_string_array", "single_element_string_array", "string_list",
  "string_list_entry", "num_array", "real_num_array",
  "single_element_num_array", "num_list", "num_list_entry", "paramlist",
  "paramlist_init", "paramlist_contents", "paramlist_entry",
  "ri_stmt_list", "ri_stmt", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short int yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    45,    46,    47,    48,    49,    50,    50,    51,    51,
      52,    53,    54,    54,    55,    56,    56,    57,    58,    59,
      59,    60,    61,    62,    63,    63,    64,    65,    65,    66,
      66,    66,    66,    66,    66,    66,    66,    66,    66,    66,
      66,    66,    66,    66,    66,    66,    66,    66,    66,    66,
      66,    66,    66,    66,    66,    66,    66,    66,    66,    66,
      66,    66,    66,    66,    66
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
{
       0,     2,     1,     0,     0,     0,     1,     1,     1,     1,
       4,     2,     2,     1,     2,     1,     1,     4,     2,     2,
       1,     2,     2,     0,     2,     0,     2,     2,     1,     3,
       3,     1,     1,     3,     2,     2,     2,     3,     1,     3,
      10,     3,     3,     3,     2,     1,     2,     3,     1,     5,
       3,     4,     2,     3,     3,     3,     5,     1,     1,     2,
       4,     3,     3,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
       0,     0,     0,    31,    32,     0,     3,     0,     0,     0,
      38,     0,     0,     0,     0,     0,     0,    45,     0,     0,
      48,     0,     0,     0,     0,     0,     0,     0,     0,    57,
      58,     3,     0,     0,     0,    63,    64,     0,     2,    28,
      23,    23,    23,     5,    34,    15,    16,    35,    36,    23,
      23,     0,    23,    23,    23,    44,    46,    23,     0,    23,
       0,    52,    23,    23,    23,     0,     0,    59,     0,    23,
      23,     1,    27,    29,    25,    30,    33,     5,     0,    18,
      37,    39,     0,    41,    42,    43,    47,     0,    50,     0,
      54,    53,    55,     0,     0,    62,    61,     3,    22,    25,
       5,    20,    21,     0,     0,    51,    23,    60,     4,    26,
       6,     8,     9,     7,    24,    17,    19,     0,    49,    56,
       4,     0,    11,     0,     4,    13,    14,     0,    10,    12,
       0,     0,    40
};

/* YYDEFGOTO[NTERM-NUM]. */
static const yysigned_char yydefgoto[] =
{
      -1,    37,    43,   121,    78,   109,   110,   111,   112,   124,
     122,    44,    45,    46,   100,    79,    73,    74,    98,    99,
      38,    39
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -120
static const yysigned_char yypact[] =
{
      60,     5,     6,  -120,  -120,     9,  -120,    13,    14,    16,
    -120,    18,    17,    23,    25,    26,    29,  -120,    30,    31,
    -120,    32,    33,    34,    35,    37,    38,    39,    40,  -120,
    -120,  -120,    41,    42,    45,  -120,  -120,    44,    60,  -120,
    -120,  -120,  -120,    43,  -120,  -120,  -120,  -120,  -120,  -120,
    -120,    46,  -120,  -120,  -120,  -120,  -120,  -120,    47,  -120,
      48,  -120,  -120,  -120,  -120,    51,    43,  -120,    50,  -120,
    -120,  -120,  -120,  -120,    53,  -120,  -120,  -120,    52,  -120,
    -120,  -120,    54,  -120,  -120,  -120,  -120,    55,  -120,    56,
    -120,  -120,  -120,    59,    58,  -120,  -120,  -120,  -120,    53,
      28,  -120,  -120,    99,   100,  -120,  -120,  -120,     1,  -120,
    -120,  -120,  -120,  -120,  -120,  -120,  -120,   101,  -120,  -120,
     102,    61,  -120,   103,   104,  -120,  -120,   105,  -120,  -120,
     107,   108,  -120
};

/* YYPGOTO[NTERM-NUM].  */
static const yysigned_char yypgoto[] =
{
    -120,  -120,   -31,  -120,  -120,  -120,  -120,  -120,  -120,  -120,
    -119,   -47,    27,  -120,  -120,   -73,   -39,  -120,   -34,  -120,
    -120,    71
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -6
static const short int yytable[] =
{
      66,   125,    75,    76,   101,   129,    -5,   120,    40,    41,
      80,    81,    42,    83,    84,    85,    47,    48,    86,    49,
      88,    50,    51,    90,    91,    92,    52,   116,    53,    54,
      95,    96,    55,    56,    57,   115,    59,    58,    61,    60,
      62,    63,    64,    65,    71,    69,    68,   101,    70,    77,
     113,    82,    87,    89,    93,    94,    97,   102,    67,   103,
     104,   105,   106,   107,   126,   114,   108,   119,     1,     2,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,   117,   118,   123,    -5,   127,    72,
     130,   128,   131,   132
};

static const unsigned char yycheck[] =
{
      31,   120,    41,    42,    77,   124,     5,     6,     3,     3,
      49,    50,     3,    52,    53,    54,     3,     3,    57,     3,
      59,     3,     5,    62,    63,    64,     3,   100,     3,     3,
      69,    70,     3,     3,     3,     7,     3,     5,     3,     5,
       3,     3,     3,     3,     0,     3,     5,   120,     3,     6,
      97,     5,     5,     5,     3,     5,     3,     5,    31,     5,
       5,     5,     3,     5,     3,    99,    97,   106,     8,     9,
      10,    11,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,     5,     5,     5,     5,     5,    38,
       5,     7,     5,     5
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    46,    65,    66,
       3,     3,     3,    47,    56,    57,    58,     3,     3,     3,
       3,     5,     3,     3,     3,     3,     3,     3,     5,     3,
       5,     3,     3,     3,     3,     3,    47,    57,     5,     3,
       3,     0,    66,    61,    62,    61,    61,     6,    49,    60,
      61,    61,     5,    61,    61,    61,    61,     5,    61,     5,
      61,    61,    61,     3,     5,    61,    61,     3,    63,    64,
      59,    60,     5,     5,     5,     5,     3,     5,    47,    50,
      51,    52,    53,    56,    63,     7,    60,     5,     5,    61,
       6,    48,    55,     5,    54,    55,     3,     5,     7,    55,
       5,     5,     5
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (0)


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (N)								\
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (0)
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
              (Loc).first_line, (Loc).first_column,	\
              (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (0)

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)		\
do {								\
  if (yydebug)							\
    {								\
      YYFPRINTF (stderr, "%s ", Title);				\
      yysymprint (stderr,					\
                  Type, Value);	\
      YYFPRINTF (stderr, "\n");					\
    }								\
} while (0)

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_stack_print (short int *bottom, short int *top)
#else
static void
yy_stack_print (bottom, top)
    short int *bottom;
    short int *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (/* Nothing. */; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_reduce_print (int yyrule)
#else
static void
yy_reduce_print (yyrule)
    int yyrule;
#endif
{
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu), ",
             yyrule - 1, yylno);
  /* Print the symbols being reduced, and their result.  */
  for (yyi = yyprhs[yyrule]; 0 <= yyrhs[yyi]; yyi++)
    YYFPRINTF (stderr, "%s ", yytname[yyrhs[yyi]]);
  YYFPRINTF (stderr, "-> %s\n", yytname[yyr1[yyrule]]);
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (Rule);		\
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      size_t yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

#endif /* YYERROR_VERBOSE */



#if YYDEBUG
/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yysymprint (FILE *yyoutput, int yytype, YYSTYPE *yyvaluep)
#else
static void
yysymprint (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);


# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  switch (yytype)
    {
      default:
        break;
    }
  YYFPRINTF (yyoutput, ")");
}

#endif /* ! YYDEBUG */
/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
        break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM);
# else
int yyparse ();
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */



/* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM)
# else
int yyparse (YYPARSE_PARAM)
  void *YYPARSE_PARAM;
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int
yyparse (void)
#else
int
yyparse ()
    ;
#endif
#endif
{
  
  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  short int yyssa[YYINITDEPTH];
  short int *yyss = yyssa;
  short int *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK   (yyvsp--, yyssp--)

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* When reducing, the number of symbols on the RHS of the reduced
     rule.  */
  int yylen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short int *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	short int *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

/* Do appropriate processing given the current state.  */
/* Read a look-ahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to look-ahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;


  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 155 "e:\\ETRI\\Projects\\Go!Rilla\\RealtimeRendering\\RealtimeRender\\GIRLCore\\core\\girlparse.y"
    {
;}
    break;

  case 3:
#line 159 "e:\\ETRI\\Projects\\Go!Rilla\\RealtimeRendering\\RealtimeRender\\GIRLCore\\core\\girlparse.y"
    {
	if (cur_array) ArrayFree( cur_array );
	cur_array = new ParamArray;
	cur_array->allocated = 0;
	cur_array->nelems = 0;
	cur_array->array = NULL;
	array_is_single_string = false;
;}
    break;

  case 4:
#line 169 "e:\\ETRI\\Projects\\Go!Rilla\\RealtimeRendering\\RealtimeRender\\GIRLCore\\core\\girlparse.y"
    {
	cur_array->element_size = sizeof( const char * );
;}
    break;

  case 5:
#line 174 "e:\\ETRI\\Projects\\Go!Rilla\\RealtimeRendering\\RealtimeRender\\GIRLCore\\core\\girlparse.y"
    {
	cur_array->element_size = sizeof( float );
;}
    break;

  case 6:
#line 179 "e:\\ETRI\\Projects\\Go!Rilla\\RealtimeRendering\\RealtimeRender\\GIRLCore\\core\\girlparse.y"
    {
	(yyval.ribarray) = (yyvsp[0].ribarray);
;}
    break;

  case 7:
#line 183 "e:\\ETRI\\Projects\\Go!Rilla\\RealtimeRendering\\RealtimeRender\\GIRLCore\\core\\girlparse.y"
    {
	(yyval.ribarray) = (yyvsp[0].ribarray);
;}
    break;

  case 8:
#line 188 "e:\\ETRI\\Projects\\Go!Rilla\\RealtimeRendering\\RealtimeRender\\GIRLCore\\core\\girlparse.y"
    {
	(yyval.ribarray) = (yyvsp[0].ribarray);
;}
    break;

  case 9:
#line 192 "e:\\ETRI\\Projects\\Go!Rilla\\RealtimeRendering\\RealtimeRender\\GIRLCore\\core\\girlparse.y"
    {
	(yyval.ribarray) = ArrayDup(cur_array);
	array_is_single_string = true;
;}
    break;

  case 10:
#line 198 "e:\\ETRI\\Projects\\Go!Rilla\\RealtimeRendering\\RealtimeRender\\GIRLCore\\core\\girlparse.y"
    {
	(yyval.ribarray) = ArrayDup(cur_array);
;}
    break;

  case 11:
#line 203 "e:\\ETRI\\Projects\\Go!Rilla\\RealtimeRendering\\RealtimeRender\\GIRLCore\\core\\girlparse.y"
    {
;}
    break;

  case 12:
#line 207 "e:\\ETRI\\Projects\\Go!Rilla\\RealtimeRendering\\RealtimeRender\\GIRLCore\\core\\girlparse.y"
    {
;}
    break;

  case 13:
#line 210 "e:\\ETRI\\Projects\\Go!Rilla\\RealtimeRendering\\RealtimeRender\\GIRLCore\\core\\girlparse.y"
    {
;}
    break;

  case 14:
#line 214 "e:\\ETRI\\Projects\\Go!Rilla\\RealtimeRendering\\RealtimeRender\\GIRLCore\\core\\girlparse.y"
    {
	char *to_add = strdup((yyvsp[0].string));
	AddArrayElement( &to_add );
;}
    break;

  case 15:
#line 220 "e:\\ETRI\\Projects\\Go!Rilla\\RealtimeRendering\\RealtimeRender\\GIRLCore\\core\\girlparse.y"
    {
	(yyval.ribarray) = (yyvsp[0].ribarray);
;}
    break;

  case 16:
#line 224 "e:\\ETRI\\Projects\\Go!Rilla\\RealtimeRendering\\RealtimeRender\\GIRLCore\\core\\girlparse.y"
    {
	(yyval.ribarray) = ArrayDup(cur_array);
;}
    break;

  case 17:
#line 229 "e:\\ETRI\\Projects\\Go!Rilla\\RealtimeRendering\\RealtimeRender\\GIRLCore\\core\\girlparse.y"
    {
	(yyval.ribarray) = ArrayDup(cur_array);
;}
    break;

  case 18:
#line 234 "e:\\ETRI\\Projects\\Go!Rilla\\RealtimeRendering\\RealtimeRender\\GIRLCore\\core\\girlparse.y"
    {
;}
    break;

  case 19:
#line 238 "e:\\ETRI\\Projects\\Go!Rilla\\RealtimeRendering\\RealtimeRender\\GIRLCore\\core\\girlparse.y"
    {
;}
    break;

  case 20:
#line 241 "e:\\ETRI\\Projects\\Go!Rilla\\RealtimeRendering\\RealtimeRender\\GIRLCore\\core\\girlparse.y"
    {
;}
    break;

  case 21:
#line 245 "e:\\ETRI\\Projects\\Go!Rilla\\RealtimeRendering\\RealtimeRender\\GIRLCore\\core\\girlparse.y"
    {
	float to_add = (yyvsp[0].num);
	AddArrayElement( &to_add );
;}
    break;

  case 22:
#line 251 "e:\\ETRI\\Projects\\Go!Rilla\\RealtimeRendering\\RealtimeRender\\GIRLCore\\core\\girlparse.y"
    {
;}
    break;

  case 23:
#line 255 "e:\\ETRI\\Projects\\Go!Rilla\\RealtimeRendering\\RealtimeRender\\GIRLCore\\core\\girlparse.y"
    {
	cur_paramlist_size = 0;
;}
    break;

  case 24:
#line 260 "e:\\ETRI\\Projects\\Go!Rilla\\RealtimeRendering\\RealtimeRender\\GIRLCore\\core\\girlparse.y"
    {
;}
    break;

  case 25:
#line 263 "e:\\ETRI\\Projects\\Go!Rilla\\RealtimeRendering\\RealtimeRender\\GIRLCore\\core\\girlparse.y"
    {
;}
    break;

  case 26:
#line 267 "e:\\ETRI\\Projects\\Go!Rilla\\RealtimeRendering\\RealtimeRender\\GIRLCore\\core\\girlparse.y"
    {
	void *arg = new char[ (yyvsp[0].ribarray)->nelems * (yyvsp[0].ribarray)->element_size ];
	memcpy(arg, (yyvsp[0].ribarray)->array, (yyvsp[0].ribarray)->nelems * (yyvsp[0].ribarray)->element_size);
	if (cur_paramlist_size >= cur_paramlist_allocated) {
		cur_paramlist_allocated = 2*cur_paramlist_allocated + 1;
		cur_paramlist_tokens = (const char **) realloc(cur_paramlist_tokens, cur_paramlist_allocated*sizeof(const char *) );
		cur_paramlist_args = (void * *) realloc( cur_paramlist_args, cur_paramlist_allocated*sizeof(void *) );
		cur_paramlist_sizes = (int *) realloc( cur_paramlist_sizes, cur_paramlist_allocated*sizeof(int) );
		cur_paramlist_texture_helper = (bool *) realloc( cur_paramlist_texture_helper, cur_paramlist_allocated*sizeof(bool) );
	}
	cur_paramlist_tokens[cur_paramlist_size] = (yyvsp[-1].string);
	cur_paramlist_sizes[cur_paramlist_size] = (yyvsp[0].ribarray)->nelems;
	cur_paramlist_texture_helper[cur_paramlist_size] = array_is_single_string;
	cur_paramlist_args[cur_paramlist_size++] = arg;
	ArrayFree( (yyvsp[0].ribarray) );
;}
    break;

  case 27:
#line 285 "e:\\ETRI\\Projects\\Go!Rilla\\RealtimeRendering\\RealtimeRender\\GIRLCore\\core\\girlparse.y"
    {
;}
    break;

  case 28:
#line 288 "e:\\ETRI\\Projects\\Go!Rilla\\RealtimeRendering\\RealtimeRender\\GIRLCore\\core\\girlparse.y"
    {
;}
    break;

  case 29:
#line 292 "e:\\ETRI\\Projects\\Go!Rilla\\RealtimeRendering\\RealtimeRender\\GIRLCore\\core\\girlparse.y"
    {
	ParamSet params;
	InitParamSet(params, CPS, CPT, CPA, CPSZ, CPTH);
	Context::girlAccelerator((yyvsp[-1].string), params);
	FreeArgs();
;}
    break;

  case 30:
#line 299 "e:\\ETRI\\Projects\\Go!Rilla\\RealtimeRendering\\RealtimeRender\\GIRLCore\\core\\girlparse.y"
    {
	ParamSet params;
	InitParamSet(params, CPS, CPT, CPA, CPSZ, CPTH);
	Context::girlAreaLightSource((yyvsp[-1].string), params);
	FreeArgs();
;}
    break;

  case 31:
#line 306 "e:\\ETRI\\Projects\\Go!Rilla\\RealtimeRendering\\RealtimeRender\\GIRLCore\\core\\girlparse.y"
    {
	Context::girlAttributeBegin();
;}
    break;

  case 32:
#line 310 "e:\\ETRI\\Projects\\Go!Rilla\\RealtimeRendering\\RealtimeRender\\GIRLCore\\core\\girlparse.y"
    {
	Context::girlAttributeEnd();
;}
    break;

  case 33:
#line 314 "e:\\ETRI\\Projects\\Go!Rilla\\RealtimeRendering\\RealtimeRender\\GIRLCore\\core\\girlparse.y"
    {
	ParamSet params;
	InitParamSet(params, CPS, CPT, CPA, CPSZ, CPTH);
	Context::girlCamera((yyvsp[-1].string), params);
	FreeArgs();
;}
    break;

  case 34:
#line 321 "e:\\ETRI\\Projects\\Go!Rilla\\RealtimeRendering\\RealtimeRender\\GIRLCore\\core\\girlparse.y"
    {
	if (VerifyArrayLength( (yyvsp[0].ribarray), 16, "ConcatTransform" ))
		Context::girlConcatTransform( (float *) (yyvsp[0].ribarray)->array );
	ArrayFree( (yyvsp[0].ribarray) );
;}
    break;

  case 35:
#line 327 "e:\\ETRI\\Projects\\Go!Rilla\\RealtimeRendering\\RealtimeRender\\GIRLCore\\core\\girlparse.y"
    {
	Context::girlCoordinateSystem( (yyvsp[0].string) );
;}
    break;

  case 36:
#line 331 "e:\\ETRI\\Projects\\Go!Rilla\\RealtimeRendering\\RealtimeRender\\GIRLCore\\core\\girlparse.y"
    {
	Context::girlCoordSysTransform( (yyvsp[0].string) );
;}
    break;

  case 37:
#line 335 "e:\\ETRI\\Projects\\Go!Rilla\\RealtimeRendering\\RealtimeRender\\GIRLCore\\core\\girlparse.y"
    {
	ParamSet params;
	InitParamSet(params, CPS, CPT, CPA, CPSZ, CPTH);
	Context::girlFilm((yyvsp[-1].string), params);
	FreeArgs();
;}
    break;

  case 38:
#line 342 "e:\\ETRI\\Projects\\Go!Rilla\\RealtimeRendering\\RealtimeRender\\GIRLCore\\core\\girlparse.y"
    {
	Context::girlIdentity();
;}
    break;

  case 39:
#line 346 "e:\\ETRI\\Projects\\Go!Rilla\\RealtimeRendering\\RealtimeRender\\GIRLCore\\core\\girlparse.y"
    {
	ParamSet params;
	InitParamSet(params, CPS, CPT, CPA, CPSZ, CPTH);
	Context::girlLightSource((yyvsp[-1].string), params);
	FreeArgs();
;}
    break;

  case 40:
#line 353 "e:\\ETRI\\Projects\\Go!Rilla\\RealtimeRendering\\RealtimeRender\\GIRLCore\\core\\girlparse.y"
    {
	Context::girlLookAt((yyvsp[-8].num), (yyvsp[-7].num), (yyvsp[-6].num), (yyvsp[-5].num), (yyvsp[-4].num), (yyvsp[-3].num), (yyvsp[-2].num), (yyvsp[-1].num), (yyvsp[0].num));
;}
    break;

  case 41:
#line 357 "e:\\ETRI\\Projects\\Go!Rilla\\RealtimeRendering\\RealtimeRender\\GIRLCore\\core\\girlparse.y"
    {
	ParamSet params;
	InitParamSet(params, CPS, CPT, CPA, CPSZ, CPTH);
	Context::girlMaterial((yyvsp[-1].string), params);
	FreeArgs();
;}
    break;

  case 42:
#line 364 "e:\\ETRI\\Projects\\Go!Rilla\\RealtimeRendering\\RealtimeRender\\GIRLCore\\core\\girlparse.y"
    {
	ParamSet params;
	InitParamSet(params, CPS, CPT, CPA, CPSZ, CPTH);
	Context::girlMakeNamedMaterial((yyvsp[-1].string), params);
	FreeArgs();
;}
    break;

  case 43:
#line 371 "e:\\ETRI\\Projects\\Go!Rilla\\RealtimeRendering\\RealtimeRender\\GIRLCore\\core\\girlparse.y"
    {
	ParamSet params;
	InitParamSet(params, CPS, CPT, CPA, CPSZ, CPTH);
	Context::girlNamedMaterial((yyvsp[-1].string), params);
	FreeArgs();
;}
    break;

  case 44:
#line 378 "e:\\ETRI\\Projects\\Go!Rilla\\RealtimeRendering\\RealtimeRender\\GIRLCore\\core\\girlparse.y"
    {
	Context::girlObjectBegin((yyvsp[0].string));
;}
    break;

  case 45:
#line 382 "e:\\ETRI\\Projects\\Go!Rilla\\RealtimeRendering\\RealtimeRender\\GIRLCore\\core\\girlparse.y"
    {
	Context::girlObjectEnd();
;}
    break;

  case 46:
#line 386 "e:\\ETRI\\Projects\\Go!Rilla\\RealtimeRendering\\RealtimeRender\\GIRLCore\\core\\girlparse.y"
    {
	Context::girlObjectInstance((yyvsp[0].string));
;}
    break;

  case 47:
#line 390 "e:\\ETRI\\Projects\\Go!Rilla\\RealtimeRendering\\RealtimeRender\\GIRLCore\\core\\girlparse.y"
    {
	ParamSet params;
	InitParamSet(params, CPS, CPT, CPA, CPSZ, CPTH);
	Context::girlPixelFilter((yyvsp[-1].string), params);
	FreeArgs();
;}
    break;

  case 48:
#line 397 "e:\\ETRI\\Projects\\Go!Rilla\\RealtimeRendering\\RealtimeRender\\GIRLCore\\core\\girlparse.y"
    {
	Context::girlReverseOrientation();
;}
    break;

  case 49:
#line 401 "e:\\ETRI\\Projects\\Go!Rilla\\RealtimeRendering\\RealtimeRender\\GIRLCore\\core\\girlparse.y"
    {
	Context::girlRotate((yyvsp[-3].num), (yyvsp[-2].num), (yyvsp[-1].num), (yyvsp[0].num));
;}
    break;

  case 50:
#line 405 "e:\\ETRI\\Projects\\Go!Rilla\\RealtimeRendering\\RealtimeRender\\GIRLCore\\core\\girlparse.y"
    {
	ParamSet params;
	InitParamSet(params, CPS, CPT, CPA, CPSZ, CPTH);
	Context::girlSampler((yyvsp[-1].string), params);
	FreeArgs();
;}
    break;

  case 51:
#line 412 "e:\\ETRI\\Projects\\Go!Rilla\\RealtimeRendering\\RealtimeRender\\GIRLCore\\core\\girlparse.y"
    {
	Context::girlScale((yyvsp[-2].num), (yyvsp[-1].num), (yyvsp[0].num));
;}
    break;

  case 52:
#line 416 "e:\\ETRI\\Projects\\Go!Rilla\\RealtimeRendering\\RealtimeRender\\GIRLCore\\core\\girlparse.y"
    {
	;//girlSearchPath($2);
;}
    break;

  case 53:
#line 420 "e:\\ETRI\\Projects\\Go!Rilla\\RealtimeRendering\\RealtimeRender\\GIRLCore\\core\\girlparse.y"
    {
	ParamSet params;
	InitParamSet(params, CPS, CPT, CPA, CPSZ, CPTH);
	Context::girlShape((yyvsp[-1].string), params);
	FreeArgs();
;}
    break;

  case 54:
#line 427 "e:\\ETRI\\Projects\\Go!Rilla\\RealtimeRendering\\RealtimeRender\\GIRLCore\\core\\girlparse.y"
    {
	ParamSet params;
	InitParamSet(params, CPS, CPT, CPA, CPSZ, CPTH);
	Context::girlPortalShape((yyvsp[-1].string), params);
	FreeArgs();
;}
    break;

  case 55:
#line 434 "e:\\ETRI\\Projects\\Go!Rilla\\RealtimeRendering\\RealtimeRender\\GIRLCore\\core\\girlparse.y"
    {
	ParamSet params;
	InitParamSet(params, CPS, CPT, CPA, CPSZ, CPTH);
	Context::girlSurfaceIntegrator((yyvsp[-1].string), params);
	FreeArgs();
;}
    break;

  case 56:
#line 441 "e:\\ETRI\\Projects\\Go!Rilla\\RealtimeRendering\\RealtimeRender\\GIRLCore\\core\\girlparse.y"
    {
	ParamSet params;
	InitParamSet(params, CPS, CPT, CPA, CPSZ, CPTH);
	Context::girlTexture((yyvsp[-3].string), (yyvsp[-2].string), (yyvsp[-1].string), params);
	FreeArgs();
;}
    break;

  case 57:
#line 448 "e:\\ETRI\\Projects\\Go!Rilla\\RealtimeRendering\\RealtimeRender\\GIRLCore\\core\\girlparse.y"
    {
	Context::girlTransformBegin();
;}
    break;

  case 58:
#line 452 "e:\\ETRI\\Projects\\Go!Rilla\\RealtimeRendering\\RealtimeRender\\GIRLCore\\core\\girlparse.y"
    {
	Context::girlTransformEnd();
;}
    break;

  case 59:
#line 456 "e:\\ETRI\\Projects\\Go!Rilla\\RealtimeRendering\\RealtimeRender\\GIRLCore\\core\\girlparse.y"
    {
	if (VerifyArrayLength( (yyvsp[0].ribarray), 16, "Transform" ))
		Context::girlTransform( (float *) (yyvsp[0].ribarray)->array );
	ArrayFree( (yyvsp[0].ribarray) );
;}
    break;

  case 60:
#line 462 "e:\\ETRI\\Projects\\Go!Rilla\\RealtimeRendering\\RealtimeRender\\GIRLCore\\core\\girlparse.y"
    {
	girlTranslate((yyvsp[-2].num), (yyvsp[-1].num), (yyvsp[0].num));
;}
    break;

  case 61:
#line 466 "e:\\ETRI\\Projects\\Go!Rilla\\RealtimeRendering\\RealtimeRender\\GIRLCore\\core\\girlparse.y"
    {
	ParamSet params;
	InitParamSet(params, CPS, CPT, CPA, CPSZ, CPTH);
	Context::girlVolumeIntegrator((yyvsp[-1].string), params);
	FreeArgs();
;}
    break;

  case 62:
#line 473 "e:\\ETRI\\Projects\\Go!Rilla\\RealtimeRendering\\RealtimeRender\\GIRLCore\\core\\girlparse.y"
    {
	ParamSet params;
	InitParamSet(params, CPS, CPT, CPA, CPSZ, CPTH);
	Context::girlVolume((yyvsp[-1].string), params);
	FreeArgs();
;}
    break;

  case 63:
#line 480 "e:\\ETRI\\Projects\\Go!Rilla\\RealtimeRendering\\RealtimeRender\\GIRLCore\\core\\girlparse.y"
    {
	Context::girlWorldBegin();
;}
    break;

  case 64:
#line 484 "e:\\ETRI\\Projects\\Go!Rilla\\RealtimeRendering\\RealtimeRender\\GIRLCore\\core\\girlparse.y"
    {
	Context::girlWorldEnd();
;}
    break;


      default: break;
    }

/* Line 1126 of yacc.c.  */
#line 1926 "e:\\ETRI\\Projects\\Go!Rilla\\RealtimeRendering\\RealtimeRender\\GIRLCore\\core\\/girlparse.cpp"

  yyvsp -= yylen;
  yyssp -= yylen;


  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (YYPACT_NINF < yyn && yyn < YYLAST)
	{
	  int yytype = YYTRANSLATE (yychar);
	  YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
	  YYSIZE_T yysize = yysize0;
	  YYSIZE_T yysize1;
	  int yysize_overflow = 0;
	  char *yymsg = 0;
#	  define YYERROR_VERBOSE_ARGS_MAXIMUM 5
	  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
	  int yyx;

#if 0
	  /* This is so xgettext sees the translatable formats that are
	     constructed on the fly.  */
	  YY_("syntax error, unexpected %s");
	  YY_("syntax error, unexpected %s, expecting %s");
	  YY_("syntax error, unexpected %s, expecting %s or %s");
	  YY_("syntax error, unexpected %s, expecting %s or %s or %s");
	  YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
#endif
	  char *yyfmt;
	  char const *yyf;
	  static char const yyunexpected[] = "syntax error, unexpected %s";
	  static char const yyexpecting[] = ", expecting %s";
	  static char const yyor[] = " or %s";
	  char yyformat[sizeof yyunexpected
			+ sizeof yyexpecting - 1
			+ ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
			   * (sizeof yyor - 1))];
	  char const *yyprefix = yyexpecting;

	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  int yyxbegin = yyn < 0 ? -yyn : 0;

	  /* Stay within bounds of both yycheck and yytname.  */
	  int yychecklim = YYLAST - yyn;
	  int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
	  int yycount = 1;

	  yyarg[0] = yytname[yytype];
	  yyfmt = yystpcpy (yyformat, yyunexpected);

	  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      {
		if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
		  {
		    yycount = 1;
		    yysize = yysize0;
		    yyformat[sizeof yyunexpected - 1] = '\0';
		    break;
		  }
		yyarg[yycount++] = yytname[yyx];
		yysize1 = yysize + yytnamerr (0, yytname[yyx]);
		yysize_overflow |= yysize1 < yysize;
		yysize = yysize1;
		yyfmt = yystpcpy (yyfmt, yyprefix);
		yyprefix = yyor;
	      }

	  yyf = YY_(yyformat);
	  yysize1 = yysize + yystrlen (yyf);
	  yysize_overflow |= yysize1 < yysize;
	  yysize = yysize1;

	  if (!yysize_overflow && yysize <= YYSTACK_ALLOC_MAXIMUM)
	    yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg)
	    {
	      /* Avoid sprintf, as that infringes on the user's name space.
		 Don't have undefined behavior even if the translation
		 produced a string with the wrong number of "%s"s.  */
	      char *yyp = yymsg;
	      int yyi = 0;
	      while ((*yyp = *yyf))
		{
		  if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		    {
		      yyp += yytnamerr (yyp, yyarg[yyi++]);
		      yyf += 2;
		    }
		  else
		    {
		      yyp++;
		      yyf++;
		    }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    {
	      yyerror (YY_("syntax error"));
	      goto yyexhaustedlab;
	    }
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror (YY_("syntax error"));
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
        {
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
        }
      else
	{
	  yydestruct ("Error: discarding", yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (0)
     goto yyerrorlab;

yyvsp -= yylen;
  yyssp -= yylen;
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping", yystos[yystate], yyvsp);
      YYPOPSTACK;
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;


  /* Shift the error token. */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK;
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}


#line 487 "e:\\ETRI\\Projects\\Go!Rilla\\RealtimeRendering\\RealtimeRender\\GIRLCore\\core\\girlparse.y"

static void InitParamSet(ParamSet &ps, int count, const char **tokens,
		void **args, int *sizes, bool *texture_helper) {
	ps.Clear();
	for (int i = 0; i < count; ++i) {
		int type;
		string name;
		if (lookupType(tokens[i], &type, name)) {
			if (texture_helper && texture_helper[i] && type != PARAM_TYPE_TEXTURE && type != PARAM_TYPE_STRING)
			{
				std::stringstream ss;
				ss<<"Bad type for "<<name<<". Changing it to a texture.";
				girlError( GIRL_SYNTAX,GIRL_WARNING,ss.str().c_str());
				//Warning( "Bad type for %s. Changing it to a texture.", name.c_str());
				type = PARAM_TYPE_TEXTURE;
			}
			void *data = args[i];
			int nItems = sizes[i];
			if (type == PARAM_TYPE_INT) {
				// parser doesn't handle ints, so convert from floats here....
				int nAlloc = sizes[i];
				int *idata = new int[nAlloc];
				float *fdata = (float *)data;
				for (int j = 0; j < nAlloc; ++j)
					idata[j] = int(fdata[j]);
				ps.AddInt(name, idata, nItems);
				delete[] idata;
			}
			else if (type == PARAM_TYPE_BOOL) {
				// strings -> bools
				int nAlloc = sizes[i];
				bool *bdata = new bool[nAlloc];
				for (int j = 0; j < nAlloc; ++j) {
					string s(*((const char **)data));
					if (s == "true") bdata[j] = true;
					else if (s == "false") bdata[j] = false;
					else {
						std::stringstream ss;
						ss<<"Value '"<<s<<"' unknown for boolean parameter '"<<tokens[i]<<"'. Using 'false'.";
						girlError( GIRL_SYNTAX,GIRL_WARNING,ss.str().c_str());
						//Warning("Value \"%s\" unknown for boolean parameter \"%s\"."
						//	"Using \"false\".", s.c_str(), tokens[i]);
						bdata[j] = false;
					}
				}
				ps.AddBool(name, bdata, nItems);
				delete[] bdata;
			}
			else if (type == PARAM_TYPE_FLOAT) {
				ps.AddFloat(name, (float *)data, nItems);
			} else if (type == PARAM_TYPE_POINT) {
				ps.AddPoint(name, (Point *)data, nItems / 3);
			} else if (type == PARAM_TYPE_VECTOR) {
				ps.AddVector(name, (Vector *)data, nItems / 3);
			} else if (type == PARAM_TYPE_NORMAL) {
				ps.AddNormal(name, (Normal *)data, nItems / 3);
			} else if (type == PARAM_TYPE_COLOR) {
				ps.AddRGBColor(name, (RGBColor *)data, nItems / COLOR_SAMPLES);
			} else if (type == PARAM_TYPE_STRING) {
				string *strings = new string[nItems];
				for (int j = 0; j < nItems; ++j)
					strings[j] = string(*((const char **)data+j));
				ps.AddString(name, strings, nItems);
				delete[] strings;
			}
			else if (type == PARAM_TYPE_TEXTURE) {
				if (nItems == 1) {
					string val(*((const char **)data));
					ps.AddTexture(name, val);
				}
				else
				{
						//Error("Only one string allowed for \"texture\" parameter \"%s\"", name.c_str());
						std::stringstream ss;
						ss<<"Only one string allowed for 'texture' parameter "<<name;
						girlError( GIRL_SYNTAX,GIRL_ERROR,ss.str().c_str());
				}
			}
		}
		else
		{
			//Warning("Type of parameter \"%s\" is unknown", tokens[i]);
			std::stringstream ss;
			ss<<"Type of parameter '"<<tokens[i]<<"' is unknown";
			girlError( GIRL_SYNTAX,GIRL_WARNING,ss.str().c_str());
		}
	}
}
static bool lookupType(const char *token, int *type, string &name) {
	BOOST_ASSERT(token != NULL);
	*type = 0;
	const char *strp = token;
	while (*strp && isspace(*strp))
		++strp;
	if (!*strp) {
		//Error("Parameter \"%s\" doesn't have a type declaration?!", token);
		std::stringstream ss;
		ss<<"Parameter '"<<token<<"' doesn't have a type declaration?!";
		girlError( GIRL_SYNTAX,GIRL_ERROR,ss.str().c_str());
		return false;
	}
	#define TRY_DECODING_TYPE(name, mask) \
		if (strncmp(name, strp, strlen(name)) == 0) { \
			*type = mask; strp += strlen(name); \
		}
	     TRY_DECODING_TYPE("float",    PARAM_TYPE_FLOAT)
	else TRY_DECODING_TYPE("integer",  PARAM_TYPE_INT)
	else TRY_DECODING_TYPE("bool",     PARAM_TYPE_BOOL)
	else TRY_DECODING_TYPE("point",    PARAM_TYPE_POINT)
	else TRY_DECODING_TYPE("vector",   PARAM_TYPE_VECTOR)
	else TRY_DECODING_TYPE("normal",   PARAM_TYPE_NORMAL)
	else TRY_DECODING_TYPE("string",   PARAM_TYPE_STRING)
	else TRY_DECODING_TYPE("texture",  PARAM_TYPE_TEXTURE)
	else TRY_DECODING_TYPE("color",    PARAM_TYPE_COLOR)
	else {
		//Error("Unable to decode type for token \"%s\"", token);
		std::stringstream ss;
		ss<<"Unable to decode type for token '"<<token<<"'";
		girlError( GIRL_SYNTAX,GIRL_ERROR,ss.str().c_str());
		return false;
	}
	while (*strp && isspace(*strp))
		++strp;
	name = string(strp);
	return true;
}

