#pragma once

namespace cs {

template<typename T>
class Delegate;

class IConnection
{
public:
    virtual void Close() = 0;
    virtual IConnection *Copy() = 0;
    virtual ~IConnection() {}
};

class ConnectionTracker
{
public:
    ConnectionTracker() {}
    explicit ConnectionTracker(IConnection *conn) : m_conn(conn) {}
    ConnectionTracker(ConnectionTracker &&rhs) { m_conn = rhs.m_conn; rhs.m_conn = nullptr; }
    void operator=(ConnectionTracker &&rhs) { m_conn = rhs.m_conn; rhs.m_conn = nullptr; }
    ConnectionTracker(const ConnectionTracker &rhs) = delete;
    void operator=(const ConnectionTracker &rhs) = delete;
    ~ConnectionTracker() {
        int i = 0;
        i = 1;
        delete m_conn; 
        i = 2;
    }

    void Close() { m_conn->Close(); }

private:
    IConnection * m_conn = nullptr;
};

template<typename T>
class Connection : public IConnection
{
public:
    Connection(Delegate<T> *d, size_t key) : m_d(d), m_key(key) {}
    ~Connection() {}
    void Close() { m_d->Remove(m_key); }
    IConnection *Copy() { return new Connection(m_d, m_key); }
    //void Track(ConnectionTracker *tracker) { tracker->Add(this); }

private:
    Delegate<T> *m_d;
    size_t m_key;
};

template<typename T>
class Delegate
{
public:
    ConnectionTracker Attach(const std::function<T> &cb)
    {
        size_t key = m_nextId;
        m_nextId++;
        m_listeners[key] = cb;
        return std::move(ConnectionTracker(new Connection<T>(this, key)));
    }

    bool Remove(size_t key)
    {
        auto iter = m_listeners.find(key);
        if (iter != m_listeners.end())
        {
            m_listeners.erase(iter);
            return true;
        }
        return false;
    }

    template<typename... Params>
    void Invoke(Params... params)
    {
        for (auto iter = m_listeners.begin(); iter != m_listeners.end(); ++iter)
        {
            (iter->second)(std::forward<Params>(params)...);
        }
    }

private:
    std::map<size_t, std::function<T>> m_listeners;
    size_t m_nextId = 0;
};

} // namespace cs
