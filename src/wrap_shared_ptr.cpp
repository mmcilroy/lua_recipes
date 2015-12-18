extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include <iostream>
#include <sstream>
#include <memory>

class Foo
{
public:
    Foo( const std::string& name ) :
        name(name)
    {
        std::cout << "Foo is born!" << std::endl;
    }

    std::string Add( int a, int b )
    {
        std::stringstream ss;
        ss << name << ": " << a << " + " << b << " = " << (a+b);
        return ss.str();
    }

    ~Foo()
    {
        std::cout << "Foo is gone!" << std::endl;
    }

private:
    std::string name;
};

int l_Foo_constructor( lua_State* l )
{
    const char* name = luaL_checkstring( l, 1 );

    // create a userdata same size as required shared_ptr
    void* udata = lua_newuserdata( l, sizeof( std::shared_ptr< Foo > ) );

    // use placement new to create the shared_ptr
    std::shared_ptr< Foo >* foo = new( udata ) std::shared_ptr< Foo >( new Foo( name ) );

    // assign the meta table
    luaL_getmetatable( l, "luaL_Foo" );
    lua_setmetatable( l, -2 );

    return 1;
}

std::shared_ptr< Foo >* l_CheckFoo( lua_State* l, int n )
{
    // check meta name and cast to shared_ptr pointer
    void* udata = luaL_checkudata( l, n, "luaL_Foo" );
    return ( std::shared_ptr< Foo >* )udata;
}

int l_Foo_add( lua_State* l )
{
    std::shared_ptr< Foo >* foo = l_CheckFoo( l, 1 );
    int a = luaL_checknumber( l, 2 );
    int b = luaL_checknumber( l, 3 );

    std::string s = (*foo)->Add( a, b );
    lua_pushstring( l, s.c_str() );

    return 1;
}

int l_Foo_destructor(lua_State * l)
{
    // explicity call the destructor on the shared_ptr to release memory
    std::shared_ptr< Foo >* foo = l_CheckFoo( l, 1 );
    foo->~shared_ptr< Foo >();

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
