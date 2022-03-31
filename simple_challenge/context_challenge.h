#ifndef CONTEXT_CHALLENGE_H
#define CONTEXT_CHALLENGE_H


/////  FILE INCLUDES  /////

#include <Windows.h>
#include <time.h>
#include <stdio.h>
#include "json.h"




/// /////  GLOBAL VARIABLES  /////

struct ChallengeEquivalenceGroup* group;
struct Challenge* challenge;




/////  STRUCTS AND ENUMS  /////

typedef unsigned char byte;

struct ChallengeEquivalenceGroup {
	char* id;
	struct KeyData* subkey;	// Not obtained from json
	struct Challenge** challenges;
};

struct Challenge {
	WCHAR* file_name;
	HINSTANCE lib_handle;
	json_value* properties;	// Properties as a json_value
};

struct KeyData {
	byte* data;
	int size;
	time_t expires;
};

#endif // !CONTEXT_CHALLENGE_H