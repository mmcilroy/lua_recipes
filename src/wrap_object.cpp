extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include <iostream>
#include <sstream>

class Foo
{
public:
    Foo( const std::string& name ) :
        name(name)
    {
        std::cout << "Foo is born" << std::endl;
    }

    std::string Add( int a, int b )
    {
        std::stringstream ss;
        ss << name << ": " << a << " + " << b << " = " << (a+b);
        return ss.str();
    }

    ~Foo()
    {
        std::cout << "Foo is gone" << std::endl;
    }

private:
    std::string name;
};

int l_Foo_constructor( lua_State* l )
{
    const char * name = luaL_checkstring( l, 1 );
    Foo ** udata = (Foo**)lua_newuserdata( l, sizeof(Foo*) );
    *udata = new Foo( name );
    luaL_getmetatable( l, "luaL_Foo" );
    lua_setmetatable( l, -2 );
    return 1;
}

Foo* l_CheckFoo( lua_State* l, int n )
{
    return *(Foo**)luaL_checkudata( l, n, "luaL_Foo" );
}

int l_Foo_add( lua_State* l )
{
    Foo * foo = l_CheckFoo( l, 1 );
    int a = luaL_checknumber( l, 2 );
    int b = luaL_checknumber( l, 3 );

    std::string s = foo->Add( a, b );
    lua_pushstring( l, s.c_str() );

    return 1;
}

int l_Foo_destructor(lua_State * l)
{
    Foo * foo = l_CheckFoo(l, 1);
    delete foo;

    return 0;
}

void RegisterFoo(lua_State * l)
{
    luaL_Reg sFooRegs[] = {
        { "new", l_Foo_constructor },
        { "add", l_Foo_add },
        { "__gc", l_Foo_destructor },
        { NULL, NULL }
    };

    luaL_newmetatable( l, "luaL_Foo" );
    luaL_setfuncs( l, sFooRegs, 0 );
    lua_pushvalue( l, -1 );
    lua_setfield( l, -1, "__index" );
    lua_setglobal( l, "Foo" );
}

int main()
{
    lua_State * l = luaL_newstate();
    luaL_openlibs( l );
    RegisterFoo( l );

    int erred = luaL_dofile( l, "fun.lua" );
    if( erred ) {
        std::cout << "Lua error: " << luaL_checkstring( l, -1 ) << std::endl;
    }

    lua_close( l );

    return 0;
}