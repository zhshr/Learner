// Learner.cpp : Defines the exported functions for the DLL application.
//



#include "stdafx.h"
#include "Learner.h"
#include "appmain.h"
#include "cqp.h"
#include <list>
#include <map>
#include <iostream>
#include <fstream>
#include "util.h"
#include <sstream>
#include "tinyxml2.h"


using namespace std;
using namespace tinyxml2;
class CLearn {
public:
	static const int STATE_WAITFORDEFINITION = 1;
	int64_t fromQQ;
	int64_t fromGroup;
	int state;
	string word;
	string definition;
};

map<int64_t, CLearn> waitlist;
list<Def> defs;
int saveCount = 0;
int readDefs() {
	int i = 0;
	defs.clear();
	XMLDocument doc;
	CQ_addLog(ac, CQLOG_DEBUG, "学习者","Load XML start");
	if (doc.LoadFile("LearnerDefs.xml")==0) {
		XMLElement *root = doc.RootElement();
		for (XMLElement *child = root->FirstChildElement(); child; child = child->NextSiblingElement()) {
			Def *def = new Def();
			def->word = child->Attribute("word");
			def->creator = std::stoi(child->Attribute("creator"));
			def->definition = child->FirstChild()->ToText()->Value();
			defs.push_back(*def);
			CQ_addLog(ac, CQLOG_DEBUG, "载入词条", (def->word + "," + def->definition + "," + std::to_string(def->creator)).c_str());
			i++;
		}
	}
	CQ_addLog(ac, CQLOG_INFO, "载入词条", std::to_string(i).c_str());
	return 0;
}
int saveDefs(bool force) {
	CQ_addLog(ac, CQLOG_INFO, "保存计数", std::to_string(saveCount).c_str());
	saveCount++;
	int i = 0;
	if (saveCount % 5 == 0 || force) {		
		XMLDocument *doc = new XMLDocument();
		XMLElement *root = doc->NewElement("Defs");		
		list<Def>::iterator p;
		for (p = defs.begin(); p != defs.end(); p++) {
			XMLElement *def = doc->NewElement("def");
			def->InsertFirstChild(doc->NewText(p->definition.c_str()));
			def->SetAttribute("word", p->word.c_str());
			def->SetAttribute("creator", std::to_string(p->creator).c_str());
			CQ_addLog(ac, CQLOG_DEBUG, "保存词条", p->definition.c_str());
			i++;
			root->InsertEndChild(def);
		}
		doc->InsertFirstChild(root);
		doc->SaveFile("LearnerDefs.xml");
	}
	return i;
}
wstring GetWstringFromChar(const char* ch) {
	string str = string(ch);
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
	std::wstring wstrTo(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
	return wstrTo;
}

char* GetCharFromWstring(wstring str) {
	size_t i;
	int buffer_size = str.length() + 1;
	char* target = (char*)malloc(buffer_size);
	wcstombs_s(&i, target, (size_t)buffer_size,
		str.c_str(), (size_t)buffer_size);
	return target;
}
/*
void SendBackMessage(int64_t fromGroup, int64_t fromQQ, const char*msg) {
	if (fromGroup < 0) {
		CQ_sendPrivateMsg(ac, fromQQ, msg);
	}
	else {
		CQ_sendGroupMsg(ac, fromGroup, msg);
	}
}*/
int ProcessMessage(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, const char *fromAnonymous, const char *msg, int32_t font) {
	if (ListPlugins(fromGroup, fromQQ, msg)) {
		return EVENT_IGNORE;
	}
	string str = string(msg);
	map<int64_t, CLearn>::iterator p;
	list<Def>::iterator lp;
	if ((p = waitlist.find(fromQQ)) != waitlist.end()) {
		//发现等待中的定义
		CLearn learn = p->second;
		if (learn.fromQQ == fromQQ && learn.fromGroup == fromGroup) {
			waitlist.erase(p);
			Def def;
			def.word = learn.word;
			def.definition = str;
			def.creator = fromQQ;
			defs.push_back(def);
			CQ_addLog(ac, CQLOG_INFO, "发现定义", str.c_str());
			sendQQMessage(fromGroup, fromQQ, ("“" + def.word + "”已被定义为“" + def.definition + "”").c_str());
			saveDefs(false);
			return EVENT_BLOCK;
		}		
	}
	if (str.substr(0, 6) == "！定义") {
		CLearn learn;
		learn.word = str.substr(6, str.length());
		if (learn.word.length() < 1) {
			CQ_sendPrivateMsg(ac, fromQQ, "定义词汇长度不能为0");
		}
		else {
			string result;
			bool flag = false;
			for (lp = defs.begin(); lp != defs.end(); lp++) {
				if (lp->word == learn.word) {
					flag = true;
					result = result + "对“" + lp->word + "”的定义有“" + lp->definition + "”\n";
				}
			}
			if (flag) {
				result = result.erase(result.size() - 1);
				sendQQMessage(fromGroup, fromQQ, result.c_str());
			}
			
			learn.fromQQ = fromQQ;
			learn.fromGroup = fromGroup;
			learn.state = CLearn::STATE_WAITFORDEFINITION;
			waitlist.insert(pair<int, CLearn>(fromQQ, learn));
			sendQQMessage(fromGroup, fromQQ, ("请输入对“" + learn.word + "”的定义").c_str());
		}
//		saveDefs();
		return EVENT_BLOCK;
	}
	if (str.substr(0, 6) == "！擦除") {
		CLearn learn;
		learn.word = str.substr(6, str.length());
		if (learn.word.length() < 1) {
			sendQQMessage(fromGroup, fromQQ, "定义词汇长度不能为0");
		}
		else {
			string result;
			bool flag = false;
			for (lp = defs.begin(); lp != defs.end(); lp++) {
				if (lp->word == learn.word && (lp->creator==fromQQ || lp->creator==0)) {
					flag = true;
					result = result + "对“" + lp->word + "”的定义有“" + lp->definition + "”，已擦除\n";
					defs.erase(lp);
				}
			}
			if (flag) {
				result = result.erase(result.size() - 1);
				sendQQMessage(fromGroup, fromQQ, result.c_str());
			}
			
		}
		saveDefs(false);
		return EVENT_BLOCK;
	}
	if (str.substr(0, 6) == "！显示") {
		string result;
		if (str.length() == 6) {
			//显示自己的定义
		int j = 0;
		bool flag = false;
		for (lp = defs.begin(); lp != defs.end(); lp++) {
				if (lp->creator == fromQQ) {
					flag = true;
					j++;
					result = result + "“" + lp->word + "”对应“" + lp->definition + "”\n";
				}
			}
			if (flag) {
				result = "由" + std::to_string(fromQQ) + "创建的定义有：\n" + result;
				result = result.erase(result.size() - 1);
			}
			else {
				result = std::to_string(fromQQ) + "没有创建任何定义";
			}			
			}
		else {
			string word = str.substr(6, str.length());
			CQ_addLog(ac, CQLOG_DEBUG, "显示", word.c_str());
			bool flag = false;
			int j = 0;
			for (lp = defs.begin(); lp != defs.end(); lp++) {
				if (lp->word == word) {
			flag = true;
			j++;
					result = result + std::to_string(lp->creator) + ":“" + lp->definition + "”\n";
				}
		}
		if (flag) {
				result = "对“" + word + "”的定义有：\n" + result;
			result = result.erase(result.size() - 1);
			}
			else {
				result = "“" + word + "”暂时没有任何定义";
			}

		}
			sendQQMessage(fromGroup, fromQQ, result.c_str());
		return EVENT_BLOCK;
	}
	if (str.substr(0, 6) == "！保存"){
		sendQQMessage(fromGroup, fromQQ, ("共保存" + std::to_string(saveDefs(true)) + "条词条").c_str());
		return EVENT_BLOCK;
	}
	string result;
	bool flag = false;
	for (lp = defs.begin(); lp != defs.end(); lp++) {		
		if (lp->word == str) {
			flag = true;
			result = result + lp->definition + "\n";
		}		
	}
	if (flag) {
		result = result.erase(result.size() - 1);
		sendQQMessage(fromGroup, fromQQ, result.c_str());
	}
	
	return EVENT_IGNORE;
}
int ProcessPrivate(int32_t subType, int32_t sendTime, int64_t fromQQ, const char *msg, int32_t font) {	
	return ProcessMessage(subType, sendTime, -1, fromQQ, NULL, msg, font);
}

int ProcessGroup(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, const char *fromAnonymous, const char *msg, int32_t font) {
	return ProcessMessage(subType, sendTime, fromGroup, fromQQ, fromAnonymous, msg, font);
}