#pragma once

namespace ltk {

template<typename T>
class Delegate;

class IConnection
{
public:
    virtual void Close() = 0;
    virtual ~IConnection() {}
};

class DelegateTracker
{
public:
    DelegateTracker() {}
    explicit DelegateTracker(IConnection *conn) : m_conn(conn) {}
    DelegateTracker(DelegateTracker &&rhs) { m_conn = rhs.m_conn; rhs.m_conn = nullptr; }
    void operator=(DelegateTracker &&rhs) { m_conn = rhs.m_conn; rhs.m_conn = nullptr; }
    DelegateTracker(const DelegateTracker &rhs) = delete;
    void operator=(const DelegateTracker &rhs) = delete;
    ~DelegateTracker() {
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
struct DelegateNode
{
    std::function<T> lambda;
    DelegateNode<T> *next = nullptr;
};

template<typename T>
class Connection : public IConnection
{
public:
    Connection(Delegate<T> *d, DelegateNode<T> *n) : m_d(d), m_n(n) {}
    ~Connection() {}
    void Close() { m_d->Remove(m_n); }

private:
    Delegate<T> *m_d;
    DelegateNode<T> *m_n;
};

template<typename T>
class Delegate
{
public:
    DelegateTracker Attach(const std::function<T> &cb)
    {
        auto node = new DelegateNode<T>;
        node->lambda = cb;
        if (m_head == nullptr) {
            m_head = node;
        }
        else {
            auto p = m_head;
            auto p2 = m_head;
            while (p) {
                p2 = p;
                p = p->next;
            }
            p2->next = node;
        }
        return std::move(DelegateTracker(new Connection<T>(this, node)));
        // TODO FIXME
    }

    bool Remove(DelegateNode<T> *node)
    {
        auto p = m_head;
        auto p2 = m_head;
        bool found = false;
        while (p) {
            if (p == node) {
                found = true;
                break;
            }
            p2 = p;
            p = p->next;
        }
        if (p) {
            p2->next = p->next;
            delete p;
        }
        return false; // TODO FIXME
    }

    template<typename... Params>
    void Invoke(Params... params)
    {
        for (auto node = m_head; node; node = node->next) {
            (node->lambda)(std::forward<Params>(params)...);
        }
    }

private:
    DelegateNode<T> *m_head = nullptr;
};

} // namespace ltk
