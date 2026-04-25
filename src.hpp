#ifndef PYLIST_H
#define PYLIST_H

#include <iostream>
#include <vector>
#include <memory>
#include <stdexcept>
#include <cstdint>

class pylist {
    struct Node {
        enum Type { INT, LIST } type;
        long long ival;
        std::vector<pylist> lval;
        Node() : type(LIST), ival(0), lval() {}
        explicit Node(long long v) : type(INT), ival(v), lval() {}
    };

    std::shared_ptr<Node> ptr;

    void ensure_list() {
        if (!ptr) ptr = std::make_shared<Node>();
        if (ptr->type != Node::LIST) {
            // convert this node to an empty list
            ptr = std::make_shared<Node>();
        }
    }

    static void print(std::ostream &os, const pylist &ls) {
        if (!ls.ptr) { os << "[]"; return; }
        if (ls.ptr->type == Node::INT) {
            os << ls.ptr->ival;
        } else {
            os << '[';
            for (size_t i = 0; i < ls.ptr->lval.size(); ++i) {
                if (i) os << ", ";
                print(os, ls.ptr->lval[i]);
            }
            os << ']';
        }
    }

public:
    // constructors
    pylist() : ptr(std::make_shared<Node>()) {}
    pylist(const pylist &) = default;
    pylist &operator=(const pylist &) = default; // share by reference

    // construct from integer
    pylist(int v) : ptr(std::make_shared<Node>(static_cast<long long>(v))) {}
    pylist(long long v) : ptr(std::make_shared<Node>(v)) {}

    // assignment from integer
    pylist &operator=(long long v) {
        ptr = std::make_shared<Node>(v);
        return *this;
    }
    pylist &operator=(int v) { return (*this = static_cast<long long>(v)); }

    // append overloads
    void append(const pylist &x) {
        ensure_list();
        ptr->lval.push_back(x);
    }
    void append(long long v) {
        ensure_list();
        ptr->lval.emplace_back(v);
    }
    void append(int v) { append(static_cast<long long>(v)); }

    // pop from back; if empty, return empty list
    pylist pop() {
        ensure_list();
        if (ptr->lval.empty()) return pylist();
        pylist back = ptr->lval.back();
        ptr->lval.pop_back();
        return back;
    }

    // index access
    pylist &operator[](size_t i) {
        ensure_list();
        if (i >= ptr->lval.size()) throw std::out_of_range("pylist index out of range");
        return ptr->lval[i];
    }
    const pylist &operator[](size_t i) const {
        if (!ptr || ptr->type != Node::LIST || i >= ptr->lval.size())
            throw std::out_of_range("pylist index out of range");
        return ptr->lval[i];
    }

    // implicit conversion to integer for arithmetic/bitwise/compare usage
    operator long long() const {
        if (!ptr) return 0;
        if (ptr->type != Node::INT) throw std::runtime_error("cannot convert list to int");
        return ptr->ival;
    }

    friend std::ostream &operator<<(std::ostream &os, const pylist &ls) {
        print(os, ls);
        return os;
    }
};

#endif // PYLIST_H
