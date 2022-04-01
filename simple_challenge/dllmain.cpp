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




/////  GLOBAL VARIABLES  /////

char* param1 = NULL;
int param2 = 0;




/////  FUNCTION DEFINITIONS  /////

void getChallengeProperties();




/////  FUNCTION IMPLEMENTATIONS  /////

int init(struct ChallengeEquivalenceGroup* group_param, struct Challenge* challenge_param){
	printf("Initializing (%ws)\n", challenge->file_name);

	int result = 0;

	// It is mandatory to fill these global variables
	group = group_param;
	challenge = challenge_param;
	if (group == NULL || challenge == NULL)
		return - 1;

	// Process challenge parameters
	getChallengeProperties();

	// It is optional to execute the challenge here
	result = executeChallenge();

	// It is optional to launch a thread to refresh the key here, but it is recommended
	if (result == 0) {
		periodicExecution(periodic_execution);
	}

	return result;
}

int executeChallenge() {
	printf("%llu: Executing challenge (%ws)\n", time(NULL), challenge->file_name);

	// Nullity check
	if (group == NULL || challenge == NULL || param1 == NULL)
		return -1;


	// Calculate new key (size, data and expire date)
	int new_size = strlen(param1) * param2 * sizeof(char);

	byte* new_key_data = (byte*)malloc(strlen(param1) * param2 * sizeof(char));
	if (new_key_data == NULL)
		return -1;

	for (int i = 0; i < param2; i++) {
		if (0 != memcpy_s(new_key_data + i * strlen(param1), new_size - i * strlen(param1), param1, strlen(param1))) {
			free(new_key_data);
			return -1;
		}
	}

	time_t new_expires = time(NULL) + validity_time;


	// Update KeyData inside critical section
	EnterCriticalSection(&(group->subkey->critical_section));
	if ((group->subkey)->data != NULL) {
		free((group->subkey)->data);
	}
	group->subkey->data = new_key_data;
	group->subkey->expires = new_expires;
	group->subkey->size = new_size;
	LeaveCriticalSection(&(group->subkey->critical_section));

	return 0;	// Always 0 means OK.
}


void getChallengeProperties() {
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
			param1 = (char*)malloc((1 + value->u.object.values[i].value->u.string.length) * sizeof(char));
			if (param1 != NULL) {
				strcpy_s(param1, (1 + value->u.object.values[i].value->u.string.length) * sizeof(char), value->u.object.values[i].value->u.string.ptr);
			}
		}
		else if (strcmp(value->u.object.values[i].name, "param2") == 0) {
			param2 = (int)(value->u.object.values[i].value->u.integer);
		}
		else fprintf(stderr, "WARNING: the field '%s' included in the json configuration file is not registered and will not be processed.\n", value->u.object.values[i].name);
	}
	printf("Challenge properties: \n  validity_time = %d \n  refresh_time = %d \n  param1 = %s \n  param2 = %d \n",
		validity_time, refresh_time, (param1 == NULL) ? "NULL" : param1, param2);
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
