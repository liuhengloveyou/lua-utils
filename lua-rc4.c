// gcc -fPIC -shared -I/usr/local/include/luajit-2.0/ lua-rc4.c -lluajit-5.1 -lcrypto -o luaRC4.so

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
	int inDataLen = luaL_checknumber(L, 3);
	
	size_t keyLen = strlen(keyData);
	if (keyLen < 1 || inDataLen < 1) {
		return 0;
	}

	unsigned char outData[inDataLen+1];
	RC4_KEY key;

	RC4_set_key(&key, keyLen, keyData);
	RC4(&key, inDataLen, inData, outData);
	lua_pushlstring(L, outData, inDataLen);
	lua_pushnumber(L, inDataLen);

	return 2;
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
