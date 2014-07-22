// gcc -fPIC -shared -I/usr/local/include/luajit-2.0/ lua-rc4.c -lluajit-5.1 -lcrypto -o luaRC4.so

/*
> require "luaRC4";
> r=luaRC4.rc4('123','12312312312312');
> print(r);
b��G5�R1�O)
> print(luaRC4.rc4('123',r));
12312312312312
*/

#include <stdio.h>
#include <string.h>
#include <openssl/rc4.h>


#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

static int l_rc4(lua_State *L)
{
	const unsigned char *keyData = luaL_checkstring(L, 1);
	const unsigned char *inData = luaL_checkstring(L, 2);

	size_t keyLen = strlen(keyData);
	size_t dataLen = strlen(inData);
	if (keyLen < 1 || dataLen < 1) {
		return 0;
	}

	unsigned char outData[dataLen+1];
	RC4_KEY key;
	
	RC4_set_key(&key, keyLen, keyData);
	RC4(&key, dataLen, inData, outData);
	lua_pushstring(L, outData);
	
	return 1;	
}


static const struct luaL_Reg luaRC4[] = {
	{"rc4", l_rc4},
	{NULL, NULL}
};

int luaopen_luaRC4(lua_State *L)
{
	luaL_register(L, "luaRC4", luaRC4);
	return 1;
}
