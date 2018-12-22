/*********************************************************************
 * mylist.h
 * Authored by Kris Hauser 2002-2003
 *
 * A template list class.
 * Copyright (c) 2002 SquireSoft, used with permission.
 *
 *********************************************************************/

/**************************************************************************
 * Copyright (c) 2003, University of California at Berkeley
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 * 
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 * 
 *     * Neither the name of the University of California nor the names
 *       of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written
 *       permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *************************************************************************/



#ifndef MY_LIST_H
#define MY_LIST_H

#include <stdlib.h>
#include <assert.h>

/* mylist -- doubly linked list template class
class mylist
{
public:
	mylist();
	mylist(int);
	mylist(int, const type&);
	mylist(const mylist<type>&);
	~mylist();

	const mylist<type>& operator = (const mylist&);
	void append(mylist&);						//note: empties the second list
	bool empty() const;
	void clear();

	int size() const;
	void resize(int);
	void resize(int, const type&);

	void push_back();
	void push_back(const type&);
	void push_front();
	void push_front(const type&);

	void pop_front();
	void pop_front(type&);
	void pop_back();
	void pop_back(type&);

	type* erase(type*);							//note: returns the temp->prev (for easy list iteration)
	void insert(type*, const type&);			//note: inserts a new node after the specified node

	type* back();
	const type* back() const;
	type* front();
	const type* front() const;
	type* next(const type*) const;
	type* prev(const type*) const;

	type* get_nth(int n);

	type* find_node(const type& t);				//returns NULL if can't find it
	int find(const type& t);					//returns -1 if can't find it
	bool contains(const type& t);
*/

template <class type>
class mylist
{
public:
	mylist()
	:_front(NULL), _back(NULL), num_nodes(0) {
	}
	mylist(int n)
	:_front(NULL), _back(NULL), num_nodes(0)  {
		resize(n);
	}
	mylist(int n, const type& t)
	:_front(NULL), _back(NULL), num_nodes(0)  {
		resize(n,t);
	}
	mylist(const mylist<type>& l)
	:_front(NULL), _back(NULL), num_nodes(0)  {
		*this = l;
	}

	virtual ~mylist() {
		clear();
	}

	bool empty() const {
		return (num_nodes == 0);
	}
	void clear() {
		type_node* temp=_front, *next;
		while(temp)
		{
			next = temp->next;
			delete temp;
			temp = next;
		}
		_front = NULL;
		_back = NULL;
		num_nodes = 0;
	}


	int size() const { 
		return num_nodes;
	}
	void resize(int n) {
		int dest=n;
		n = n-num_nodes;
		if(n<0) {		//resize smaller
			while(n++)
				pop_back();
		}
		else {		//resize bigger
			while(n--)
				push_back();
		}
		assert(num_nodes == dest);
	}
	void resize(int n, const type& t) {
		int dest=n;
		n = n-num_nodes;
		if(n<0) {		//resize smaller
			while(n++)
				pop_back();
		}
		else {		//resize bigger
			while(n--)
				push_back(t);
		}
		assert(num_nodes == dest);
	}

	void append(mylist<type>& l) {
		_back->next = l._front;
		_back->next->prev = _back;
		_back = l._back;
		num_nodes = num_nodes + l.num_nodes;
		l._back = NULL;
		l._front = NULL;
		l.clear();
	}

	void push_back() {
		type_node* temp=new type_node;
		temp->prev = _back;
		temp->next = NULL;
		if(!_back)
			_front = temp;
		else
			_back->next = temp;
		_back = temp;
		num_nodes++;
	}
	void push_back(const type& t) {
		type_node* temp=new type_node;
		temp->t = t;
		temp->prev = _back;
		temp->next = NULL;
		if(!_back)
			_front = temp;
		else
			_back->next = temp;
		_back = temp;
		num_nodes++;
	}

	void push_front() {
		type_node* temp=new type_node;
		temp->next = _front;
		temp->prev = NULL;
		if(!_front)
			_back = temp;
		else
			_front->prev = temp;
		_front = temp;
		num_nodes++;
	}
	void push_front(const type& t) {
		type_node* temp=new type_node;
		temp->t = t;
		temp->next = _front;
		temp->prev = NULL;
		if(!_front)
			_back = temp;
		else
			_front->prev = temp;
		_front = temp;
		num_nodes++;
	}


	void pop_front() {
		assert(_front != NULL);
		num_nodes--;
		type_node* temp = _front;
		_front = temp->next;
		if(_front)
			_front->prev = NULL;
		else
			_back = NULL;
		delete temp;
	}
	void pop_front(type& t) {
		assert(_front != NULL);
		num_nodes--;
		type_node* temp = _front;
		_front = temp->next;
		if(_front)
			_front->prev = NULL;
		else
			_back = NULL;
		t = temp->t;
		delete temp;
	}
	void pop_back() {
		assert(_back != NULL);
		num_nodes--;
		type_node* temp = _back;
		_back = temp->prev;
		if(_back)
			_back->next = NULL;
		else
			_front = NULL;
		delete temp;
	}
	void pop_back(type& t) {
		assert(_back != NULL);
		num_nodes--;
		type_node* temp = _back;
		_back = temp->prev;
		if(_back)
			_back->next = NULL;
		else
			_front = NULL;
		t = temp->t;
		delete temp;
	}

	type* erase(type* t){
		if(num_nodes == 0 || !t)
			abort();

		type_node* temp = (type_node*)t;
		if(temp == _front)
		{
			_front = temp->next;
		}
		if(temp == _back)
		{
			_back = temp->prev;
		}

		if(temp->prev)
			temp->prev->next = temp->next;
		if(temp->next)
			temp->next->prev = temp->prev;

		type_node* prev = temp->prev;

		num_nodes--;
		delete temp;
		return &prev->t;
	}

	void insert(type* nt, const type& t) {
		type_node* node = (type_node*)nt, *temp = new type_node;

		temp->t = t;
		temp->prev = node;

		if(node == _back)
			_back = temp;

		if(node)
		{
			temp->next = node->next;
			node->next = temp;
		}
		else 					//call to null lets you insert before the front
		{
			temp->next = _front;
			_front = temp;
		}

		if(temp->next)
			temp->next->prev = temp;

		num_nodes++;
	}

	type* back(){
		if(!_back)
			return NULL;
		return &_back->t;
	}
	const type* back() const {
		if(!_back)
			return NULL;
		return &_back->t;
	}
	type* front() {
		if(!_front)
			return NULL;
		return &_front->t;
	}
	const type* front() const {
		if(!_front)
			return NULL;
		return &_front->t;
	}
	type* next(const type* t) const {
		if(!t)
			return (_front ? &_front->t : NULL);
		type_node* node = (type_node*)t;
		if(node->next)
			return &node->next->t;
		else
			return NULL;
	}
	type* prev(const type* t) const {
		if(!t)
			return (_back ? &_back->t : NULL);
		type_node* node = (type_node*)t;
		if(node->prev)
			return &node->prev->t;
		else
			return NULL;
	}

	type* get_nth(int n) {
	  int i=0;
	  type_node* temp = _front;
	  while(temp) {
	    if(i == n)
	      return &temp->t;
	    temp = temp->next;
	    i++;
	  }
	  return NULL;
	}

	type* find_node(const type& t) {
		type_node* temp = _front;
		while(temp)
		{
			if(temp->t == t)
				return &temp->t;
			temp = temp->next;
		}
		return NULL;
	}
	int find(const type& t) {
		int i=0;
		type_node* temp = _front;
		while(temp)
		{
			if(temp->t == t)
				return i;
			i++;
			temp = temp->next;
		}
		return -1;
	}
	bool contains(const type& t) {
		return (find_node(t) != NULL);
	}

	const mylist<type>& operator = (const mylist&l) {
	resize(l.num_nodes);
	type_node* temp = _front;
	type_node* temp2 = l._front;
	while(temp)
	{
		temp->t = temp2->t;
		temp = temp->next;
		temp2 = temp2->next;
	}
	return *this;
}


protected:
	struct type_node
	{
		type t;
		type_node *next, *prev;
	};
	type_node *_front, *_back;
	int num_nodes;
};






/* myindexedlist -- doubly linked list template class with indexing and caching
NOTE: is occasionally incompatible with normal list operations.  Call reset() between an operation and [] access
class myindexedlist : public mylist
{
public:
	void reset();
	type& operator [] (int);
}
*/

template <class type>
class myindexedlist : public mylist<type>
{
public:
	myindexedlist()
	:mylist<type>(), _last(NULL), last_index(-1) {
	}
	myindexedlist(int n)
	:mylist<type>(n), _last(NULL), last_index(-1) {
	}
	myindexedlist(int n, const type& t)
	:mylist<type>(n, t), _last(NULL), last_index(-1) {
	}
	myindexedlist(const mylist<type>& l)
	:mylist<type>(l), _last(NULL), last_index(-1) {
	}

	void reset() {
		_last = NULL;
		last_index = -1;
	}

	type& operator [] (int i) {
		if(i<0 || i>=num_nodes)
			abort();
		
		int front_dist=i, back_dist = num_nodes-1-i;
		int last_dist=(_last ? i-last_index : num_nodes);

		type_node* t;
		if(front_dist <= back_dist)
		{
			if(abs(last_dist) < front_dist)
				t = seek(_last, last_dist);
			else
				t = seek(_front, front_dist);
		}
		else
		{
			if(abs(last_dist) < back_dist)
				t = seek(_last, last_dist);
			else
				t = seek(_back, -back_dist);
		}
		last_index = i;
		_last = t;

		return t->t;
	}

private:
	type_node *_last;
	int last_index;

	type_node* seek_backward(type_node* t, int n) {
		while(n)
		{
			t = t->prev;
			n--;
		}
		return t;
	}
	type_node* seek_forward(type_node* t, int n) {
		while(n)
		{
			t = t->next;
			n--;
		}
		return t;
	}
	type_node* seek(type_node* t, int n) {
		if(n>0)
			return seek_forward(t,n);
		else if(n<0)
			return seek_backward(t,-n);
		return t;
	}
};










#ifndef MY_LIST_H
#define MY_LIST_H

#include <stdlib.h>


template <class type>
class mylist
{
public:
	mylist();
	mylist(int);
	mylist(int, const type&);
	mylist(const mylist<type>&);
	~mylist();

	const mylist<type>& operator = (const mylist&);
	bool empty() const;
	void clear();

	type& operator [] (int);

	int size() const;
	void resize(int);
	void resize(int, const type&);

	void push_back();
	void push_back(const type&);
	void push_front();
	void push_front(const type&);

	void pop_front();
	void pop_front(type&);
	void pop_back();
	void pop_back(type&);

	type* erase(type*);
	void insert(type*, const type&);

	type* back();
	const type* back() const;
	type* front();
	const type* front() const;
	type* next(const type*) const;
	type* prev(const type*) const;

	int find(const type&);
	bool contains(const type& t) { return (find(t)!=-1);}

private:
	struct type_node
	{
		type t;
		type_node *next, *prev;
	};
	type_node *_front, *_back;
	int num_nodes;

	type_node *_last;
	int last_index;

	type_node* seek_backward(type_node* t, int n)
	{
		while(n)
		{
			t = t->prev;
			n--;
		}
		return t;
	}
	type_node* seek_forward(type_node* t, int n)
	{
		while(n)
		{
			t = t->next;
			n--;
		}
		return t;
	}
	type_node* seek(type_node* t, int n)
	{
		if(n>0)
			return seek_forward(t,n);
		else if(n<0)
			return seek_backward(t,-n);
		return t;
	}
};

template <class type>
mylist<type>::mylist()
:_front(NULL), _back(NULL), num_nodes(0), _last(NULL)
{
}

template <class type>
mylist<type>::mylist(int n)
:_front(NULL), _back(NULL), num_nodes(0), _last(NULL)
{
	resize(n);
}

template <class type>
mylist<type>::mylist(int n, const type& t)
:_front(NULL), _back(NULL), num_nodes(0), _last(NULL)
{
	resize(n,t);
}

template <class type>
mylist<type>::mylist(const mylist<type>& l)
:_front(NULL), _back(NULL), num_nodes(0), _last(NULL)
{
	*this = l;
}

template <class type>
mylist<type>::~mylist()
{
	clear();
}

template <class type>
void mylist<type>::resize(int n)
{
	int dest=n;
	n = n-num_nodes;
	if(n<0)		//resize smaller
	{
		type_node* temp = _back, *prev;
		while(n++)
		{
			prev = temp->prev;
			prev->next = NULL;
			delete temp;
			temp = prev;
			num_nodes --;
		}
		_back = temp;
		if(num_nodes == 1)
		{
			_front = temp;
		}
		else if(num_nodes == 0)
		{
			_front = _back = NULL;
		}
	}
	else		//resize bigger
	{
		while(n--)
		{
			push_back();
		}
	}
	if(num_nodes != dest)
		abort();
}

template <class type>
void mylist<type>::resize(int n, const type& t)
{
	int dest=n;
	n = n-num_nodes;
	if(n<0)		//resize smaller
	{
		type_node* temp = _back, *prev;
		while(n++)
		{
			prev = temp->next;
			delete temp;
			temp = prev;
			num_nodes --;
		}
		_back = temp;
		if(num_nodes == 1)
		{
			_front = temp;
		}
		else if(num_nodes == 0)
		{
			_front = _back = NULL;
		}
	}
	else		//resize bigger
	{
		while(n--)
		{
			push_back(t);
		}
	}
	if(num_nodes != dest)
		abort();
}

template <class type>
int mylist<type>::size() const
{
	return num_nodes;
}

template <class type>
bool mylist<type>::empty() const
{
	return (num_nodes == 0);
}

template <class type>
void mylist<type>::clear()
{
	type_node* temp=_front, *next;
	while(temp)
	{
		next = temp->next;
		delete temp;
		temp = next;
	}
	_front = NULL;
	_back = NULL;
	num_nodes = 0;
}


template <class type>
type* mylist<type>::back()
{
	if(!_back)
		return NULL;
	return &_back->t;
}


template <class type>
const type* mylist<type>::back() const
{
	if(!_back)
		return NULL;
	return &_back->t;
}

template <class type>
type* mylist<type>::front()
{
	if(!_front)
		return NULL;
	return &_front->t;
}


template <class type>
const type* mylist<type>::front() const
{
	if(!_front)
		return NULL;
	return &_front->t;
}

template <class type>
void mylist<type>::push_back()
{
	type_node* temp=new type_node;
	temp->prev = _back;
	temp->next = NULL;
	if(!_back)
	{
		_front = temp;
	}
	else
	{
		_back->next = temp;
	}
	_back = temp;
	num_nodes++;
}


template <class type>
void mylist<type>::push_back(const type& t)
{
	type_node* temp=new type_node;
	temp->t = t;
	temp->prev = _back;
	temp->next = NULL;
	if(!_back)
	{
		_front = temp;
	}
	else
	{
		_back->next = temp;
	}
	_back = temp;
	num_nodes++;
}

template <class type>
void mylist<type>::push_front()
{
	type_node* temp=new type_node;
	temp->next = _front;
	temp->prev = NULL;
	if(!_front)
	{
		_back = temp;
	}
	else
	{
		front->prev = temp;
	}
	_front = temp;
	num_nodes++;
}

template <class type>
void mylist<type>::push_front(const type& t)
{
	type_node* temp=new type_node;
	temp->t = t;
	temp->next = _front;
	temp->prev = NULL;
	if(!_front)
	{
		_back = temp;
	}
	else
	{
		_front->prev = temp;
	}
	_front = temp;
	num_nodes++;
}

template <class type>
void mylist<type>::pop_front()
{
	if(!_front)
		abort();
	type_node* temp = _front;
	_front = temp->next;
	_front->prev = NULL;
	if(!_front)
		_back = NULL;
	delete temp;
}

template <class type>
void mylist<type>::pop_front(type& t)
{
	if(!_front)
		abort();
	type_node* temp = _front;
	_front = temp->next;
	_front->prev = NULL;
	if(!_front)
		_back = NULL;
	t = temp->t;
	delete temp;
}

template <class type>
void mylist<type>::pop_back()
{
	if(!_back)
		abort();
	type_node* temp = _back;
	_back = temp->prev;
	_back->next = NULL;
	if(!_back)
		_front = NULL;
	delete temp;
}

template <class type>
void mylist<type>::pop_back(type& t)
{
	if(!_back)
		abort();
	type_node* temp = _back;
	_back = temp->prev;
	_back->next = NULL;
	if(!_back)
		_front = NULL;
	t = temp->t;
	delete temp;
}

template <class type>
type* mylist<type>::erase(type* t)
{
	if(num_nodes == 0 || !t)
		abort();
	type_node* temp = (type_node*)t;
	if(temp == _front)
	{
		_front = temp->next;
	}
	if(temp == _back)
	{
		_back = temp->prev;
	}

	if(temp->prev)
		temp->prev->next = temp->next;
	if(temp->next)
		temp->next->prev = temp->prev;

	type_node* prev = temp->prev;

	num_nodes--;
	delete temp;
	return &prev->t;
}

template <class type>
void mylist<type>::insert(type* nt, const type& t)
{
	type_node* node = (type_node*)nt, *temp = new type_node;

	temp->t = t;
	temp->prev = node;

	if(node == _back)
	{
		_back=temp;
	}

	if(node)
	{
		temp->next = node->next;
		node->next = temp;
	}
	else 					//call to null lets you insert before the front
	{
		temp->next = _front;
		_front = temp;
	}

	if(temp->next)
		temp->next->prev = temp;

	num_nodes++;
}

template <class type>
type* mylist<type>::next(const type* t) const
{
	if(!t)
		return &_front->t;
	type_node* node = (type_node*)t;
	if(node->next)
		return &node->next->t;
	else
		return NULL;
}

template <class type>
type* mylist<type>::prev(const type* t) const
{
	if(!t)
		return &_back->t;
	type_node* node = (type_node*)t;
	if(node->prev)
		return &node->prev->t;
	else
		return NULL;
}


template <class type>
type& mylist<type>::operator [] (int i)
{
	if(i<0 || i>=num_nodes)
		abort();
	
	int front_dist=i, back_dist = num_nodes-1-i;
	int last_dist=(_last ? i-last_index : num_nodes);

	type_node* t;
	if(front_dist <= back_dist)
	{
		if(abs(last_dist) < front_dist)
			t = seek(_last, last_dist);
		else
			t = seek(_front, front_dist);
	}
	else
	{
		if(abs(last_dist) < back_dist)
			t = seek(_last, last_dist);
		else
			t = seek(_back, -back_dist);
	}
	last_index = i;
	_last = t;

	return t->t;
}

template <class type>
const mylist<type>& mylist<type>::operator = (const mylist<type>& l)
{
	resize(l.num_nodes);
	type_node* temp = _front;
	type_node* temp2 = l._front;
	while(temp)
	{
		temp->t = temp2->t;
		temp = temp->next;
		temp2 = temp2->next;
	}
	return *this;
}

template <class type>
int mylist<type>::find(const type& t)
{
	int i=0;
	type_node* temp = _front;
	while(temp)
	{
		if(temp->t == t)
			return i;
		i++;
		temp = temp->next;
	}
	return -1;
}


#endif

#endif
