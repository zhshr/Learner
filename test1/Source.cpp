#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <list>
#include "tinyxml2.h"
#define WINDOWS
#ifdef WINDOWS
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif


using namespace std;
using namespace tinyxml2;

struct Def {
	string word;
	string definition;
	int64_t creator;
};

list<Def> defs;


void main() {
	cout << string("£¡ÏÔÊ¾").length();
	string line;
	int i = 0;
		char cCurrentPath[FILENAME_MAX];

		if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath)))
		{
	//		return errno;
		}

		cCurrentPath[sizeof(cCurrentPath) - 1] = '\0'; /* not really required */

		printf("The current working directory is %s", cCurrentPath);
	ifstream myxml("LearnerDefs.xml");
		
	defs.clear();
	XMLDocument doc;
	if (doc.LoadFile("LearnerDefs.xml")==0) {
		doc.Print();
		XMLElement *root = doc.RootElement();
		for (XMLElement *child = root->FirstChildElement(); child; child = child->NextSiblingElement()) {
			Def *def = new Def();
			def->word = child->Attribute("word");
			def->creator = std::stoi(child->Attribute("creator"));
			def->definition = child->Value();
			defs.push_back(*def);			
			i++;
		}
	}
	list<Def>::iterator p;
	for (p = defs.begin(); p != defs.end(); p++) {
		cout << p->word << "," << p->definition << "," << p->creator << "\n";
	}
	getchar();
}
