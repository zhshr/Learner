#include "stdafx.h"
#include "util.h"


using namespace std;
void sendQQMessage(int64_t fromGroup, int64_t fromQQ, const char*msg) {
	if (fromGroup == -1) {
		CQ_sendPrivateMsg(ac, fromQQ, msg);
	}
	else {
		CQ_sendGroupMsg(ac, fromGroup, msg);
	}
}
bool ListPlugins(int64_t fromGroup, int64_t fromQQ, const char*msg) {
	string str = string(msg);
	CQ_addLog(ac, CQLOG_DEBUG, "��ʾ�б�", CQAPPNAME);
	if (str == "��ʾ����б�") {
		sendQQMessage(fromGroup, fromQQ, CQAPPNAME);
		return true;
	}
	else {
		return false;
	}
}