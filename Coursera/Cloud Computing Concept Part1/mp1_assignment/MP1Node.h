/**********************************
 * FILE NAME: MP1Node.cpp
 *
 * DESCRIPTION: Membership protocol run by this Node.
 * 				Header file of MP1Node class.
 **********************************/

#ifndef _MP1NODE_H_
#define _MP1NODE_H_

#include "stdincludes.h"
#include "Log.h"
#include "Params.h"
#include "Member.h"
#include "EmulNet.h"
#include "Queue.h"

/**
 * Macros
 */
#define TREMOVE 20
#define TFAIL 5

/*
 * Note: You can change/add any functions in MP1Node.{h,cpp}
 */

/**
 * Message Types
 */
enum MsgTypes{
    JOINREQ,
    JOINREP,
    SENTLIST,
    REQUEST,
    REQUESTREP,
    DUMMYLASTMSGTYPE
};

/**
 * STRUCT NAME: MessageHdr
 *
 * DESCRIPTION: Header and content of a message
 */
typedef struct MessageHdr {
	enum MsgTypes msgType;
}MessageHdr;


/**
 * CLASS NAME: MessagePack
 *
 * DESCRIPTION: Define the Messgage packge.
 */

typedef struct MessagePack{
    MessageHdr my_hdr;
    char address[6];   //Adress.arr[0:4] ip    Adress.arr[4:6] port
    //vector<MemberListEntry> myList;
    MemberListEntry myListEntry;
    long heartbeat;
    char pad;
}MessagePack;

/**
 * CLASS NAME: MP1Node
 *
 * DESCRIPTION: Class implementing Membership protocol functionalities for failure detection
 */
class MP1Node {
private:
	EmulNet *emulNet;
	Log *log;
	Params *par;
	Member *memberNode;
	char NULLADDR[6];

public:
	MP1Node(Member *, Params *, EmulNet *, Log *, Address *);
	Member * getMemberNode() {
		return memberNode;
	}
	int recvLoop();
	static int enqueueWrapper(void *env, char *buff, int size);
	void nodeStart(char *servaddrstr, short serverport);
	int initThisNode(Address *joinaddr);
	int introduceSelfToGroup(Address *joinAddress);
	int finishUpThisNode();
	void nodeLoop();
	void checkMessages();
	bool recvCallBack(void *env, char *data, int size);
	void nodeLoopOps();
	int isNullAddress(Address *addr);
	Address getJoinAddress();
	void initMemberListTable(Member *memberNode);
	void printAddress(Address *addr);
	virtual ~MP1Node();
	
	void handleJoinRequest(MessagePack*);
	void handleJoinSetup(MessagePack *);
	void handleJoinList(MessagePack *);
	void handleRequest(MessagePack *);
	void handleRequestRep(MessagePack *);
	void printListMembers();
	void sentListMembers();
	//bool compareId(const MemberListEntry&, const MemberListEntry&);
};

#endif /* _MP1NODE_H_ */
