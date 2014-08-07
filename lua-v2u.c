// gcc -std=gnu99 -fPIC -shared -I/usr/local/include/luajit-2.0/ conv2u.c -lluajit-5.1 -o luaV2U.so

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>

#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"

static int conv2u(const char* codepage, const char* source, int slen, char* desc, int dlen)
{
	if (!codepage || !source || !desc) {
		return -1;
	}
	
	char *rst = setlocale(LC_CTYPE, codepage);
	if (!rst) {
		return -1;
	}
	
	mbstate_t mbst = {0};
	wchar_t* lpWC = (wchar_t *)malloc(slen * sizeof(wchar_t) + 10);
	if (!lpWC) {
		return -1;
	}
	memset((char*)lpWC, 0, slen * sizeof(wchar_t) + 10);

	int rt = mbsrtowcs(lpWC, (const char**)&source, slen, &mbst);
	if (rt == -1) {
		free(lpWC);
		return -1;
	}

	int idx = 0;
	int tmplen = wcslen(lpWC);
	for (int i = 0; i < tmplen; i++) {
		char c[5] = {0};
		snprintf(c, 5, "%x", lpWC[i]);
		if (strlen(c) == 4){
			idx += snprintf(desc+idx, dlen - idx, "\\u%s", c);
		} else if (strlen(c) == 3) {
			idx += snprintf(desc+idx, dlen - idx, "\\u0%s", c);
		} else {
			unsigned char d = 0;
			memcpy((char*)&d, (char*)(&lpWC[i]), 1);
			if (d > 127) {
				idx += snprintf(desc+idx, dlen - idx, "\\u00%s", c);
			} else {
				idx += snprintf(desc+idx, dlen - idx, "%c", lpWC[i]);
			}
		}
	}
	
	free(lpWC);
	return idx;
}

static int l_v2u(lua_State *L)
{
	const char *codepage = luaL_checkstring(L, 1);
	const char *source = luaL_checkstring(L, 2);
	int slen = luaL_checknumber(L, 3);
	
	char *desc = malloc(slen * 3);
	if (!desc) {
		return -1;
	}

	int rst = conv2u(codepage, source, slen, desc, slen*3);
	if (rst == -1) {
		return -1;
	}
	
	lua_pushlstring(L, desc, rst);

	return 1;
}


static const struct luaL_Reg luaV2U[] = {
	{"conv2u", l_v2u},
	{NULL, NULL}
};

int luaopen_luaV2U(lua_State *L)
{
	luaL_register(L, "luaV2U", luaV2U);
	return 1;
}

/*
  print(luaV2U.conv2u("zh_CN.UTF-8", "这ＪＢ就是在搞笑aaaaaaaa:ab", string.len("这ＪＢ就是在搞笑aaaaaaaa:ab")))
int main() 
{
	char *src = "这ＪＢ就是在搞笑aaaaaaaa:abd";
	char desc[1024] = {0};
	int len = 1024;

	conv2u("zh_CN.UTF-8", src, strlen(src), desc, len);
	printf("%s\n", desc);

	return 0;
}
*/
