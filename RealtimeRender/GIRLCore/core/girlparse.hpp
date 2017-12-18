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




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
#line 132 "e:\\ETRI\\Projects\\Go!Rilla\\RealtimeRendering\\RealtimeRender\\GIRLCore\\core\\girlparse.y"
typedef union YYSTYPE {
char string[1024];
float num;
ParamArray *ribarray;
} YYSTYPE;
/* Line 1403 of yacc.c.  */
#line 132 "e:\\ETRI\\Projects\\Go!Rilla\\RealtimeRendering\\RealtimeRender\\GIRLCore\\core\\/girlparse.hpp"
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;



