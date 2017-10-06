* Login-Dialog just with Internet
* Encrypt Modules with Platform hash
* Sign Modules
* Move Website from Telegram to Email
* Add Local Server
* Unify all Data(hide extension and name)
* Change to Cert-Auth with cert signed by global Server on first Login

# Local Server
An Local Server sends UDP-Broadcast packets into Local Network.
If QDigi receives such, it connects to the address from this Server
* Server must have a List of Users and their bought modules.
* Every module must have a Version, that changes at every Edit.
** So, even Modules which are bought can be synced
* Maybe every QDigi can share the basic modules with each other.
** Extensions must contain hashes of old Version. If another QDigi has a newer, it adds them.
** Merge-Conflict?
* Servers should be signed.
** Web of Trust(maybe)
