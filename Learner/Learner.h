// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the LEARNER_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// LEARNER_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef LEARNER_EXPORTS
#define LEARNER_API __declspec(dllexport)
#else
#define LEARNER_API __declspec(dllimport)
#endif

#include <list>
#include "util.h"
using namespace std;
struct Def {
	string word;
	string definition;
	int64_t creator;
};
extern list<Def> defs;
int saveDefs(bool force);
int readDefs();
int ProcessPrivate(int32_t subType, int32_t sendTime, int64_t fromQQ, const char *msg, int32_t font);
int ProcessGroup(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, const char *fromAnonymous, const char *msg, int32_t font);