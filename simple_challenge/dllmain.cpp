// dllmain.cpp : Defines the entry point for the DLL application.
/*
This is the expected json associated to the challenge:
{
	"FileName": "simple_challenge.dll",
	"Description": "This is a simple challenge that does nothing.",
	"Props": {
		"validity_time": 3600,
		"refresh_time": 3000,
		"param1": "texto",
		"param2": 3
	},
	"Requirements": "none"
}
*/

/////  FILE INCLUDES  /////

#include "pch.h"
#include "context_challenge.h"




/////  DEFINITIONS  /////

#define VALID_KEY 0




/////  GLOBAL VARIABLES  /////

int validity_time = 0;
int refresh_time = 0;
char* param1 = NULL;
int param2 = 0;

HANDLE h_thread;




/////  FUNCTION DEFINITIONS  /////

extern "C" _declspec(dllexport) int init(struct ChallengeEquivalenceGroup* group_param, struct Challenge* challenge_param);
void refresh_subkey(LPVOID th_param);
extern "C" _declspec(dllexport) int executeChallenge();
void getChallengeParameters();




/////  FUNCTION IMPLEMENTATIONS  /////

int init(struct ChallengeEquivalenceGroup* group_param, struct Challenge* challenge_param){
	int result = 0;

	// It is mandatory to fill these global variables
	group = group_param;
	challenge = challenge_param;
	if (group == NULL || challenge == NULL)
		return - 1;

	// Process challenge parameters
	getChallengeParameters();

	printf("Initializing (%ws)\n", challenge->file_name);

	// It is optional to execute the challenge here
	result = executeChallenge();

	// It is optional to launch a thread to refresh the key here, but it is recommended
	if (result == 0) {
		h_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)refresh_subkey, NULL, 0, NULL);
	}

	return result;
}

void refresh_subkey(LPVOID th_param) {
	printf("Refresh (%ws)\n", challenge->file_name);
	while (true) {
		Sleep(refresh_time);      // Sleep first due to execute function already launched by init()
		executeChallenge();
	}
	return;
}

int executeChallenge() {
	printf("Execute (%ws)\n", challenge->file_name);
	if (group == NULL || challenge == NULL)
		return -1;

	int size_of_key = strlen(param1) * param2 * sizeof(char);
	byte* key = (byte*)malloc(strlen(param1) * param2 * sizeof(char));

	// Calculate key
	for (int i = 0; i < param2; i++) {
		memcpy_s(key+i*strlen(param1), size_of_key- i * strlen(param1), param1, strlen(param1));
	}

	// AbrirSecciónCrítica()
	if (group->subkey->data != NULL)
		free(group->subkey->data);
	group->subkey->data = key;
	group->subkey->expires = time(NULL) + validity_time;
	group->subkey->size = size_of_key;
	// CerrarSecciónCrítica()

	return 0;   // Always 0 means OK.
}

void getChallengeParameters() {
	printf("Getting challenge parameters\n");
	json_value* value = challenge->properties;
	for (int i = 0; i < value->u.object.length; i++) {
		if (strcmp(value->u.object.values[i].name, "validity_time") == 0) {
			validity_time = (int)(value->u.object.values[i].value->u.integer);
		}
		else if (strcmp(value->u.object.values[i].name, "refresh_time") == 0) {
			refresh_time = (int)(value->u.object.values[i].value->u.integer);
		}
		else if (strcmp(value->u.object.values[i].name, "param1") == 0) {
			param1 = (char*)malloc(value->u.object.values[i].value->u.string.length * sizeof(char));
			strcpy_s(param1, value->u.object.values[i].value->u.string.length * sizeof(char), value->u.object.values[i].value->u.string.ptr);
		}
		else if (strcmp(value->u.object.values[i].name, "param2") == 0) {
			validity_time = (int)(value->u.object.values[i].value->u.integer);
		}
		else fprintf(stderr, "WARINING: the field '%s' included in the json configuration file is not registered and will not be processed.\n", value->u.object.values[i].name);
	}
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
