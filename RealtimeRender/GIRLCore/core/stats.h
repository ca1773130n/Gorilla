/////////////// GIRL (Global Ilummination Renderer Library) Core //////////////////
//	FileName:   stats.h
//	Author	:   Sung-Soo Kim
//	Purpose :   
//	Modification History
//  ====================
//	Date    :	Modifications
//	=========================== 
//	2008-10-04  :   First version - stats.h initial implementation
//
//	E-mail  :  sungsoo@etri.re.kr
//	Copyright(C) 2008 Electronics and Telecommunication Research Institute All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////////

#ifndef GIRL_STATS_H
#define GIRL_STATS_H

// stats.h*

#include "girl.h"

namespace girl
{
  void StatsPrint(FILE *dest);
  void StatsCleanup();
}

class GIRLCOREDLL  ProgressReporter {
public:
	// ProgressReporter Public Methods
	ProgressReporter(int totalWork, const string &title, int barLength=58);
	~ProgressReporter();
	void Update(int num = 1) const;
	void Done() const;
	// ProgressReporter Data
	const int totalPlusses;
	float frequency;
	mutable float count;
	mutable int plussesPrinted;
	mutable Timer *timer;
	FILE *outFile;
	char *buf;
	mutable char *curSpace;
};
class GIRLCOREDLL  StatsCounter {
public:
	// StatsCounter Public Methods
	StatsCounter(const string &category, const string &name);
	void operator++() { ++num; }
	void operator++(int) { ++num; }
	void Max(StatsCounterType val) { num = max(val, num); }
	void Min(StatsCounterType val) { num = min(val, num); }
	operator double() const { return (double)num; }
private:
	// StatsCounter Private Data
	StatsCounterType num;
};
class GIRLCOREDLL  StatsRatio {
public:
	// StatsRatio Public Methods
	StatsRatio(const string &category, const string &name);
	void Add(int a, int b) { na += a; nb += b; }
private:
	// StatsRatio Private Data
	StatsCounterType na, nb;
};
class GIRLCOREDLL  StatsPercentage {
public:
	// StatsPercentage Public Methods
	void Add(int a, int b) { na += a; nb += b; }
	StatsPercentage(const string &category, const string &name);
private:
	// StatsPercentage Private Data
	StatsCounterType na, nb;
};

#endif // GIRL_STATS_H

