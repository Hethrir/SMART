/**
 *  Copyright 2012 by Benjamin J. Land (a.k.a. BenLand100)
 *
 *  This file is part of the SMART
 *
 *  SMART is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  SMART is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with SMART. If not, see <http://www.gnu.org/licenses/>.
 */
 
#ifndef _SMARTREMOTE_H
#define	_SMARTREMOTE_H

typedef struct {
    int port;           //The port the client listens on
    int id;             //ID of the client: SMART.id
    int width,height;   //Size of the images that follow this structure in the SHM
    int paired;         //TID of the controller or ZERO if none
    int imgoff;         //Offset in the SHM where the image is stored
    int dbgoff;         //Offset in the SHM where the debug image is stored
    
    /**
     * Comm protocol - Controller sets arguments then writes funid to the client's
     * socket to notify it of the new call. The Client executes the function
     * then stores the results in args. Finally, the client echos the funid to
     * the controller
     */
    unsigned char args[4096];
} shm_data;


#ifndef _WIN32
    #include <unistd.h>
    #include <string.h>
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <netdb.h> 
#else
    #if __SIZEOF_POINTER__ == 4
       #define _WIN32_WINNT 0x0501
    #endif
    #include <windows.h>
#endif

/**
 * Comm protocol: use these numbers as funid, everything less or equal to 
 * NumImports is a direct export from SMART and must correspond to the name in
 * imports[def-FirstFunc]
 */

#define FirstFunc           1

#define getRefresh          FirstFunc+0
#define setRefresh          FirstFunc+1
#define setTransparentColor FirstFunc+2
#define setDebug            FirstFunc+3
#define setGraphics         FirstFunc+4 
#define setEnabled          FirstFunc+5
#define isActive            FirstFunc+6
#define isBlocking          FirstFunc+7
#define getMousePos         FirstFunc+8
#define holdMouse           FirstFunc+9
#define releaseMouse        FirstFunc+10
#define holdMousePlus       FirstFunc+11
#define releaseMousePlus    FirstFunc+12
#define moveMouse           FirstFunc+13
#define windMouse           FirstFunc+14
#define clickMouse          FirstFunc+15
#define clickMousePlus      FirstFunc+16
#define isMouseButtonHeld   FirstFunc+17
#define sendKeys            FirstFunc+18
#define holdKey             FirstFunc+19
#define releaseKey          FirstFunc+20
#define isKeyDown           FirstFunc+21

#define ExtraFuncs          FirstFunc+22

#define Ping                ExtraFuncs+0
#define Die                 ExtraFuncs+1



#ifndef _WIN32
#include <dlfcn.h>
void load() __attribute__((constructor));
void unload() __attribute__((destructor));
#else
extern "C" bool DllMain(HINSTANCE, int, void*) __attribute__((stdcall));
#endif

#if __SIZEOF_POINTER__ == 4
    #define bits "32"
#else
    #define bits "64"
#endif
#define TIMEOUT 5

extern "C" int GetPluginABIVersion();
extern "C" int GetFunctionCount();
extern "C" int GetFunctionInfo(int, void*&, char*&);

typedef struct {
    int width,height;
    int refcount;
    #ifndef _WIN32
    int socket;
    int fd;
    void* memmap;
    #else
    SOCKET socket;
    HANDLE file;
    HANDLE memmap;
    #endif
    shm_data *data;
} SMARTClient;

typedef struct {
    int count;
    int *ids;
} clients_dat;

typedef SMARTClient* Target;
typedef union { 
	struct { char b, g, r, a; }; 
	unsigned int color; 
} rgb; 

void call(int funid);
 
extern "C" Target EIOS_RequestTarget(char *initargs) __attribute__((stdcall)); 
extern "C" void EIOS_ReleaseTarget(Target t) __attribute__((stdcall)); 
extern "C" void EIOS_GetTargetDimensions(Target t, int* width, int* height) __attribute__((stdcall)); 
extern "C" rgb* EIOS_GetImageBuffer(Target t) __attribute__((stdcall)); 
extern "C" void EIOS_UpdateImageBuffer(Target t) __attribute__((stdcall)); 
extern "C" void EIOS_GetMousePosition(Target t, int* x, int* y) __attribute__((stdcall)); 
extern "C" void EIOS_MoveMouse(Target t, int x, int y) __attribute__((stdcall)); 
extern "C" void EIOS_HoldMouse(Target t, int x, int y, int button) __attribute__((stdcall)); 
extern "C" void EIOS_ReleaseMouse(Target t, int x, int y, int button) __attribute__((stdcall)); 
extern "C" bool EIOS_IsMouseHeld(Target t, int button) __attribute__((stdcall)); 
extern "C" void EIOS_SendString(Target t, char* str, int keywait, int keymodwait) __attribute__((stdcall)); 
extern "C" void EIOS_HoldKey(Target t, int key) __attribute__((stdcall)); 
extern "C" void EIOS_ReleaseKey(Target t, int key) __attribute__((stdcall)); 
extern "C" bool EIOS_IsKeyHeld(Target t, int key) __attribute__((stdcall)); 


extern "C" int exp_clientID(int idx);
extern "C" int exp_getClients(bool only_unpaired);
extern "C" int exp_spawnClient(char* remote_path, char *root, char *params, int width, int height, char *initseq, char *useragent, char* javaargs, char* Plugins);
extern "C" bool exp_pairClient(int pid);
extern "C" int exp_getCurrent();
extern "C" bool exp_killClient(int pid);
extern "C" void* exp_getImageArray();
extern "C" void* exp_getDebugArray();
extern "C" int exp_getRefresh();
extern "C" void exp_setRefresh(int x);
extern "C" void exp_setTransparentColor(int color);
extern "C" void exp_setDebug(bool enabled);
extern "C" void exp_setGraphics(bool enabled);
extern "C" void exp_setEnabled(bool enabled);
extern "C" bool exp_isActive();
extern "C" bool exp_isBlocking();
extern "C" void exp_getMousePos(int &x, int &y);
extern "C" void exp_holdMouse(int x, int y, bool left);
extern "C" void exp_releaseMouse(int x, int y, bool left);
extern "C" void exp_holdMousePlus(int x, int y, int button);
extern "C" void exp_releaseMousePlus(int x, int y, int button);
extern "C" void exp_moveMouse(int x, int y);
extern "C" void exp_windMouse(int x, int y);
extern "C" void exp_clickMouse(int x, int y, bool left);
extern "C" void exp_clickMousePlus(int x, int y, int button);
extern "C" bool exp_isMouseButtonHeld(int button);
extern "C" void exp_sendKeys(char *text, int keywait, int keymodwait);
extern "C" void exp_holdKey(int code);
extern "C" void exp_releaseKey(int code);
extern "C" bool exp_isKeyDown(int code);

//Exports for Local
#define NumExports 30
static char* exports[] = {
    (char*)"exp_clientID", (char*)"function SmartClientID(idx: integer): integer;",
    (char*)"exp_getClients", (char*)"function SmartGetClients(only_unpaired: boolean): integer;",
    (char*)"exp_spawnClient",(char*)"function SmartSpawnClient(remote_path, root, params: string; width, height: integer; initseq, useragent, javaargs, Plugins: string): integer;",
    (char*)"exp_pairClient", (char*)"function SmartPairClient(pid: integer): boolean;",
    (char*)"exp_killClient", (char*)"function SmartKillClient(pid: integer): boolean;",
    (char*)"exp_getCurrent", (char*)"function SmartCurrentClient(): integer;",
    (char*)"exp_getImageArray", (char*)"function SmartImageArray(): int" 
    #if __SIZEOF_POINTER__ == 4
        "eger;",
    #else
        "64;",
    #endif  
    (char*)"exp_getDebugArray", (char*)"function SmartDebugArray(): int" 
    #if __SIZEOF_POINTER__ == 4
        "eger;",
    #else
        "64;",
    #endif
    (char*)"exp_getRefresh", (char*)"function SmartGetRefresh: integer;",
    (char*)"exp_setRefresh", (char*)"procedure SmartSetRefresh(x: integer);",
    (char*)"exp_setTransparentColor", (char*)"procedure SmartSetTransparentColor(color: integer);",
    (char*)"exp_setDebug", (char*)"procedure SmartSetDebug(enabled: boolean);",
    (char*)"exp_setGraphics", (char*)"procedure SmartSetGraphics(enabled: boolean);",
    (char*)"exp_setEnabled", (char*)"procedure SmartSetEnabled(enabled: boolean);",
    (char*)"exp_isActive", (char*)"function SmartActive: boolean;",
    (char*)"exp_isBlocking", (char*)"function SmartEnabled: boolean;",
    (char*)"exp_getMousePos", (char*)"procedure SmartGetMousePos(var x, y: integer);",
    (char*)"exp_holdMouse", (char*)"procedure SmartHoldMouse(x, y: integer; left: boolean);",
    (char*)"exp_releaseMouse", (char*)"procedure SmartReleaseMouse(x, y: integer; left: boolean);",
    (char*)"exp_holdMousePlus", (char*)"procedure SmartHoldMousePlus(x, y, button: integer);",
    (char*)"exp_releaseMousePlus", (char*)"procedure SmartReleaseMousePlus(x, y, button: integer);",
    (char*)"exp_moveMouse", (char*)"procedure SmartMoveMouse(x, y: integer);",
    (char*)"exp_windMouse", (char*)"procedure SmartWindMouse(x, y: integer);",
    (char*)"exp_clickMouse", (char*)"procedure SmartClickMouse(x, y: integer; left: boolean);",
    (char*)"exp_clickMousePlus", (char*)"procedure SmartClickMousePlus(x, y, button: integer);",
    (char*)"exp_isMouseButtonHeld", (char*)"function SmartIsMouseButtonHeld(button: integer): boolean;",
    (char*)"exp_sendKeys", (char*)"procedure SmartSendKeys(Text: String; keywait, keymodwait: integer);",
    (char*)"exp_holdKey", (char*)"procedure SmartHoldKey(Code: Integer);",
    (char*)"exp_releaseKey", (char*)"procedure SmartReleaseKey(Code: Integer);",
    (char*)"exp_isKeyDown", (char*)"function SmartIsKeyDown(Code: Integer): Boolean;"
};

#endif	/* _LOCAL_H */

