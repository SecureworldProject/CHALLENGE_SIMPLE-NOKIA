// dllmain.cpp : Defines the entry point for the DLL application.


/////  FILE INCLUDES  /////

#include "pch.h"
#include "context_challenge.h"




/////  DEFINITIONS  /////

#define VALID_TIME 10   // seconds
#define VALID_KEY 0




/////  GLOBAL VARIABLES  /////

struct ChallengeEquivalenceGroup* group;
struct Challenge* challenge;
HANDLE h_thread;




/////  FUNCTION DEFINITIONS  /////

extern "C" _declspec(dllexport) int init(struct ChallengeEquivalenceGroup* group_param, struct Challenge* challenge_param);
void refresh_subkey(LPVOID th_param);
extern "C" _declspec(dllexport) int executeChallenge();




/////  FUNCTION IMPLEMENTATIONS  /////

int init(struct ChallengeEquivalenceGroup* group_param, struct Challenge* challenge_param){
    int result = 0;

    group = group_param;
    challenge = challenge_param;

    printf("Initializing (%ws)\n", challenge->file_name);

    result = executeChallenge();
    if (result == 0) {
        h_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)refresh_subkey, NULL, 0, NULL);
    }

    return result;
}

void refresh_subkey(LPVOID th_param) {
    printf("Refresh (%ws)\n", challenge->file_name);
    while (true) {
        Sleep(VALID_TIME / 2 * 1000);      // Sleep first due to execute function already launched by init()
        executeChallenge();
    }
    return;
}

int executeChallenge() {
    printf("Execute (%ws)\n", challenge->file_name);

    group->subkey->expires = time(NULL) + VALID_TIME;
    group->subkey->data = VALID_KEY;

    return 0;   // Always 0 means OK.
}



BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
