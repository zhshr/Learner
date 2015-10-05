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

typedef rapidxml::xml_node<wchar_t> const *      xml_node_cptr;
typedef rapidxml::xml_node<wchar_t> *            xml_node_ptr;
typedef rapidxml::xml_attribute<wchar_t> const * xml_attribute_cptr;
typedef rapidxml::xml_attribute<wchar_t> *       xml_attribute_ptr;
typedef rapidxml::xml_document<wchar_t>          xml_doc;

struct Def {
	string word;
	string definition;
	int64_t creator;
};
Def *xmlNodeToDef(xml_node<> *node) {
	Def *def = (Def*)calloc(1, sizeof(Def));
	def->definition = node->value();
	xml_attribute<> *attr = node->first_attribute();
	def->word = attr->value();
	xml_attribute<> *next = attr->next_attribute();
	if (next) {
		char* creator = next->value();
		if (strcmp(creator, "") == 0) {
			def->creator = 0;
		}
		else {
			def->creator = std::stoi(creator);
		}
	}
	else {
		def->creator = 0;
	}
	return def;
}
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
		
	if (myxml.is_open()) {
		stringstream buffer;
		buffer << myxml.rdbuf();
		xml_document<> xml;
		string st = buffer.str();
		char *str = (char*)st.c_str();
		cout << str;
		xml.parse<0>(str);
		xml_node<> *root = xml.first_node();
		if (root) {
			xml_node<> *defnode = root->first_node();
			while (defnode) {
				Def *def = xmlNodeToDef(defnode);
				i++;
				cout << def->word << "," << def->definition << "," << def->creator << "\n";
				defnode = defnode->next_sibling();
			}
		}

	}
	getchar();
}
