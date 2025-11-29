#pragma once

template<typename T>
class MyStack{
private:
    struct Node{
        T data;
        Node* next;
        Node(const T& val, Node* nxt = nullptr) : data(val), next(nxt) {}
    };
    Node* head;

public:
    MyStack() : head(nullptr) {}

    ~MyStack(){
        while (!empty()) pop();
    }

    void push(const T& val){
        head = new Node(val, head);
    }

    void pop(){
        if (head){
            Node* tmp = head;
            head = head->next;
            delete tmp;
        }
    }

    T& top(){
        return head->data;
    }

    bool empty() const{
        return head == nullptr;
    }

    void clear(){
        while (!empty()) pop();
    }
};
