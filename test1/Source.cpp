#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "rapidxml.hpp"
#define WINDOWS
#ifdef WINDOWS
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif


using namespace std;
using namespace rapidxml;
void main() {
	string line;
	int i = 0;
	/*
	if (myfile.is_open()) {
	while (getline(myfile, line)) {
	i++;
	Def def;
	def.word = line;
	getline(myfile, line);
	def.definition = line;
	defs.push_back(def);
	}
	}*/
	

		char cCurrentPath[FILENAME_MAX];

		if (!GetCurrentDir(cCurrentPath, sizeof(cCurrentPath)))
		{
	//		return errno;
		}

		cCurrentPath[sizeof(cCurrentPath) - 1] = '\0'; /* not really required */

		printf("The current working directory is %s", cCurrentPath);
	ifstream myxml("LearnerDefs.xml");
	cout << "open file\n";
	if (myxml.is_open()) {
		stringstream buffer;
		buffer << myxml.rdbuf();
		xml_document<> xml;
		string st = buffer.str();
		char *str = (char*)st.c_str();
		xml.parse<0>(str);
		xml_node<> *root = xml.first_node();
		if (root) {
			xml_node<> *defnode = root->first_node();
			while (defnode) {
				//Def def;
				i++;
				/*def.definition = defnode->value();
				def.word = defnode->first_attribute()->value();
				CQ_addLog(ac, CQLOG_INFO, def.word.c_str(), def.definition.c_str());
				defs.push_back(def);*/
				cout << defnode->first_attribute()->value() << "  " << defnode->value() << "\n";
				defnode = defnode->next_sibling();
			}
		}

	}
	getchar();
}
