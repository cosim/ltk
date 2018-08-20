#include "stdafx.h"
#include "Serializer.h"
#include "NetworkBuffer.h"

namespace ltk {

Serializer::Serializer() : m_buffer(480)
{
}


Serializer::~Serializer()
{
}

void Serializer::RecSerialize(lua_State *L, int idx)
{
    LuaStackCheck chk(L);
    if (m_cnt > 200) {
        luaL_error(L, "Cycle detected when serializing data.");
        return;
    }
    m_cnt++;
    int type = lua_type(L, idx);

    // s string
    // i integer
    // d number
    // 1 true
    // 0 false
    // u lightuserdata
    // t table
    switch (type) {
    case LUA_TSTRING:
        do {
            size_t size = 0;
            const char *str = nullptr;
            m_buffer.WriteByte('s');
            str = lua_tolstring(L, idx, &size);
            m_buffer.WriteData(str, size);
        } while (0);
        break;
    case LUA_TNUMBER:
        if (lua_isinteger(L, idx)) {
            m_buffer.WriteByte('i');
            m_buffer.WriteInt64(lua_tointeger(L, idx));
        }
        else {
            m_buffer.WriteByte('d');
            m_buffer.WriteDouble(lua_tonumber(L, idx));
        }
        break;
    case LUA_TBOOLEAN:
        if (lua_toboolean(L, idx)) {
            m_buffer.WriteByte('1');
        }
        else {
            m_buffer.WriteByte('0');
        }
        break;
    case LUA_TLIGHTUSERDATA:
        do 
        {
            long long data = (long long)lua_touserdata(L, idx);
            m_buffer.WriteByte('u');
            m_buffer.WriteInt64(data);
        } while (0);
        break;
    case LUA_TTABLE:
        do {
            // array part
            int arr_head = m_buffer.Tell();
            m_buffer.WriteInt32(0);
            int hash_head = m_buffer.Tell();
            m_buffer.WriteInt32(0);
            int arr_i = 1;
            int arr_size = 0;
            lua_rawgeti(L, idx, arr_i);
            while (!lua_isnil(L, -1)) {
                int type = lua_type(L, -1);
                switch (type) {
                case LUA_TSTRING:
                case LUA_TNUMBER:
                case LUA_TBOOLEAN:
                case LUA_TLIGHTUSERDATA:
                case LUA_TTABLE:
                    RecSerialize(L, lua_gettop(L));
                    arr_size++;
                    break;
                }
                lua_pop(L, 1);
                arr_i++;
                lua_rawgeti(L, idx, arr_i);
            }
            lua_pop(L, 1); // for nil
            int cur = m_buffer.Tell();
            m_buffer.Seek(arr_head);
            m_buffer.WriteInt32(arr_size);
            m_buffer.Seek(cur);
            
            int hash_size = 0;
            // hash part
            lua_pushnil(L);
            while (lua_next(L, idx)) {
                if (lua_isinteger(L, -2)) {
                    int key = (int)lua_tointeger(L, -2);
                    if (key <= arr_size) {
                        lua_pop(L, 1); // pop value
                        continue; // skip array part
                    }
                }
                RecSerialize(L, lua_gettop(L) - 1); // -2 key
                RecSerialize(L, lua_gettop(L)); // -1 value
                lua_pop(L, 1); // pop value
                hash_size++;
            }
            cur = m_buffer.Tell();
            m_buffer.Seek(hash_head);
            m_buffer.WriteInt32(hash_size);
            m_buffer.Seek(cur);
        } while (0);
        break;
    }
}

int Serializer::Serialize(lua_State *L)
{
    Serializer *thiz = CheckLuaObject<Serializer>(L, 1);
    luaL_checkstack(L, 2, "no data");
    thiz->m_buffer.Rewind();
    thiz->m_cnt = 0;
    int top = lua_gettop(L);
    thiz->m_buffer.WriteInt32(top - 1);
    for (int i = 2; i <= top; i ++) {
        thiz->RecSerialize(L, i);
    }
    return 0;
}

int Serializer::LuaConstructor(lua_State *L)
{
    Serializer *thiz = new Serializer;
    thiz->PushToLua(L, "Serializer");
    thiz->Unref();
    return 1;
}

int Serializer::Deserialize(lua_State *L)
{
    Serializer *thiz = CheckLuaObject<Serializer>(L, 1);
    thiz->m_buffer.Rewind();
    long size = 0;
    if (thiz->m_buffer.ReadInt32(size) != NetworkBuffer::eBufferOk) {
        goto fail;
    }
    for (int i = 0; i < size; i++) {
        thiz->RecDeserialize(L);
    }

fail:
    return 0;
}

void Serializer::RecDeserialize(lua_State *L)
{
    unsigned char type = 0;
    if (m_buffer.ReadByte(type) != NetworkBuffer::eBufferOk) {
        return;
    }
    switch (type) {
    case 's':
        break;
    case 'i':
        break;
    case 'd':
        break;
    case '1':
        break;
    case '0':
        break;
    case 'u':
        break;
    case 't':
        break;
    }
}

}