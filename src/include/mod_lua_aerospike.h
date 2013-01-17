#pragma once

#include "as_aerospike.h"
#include "mod_lua_val.h"
#include <lua.h>

int mod_lua_aerospike_register(lua_State *);

as_aerospike * mod_lua_pushaerospike(lua_State *, mod_lua_scope, as_aerospike * );

as_aerospike * mod_lua_toaerospike(lua_State *, int);
