/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   parser.cpp
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - parser.cpp initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

// parser.cpp*
#include "girl.h"
#include "error.h"
// Parsing Global Interface
extern GIRLCOREDLL bool ParseFile(const char *filename) {
	extern FILE *yyin;
	extern int yyparse(void);
	extern string current_file;
	extern int line_num;

	/*extern int yydebug;

	if (getenv("GIRL_YYDEBUG") != NULL)
		yydebug = 1;*/

	if (strcmp(filename, "-") == 0)
		yyin = stdin;
	else
		yyin = fopen(filename, "r");
	if (yyin != NULL) {
		current_file = filename;
		if (yyin == stdin) current_file = "<standard input>";
		line_num = 1;
		yyparse();
		if (yyin != stdin) fclose(yyin);
	} else {
		std::stringstream ss;
		ss<<"Unable to read scenefile '"<<filename<<"'";
		girlError(GIRL_NOFILE, GIRL_SEVERE, ss.str ().c_str());
	}

	current_file = "";
	line_num = 0;
	return (yyin != NULL);
}
