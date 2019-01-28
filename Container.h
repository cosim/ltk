#pragma once

template<typename T>
class SinglyLinkedList
{
private:
    struct Node {
        Node(const T& _data) : next(nullptr), data(_data) {}
        Node(T&& _data) : next(nullptr), data(std::move(_data)) {}
        Node *next;
        T data;
    };
public:
    typedef void* Cookie;

    ~SinglyLinkedList() {
        auto node = m_head;
        while (node) {
            auto next = node->next;
            delete node;
            node = next;
        }
    }

    // add item to the end of list, O(n).
    Cookie PushBack(const T& data)
    {
        auto node = new Node(data);
        return PushBackRaw(node);
    }

    // add item to the end of list, O(n).
    Cookie PushBack(T&& data)
    {
        auto node = new Node(std::move(data));
        return PushBackRaw(node);
    }

    bool Remove(Cookie cookie)
    {
        auto node = reinterpret_cast<DelegateNode<T> *>(cookie);
        if (!node) {
            return false;
        }
        if (!m_head) {
            return false;
        }
        if (m_head == node) {
            auto tmp = m_head->next;
            delete m_head;
            m_head = tmp;
            return true;
        }
        auto p = m_head;
        while (p) {
            if (p->next == node) {
                p->next = p->next->next;
                delete node;
                return true;
            }
            p = p->next;
        }
        return false;
    }

    bool IsEmpty()
    {
        return m_head == nullptr;
    }

    void ForEach(std::function<bool(T)> visitor)
    {
        auto node = m_head;
        while (node) {
            if (!visitor(node->data)) break;
            node = node->next;
        }
    }

private:
    Cookie PushBackRaw(Node *node)
    {
        if (m_head == nullptr) {
            m_head = node;
        }
        else {
            auto p = m_head;
            while (p->next) {
                p = p->next;
            }
            p->next = node;
        }
        return node;
    }

private:
    Node *m_head = nullptr;
};