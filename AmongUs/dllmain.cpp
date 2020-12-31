// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <locale>
#include <codecvt>
#include <map>
#include <sstream>
#include <list>

struct Vec2
{
    float x, y;
};

struct ClientData {
private:
    uintptr_t b1, b2, b3, b4, b5, b6;
public:
    int dunno;
    BYTE Id;
    float MaxReportDistance;
    bool moveable;
    bool inVent;
private:
    bool b7, b8;
public:
    uintptr_t PlayerData;
    uintptr_t footSteps;
    uintptr_t KillSfx;
    uintptr_t killAnimations;
    float killTimer;
    int RemainingEmergencies;
    uintptr_t nameText;
    uintptr_t lightPrefab;
    uintptr_t lighting;
    uintptr_t collider;
    uintptr_t myPhysics;
    uintptr_t netTransform;
    uintptr_t currentPet;
    uintptr_t hatRenderer;
    uintptr_t dunno2;
    uintptr_t myTasks;
    uintptr_t scannerAnims;
    uintptr_t scannerImages;
};

struct PlayerData {
    uintptr_t blankSpace1, blankSpace2;
    BYTE Id;
    uintptr_t name;
    BYTE color;
    unsigned int HatId, PetId, SkinId;
    bool disconnected;
    uintptr_t taskList;
    bool isDead;
    ClientData* ClientData;

};

enum GameType {
    LocalGame, OnlineGame, FreePlay
};

struct AmongUsClient {
private:
    uintptr_t b1, b2, b3, b4, b5, b6, b7, b8, b9, b10, b11, b12, b13, b14, b15, b16, b17;
    bool b18, b19;
public:
    int AutoOpenStore;
    GameType GameMode;
    uintptr_t OnlineScene;
    uintptr_t MainMenuScene;
    uintptr_t GameDataPrefab;
    uintptr_t PlayerPrefab;
    uintptr_t ShipPrefab;
    int TutorialMapId;
    float SpawnRadius;
    uintptr_t DisconnectHandlers;
    uintptr_t GameListHandlers;
};

std::string input = "0xCF6A00|0x5EB310|0x265140|0x8E6360|0x8F2140|0x2466C0|0x5E8B50|0xCF6360|0xEF09F0|0x143BF38|0x1D3AD0|0xB77820|0x5EE230|0x5EE0C0|0x5EDB90#0x10|0x5C|0x24|0x8|0xC|0x10";
/*
    0. InterClientIsGameStarted x
    1. GetPlayerById x
    2. InterClientIsGamePublic x
    3. GetTruePosition x
    4. GetPlayerData from Player Control x
    5. 
    6. AddPlayer
    7. InterClientUpdate x
    8. 
    9. 
    10. StringLength x
    11. StringChar x
    12. UpdateName x
    13. UpdateColor xxx
    14. RemovePlayer x
*/
uintptr_t pointers[2][20];
std::string output = "";


class Player {

    bool dead;
    BYTE id;
    std::string name;
    BYTE Color;
    UINT HatId;
    UINT PetId;
    UINT SkinId;
    bool Disonnected;
    uintptr_t tasks;
    bool joinedWhilePublic;
    float x;
    float y;


public:
    Player(BYTE id) {
        this->dead = false;
        this->name = "";
        this->id = id;
    }

    Player() {}

    void setDead(bool dead) {
        this->dead = dead;
    }
    bool getDead() {
        return this->dead;
    }

    void setName(std::string name) {
        this->name = name;
    }

    void setColor(BYTE color) {
        this->Color = color;
    }

    void setPublicJoin(bool join) {
        this->joinedWhilePublic = join;
    }

    void setPos(float x, float y) {
        this->x = x;
        this->y = y;
    }

    std::string getName() {
        return this->name;
    }

    BYTE getId() {
        return this->id;
    }

    BYTE getColor() {
        return this->Color;
    }

    bool getPublicJoin() {
        return this->joinedWhilePublic;
    }

    float getX() {
        return this->x;
    }
    
    float getY() {
        return this->y;
    }
};

std::vector<Player*> players = {};
bool gameStart = false;
bool publicJoin = false;
uintptr_t GameDataF;
uintptr_t AmongUsClientF;
uintptr_t InterClientF = 0;

void stringifyData() {
    std::ofstream stream;
    std::string plrs = "";

    for (int i = 0; i < players.size(); i++) {
        plrs += (*players[i]).getName() + ":" + std::to_string((*players[i]).getDead()) + ":" + std::to_string((*players[i]).getPublicJoin()) + ":" + std::to_string((*players[i]).getX()) + "|" + std::to_string((*players[i]).getY()) + ",";
    }

    stream.open("C:\\Users\\rman9\\Desktop\\reversing\\reverseData\\info.txt");

    stream << plrs + "||" + std::to_string(gameStart) + " " + output;

    stream.close();
}

std::vector<std::string> splitLine(std::string line, char del) {
    std::vector <std::string> tokens;

    std::stringstream check1(line);

    std::string intermediate;

    while (getline(check1, intermediate, del))
    {
        tokens.push_back(intermediate);
    }

    return tokens;
}



void Hook(void* src, void* dst, unsigned int len)
{
    if (len < 5) return;

    DWORD curProtection;
    VirtualProtect(src, len, PAGE_EXECUTE_READWRITE, &curProtection);

    memset(src, 0x90, len);

    uintptr_t relativeAddress = (uintptr_t)dst - (uintptr_t)src - 5;

    *(BYTE*)src = 0xE9;

    *(uintptr_t*)((BYTE*)src + 1) = relativeAddress;
    VirtualProtect(src, len, curProtection, &curProtection);
}

BYTE* TrampHook(void* src, void* dst, unsigned int len)
{
    if (len < 5) return 0;

    BYTE* gateway = (BYTE*)VirtualAlloc(0, len, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

    memcpy_s(gateway, len, src, len);

    uintptr_t gatewayRelativeAddr = (BYTE*)src - gateway - 5;
    *(gateway + len) = 0xE9;

    *(uintptr_t*)((uintptr_t)gateway + len + 1) = gatewayRelativeAddr;

    Hook(src, dst, len);
    return gateway;
}

uintptr_t moduleAssembly = (uintptr_t)GetModuleHandleW(L"GameAssembly.dll");

typedef wchar_t (*tGetChar)(void* Str, int Pos);
tGetChar oGetChar;

typedef int (*tGetLegnth)(void* Str);
tGetLegnth oGetLength;

typedef Vec2 (*tGetTruePosition)(void* PlayerControl);
tGetTruePosition oGetTruePosition;

typedef void* (*tGetPlayerDataControl)(void* PlayerControl);
tGetPlayerDataControl oGetPlayerDataControl;

typedef bool (*tGetIsGamePublic)(void* AmongUsClient);
tGetIsGamePublic oGetIsGamePublic;

typedef void (*tCloseDoorOfType)(void* dunno, void* Location);
tCloseDoorOfType oCloseDoorOfType;

typedef void(*tAddPlayer)(void* GameData, void* PlayerControl);
tAddPlayer oAddPlayer;

typedef void(*tUpdateName)(void* GameData, void* playerId, void* name);
tUpdateName oUpdateName;

typedef void(*tUpdateColor)(void* GameData, BYTE playerId, BYTE color);
tUpdateColor oUpdateColor;

typedef void(*tInterClientUpdate)(void* InterClient);
tInterClientUpdate oInterClientUpdate;

typedef PlayerData* (*tGetPlayerById)(void* GameData, BYTE playerId);
tGetPlayerById oGetPlayerById;

void setupPointers() {
    std::vector<std::string> splitData = splitLine(input, '#');

    std::vector<std::string> splitFuncPoint = splitLine(splitData[0], '|');
    std::vector<std::string> splitVarPoint = splitLine(splitData[1], '|');

    for (int i = 0; i < splitFuncPoint.size(); i++) {
        std::string func = splitFuncPoint[i];

        pointers[0][i] = std::stoi(func, nullptr, 16);
    }

    for (int i = 0; i < splitVarPoint.size(); i++) {
        std::string func = splitVarPoint[i];

        pointers[1][i] = std::stoi(func, nullptr, 16);
    }

}

void setupModules() {
    moduleAssembly = (uintptr_t)GetModuleHandleW(L"GameAssembly.dll");

    oGetPlayerById = (tGetPlayerById)(moduleAssembly + pointers[0][1]);
    oGetIsGamePublic = (tGetIsGamePublic)(moduleAssembly + pointers[0][2]);
    oGetTruePosition = (tGetTruePosition)(moduleAssembly + pointers[0][3]);
    oGetPlayerDataControl = (tGetPlayerDataControl)(moduleAssembly + pointers[0][4]);
    oAddPlayer = (tAddPlayer)(moduleAssembly + pointers[0][6]);
    oInterClientUpdate = (tInterClientUpdate)(moduleAssembly + pointers[0][7]);
    oUpdateName = (tUpdateName)(moduleAssembly + pointers[0][12]);
    oUpdateColor = (tUpdateColor)(moduleAssembly + pointers[0][13]);

    oGetChar = (tGetChar)(moduleAssembly + pointers[0][11]);
    oGetLength = (tGetLegnth)(moduleAssembly + pointers[0][10]);
}

std::string getString(uintptr_t ptr) {
   int strLength = oGetLength((void*)ptr);
   std::string str = "";
    std::wstring_convert< std::codecvt_utf8<wchar_t>, wchar_t > converter;

    for (int i = 0; i < strLength; i++) {
        try {
            str += converter.to_bytes(oGetChar((void*)ptr, i));
        }
        catch (const char* msg) {
            //str = msg;
        }
    }
    
    return str;
}

Player addPlayer(uintptr_t pointer) {
    PlayerData data = *(PlayerData*)pointer;
    Player* plr = new Player();
    BYTE playerId = data.Id;
    bool found = false;
    

    for (int i = 0; i < players.size(); i++) {
        if (players[i]->getId() == playerId) {
            found = true;
            if (InterClientF > 0) {
                players[i]->setPublicJoin(oGetIsGamePublic((void*)InterClientF));
                plr = players[i];
            }
        }
    }

    if (!found) {
        plr = new Player(playerId);
        plr->setName(getString(data.name));
        plr->setColor(data.color);
        plr->setDead(data.isDead);


        if (InterClientF > 0) {
            plr->setPublicJoin(oGetIsGamePublic((void*)InterClientF));
        }

        players.push_back(plr);
    }

    stringifyData();
     
    return *plr;
}

Player getPlayer(BYTE id) {
    Player retPlayer;
    bool found = false;

    for (int i = 0; i < players.size(); i++) {
        Player getPlayer = *players[i];
        if (getPlayer.getId() == id) {
            retPlayer = getPlayer;
            found = true;
        }
    }

    if (!found) {
        retPlayer = addPlayer((uintptr_t)oGetPlayerById((void*)GameDataF, id));
    }

    return retPlayer;

}

Vec2 hGetTruePosition(void* PlayerControl) {

    uintptr_t PlayerData = (uintptr_t)oGetPlayerDataControl(PlayerControl);
    BYTE playerId = *(BYTE*)(PlayerData + 0x10);

    return oGetTruePosition(PlayerControl);
}

void hAddPlayer(void* GameData, void* PlayerControl) {
    oAddPlayer(GameData, PlayerControl);
    GameDataF = (uintptr_t)GameData;
    uintptr_t PlayerData = (uintptr_t)oGetPlayerDataControl(PlayerControl);

    addPlayer(PlayerData);

}

void hUpdateName(void* GameData, void* playerId, void* name) {
    oUpdateName(GameData, playerId, name);
    GameDataF = (uintptr_t)GameData;

    
}  

void hUpdateColor(void* GameData, BYTE playerId, BYTE color) {
    oUpdateColor(GameData, playerId, color);
    GameDataF = (uintptr_t)GameData;

    stringifyData();
}

void hInterClientUpdate(void* InterClient) {
    oInterClientUpdate(InterClient);

    InterClientF = (uintptr_t)InterClient;

}

DWORD WINAPI myThread(HMODULE hModule)
{
    setupPointers();
    setupModules();
    oGetTruePosition = (tGetTruePosition)TrampHook(oGetTruePosition, hGetTruePosition, 6);
    oAddPlayer = (tAddPlayer)TrampHook(oAddPlayer, hAddPlayer, 10);
    oInterClientUpdate = (tInterClientUpdate)TrampHook(oInterClientUpdate, hInterClientUpdate, 5);
    oUpdateColor = (tUpdateColor)TrampHook(oUpdateColor, hUpdateColor, 7);
    oUpdateName = (tUpdateName)TrampHook(oUpdateName, hUpdateName, 7);
    stringifyData();
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CloseHandle(CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)myThread, hModule, NULL, nullptr));
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}