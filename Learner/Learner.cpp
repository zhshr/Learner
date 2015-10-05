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

using namespace std;
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
int saveDefs(bool force) {
	CQ_addLog(ac, CQLOG_INFO, "����", std::to_string(saveCount).c_str());
	saveCount++;
	int i = 0;
	if (saveCount % 5 == 0 || force) {		
		ofstream myfile;
		myfile.open("LearnerDefs.txt");
		list<Def>::iterator p;
		for (p = defs.begin(); p != defs.end(); p++) {
			myfile << p->word << "\n" << p->definition << "\n";
			i++;
		}
		myfile.close();
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
}
void SendBackMessage(int64_t fromGroup, int64_t fromQQ, const char*msg) {
	if (fromGroup < 0) {
		CQ_sendPrivateMsg(ac, fromQQ, msg);
	}
	else {
		CQ_sendGroupMsg(ac, fromGroup, msg);
	}
}
int ProcessMessage(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, const char *fromAnonymous, const char *msg, int32_t font) {
	string str = string(msg);
	map<int64_t, CLearn>::iterator p;
	list<Def>::iterator lp;
	if ((p = waitlist.find(fromQQ)) != waitlist.end()) {
		//���ֵȴ��еĶ���
		CLearn learn = p->second;
		if (learn.fromQQ == fromQQ && learn.fromGroup == fromGroup) {
			waitlist.erase(p);
			Def def;
			def.word = learn.word;
			def.definition = str;
			defs.push_back(def);
			CQ_addLog(ac, CQLOG_INFO, "���ֶ���", str.c_str());
			SendBackMessage(fromGroup, fromQQ, ("��" + def.word + "���ѱ�����Ϊ��" + def.definition + "��").c_str());
			saveDefs(false);
			return EVENT_BLOCK;
		}		
	}
	if (str.substr(0, 6) == "������") {
		CLearn learn;
		learn.word = str.substr(6, str.length());
		if (learn.word.length() < 1) {
			CQ_sendPrivateMsg(ac, fromQQ, "����ʻ㳤�Ȳ���Ϊ0");
		}
		else {
			string result;
			bool flag = false;
			for (lp = defs.begin(); lp != defs.end(); lp++) {
				if (lp->word == learn.word) {
					flag = true;
					result = result + "�ԡ�" + lp->word + "���Ķ����С�" + lp->definition + "��\n";
				}
			}
			if (flag) {
				result = result.erase(result.size() - 1);
				SendBackMessage(fromGroup, fromQQ, result.c_str());
			}
			
			learn.fromQQ = fromQQ;
			learn.fromGroup = fromGroup;
			learn.state = CLearn::STATE_WAITFORDEFINITION;
			waitlist.insert(pair<int, CLearn>(fromQQ, learn));
			SendBackMessage(fromGroup, fromQQ, ("������ԡ�" + learn.word + "���Ķ���").c_str());
		}
//		saveDefs();
		return EVENT_BLOCK;
	}
	if (str.substr(0, 6) == "������") {
		CLearn learn;
		learn.word = str.substr(6, str.length());
		if (learn.word.length() < 1) {
			SendBackMessage(fromGroup, fromQQ, "����ʻ㳤�Ȳ���Ϊ0");
		}
		else {
			string result;
			bool flag = false;
			for (lp = defs.begin(); lp != defs.end(); lp++) {
				if (lp->word == learn.word) {
					flag = true;
					result = result + "�ԡ�" + lp->word + "���Ķ����С�" + lp->definition + "�����Ѳ���\n";
					defs.erase(lp);
				}
			}
			if (flag) {
				result = result.erase(result.size() - 1);
				SendBackMessage(fromGroup, fromQQ, result.c_str());
			}
			
		}
		saveDefs(false);
		return EVENT_BLOCK;
	}
	if (str.substr(0, 6) == "����ʾ") {
		int j = 0;
		string result;
		bool flag = false;
		for (lp = defs.begin(); lp != defs.end(); lp++) {
			if (j > 10) {
				break;
			}
			flag = true;
			j++;
			result = result + "��" + lp->word + "����Ӧ��" + lp->definition + "��\n";			
		}
		if (flag) {
			result = result.erase(result.size() - 1);
			SendBackMessage(fromGroup, fromQQ, result.c_str());
		}		
		return EVENT_BLOCK;
	}
	if (str.substr(0, 6) == "������"){
		SendBackMessage(fromGroup, fromQQ, ("������" + std::to_string(saveDefs(true)) + "������").c_str());
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
		SendBackMessage(fromGroup, fromQQ, result.c_str());
	}
	
	return EVENT_IGNORE;
}
int ProcessPrivate(int32_t subType, int32_t sendTime, int64_t fromQQ, const char *msg, int32_t font) {	
	return ProcessMessage(subType, sendTime, -1, fromQQ, NULL, msg, font);
}

int ProcessGroup(int32_t subType, int32_t sendTime, int64_t fromGroup, int64_t fromQQ, const char *fromAnonymous, const char *msg, int32_t font) {
	return ProcessMessage(subType, sendTime, fromGroup, fromQQ, fromAnonymous, msg, font);
}