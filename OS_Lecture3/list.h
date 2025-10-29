#pragma once
#include <iostream>
struct Node 
{
    int data;
    Node* ssylka;
    Node(int val) : data(val), ssylka(nullptr) {}
};
void printlist(Node* head);
