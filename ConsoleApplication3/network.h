#ifndef NETWORK_H
#define NETWORK_H

#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

// Message types
enum MessageType {
    MSG_GAME_START = 1,
    MSG_GAME_STATE = 2,
    MSG_ACTION = 3,
    MSG_GAME_OVER = 4,
    MSG_DISCONNECT = 5
};

// Action types
enum NetworkAction {
    NET_ACTION_NONE = 0,
    NET_ACTION_SHOOT_OPPONENT = 1,
    NET_ACTION_SHOOT_SELF = 2,
    NET_ACTION_USE_ITEM = 3
};

// Network message structure
struct GameMessage {
    int messageType;

    // Game state
    int hostHP;
    int clientHP;
    int maxHP;
    bool hostTurn;
    bool sawActive;
    bool skipTurn;
    int round;

    // Shells
    int totalShells;
    int currentShell;
    bool shells[10];

    // Items (just the types)
    int hostItemCount;
    int clientItemCount;
    int hostItems[8];
    int clientItems[8];

    // Action data
    int actionType;
    int itemIndex;

    // Result info
    bool wasLive;
    int damageDealt;
    bool shellEjected;

    char statusMessage[256];
};

// Network helper functions
class NetworkHelper {
public:
    static bool sendMessage(SOCKET socket, const GameMessage& msg) {
        int bytesSent = send(socket, (const char*)&msg, sizeof(GameMessage), 0);
        return (bytesSent == sizeof(GameMessage));
    }

    static bool receiveMessage(SOCKET socket, GameMessage& msg) {
        int bytesReceived = recv(socket, (char*)&msg, sizeof(GameMessage), 0);
        return (bytesReceived == sizeof(GameMessage));
    }
};

#endif

/*for testing online write this command in cmd/terminal for localhost exposing -

ssh -p 443 -R0:127.0.0.1:8000 tcp@free.pinggy.io
						   /\
							\__________this your local port. It should be same as server port

and use the given address and port to connect to the server
*/