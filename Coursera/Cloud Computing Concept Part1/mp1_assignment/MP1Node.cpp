/**********************************
 * FILE NAME: MP1Node.cpp
 *
 * DESCRIPTION: Membership protocol run by this Node.
 * 				Definition of MP1Node class functions.
 **********************************/

#include "MP1Node.h"
#include <algorithm>
#define DEBUGLOG 1
#define MY_DEBUGLOG 1

/*
 * Note: You can change/add any functions in MP1Node.{h,cpp}
 */

/**
 * Overloaded Constructor of the MP1Node class
 * You can add new members to the class if you think it
 * is necessary for your logic to work
 */
MP1Node::MP1Node(Member *member, Params *params, EmulNet *emul, Log *log, Address *address) {
	for( int i = 0; i < 6; i++ ) {
		NULLADDR[i] = 0;
	}
	this->memberNode = member;
	this->emulNet = emul;
	this->log = log;
	this->par = params;
	this->memberNode->addr = *address;
}

/**
 * Destructor of the MP1Node class
 */
MP1Node::~MP1Node() {}

/**
 * FUNCTION NAME: recvLoop
 *
 * DESCRIPTION: This function receives message from the network and pushes into the queue
 * 				This function is called by a node to receive messages currently waiting for it
 */
int MP1Node::recvLoop() {
    if ( memberNode->bFailed ) {
    	return false;
    }
    else {
    	return emulNet->ENrecv(&(memberNode->addr), enqueueWrapper, NULL, 1, &(memberNode->mp1q));
    }
}

/**
 * FUNCTION NAME: enqueueWrapper
 *
 * DESCRIPTION: Enqueue the message from Emulnet into the queue
 */
int MP1Node::enqueueWrapper(void *env, char *buff, int size) {
	Queue q;
	return q.enqueue((queue<q_elt> *)env, (void *)buff, size);
}

/**
 * FUNCTION NAME: nodeStart
 *
 * DESCRIPTION: This function bootstraps the node
 * 				All initializations routines for a member.
 * 				Called by the application layer.
 */
void MP1Node::nodeStart(char *servaddrstr, short servport) {
    Address joinaddr;
    joinaddr = getJoinAddress();

    // Self booting routines
    if( initThisNode(&joinaddr) == -1 ) {
#ifdef DEBUGLOG
        log->LOG(&memberNode->addr, "init_thisnode failed. Exit.");
#endif
        exit(1);
    }

    if( !introduceSelfToGroup(&joinaddr) ) {
        finishUpThisNode();
#ifdef DEBUGLOG
        log->LOG(&memberNode->addr, "Unable to join self to group. Exiting.");
#endif
        exit(1);
    }

    return;
}

/**
 * FUNCTION NAME: initThisNode
 *
 * DESCRIPTION: Find out who I am and start up
 */
int MP1Node::initThisNode(Address *joinaddr) {
	/*
	 * This function is partially implemented and may require changes
	 */
	int id = *(int*)(&memberNode->addr.addr);
	int port = *(short*)(&memberNode->addr.addr[4]);

	memberNode->bFailed = false;
	memberNode->inited = true;
	memberNode->inGroup = false;
    // node is up!
	memberNode->nnb = 0;
	memberNode->heartbeat = 0;
	memberNode->pingCounter = TFAIL;
	memberNode->timeOutCounter = -1;
    initMemberListTable(memberNode);
  
    return 0;
}


void stringCopy(char* addr1,char* addr2){
    addr1[0] = addr2[0];
    addr1[1] = addr2[1];
    addr1[2] = addr2[2];
    addr1[3] = addr2[3];
    addr1[4] = addr2[4];
    addr1[5] = addr2[5];
}

/**
 * FUNCTION NAME: introduceSelfToGroup
 *
 * DESCRIPTION: Join the distributed system
 */
int MP1Node::introduceSelfToGroup(Address *joinaddr) {
	//MessageHdr *msg;
	MessagePack *my_pack;
	
#ifdef DEBUGLOG
    static char s[1024];
#endif

    if ( 0 == memcmp((char *)&(memberNode->addr.addr), (char *)&(joinaddr->addr), sizeof(memberNode->addr.addr))) {
        // I am the group booter (first process to join the group). Boot up the group
#ifdef DEBUGLOG
        log->LOG(&memberNode->addr, "Starting up group...");
#endif
        memberNode->inGroup = true;
        memberNode->memberList.insert(memberNode->memberList.end(), MemberListEntry(*(int*)&memberNode->addr.addr, *(short*)&memberNode->addr.addr[4],memberNode->heartbeat, par->getcurrtime()));
        
    }
    else {
        size_t msgsize = sizeof(MessageHdr) + sizeof(joinaddr->addr) + sizeof(MemberListEntry) + sizeof(long) + 1;
        //msg = (MessageHdr *) malloc(msgsize * sizeof(char));

        my_pack = (MessagePack *) malloc(sizeof(MessagePack));
        
        // create JOINREQ message: format of data is {struct Address myaddr}
        //msg->msgType = JOINREQ;
        
        //(*my_pack).myList.clear();
        (*my_pack).my_hdr.msgType = JOINREQ;
        stringCopy((*my_pack).address,memberNode->addr.addr);
#ifdef MY_DEBUGLOG_2
        char temp_str[100];
        sprintf(temp_str, "The member ID: %d.%d.%d.%d:%d.%d",memberNode->addr.addr[0],memberNode->addr.addr[1],memberNode->addr.addr[2],
                                                             memberNode->addr.addr[3],memberNode->addr.addr[4],memberNode->addr.addr[5]);
        log->LOG(&memberNode->addr, temp_str);
#endif
        (*my_pack).heartbeat = memberNode->heartbeat;
        (*my_pack).pad = '\0';
        
        //memcpy((char *)(msg+1), &memberNode->addr.addr, sizeof(memberNode->addr.addr));
        //memcpy((char *)(msg+1) + 1 + sizeof(memberNode->addr.addr), &memberNode->heartbeat, sizeof(long));

#ifdef DEBUGLOG
        sprintf(s, "Trying to join...");
        log->LOG(&memberNode->addr, s);
        
#endif

        // send JOINREQ message to introducer member
        //emulNet->ENsend(&memberNode->addr, joinaddr, (char *)msg, msgsize);
        emulNet->ENsend(&memberNode->addr, joinaddr, (char *)my_pack, msgsize);

        //free(msg);
        free(my_pack);
    }

    return 1;

}

/**
 * FUNCTION NAME: finishUpThisNode
 *
 * DESCRIPTION: Wind up this node and clean up state
 */
int MP1Node::finishUpThisNode(){
   /*
    * Your code goes here
    */
}

/**
 * FUNCTION NAME: nodeLoop
 *
 * DESCRIPTION: Executed periodically at each member
 * 				Check your messages in queue and perform membership protocol duties
 */
void MP1Node::nodeLoop() {
    if (memberNode->bFailed) {
    	return;
    }

    // Check my messages
    checkMessages();

    // Wait until you're in the group...
    if( !memberNode->inGroup ) {
    	return;
    }

    // ...then jump in and share your responsibilites!
    nodeLoopOps();

    return;
}

/**
 * FUNCTION NAME: checkMessages
 *
 * DESCRIPTION: Check messages in the queue and call the respective message handler
 */
void MP1Node::checkMessages() {
    void *ptr;
    int size;

    // Pop waiting messages from memberNode's mp1q
    while ( !memberNode->mp1q.empty() ) {
    	ptr = memberNode->mp1q.front().elt;
    	size = memberNode->mp1q.front().size;
    	memberNode->mp1q.pop();
    	recvCallBack((void *)memberNode, (char *)ptr, size);
    }
    return;
}

/**
 * FUNCTION NAME: recvCallBack
 *
 * DESCRIPTION: Message handler for different message types
 */
bool MP1Node::recvCallBack(void *env, char *data, int size ) {
    MessagePack *my_pack;
    my_pack = (MessagePack *) data;
    
#ifdef MY_DEBUGLOG_2
    char temp_str[100];
    sprintf(temp_str,"messages type: %d Joinaddr: %d.%d.%d.%d:%hu",
                    (*my_pack).my_hdr,
                    (*my_pack).address[0],(*my_pack).address[1],(*my_pack).address[2],(*my_pack).address[3],*(short*)&(*my_pack).address[4]);
    log->LOG(&memberNode->addr, temp_str);
#endif
    
    if((*my_pack).my_hdr.msgType == JOINREQ)
        handleJoinRequest(my_pack);
    else if((*my_pack).my_hdr.msgType == JOINREP)
        handleJoinSetup(my_pack);
    else if((*my_pack).my_hdr.msgType == SENTLIST)
        handleJoinList(my_pack);
    else if((*my_pack).my_hdr.msgType == REQUEST)
        handleRequest(my_pack);
    else if((*my_pack).my_hdr.msgType == REQUESTREP)
        handleRequestRep(my_pack);
        
}

/**
 * FUNCTION NAME: handleJoinRequest
 *
 * DESCRIPTION: 
 * 				
 */

void MP1Node::handleJoinRequest(MessagePack* my_pack){

    Address joinaddr;
    MessagePack* return_pack = (MessagePack*) malloc(sizeof(MessagePack));
    
    //Create reply messgae
    stringCopy(joinaddr.addr, (*my_pack).address);
    return_pack->my_hdr.msgType = JOINREP;
    stringCopy(return_pack->address, memberNode->addr.addr);
    //return_pack.myList = memberNode->memberList;
    return_pack->heartbeat = memberNode->heartbeat;
    return_pack->pad = '\0';
    
    size_t msgsize = sizeof(MessageHdr) + sizeof(my_pack->address) + sizeof(MemberListEntry) + sizeof(long) + 1;
    emulNet->ENsend(&memberNode->addr, &joinaddr, (char *)return_pack, msgsize);
    free(return_pack);

    vector<MemberListEntry>::iterator post;
    for (post = memberNode->memberList.begin(); post!= memberNode->memberList.end(); post++){
        return_pack = (MessagePack*) malloc(sizeof(MessagePack));
        return_pack->my_hdr.msgType = SENTLIST;
        stringCopy(return_pack->address, memberNode->addr.addr);
        return_pack->myListEntry = *post;
        return_pack->heartbeat = memberNode->heartbeat;
        return_pack->pad = '\0';
        emulNet->ENsend(&memberNode->addr, &joinaddr, (char *)return_pack, msgsize);
        free(return_pack);
    }
 
    // Add the new memberEntry
    int id = *(int*)&my_pack->address;
    unsigned short port = *(short*)&my_pack->address[4];
    //memberNode->memberList.insert(memberNode->memberList.end(),MemberListEntry(*(int*)(&my_pack->address), *(short*)&my_pack->address[4],(*my_pack).heartbeat, par->getcurrtime()) );
    memberNode->memberList.insert(memberNode->memberList.end(),MemberListEntry(id,port ,(*my_pack).heartbeat, par->getcurrtime()) );
#ifdef DEBUGLOG
    //char temp_str[100];
    //sprintf(temp_str, "The member ID: %d.%d.%d.%d:%d.%d",my_pack->address[0],my_pack->address[1],my_pack->address[2],my_pack->address[3],my_pack->address[4],my_pack->address[5]);
    stringCopy(joinaddr.addr, my_pack->address);
    log->logNodeAdd(&memberNode->addr,&joinaddr);
#endif
}

/**
 * FUNCTION NAME: handleJoinSetup
 *
 * DESCRIPTION: 
 * 				
 */

void MP1Node::handleJoinSetup(MessagePack* my_pack){
    memberNode->inGroup = true;
    //MemberListEntry newentry(*(int*)(&my_pack->address), *(short*)&my_pack->address[4],(*my_pack).heartbeat, par->getcurrtime());
    memberNode->memberList.insert(memberNode->memberList.end(), MemberListEntry(*(int*)&memberNode->addr.addr, *(short*)&memberNode->addr.addr[4],memberNode->heartbeat, par->getcurrtime()));
    memberNode->memberList.insert(memberNode->memberList.end(), MemberListEntry(*(int*)(&my_pack->address), *(short*)&my_pack->address[4],(*my_pack).heartbeat, par->getcurrtime()));
#ifdef DEBUGLOG
    Address joinaddr;
    stringCopy(joinaddr.addr, my_pack->address);
    log->logNodeAdd(&memberNode->addr,&joinaddr);
#endif
}

/**
 * FUNCTION NAME: handleSentList
 *
 * DESCRIPTION: 
 * 				
 */
 
void MP1Node::handleJoinList(MessagePack* my_pack){
    int myid = *(int*)(&memberNode->addr.addr);
    bool flag = false;
    
    if(myid == my_pack->myListEntry.id)
        return;
    
    vector<MemberListEntry>::iterator pos;
    for (pos = memberNode->memberList.begin(); pos!= memberNode->memberList.end(); pos++){
        if(pos->id == my_pack->myListEntry.id){
            if(pos -> timestamp < my_pack-> myListEntry.timestamp && par->getcurrtime() - my_pack-> myListEntry.timestamp < TREMOVE ){
                pos -> timestamp = my_pack-> myListEntry.timestamp;
                pos -> heartbeat = my_pack-> myListEntry.heartbeat;
            }
            flag =true;
        }
    }
    
    if(!flag){
        memberNode->memberList.insert(memberNode->memberList.end(),MemberListEntry(my_pack->myListEntry));
#ifdef DEBUGLOG
        Address joinaddr;
        sprintf(joinaddr.addr, "%c%c%c%c%c%c", (char) (my_pack->myListEntry.id), (char)(my_pack->myListEntry.id >> 8) ,(char) (my_pack->myListEntry.id  >> 16), 
                                               (char)(my_pack->myListEntry.id >> 24),(char)(my_pack->myListEntry.port),(char)(my_pack->myListEntry.port >> 8));
        log->logNodeAdd(&memberNode->addr,&joinaddr);
#endif
    }

}

/**
 * FUNCTION NAME: printListMembers
 *
 * DESCRIPTION: 
 * 				
 */
void MP1Node::printListMembers(){
    char temp_str[100] = "The members: ", temp_cat[100];
    vector<MemberListEntry>::iterator pos;
    for (pos = memberNode->memberList.begin(); pos!= memberNode->memberList.end(); pos++){
        sprintf(temp_cat, "%d.%d.%d.%d:%hu ", (char) (pos->id), (char)(pos->id >> 8) ,(char) (pos->id  >> 16), (char)(pos->id >> 24), pos->port);
        strcat(temp_str,temp_cat);
    }
    log->LOG(&memberNode->addr, temp_str);
}


/**
 * FUNCTION NAME: handleRequest
 *
 * DESCRIPTION: 
 * 				
 */
void MP1Node::handleRequest(MessagePack* my_pack){
    Address joinaddr;
    MessagePack* return_pack = (MessagePack*) malloc(sizeof(MessagePack));
    
    //Create reply messgae
    stringCopy(joinaddr.addr, (*my_pack).address);
    return_pack->my_hdr.msgType = REQUESTREP;
    stringCopy(return_pack->address, memberNode->addr.addr);
    //return_pack.myList = memberNode->memberList;
    return_pack->heartbeat = memberNode->heartbeat;
    return_pack->pad = '\0';
    
    size_t msgsize = sizeof(MessageHdr) + sizeof(my_pack->address) + sizeof(MemberListEntry) + sizeof(long) + 1;
    emulNet->ENsend(&memberNode->addr, &joinaddr, (char *)return_pack, msgsize);
    free(return_pack);

    vector<MemberListEntry>::iterator post;
    for (post = memberNode->memberList.begin(); post!= memberNode->memberList.end(); post++){
        return_pack = (MessagePack*) malloc(sizeof(MessagePack));
        return_pack->my_hdr.msgType = SENTLIST;
        stringCopy(return_pack->address, memberNode->addr.addr);
        return_pack->myListEntry = *post;
        return_pack->heartbeat = memberNode->heartbeat;
        return_pack->pad = '\0';
        emulNet->ENsend(&memberNode->addr, &joinaddr, (char *)return_pack, msgsize);
        free(return_pack);
    }
    
}

/**
 * FUNCTION NAME: handleRequestRep
 *
 * DESCRIPTION: 
 * 				
 */
void MP1Node::handleRequestRep(MessagePack* my_pack){
    vector<MemberListEntry>::iterator pos;
    for (pos = memberNode->memberList.begin(); pos!= memberNode->memberList.end(); pos++){
        if(pos->id == *(int*)my_pack->address){
            pos->timestamp =  par->getcurrtime();
        }
    }
}

/**
 * FUNCTION NAME: sentListMembers
 *
 * DESCRIPTION: 
 * 				
 */
void sentListMembers(){
    
    
}


bool compareId(MemberListEntry &e1, MemberListEntry &e2){
   return e1.id < e2.id ;
}

/**
 * FUNCTION NAME: nodeLoopOps
 *
 * DESCRIPTION: Check if any node hasn't responded within a timeout period and then delete
 * 				the nodes
 * 				Propagate your membership list
 */
void MP1Node::nodeLoopOps() {
    Address joinaddr;
    MessagePack* return_pack = (MessagePack*) malloc(sizeof(MessagePack));
    
    vector<MemberListEntry>::iterator pos;
    for (pos = memberNode->memberList.begin(); pos!= memberNode->memberList.end(); pos++){
        if(par->getcurrtime() - pos->timestamp >= TFAIL && par->getcurrtime() - pos->timestamp < TREMOVE){
            if(pos->id  == *(int*)(&memberNode->addr.addr))
                pos->timestamp = par->getcurrtime();
            else{
                sprintf(joinaddr.addr, "%c%c%c%c%c%c", (char) (pos->id), (char)(pos->id >> 8) ,(char) (pos->id  >> 16), (char)(pos->id >> 24),(char)(pos->port),(char)(pos->port >> 8));
                return_pack->my_hdr.msgType = REQUEST;
                stringCopy(return_pack->address, memberNode->addr.addr);
                return_pack->heartbeat = memberNode->heartbeat;
                return_pack->pad = '\0';
                size_t msgsize = sizeof(MessageHdr) + sizeof(joinaddr.addr) + sizeof(MemberListEntry) + sizeof(long) + 1;
                emulNet->ENsend(&memberNode->addr, &joinaddr, (char *)return_pack, msgsize);
            }
        }
        if(par->getcurrtime() - pos->timestamp >= TREMOVE){
#ifdef DEBUGLOG
            sprintf(joinaddr.addr, "%c%c%c%c%c%c", (char) (pos->id), (char)(pos->id >> 8) ,(char) (pos->id  >> 16), (char)(pos->id >> 24),(char)(pos->port),(char)(pos->port >> 8));
            log->logNodeRemove(&memberNode->addr, &joinaddr);
#endif
            memberNode->memberList.erase(pos);
            std::sort(memberNode->memberList.begin(),memberNode->memberList.end(),compareId);
#ifdef MY_DEBUGLOG2
            printListMembers();
#endif
            break;
        }
    }
    return;
}



/**
 * FUNCTION NAME: isNullAddress
 *
 * DESCRIPTION: Function checks if the address is NULL
 */
int MP1Node::isNullAddress(Address *addr) {
	return (memcmp(addr->addr, NULLADDR, 6) == 0 ? 1 : 0);
}

/**
 * FUNCTION NAME: getJoinAddress
 *
 * DESCRIPTION: Returns the Address of the coordinator
 */
Address MP1Node::getJoinAddress() {
    Address joinaddr;

    memset(&joinaddr, 0, sizeof(Address));
    *(int *)(&joinaddr.addr) = 1;
    *(short *)(&joinaddr.addr[4]) = 0;

    return joinaddr;
}

/**
 * FUNCTION NAME: initMemberListTable
 *
 * DESCRIPTION: Initialize the membership list
 */
void MP1Node::initMemberListTable(Member *memberNode) {
	memberNode->memberList.clear();
}

/**
 * FUNCTION NAME: printAddress
 *
 * DESCRIPTION: Print the Address
 */
void MP1Node::printAddress(Address *addr)
{
    printf("%d.%d.%d.%d:%d \n",  addr->addr[0],addr->addr[1],addr->addr[2],
                                                       addr->addr[3], *(short*)&addr->addr[4]) ;    
}
