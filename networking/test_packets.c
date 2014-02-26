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
    //int len = packCreateGame(outbuf, "game", maxp1, "player1");
    memcpy(inbuf, outbuf, len);
    unpackCreateGame(inbuf, rcvgameName, &maxp2, player2);    
    
    if(
    strcmp(gameName, rcvgameName) == 0 &&
    maxp1 == maxp2 &&
    strcmp(player1, player2) == 0){
        printf("\nCreate game test OK\n");
    } else {
        printf("\nCreate game test Fail\n");}
    
}

void test_join_game()
{
    char gameName[] = "peli123";
    char player1[] = "player_name";
    
    char rcvgameName[16];
    char player2[16];
    
    int len = packJoinGame(outbuf, gameName, player1);
    memcpy(inbuf, outbuf, 512);
    unpackJoinGame(inbuf, rcvgameName, player2);    
    
    if(
            strcmp(gameName, rcvgameName) == 0 &&
            strcmp(player1, player2) == 0
    ){
        printf("\nJoin game test OK\n");
    } else {
        printf("\nJoin game test Fail\n");}
}

void test_client_state()
{
    player out;
    int msgnum1 = 10;
    
    player in;
    int msgnum2 = 0;
    
    out.xcoord = 1; // X coordinate, upper left corner
    out.ycoord = 2; // Y coordinate, upper left corner
    out.viewDirection = 3; // Direction the PC is facing, 0-359 degrees
    out.hasShot = 1; // Has the player shot after sending the last clientState
    
    int size = packClientState(outbuf, out, msgnum1);
    memcpy(inbuf, outbuf, 512);
    unpackClientState(inbuf, &in, &msgnum2);
    
    if (    out.xcoord == in.xcoord &&
            out.ycoord == in.ycoord &&
            out.viewDirection == in.viewDirection &&
            out.hasShot == in.hasShot)
        printf("\nClient state test OK\n");
    else
        printf("\nClient state test Fail\n");
    
}

void test_lobby_state()
{
    char pl1[] = "nimi1";
    char pl2[] = "namn2";
    char pl3[] = "name3";
    char pl4[] = "nombre4";

    char _pl1[16];
    char _pl2[16];
    char _pl3[16];
    char _pl4[16];
    
    int size = packLobbyState(outbuf, pl1, pl2, pl3, pl4);
    memcpy(inbuf, outbuf, 512);
    unpackLobbyState(inbuf, _pl1, _pl2, _pl3, _pl4);
    
    if(     strcmp(pl1, _pl1) == 0 &&
            strcmp(pl2, _pl2) == 0 &&
            strcmp(pl3, _pl3) == 0 &&
            strcmp(pl4, _pl4) == 0)
        printf("\nLobby state test OK\n");
    else
        printf("\nLobby state test Fail\n");
    
    printf("%s, %s, %s, %s", _pl1, _pl2, _pl3, _pl4);

}

void test_game_start()
{
    int outlevel = 2;
    int inlevel;
    
    int size = packGameStart(outbuf, outlevel);
    memcpy(inbuf, outbuf, 512);
    unpackGameStart(inbuf, &inlevel);
    
    if (outlevel == inlevel)
        printf("\nGame start test OK\n");
    else
        printf("\nGame start test Fail\n");
}

void test_chat_relay()
{
    char message[] = "herp derp tässä sulle message juu";
    char in_msg[100];
    int msg_len;
    
    int size = packChatRelay(outbuf, message);
    memcpy(inbuf, outbuf, 512);
    unpackChatRelay(inbuf, in_msg, &msg_len);
    
    if (strcmp(message, in_msg) ==0 )
        printf("\nChat Relay test OK\n");
    else
        printf("\nChat Relay test Fail\n");
}

void test_server_state()
{
    game _game;
    player out_p;
    enemy out_e;
    
    out_p.xcoord = 1; // X coordinate, upper left corner
    out_p.ycoord = 2; // Y coordinate, upper left corner
    out_p.viewDirection = 3; // Direction the PC is facing, 0-359 degrees
    out_p.hasShot = 1; // Has the player shot after sending the last clientState
    out_p.health = 3;
    
    out_e.xcoord = 10; // X coordinate, upper left corner
    out_e.ycoord = 20; // Y coordinate, upper left corner
    out_e.viewDirection = 4; // Direction the PC is facing, 0-359 degrees
    out_e.isShot = 1; // Has the player shot after sending the last clientState
    out_e.health = 2;
    
    game game2;
    
    _game.playerList[0] = out_p;
    _game.enemyList[0] = out_e;
    _game.playerCount = 1;
    _game.enemyCount = 1;
    
    int msgnu;
    
    packServerState(outbuf, &_game, 10);
    unpackServerState(outbuf, &game2, &msgnu);
    printf("%d\n", game2.enemyList[0].viewDirection);
}

void test_client()
{
    printf("-----Start Client tests\n");
    test_chat();
    test_create_game();
    test_join_game();
    test_client_state();
    printf("-----End Client tests\n");
}

void test_server()
{
    printf("-----Start Server tests\n");
    test_lobby_state();
    test_game_start();
    test_chat_relay();
    test_server_state();
    printf("-----End Server tests\n");
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
    
    test_client();
    test_server();
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
