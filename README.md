# CHALLENGE_SIMPLE-NOKIA

A challenge is a piece of code that executes a procedure to generate a result that can be used as a key or a part of it.
Each different challenge result can have a different result length. The combination of different challenge results is the final key used to cipher/decipher.
The challenge result depends on user context (time, computer fingerprint, location, network characteristics, etc.). The right key is unknown, therefore it is  impossible to validate if the result obtained from the challenge is correct or not.

A challenge can have specific parameters. For example a GPS positioning challenge can use the coordinates as a key and the result will be valid for deciphering top secret document only in the secret laboratory location. The parameters for this challenge could be the type of coordinates (latitude only, degrees with or without decimals, etc.).

This challenge is just given as an example to build your own challenges.
This challenge is extremely simple. Contains:
 - A thread that preiodically calls the execute function.
 - A function invoked by the thread to execute the challenge itself (and updates the key data).

Every challenge must implement the API:
 - init(): initialize challenge custom global variables, and whatever the challenge programmer wants. This function is also useful to launch a thread to periodically execute the executeChallenge() function to update the key data (key and expire time).
 - executeChallenge(): executes the procedure defined by programmer to generate the key based on user context. Also updates the expire time.

In the case that ther is not a thread updating the key data periodically, the secure virtual filesystem logic will invoke executeChallenge() when the key expires.
