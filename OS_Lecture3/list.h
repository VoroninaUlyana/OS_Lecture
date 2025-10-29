#pragma once
#include <iostream>
#include <cstdlib>
struct Node 
{
    int data;
    Node* ssylka;
    Node(int val) : data(val), ssylka(nullptr) {}
};
void printlist(Node* head);
Node* reverselist(Node* head);
void newelem(Node*& head, int value);
void clearlist(Node*& head);
