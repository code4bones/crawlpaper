/*
	TStack.h
	Stack using templates.
	Bruce Eckel's Thinking in C++, 2nd Ed.

	Modificato a partire dall'originale.
	Luca Piergentili, 13/01/01
	lpiergentili@yahoo.com
*/
#ifndef TSTACK_H
#define TSTACK_H 1

#define MAX_STACK_SIZE 256

template<class T> class StacktIter;

template<class T,int ssize = MAX_STACK_SIZE> class TStack
{
public:
	TStack()
	{
		top = -1;
	}
	
	int Push(const T& i)
	{
		int index = -1;
		
		if(top < ssize)
		{
			if(top < 0)
				top = 0;
			index = top;
			stack[top++] = i;
		}
		
		return(index);
	}
	
	T Pop(int& index)
	{
		if(--top >= 0)
		{
			index = top;
			return(stack[top]);
		}
		else
		{
			index = top = -1;
			return(NULL);
		}
	}

	int Deep(void) {return(top < 0 ? 0 : top);}

	friend class StacktIter<T>;

private:
	T stack[ssize];
	int top;
};

template<class T> class StacktIter
{
public:
	StacktIter(TStack<T>& is)
	{
		s = is;
		index = 0;
	}
	
	T& operator++()
	{
		if(index < s.top - 1)
			index++;
		return(s.stack[index]);
	}
	
	T& operator++(int)
	{
		int returnIndex = index;
		if(index < s.top - 1)
			index++;
		return(s.stack[returnIndex]);
	}

private:
	TStack<T>& s;
	int index;
};

/*
// Declaration required:
template<class T> class TStackIterator;

template<class T> class TStack {
  struct Link {
    T* data;
    Link* next;
    Link(T* dat, Link* nxt) {
	 data = dat;
	 next = nxt;
    }
  }* head;
  int _owns;
public:
  TStack(int own = 1) : head(0), _owns(own) {}
  ~TStack();
  void push(T* dat) {
    head = new Link(dat,head);
  }
  T* peek() const { return head->data; }
  T* pop();
  int owns() const { return _owns; }
  void owns(int newownership) {
    _owns = newownership;
  }
  friend class TStackIterator<T>;
};

template<class T> T* TStack<T>::pop() {
  if(head == 0) return 0;
  T* result = head->data;
  Link* oldHead = head;
  head = head->next;
  delete oldHead;
  return result;
}

template<class T> TStack<T>::~TStack() {
  Link* cursor = head;
  while(head) {
    cursor = cursor->next;
    // Conditional cleanup of data:
    if(_owns) delete head->data;
    delete head;
    head = cursor;
  }
}

template<class T> class TStackIterator {
  TStack<T>::Link* p;
public:
  TStackIterator(const TStack<T>& tl)
    : p(tl.head) {}
  TStackIterator(const TStackIterator& tl)
    : p(tl.p) {}
  // operator++ returns boolean indicating end:
  int operator++() {
    if(p->next)
	 p = p->next;
    else p = 0; // Indicates end of list
    return int(p);
  }
  int operator++(int) { return operator++(); }
  // Smart pointer:
  T* operator->() const {
    if(!p) return 0;
    return p->data;
  }
  T* current() const {
    if(!p) return 0;
    return p->data;
  }
  // int conversion for conditional test:
  operator int() const { return p ? 1 : 0; }
};
*/

#endif // TSTACK_H
