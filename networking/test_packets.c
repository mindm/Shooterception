#include "packets.h"

char inbuf[512];
char outbuf[512];

void test_chat()
{
    char msg[] = "asdf fasdf dun dun !";
    char rcvmsg[100];
    int len = packChatMessage(outbuf, msg);
    memcpy(inbuf, outbuf, 512);
    unpackChatMessage(inbuf, rcvmsg);
    
    printf("\n\npackChatMessage test\nMessage:%s\n", rcvmsg);
    
    if (strcmp(msg, rcvmsg) == 0)
        printf("Check OK\n");
    else 
        printf("Check failed\n");
    
}

void test_create_game()
{
    char gameName[] = "peli123";
    int maxp1 = 4;
    char player1[] = "player_name";
    
    char rcvgameName[16];
    int maxp2 = 0;
    char player2[16];
    
    int len = packCreateGame(outbuf, gameName, maxp1, player1);
    memcpy(inbuf, outbuf, 512);
    unpackCreateGame(inbuf, rcvgameName, &maxp2, player2);    
    
    if(
    strcmp(gameName, rcvgameName) == 0 &&
    maxp1 == maxp2 &&
    strcmp(player1, player2) == 0){
        printf("\nCreate game test OK\n");
    } else {
        printf("Create game test Fail\n");}
    
}



int main (void)
{
    serverList slist;
    serverList slist2;
    
    slist.count = 0;
    
    server s1;
    strcpy(s1.host, "127.0.0.1");
    strcpy(s1.port, "1321456");
    s1.serverState = 1;
    s1.playerNumber = 3;
    s1.maxPlayers = 4;
    strcpy(s1.gameName, "peli");
    
    slist.count++;
    slist.servers[0] = s1;
    
    packGameList(outbuf, &slist);
    
    memcpy(inbuf, outbuf, 512);
    
    unpackGameList(inbuf, &slist2);
    
    printf("host:%s\nport:%s\nstate:%d\nplayers:(%d/%d)\ngamename:%s\n", slist2.servers[0].host,slist2.servers[0].port, slist2.servers[0].serverState, 
    slist2.servers[0].playerNumber, slist2.servers[0].maxPlayers, slist2.servers[0].gameName);
    
    test_chat();
    test_create_game();

    return 0;
}

//prints
/*
host:127.0.0.1
port:1321456
state:0
players:(4/4)
gamename:peli
*/
