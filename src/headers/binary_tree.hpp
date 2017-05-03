/*
 * Copyright (C) 2016 Ajado Aduragbemi Joseph <agbemi19@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef BINARY_TREE
#define BINARY_TREE

#include<iostream>
#include<list>

typedef struct TestDataStruct{
	int key;
} DataX;

template<typename Tp> struct Node{
	Tp data;	// data of a pre-defined structure represented by Tp.
	bool overwrite;
	Node<Tp>* parent;
	Node<Tp>* left;
	Node<Tp>* right;
};

enum class TraverseOrder{
	IN,
	PRE,
	POST
};

//Tp0 would be the predefined data structure
//Tp1 should be the data type of "key" in user-defined data structure
template<typename Tp0, typename Tp1> class BinarySearchTree{
	
	Node<Tp0>* m_node;
	Node<Tp0>* find(Node<Tp0>*, Tp1);
	void setDataArray(Node<Tp0>*);
	std::list<Tp0> m_data_array;

protected:
	void in_order(Node<Tp0>*, void(*)(Node<Tp0>*));
	void pre_order(Node<Tp0>*, void(*)(Node<Tp0>*));
	void post_order(Node<Tp0>*, void(*)(Node<Tp0>*));
	
public:
	BinarySearchTree() : m_node(nullptr){}
	
	void insert(Tp0);
	
	bool isEmpty();
	
	const Node<Tp0>* find(Tp1);
	
	void traverse(void(*)(Node<Tp0>*), TraverseOrder=TraverseOrder::IN);
	
	const Node<Tp0>* remove(Tp1);
	
	void setDataArray();
	
	std::list<Tp0> getDataArray();
};

#endif