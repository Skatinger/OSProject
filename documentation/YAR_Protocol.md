# YAR Protocol definition
## Introduction
For our project, we need to exchange messages between a server and a client. It is imperative that this communication be well-defined, i.e. it observes a clear protocol. Since this is really a project of our own, we figured we would come up with our own very simple set of rules.

## General remarks
It is vital that the exact commands be used, otherwise the connection will fail. \<attr\> is a placeholder for the string representing the value of the attribute "attr". For parsing simplicity, every message is terminated by a beloved ";".

## Conditions for Success
* keys and values may never contain the chars ':' or ';' due to protocol reasons. This should be checked before sending!
* The same goes for usernames and passwords, they should not contain ':' or ';'.

## Primitives
### KVS-related
* GET: get a key-value from the server
* PUT: put a new key-value to the server
* DEL: delete a key-value from the server
* UPD: replace a value of a key on the server

### Login-related
* LOGIN: login a user and get access to the server
* LOGOUT: logout and terminate session
* ADD_U: add a new user to the database
* DEL_U: delete a user from the database
* CHG_U: change username / password
* MK_ADM: make a user an admin

## Example
The following adds a new user unifr with password OPisgreat, adds a few values, updates one, and logs out.
```
ADD_U unifr:OPisgreat;
LOGIN unifr:OPisgreat;
PUT myFirstKey:myFirstValue;
PUT hello:bonjour;
PUT thanks:pomme;
DEL myFirstKey;
UPD thanks:merci;
LOGOUT unifr;
```

## GET
Is used to get a KVP from the server.
*Usage*: `GET <key>;`
In case of success, the server will reply with `10 GOT <key>:<value>;`
If the key couldn't be found, the server will reply with `100 KEY <key> NOT FOUND;`
If the key is invalid, the client implementation will notify `201 INVALID KEY;`
If the value is invalid, the client will complain `202 INVALID VALUE;`

## PUT
Used to add a new KVP to the server.
*Usage*: `PUT <key>:<value>;`
In case of success, the server will reply with `20 PUT <key>:<value>;`
If the key is already in use, the server will reply `203 KEY <key> NOT FREE;`
If the storage is full, the server will reply "999 SERVER OVERFLOW;"

## DEL
Used to delete a KVP from the server.
*Usage*: `DEL <key>;`
In case of success, the server will reply with `30 DELETED <key>:<value>;`
If the key wasn't found, it will send back `100 KEY <key> NOT FOUND;`

## UPDATE
Used to update the value of a certain KVP on the server.
*Usage*: `UPD <key>:<newValue>;`
In case of success, the server will reply with `40 UPDATED <key>:<newValue>;`
If the key wasn't found, it will reply `100 KEY <key> NOT FOUND;`

## LOGIN
Used to ask the server for access with given login information.
*Usage*: `LOGIN <username>:<password>;"
In case of success, the server will return `50 USER <username> LOGGED IN;`
If access is denied, the server will return `500 ACCESS FOR USER <username> DENIED;`

## LOGOUT
Logging out of the current server session to prevent any more changes.
*Usage*: `LOGOUT <username>;`
In case of success: `60 LOGOUT SUCCESSFUL;`
In case of FAILURE: `600 LOGOUT FAILED;`

## ADD_U
Allows to add a new user to the database. May only be done from admin. The server will check if the caller has sufficient access for this operation.
*Usage*: `ADD_U <username>:<password>;`

## DEL_U
Allows to delete a user from the database. May only be done from the user themself or an admin. It's the servers duty to check if this operation is allowed.
*Usage*: `DEL_U <username>`

## CHG_U
Used to update the credentials of a user. Again, the server must check that the caller has the rights to do so.
*Usage*: `CHG_U <username>:<newUserName>:<newPassword>;`

## MK_ADM
Used to give a user admin rights. May only be called by an existing admin, the server must check so.
*Usage*: `MK_ADM <username>;`
