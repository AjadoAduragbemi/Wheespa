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
 
#include "binary_tree.hpp"

template<typename Tp0, typename Tp1> bool BinarySearchTree<Tp0, Tp1>::isEmpty(){
	return (m_node == nullptr);
}


/* Insert */
template<typename Tp0, typename Tp1> void BinarySearchTree<Tp0, Tp1>::insert(Tp0 data){

	Node<Tp0>* tmp_node = new Node<Tp0>();
	tmp_node->data = data;
	
	if(isEmpty()) m_node = tmp_node;
	else{
	
		Node<Tp0>* node_ptr = m_node;
		
		while(true){
		
			if(node_ptr->data.key > data.key && node_ptr->left == nullptr && !node_ptr->overwrite){
				tmp_node->parent = node_ptr;
				node_ptr->left = tmp_node;
				break;
			}else if(node_ptr->data.key < data.key && node_ptr->right == nullptr && !node_ptr->overwrite){
				tmp_node->parent = node_ptr;
				node_ptr->right = tmp_node;
				break;
			}else if(node_ptr->overwrite){
				tmp_node->parent = node_ptr;
				node_ptr->data = data;
				node_ptr->overwrite = false;
				break;
			}
			
			node_ptr = (node_ptr->data.key >= data.key) ? node_ptr->left : node_ptr->right;
		}
	}
}


/* Search */
template<typename Tp0, typename Tp1> const Node<Tp0>* BinarySearchTree<Tp0, Tp1>::find(const Tp1& key){
	return find(m_node, key);
}


template<typename Tp0, typename Tp1> Node<Tp0>* BinarySearchTree<Tp0, Tp1>::find(Node<Tp0>* node, const Tp1& key){
	if(node == nullptr) return nullptr;
	if(node->data.key > key) return find(node->left, key);
	else if(node->data.key < key) return find(node->right, key);
	else return (!node->overwrite) ? node : nullptr;
}


/* Traverse: In-Order */
template<typename Tp0, typename Tp1> 
void BinarySearchTree<Tp0, Tp1>::traverse(void(*f)(Node<Tp0>*), TraverseOrder order){
	switch(order){
		case TraverseOrder::IN:
			inOrder(m_node, f);
			break;
		case TraverseOrder::PRE:
			preOrder(m_node, f);
			break;
		case TraverseOrder::POST:
			postOrder(m_node, f);
	}
}


template<typename Tp0, typename Tp1> void BinarySearchTree<Tp0, Tp1>::inOrder(Node<Tp0>* node,
																			  void(*foo)(Node<Tp0>*)){
	if(node != nullptr){
		inOrder(node->left, foo);
		if(!node->overwrite) foo(node);
		inOrder(node->right, foo);
	}
}


template<typename Tp0, typename Tp1> void BinarySearchTree<Tp0, Tp1>::preOrder(Node<Tp0>* node,
																			   void(*foo)(Node<Tp0>*)){
	if(node != nullptr){
		if(!node->overwrite) foo(node);
		preOrder(node->left, foo);
		preOrder(node->right, foo);
	}
}

template<typename Tp0, typename Tp1> void BinarySearchTree<Tp0, Tp1>::postOrder(Node<Tp0>* node,
																				void(*foo)(Node<Tp0>*)){
	if(node != nullptr){
		postOrder(node->left, foo);
		postOrder(node->right, foo);
		if(!node->overwrite) foo(node);
	}
}

/* Remove */
template<typename Tp0, typename Tp1> const Node<Tp0>* BinarySearchTree<Tp0, Tp1>::remove(Tp1 in){
	Node<Tp0>* ret;
	ret = find(m_node, in);
	if(ret != nullptr) ret->overwrite = true;
	return ret;
}


/* To Array */
template<typename Tp0, typename Tp1> const std::vector<Tp0>& BinarySearchTree<Tp0, Tp1>::getDataArray(){
	return m_data_array;
}


template<typename Tp0, typename Tp1> void BinarySearchTree<Tp0, Tp1>::setDataArray(Node<Tp0>* node){
	if(node != nullptr){
		setDataArray(node->left);
		if(!node->overwrite) m_data_array.push_back(node->data);
		setDataArray(node->right);
	}
}


template<typename Tp0, typename Tp1> void BinarySearchTree<Tp0, Tp1>::setDataArray(){
	setDataArray(m_node);
}


/*
// Test
int main(){

	BinarySearchTree<DataX, int> bst;
	
	int to_insert[] = {27, 35, 14, 10, 19, 31, 42, 23,107};
	for(int i : to_insert){
		DataX d;
		d.key = i;
		bst.insert(d);
	}
	
	bst.remove(31);
	bst.remove(27);
	
	const Node<DataX>* ptr = bst.find(19);
	if(ptr == nullptr) std::cout << "Not Found!\n";
	else std::cout << "Found(" << ptr->data.key << ")\n";
	
	bst.traverse([](Node<DataX>* node){ std::cout << node->data.key << std::endl; });
	
	bst.setDataArray();
	for(auto v : bst.getDataArray()){ std::cout << v.key << ", "; }
	std::cout << std::endl;
	
	return 0;
}*/

