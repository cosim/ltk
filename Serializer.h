#pragma once
#include "LuaObject.h"
#include "NetworkBuffer.h"

namespace ltk {

class Serializer : public LuaObject
{
public:
    RTTI_DECLARATIONS(Serializer, LuaObject);
    Serializer();
    ~Serializer();

    static int LuaConstructor(lua_State *L);
    static int Serialize(lua_State *L);
    static int Deserialize(lua_State *L);

    void RecSerialize(lua_State *L, int idx);
    void RecDeserialize(lua_State *L);

    BEGIN_LUA_METHOD_MAP(Sprite)
        LUA_METHOD_ENTRY(Serialize)
    END_LUA_METHOD_MAP()

public:
    NetworkBuffer m_buffer;
    int m_cnt = 0;
};

}
