/*-----------------------------------------------------------------------------

	tree.h

	STL-like tree

-----------------------------------------------------------------------------*/


#ifndef _TREE_H
#define _TREE_H

#include <memory>
#include <iterator>
#include <cassert>

//-----------------------------------------------------------------------------

namespace gems
{

//-----------------------------------------------------------------------------

template <class T, class Alloc = std::allocator<T> >
class tree
{
public:
	typedef tree<T, Alloc> TreeT;

	typedef TreeT value_type;
	typedef TreeT* pointer;
	typedef const TreeT* const_pointer;
	typedef TreeT& reference;
	typedef const TreeT& const_reference;
	typedef size_t size_type;
	typedef ptrdiff_t difference_type;

// provide special handling for non-standard VC6 allocators
#if _MSC_VER < 1300
	typedef Alloc allocator_type;
#else
	typedef typename Alloc::template rebind<TreeT>::other allocator_type;
#endif

	class iterator;
	class const_iterator;
	class child_iterator;
	class const_child_iterator;

	tree() { init(); }

	tree(const T& t) : value(t) { init(); }

	tree(const TreeT& copy) : value(copy.value)
	{
		init();
		copy_children(copy);
	}

	~tree() { clear(); }

	const tree& operator=(const TreeT& rhs)
	{
		TreeT temp(rhs);
		swap(temp);
		return *this;
	}

	// iterators

	class iterator : public std::iterator<std::bidirectional_iterator_tag, value_type, void>
	{
	public:
		iterator() : _ptr(0), _root(0) {}

		TreeT& operator*() const {return *_ptr;}
		TreeT* operator->() const {return _ptr;}

		bool operator==(const iterator& rhs) const {return _ptr == rhs._ptr;}
		bool operator!=(const iterator& rhs) const {return _ptr != rhs._ptr;}

		iterator& operator++() {_ptr = _ptr->_next; return *this;}
		iterator& operator--() {_ptr = (_ptr ? _ptr->prev() : _root->_last_descendant); return *this;}
		iterator operator++(int i) { iterator temp = *this; ++*this; return temp; }
		iterator operator--(int i) { iterator temp = *this; --*this; return temp; }

		// advance past current subtree
		friend void skip(iterator& it) { it._ptr = it._ptr->next_sibling(); }

	protected:
		TreeT* _ptr;
		TreeT* _root;

		iterator(TreeT* ptr, TreeT* root) : _ptr(ptr), _root(root) {}
		friend class tree<T, Alloc>;
		friend class const_iterator;
		friend class child_iterator;
		friend class const_child_iterator;
	};

	class const_iterator : public std::iterator<std::bidirectional_iterator_tag, value_type, void>
	{
	public:
		const_iterator() : _ptr(0), _root(0) {}
		const_iterator(const iterator& it) : _ptr(it._ptr), _root(it._root) {}

		const TreeT& operator*() const {return *_ptr;}
		const TreeT* operator->() const {return _ptr;}

		bool operator==(const const_iterator& rhs) const {return _ptr == rhs._ptr;}
		bool operator!=(const const_iterator& rhs) const {return _ptr != rhs._ptr;}

		const_iterator& operator++() {_ptr = _ptr->_next; return *this;}
		const_iterator& operator--() {_ptr = (_ptr ? _ptr->prev() : _root->_last_descendant); return *this;}
		const_iterator operator++(int i) { const_iterator temp = *this; ++*this; return temp; }
		const_iterator operator--(int i) { const_iterator temp = *this; --*this; return temp; }

	protected:
		const TreeT* _ptr;
		const TreeT* _root;

		const_iterator(const TreeT* ptr, const TreeT* root) : _ptr(ptr), _root(root) {}
		friend class tree<T, Alloc>;
		friend class const_child_iterator;
	};

	class child_iterator : public std::iterator<std::bidirectional_iterator_tag, value_type, void>
	{
	public:
		child_iterator() : _ptr(0), _root(0) {}
		child_iterator(const iterator& it) : _ptr(it._ptr), _root(it._root) {}
		operator iterator() const { return iterator(_ptr, _root); }

		TreeT& operator*() const {return *_ptr;}
		TreeT* operator->() const {return _ptr;}

		bool operator==(const child_iterator& rhs) const {return _ptr == rhs._ptr;}
		bool operator!=(const child_iterator& rhs) const {return _ptr != rhs._ptr;}

		child_iterator& operator++() {_ptr = _ptr->next_sibling(); return *this;}
		child_iterator& operator--() {_ptr = (_ptr ? _ptr->prev_sibling() : _root->last_child()); return *this;}
		child_iterator operator++(int i) { child_iterator temp = *this; ++*this; return temp; }
		child_iterator operator--(int i) { child_iterator temp = *this; --*this; return temp; }

	protected:
		TreeT* _ptr;
		TreeT* _root;

		child_iterator(TreeT* ptr, TreeT* root) : _ptr(ptr), _root(root) {}
		friend class tree<T, Alloc>;
		friend class const_child_iterator;
	};

	class const_child_iterator : public std::iterator<std::bidirectional_iterator_tag, value_type, void>
	{
	public:
		const_child_iterator() : _ptr(0), _root(0) {}
		const_child_iterator(const child_iterator& it) : _ptr(it._ptr), _root(it._root) {}
		const_child_iterator(const iterator& it) : _ptr(it._ptr), _root(it._root) {}
		const_child_iterator(const const_iterator& it) : _ptr(it._ptr), _root(it._root) {}
		operator const_iterator() const { return iterator(_ptr, _root); }

		const TreeT& operator*() const {return *_ptr;}
		const TreeT* operator->() const {return _ptr;}

		bool operator==(const const_child_iterator& rhs) const {return _ptr == rhs._ptr;}
		bool operator!=(const const_child_iterator& rhs) const {return _ptr != rhs._ptr;}

		const_child_iterator& operator++() {_ptr = _ptr->next_sibling(); return *this;}
		const_child_iterator& operator--() {_ptr = (_ptr ? _ptr->prev_sibling() : _root->last_child()); return *this;}
		const_child_iterator operator++(int i) { const_child_iterator temp = *this; ++*this; return temp; }
		const_child_iterator operator--(int i) { const_child_iterator temp = *this; --*this; return temp; }

	protected:
		const TreeT* _ptr;
		const TreeT* _root;

		const_child_iterator(const TreeT* ptr, const TreeT* root) : _ptr(ptr), _root(root) {}
		friend class tree<T, Alloc>;
	};

	typedef std::reverse_bidirectional_iterator<iterator,
		value_type, reference, pointer, difference_type>
			reverse_iterator;
	typedef std::reverse_bidirectional_iterator<const_iterator,
		value_type, const_reference, const_pointer, difference_type>
			const_reverse_iterator;

	typedef std::reverse_bidirectional_iterator<child_iterator,
		value_type, reference, pointer, difference_type>
			reverse_child_iterator;
	typedef std::reverse_bidirectional_iterator<const_child_iterator,
		value_type, const_reference, const_pointer, difference_type>
			const_reverse_child_iterator;


	const_iterator begin() const {return const_iterator(this, this);}
	const_iterator end() const {return const_iterator(0, this);}

	iterator begin() {return iterator(this, this);}
	iterator end() {return iterator(0, this);}

	const_reverse_iterator rbegin() const {return const_reverse_iterator(end());}
	const_reverse_iterator rend() const {return const_reverse_iterator(begin());}

	reverse_iterator rbegin() {return reverse_iterator(end());}
	reverse_iterator rend() {return reverse_iterator(begin());}

	const_child_iterator begin_child() const {return const_child_iterator(first_child(), this);}
	const_child_iterator end_child() const {return const_child_iterator(0, this);}

	child_iterator begin_child() {return child_iterator(first_child(), this);}
	child_iterator end_child() {return child_iterator(0, this);}

	const_reverse_child_iterator rbegin_child() const {return const_reverse_child_iterator(end_child());}
	const_reverse_child_iterator rend_child() const {return const_reverse_child_iterator(begin_child());}

	reverse_child_iterator rbegin_child() {return reverse_child_iterator(end_child());}
	reverse_child_iterator rend_child() {return reverse_child_iterator(begin_child());}


	void push_back(const TreeT& subtree)
	{
		TreeT* child = create_subtree(subtree);
		insert_subtree(*child, 0);
	}

	void push_front(const TreeT& subtree)
	{
		TreeT* child = create_subtree(subtree);
		TreeT* next = first_child();
		insert_subtree(*child, next);
	}

	void pop_back()
	{
		TreeT* last = last_child();
		assert(last);
		erase(iterator(last, this));
	}

	void pop_front()
	{
		TreeT* first = first_child();
		assert(first);
		erase(iterator(first, this));
	}

	void clear() { destroy_descendants(); }

	TreeT& front() {return *first_child();}
	const TreeT& front() const {return *first_child();}
	TreeT& back() {return *last_child();}
	const TreeT& back() const {return *last_child();}

	iterator insert(const TreeT& subtree, iterator it)
	{
		TreeT* child = create_subtree(subtree);
		insert_subtree(*child, &*it);	// if end(), append to this
		return iterator(child, this);
	}

	iterator erase(iterator it)
	{
		TreeT* child = &*it;
		assert(child);
		TreeT* parent = child->parent();
		assert(parent);
		TreeT* next = child->next_sibling();
		parent->remove_subtree(*child);
		child->destroy();
		return iterator(next, this);
	}

	void splice(iterator it, child_iterator first, child_iterator last)
	{
		while (first != last)
		{
			TreeT* parent = first->parent();
			child_iterator next = first;
			++next;
			if (parent)
			{
				parent->remove_subtree(*first);
				insert_subtree(*first, &*it);	// if it == end(), append to this; that's why this fn can't be static
			}
			first = next;
		}
	}

	friend bool operator==(const TreeT& lhs, const TreeT& rhs)
	{
		const TreeT* lChild = lhs.first_child();
		const TreeT* rChild = rhs.first_child();
		while (lChild && rChild && (*lChild == *rChild))
		{
			lChild = lChild->next_sibling();
			rChild = rChild->next_sibling();
		}

		return (!lChild && !rChild && (lhs.value == rhs.value));
	}

	friend bool operator!=(const TreeT& lhs, const TreeT& rhs) { return !(lhs == rhs); }

	bool is_root() const { return _parent == 0; }

	bool is_leaf() const { return _last_descendant == this; }

	bool is_descendant_of(const TreeT& ancestor)
	{
		for (TreeT* t = this; t; t = t->_parent)
			if (t == &ancestor)
				break;
		return (t != 0);
	}

	size_type size() const		// number of descendants
	{
		size_type n = 1;
		for (const TreeT* t = this; t != _last_descendant; t = t->_next)
			n++;
		return n;
	}

	size_type degree() const	// number of children
	{
		size_type n = 0;
		for (const TreeT* t = first_child(); t; t = t->next_sibling())
			n++;
		return n;
	}

	size_type level() const	// depth in tree
	{
		size_type l = 0;
		for (const TreeT* t = _parent; t; t = t->_parent)
			l++;
		return l;
	}

	TreeT* parent() const { return _parent; }

	TreeT* first_child() const
	{
		TreeT* child = 0;
		if (_next && (_next->_parent == this))
			child = _next;
		return child;
	}

	TreeT* last_child() const
	{
		TreeT* child = first_child();
		if (child)
			child = child->_prev_sibling;
		return child;
	}

	TreeT* next_sibling() const
	{
		TreeT* sib = _last_descendant->_next;
		if (sib && (sib->_parent != _parent))
			sib = 0;
		return sib;
	}

	TreeT* prev_sibling() const
	{
		TreeT* sib = 0;
		if (_parent && (_parent->_next != this))
			sib = _prev_sibling;
		return sib;
	}

	TreeT* last_descendant() const { return _last_descendant; }

	void swap(TreeT& other)
	{
		child_iterator it = other.begin_child();
		other.splice(it, begin_child(), end_child());
		splice(begin_child(), it, other.end_child());
		std::swap(value, other.value);
	}

	Alloc get_allocator() const { return _alloc; }

	size_type max_size() const { return (_alloc.max_size()); }

public:
	T value;
private:
	TreeT* _parent;
	TreeT* _prev_sibling;
	TreeT* _next;
	TreeT* _last_descendant;

	static allocator_type _alloc;

	friend iterator;
	friend const_iterator;
	friend child_iterator;
	friend const_child_iterator;


	void init()
	{
		_parent = _next = 0;
		_prev_sibling = _last_descendant = this;
	}

	static TreeT* create_subtree(const TreeT& copy)
	{
#if _MSC_VER < 1300
		auto_ptr<TreeT> a((TreeT*)_alloc._Charalloc(sizeof(TreeT)));
		TreeT* t = a.get();
		_alloc.construct(&t->value, copy.value);
		t->init();
		t->copy_children(copy);
#else
		auto_ptr<TreeT> a((TreeT*)_alloc.allocate(1));
		TreeT* t = a.get();
		_alloc.construct(t, copy);
#endif
		return a.release();
	}

	void copy_children(const TreeT& other)
	{
		for (TreeT* child = other.first_child(); child; child = child->next_sibling())
			insert_subtree(*create_subtree(*child), 0);
	}

	void destroy()
	{
#if _MSC_VER < 1300
		destroy_descendants();
		_alloc.destroy(&value);
		_alloc.deallocate((T*)this, 1);
#else
		_alloc.destroy(this);
		_alloc.deallocate(this, 1);
#endif
	}

	void destroy_descendants()
	{
		TreeT* descendant = first_child();
		TreeT* end = last_descendant()->next();
		while (descendant)
		{
			TreeT* next = descendant->next_sibling();
			descendant->destroy();
			descendant = next;
		}

		_next = end;
		_last_descendant = this;
	}

	void insert_subtree(TreeT& child, TreeT* next)
	{
		if (next == 0)
		{
			// append as last child
			child._parent = this;
			child._last_descendant->_next = _last_descendant->_next;
			_last_descendant->_next = &child;

			child._prev_sibling = last_child();
			if (is_leaf())
				_next = &child;
			first_child()->_prev_sibling = &child;

			change_last_descendant(child._last_descendant);
		}
		else
		{
			TreeT* parent = next->_parent;
			child._parent = parent;
			child._prev_sibling = next->_prev_sibling;

			child._last_descendant->_next = next;
			if (parent->_next == next)	// inserting before first child?
				parent->_next = &child;
			else
				next->_prev_sibling->_last_descendant->_next = &child;

			next->_prev_sibling = &child;
		}
	}

	void remove_subtree(TreeT& child)
	{
		TreeT* sib = child.next_sibling();
		if (sib)
			sib->_prev_sibling = child._prev_sibling;
		else
			first_child()->_prev_sibling = child._prev_sibling;

		if (_last_descendant == child._last_descendant)
			change_last_descendant(child.prev());

		if (_next == &child)	// deleting first child?
			_next = child._last_descendant->_next;
		else
			child._prev_sibling->_last_descendant->_next = child._last_descendant->_next;
	}

	void change_last_descendant(TreeT* newLast)
	{
		TreeT* oldLast = _last_descendant;
		TreeT* ancestor = this;
		do
		{
			ancestor->_last_descendant = newLast;
			ancestor = ancestor->_parent;
		}
		while (ancestor && (ancestor->_last_descendant == oldLast));
	}

	TreeT* next() const { return _next; }

	TreeT* prev() const
	{
		TreeT* prev = 0;
		if (_parent)
		{
			if (_parent->_next == this)
				prev = _parent;
			else
				prev = _prev_sibling->_last_descendant;
		}
		return prev;
	}

};	// class tree<T, Alloc>

template <class T, class Alloc>
typename tree<T, Alloc>::allocator_type tree<T, Alloc>::_alloc;

//-----------------------------------------------------------------------------

} // namespace gems

#endif	// _TREE_H

