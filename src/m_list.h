/**
* @file m_list.h
* @brief ���� �������� ������� �������
*/

#ifndef M_LISTH
#define M_LISTH

#include <stdlib.h>
#include <string.h>

//- List -----------------------------------------------------------------------

template <class Type> class TList;

/**
* ����� ��������� ������� �������� ��������� � ����������� �������
*/
template <class Type>
class TListItem {
	friend class TList<Type>;
private:
	/**
	* ��������� �� ��������� ������ � ������
	*/
	TListItem<Type>* Next;
	
	/**
	* ������� ������
	*/
	Type*            Item;

protected:
	/**
	* �����������
	* @param p ��������� �� ����������� �������
	*/
	TListItem( Type *p ): Item(p)   { Next = NULL; }
	
	/**
	* �����������
	* @param p ��������� �� ����������� �������
	* @param p_next ��������� �� ��������� �������
	*/
	TListItem( Type *p, TListItem<Type>* p_next ): Item(p),Next(p_next)   {}
	
	/**
	* ���������� �� ��������� ������� ��������
	*/
	virtual ~TListItem( void )   {}
	
public:
	/**
	* �������� �������� �������
	* @return ��������� �� �������
	*/
	Type* GetItem(void)   { return Item; }
};

//------------------------------------------------------------------

/**
* ������� ����� ��� ������ � ������� � ��������
*/
template <class Type>
class TFind {
public:
	/**
	* ����� ���������
	* @param elem ������� ��� ���������
	* @return true ���� ������� ������ - ����� false
	*/
	virtual bool Is(Type*elem) = NULL;
};

/**
* ������� ����� ��� ������� ������� � ��������
*/
template <class Type>
class TIterator {
public:
	/**
	* ����� ������������ ������ ���������
	* @param elem ������� ��� ���������� ��������
	*/
	virtual void Do(Type*elem) = NULL;
};


/**
* ����� ������������ ������
* ����� �������� �������� ������� ������ ����������� ������ ��������� ��������� ����.
* � ������ �������� �� ������� ���������� ����, � ��������� �� ����� TListItem �������
* �������� ��������� �� ������� ���������� ����.
* @see TListItem
* @see TIterator
* @see TFind
*/
template <class Type>
class TList {
private:
	/**
	* ������� �������� ��������� � �����������
	*/
	const bool      fItemDel;

	/**
	* ��������� �� ��������� ������� ������
	*/
	TListItem<Type> *Root;
	
	/**
	* ��������� �� ��������� ������� ������
	*/	
	TListItem<Type> *Last;

	/**
	* ����� ���������� ��� ������� � ��������� ������ ��� � �������
	*/
	TListItem<Type> **p_List;

	/**
	* ���������� ��������� � ������
	*/
	int             p_ListCount;
protected:
	/**
	* ����������� � �������� ���������� ��������� � ������
	* @return ���������� ��������� � ������
	*/
	int ListCount(void) {
		int c = 0;
		TListItem<Type> *p = Root;
		while( p != NULL )
		{
			c++;
			p = p->Next;
		}
		return c;
	}
	
	/**
	* ������������� ������
	*/
	void Init(void) {
		Reset();
		int lcnt = ListCount();
		if ( lcnt ) {
			p_List = new TListItem<Type>*[lcnt];
		} else {
			p_List = NULL;
		}
		TListItem<Type> *p = Root;
		while ( p != NULL )
		{
			p_List[p_ListCount++] = p;
			p = p->Next;
		}
	}
	
	/**
	* ����� ��������� ������. ��������� ��������� ��������. ������������ ������� ���������.
	*/
	void Reset(void) {
		if( p_List != NULL ) {
			delete[] p_List;
			p_List = NULL;
			p_ListCount = 0;
		}
	}

	/**
	* �������� ����� ������� � ������. ����������� ������� �� ����������.
	* @param p ��������� �� ����� �������
	* @see TListItem
	*/
	inline void Add(TListItem<Type>* p) {
		if( isEmpty() )
			Root = Last = p;
		else {
			Last->Next = p;
			Last = p;
		}
	}
public:

	/**
	* ����������� ������.
	* ��� �������� ������ �������� ��������� ItemDel �������� ��������� ��
	* ������. ���� �������� �������� ��������������� �����������, �������
	* ������� � ��������� false - ����� �������� ������ �� ����� �������.
	*
	* @param ItemDel ���� ����������� �� �������� ��������� � �����������
	*/
	TList( const bool ItemDel = true ): fItemDel(ItemDel), p_List(NULL), p_ListCount(0) {
		Root = Last = NULL;
	}

	/**
	* ���������� ������� �������� �� ������ ���� ������� � ���������� ������������
	*/
	virtual ~TList( void ) {
		Reset();
		RemoveAll(fItemDel);
	}
	
	/**
	* ����� ������� � ������. ���� ��� ��������� � ������ ������ �������� "=="
	* ����� ����������� ����� �������� ��������� ���� �������.
	* @param item ������ �������
	* @return ��������� �� ������� ������
	* @see TListItem
	*/
	TListItem<Type>*  hasObject(const Type& item);

	/**
	* ��������� �������� �� ������ ��������
	* @return true ���� �������� ������ ���� ������� - ����� false
	*/
	bool              isEmpty(void)   { return (Root==NULL); }

	/**
	* �������� ������. ���� � ������ ������������ ������ �� �������������� ����������
	* ������� ������� ���������� false - ����� ������ ����� ������, �� �������� �� �������.
	* @param fItemDel ���� ����������� �� �� ��� ������ ������� ������ ���� ����� ������
	*/
	void              RemoveAll(const bool fItemDel);

	/**
	* ������� ��� �������� �� ������.
	* ������ ��������� � ��������� ��� ��� ��������. ���������� ������ RemoveAll(true).
	*/
	void              Clear() { RemoveAll(true); }

	/**
	* �������� � ������ ���������. ���� ������ �������� true �� ����� ������� ����� ���������
	* (�������� �� ������������� ������ ����� ����� ������� � ��������), ����� - ����������� 
	* ������ ��������� �� �������� ������.
	* @param List ������ �� �������� ���������� ��������
	* @param fCopy ���� �����������
	*/
	void              Add(TList<Type>* List, const bool fCopy);

	/**
	* �������� ������� � ������ ����� ���������. ����������� ����������� ��������.
	* @param p ����� �������
	* @param p_item ��������� �� ������� ����� ������� ��������� ����� �������
	* @return ��������� �� ���������� ������� ������
	* @see TListItem
	*/
	TListItem<Type>*  Insert(TListItem<Type>* p, Type* p_item);

	/**
	* �������� ������� � ����� ������. ����������� ����������� ��������.
	* @param p ��������� �� ����� �������
	* @return ��������� �� �������� ������� ������
	*/
	TListItem<Type>*  Add(Type* p);

	/**
	* ������� ������� �� �����.
	* @param p ��������� �� ������� ��� ��������
	* @return true ���� ������� ������ � ������ - ����� fals
	*/
	bool              Del(Type* p);

	/**
	* ������� ������� �� �����.
	* @param p ��������� �� ������� ��� ��������
	* @return true ���� ������� ������ � ������ - ����� fals
	* @see TListItem
	*/
	bool              Del(TListItem<Type>* p);

	/**
	* �������� ���������� ��������� � ������
	* @return ���������� ��������� � ������
	*/
	int               Count(void);

	/**
	* �������� ������� �� ��������� �������. � ������ ����� ����� ���������� ��� � �������.
	* ��� ���� ������ ��������� ��������� � �������� �� ���������� � ������.
	* @param Index ������ �������� (������� � 0)
	* @return ������� ������ ��� NULL
	*/
	Type*             Item(unsigned int Index);
	
	/**
	* ��������� ��� ����� ���������� ������ �������� ��������.
	* @param Iterator ������ ���������
	* @see TIterator
	*/
	void              For(TIterator<Type>& Iterator);

	/**
	* ����� ������� ������ ��������� ����� ������.
	* @param Find ����� ������
	* @return ������� ������ ��� NULL
	* @see TFind
	*/
	Type*             FindItem(TFind<Type>& Find);

	/**
	* ����� ������ ������ �� ���������.
	* @param elem ��������� �� �������
	* @return ������� ������ ��� NULL
	* @see TListItem
	*/
	TListItem<Type>*  Find(const Type *elem);

	/**
	* ����� ������� ������ ��������� ����� ������.
	* @param Find ����� ������
	* @return ������� ������ ��� NULL
	* @see TFind
	* @see TListItem
	*/
	TListItem<Type>*  Find(TFind<Type>& Find);

	/**
	* �������� ������ ������� ������
	* @return �������� ������� ������ ��� NULL
	*/
	TListItem<Type>*  First(void) { return Root; }

	/**
	* �������� ��������� ������� ������
	* @return ��������� ������� ������ ��� NULL
	*/
	TListItem<Type>*  FindLast(void) { return Last; }

	/**
	* �������� ��������� �� ��������� �������
	* @param p ������� �������
	* @return ��������� �� ��������� ������� ��� NULL
	* @see TListItem
	*/
	TListItem<Type>*  Next(TListItem<Type>* p);
	
	/**
	* �������� ��������������.
	* ���������� ������ ������ Item(idx).
	* @param idx ������ ��������
	* @return ������� ������ ��� NULL
	*/
	Type* operator [](const int idx) const     { return Item(idx); }

	/**
	* �������� ��������������.
	* ���������� ������ ������ Item(idx).
	* @param idx ������ ��������
	* @return ������� ������ ��� NULL
	*/
	Type* operator [](const int idx)           { return Item(idx); }
};

template<class Type>
bool TList<Type>::Del(Type* Item) {
  Reset();
  if( Last->Item == Item )
    return Del(Last);
  else {
    TListItem<Type>* p = First();
    while( p != NULL )
    {
      if( p->Item == Item )
        return Del(p);
      p = Next(p);
    }
  }
  return false;
}

template<class Type>
bool TList<Type>::Del(TListItem<Type>* p) {
  if( p == NULL )
    return false;
 
  if ( Count() == 1 ) {
  	RemoveAll(fItemDel);
	return true;
  }
  
  Reset();
  TListItem<Type>* p_next = p->Next;
  if( p_next == NULL ) {
    TListItem<Type>* p_prev = Root;
    while( p_prev->Next != p )
      if( (p_prev = p_prev->Next) == NULL )
        return false;
    Last = p_prev;
    p_prev->Next = NULL;
    if( fItemDel )
      delete p->Item;
    delete p;
  } else {
    if( fItemDel )
      delete p->Item;
    p->Item = p_next->Item;
    p->Next = p_next->Next;
    if( Last == p_next )
      Last = p;
    delete p_next;
  }
  return true;
}

template<class Type>
inline TListItem<Type>* TList<Type>::Next(TListItem<Type>* p) {
  if( p != NULL )
    p = p->Next;
  return p;
}

template<class Type>
inline TListItem<Type>* TList<Type>::Add(Type* p)
{
  Reset();
  TListItem<Type>* p_list = new TListItem<Type>(p);
  Add(p_list);
  return p_list;
}

template<class Type>
inline TListItem<Type>* TList<Type>::Insert(TListItem<Type>* p, Type* p_item)
{
  Reset();
  TListItem<Type>* p_list = new TListItem<Type>(p);
  if( p == NULL ) {
    p_list->Next = Root;
    if( Last == Root )
      Last = p_list;
    Root = p_list;
  } else {
    p_list->Next = p->Next;
    p->Next = p_list;
    if( Last == p )
      Last = p_list;
  }
  return p_list;
}

template<class Type>
void TList<Type>::RemoveAll( const bool fItemDel ) {
  Reset();
  while( Root != NULL)
  {
    TListItem<Type>* p = Root->Next;
    if( fItemDel )
      delete Root->Item;
    delete Root;
    Root = p;
  }
  Root = Last = NULL;
}

template<class Type>
void TList<Type>::Add(TList<Type>* List, const bool fCopy) {
  Reset();
  TListItem<Type>* p = List->First();
  while( p != NULL)
  {
    if( fCopy ) {
      Type* p_item = new Type(*(p->GetItem()));
      Add(p_item);
    } else
      Add(p->GetItem());
    p = List->Next(p);
  }
}

template<class Type>
int TList<Type>::Count(void) {
  if( p_List == NULL )
    Init();
  return p_ListCount;
}

template<class Type>
TListItem<Type>* TList<Type>::Find(TFind<Type>& Find) {
  TListItem<Type> *p = Root;
  while( p != NULL)
  {
    if( Find.Is(p->Item) )
      return p;
    p = p->Next;
  }
  return NULL;
}

template<class Type>
TListItem<Type>* TList<Type>::Find(const Type *p_item) {
  TListItem<Type> *p = Root;
  while( p != NULL)
  {
    if( p->Item == p_item )
      return p;
    p = p->Next;
  }
  return NULL;
}

template<class Type>
Type* TList<Type>::FindItem(TFind<Type>& Find) {
  TListItem<Type> *p = Find(Find);
  if( p != NULL )
    return p->Item;
  return NULL;
}

template<class Type>
TListItem<Type>* TList<Type>::hasObject(const Type& item) {
  if( isEmpty() )
    return NULL;
  else {
    TListItem<Type> *p = Root;
    while( p != NULL )
    {
      if( item == *(p->Item) )
        return p;
      else
        p = p->Next;
    }
    return NULL;
  }
}

template<class Type>
void TList<Type>::For(TIterator<Type>& Iterator) {
  TListItem<Type> *p = Root;
  while( p != NULL)
  {
    Iterator.Do(p->Item);
    p = p->Next;
  }
}

template<class Type>
Type* TList<Type>::Item( unsigned int Index ) {
  if( p_List == NULL )
    Init();
  return p_List[Index]->Item;
}

//- Array ----------------------------------------------------------------------

/**
* ����� �������� �������� ��� ������������ �������� ���������� ����
*/
template <class Type>
class TArray
{
private:
	/**
	* ���� ��������������� �������� �������� �� �������
	*/
	const bool fItemDel;

	/**
	* ������ ��������� ���������� ����
	*/
	Type** fArray;

	/**
	* ���������� ��������� � �������
	*/
	unsigned int fCount;

	/**
	* ������ ������� (������������� ������������� ��� ����������)
	*/
	unsigned int fSize;
protected:
public:

	/**
	* ����������� ������� ������� ������ ������ ������ �� ��������� ������
	* @param Size ���������� ������ ������� ��� ������� ������� ��������� (1000 �� ���������)
	* @param ItemDel ���� �������� ��������� � ����������� (�� ��������� false)
	*/
	TArray( const int Size = 1000, const bool ItemDel = true ): fItemDel(ItemDel)   { fArray = new Type*[fSize=Size]; fCount = 0; }

	/**
	* ���������� ������� �������� ������� ���� ��� ������� � ������������
	*/
	virtual ~TArray( void )   { RemoveAll(fItemDel); delete[] fArray; }
	
	/**
	* ������� ��� �������� �������. � ����������� �� ��������� �������� ����� ������� (true)
	* ���� ������ ������ ����� ������� (false).
	* @param ItemDel
	*/
	void         RemoveAll(const bool ItemDel);

	/**
	* �������� ����� ������� � ����� �������. ������� �� ����������.
	* @return ����� ������������ �������� � �������
	*/
	inline int   Add(Type* p);

	/**
	* �������� ���������� ��������� � �������
	*/
	int          Count(void) { return fCount; }

	/**
	* �������� ������� �� �������.
	* @param Index ����� ��������
	* @return ������� ������� ��� NULL ���� �������� ������ ��� ��������� ��������
	*/
	Type*        Item(unsigned int Index) { 
		if ( Index < 0 || Index >= fCount ) {
			return NULL;
		}
		return fArray[Index];
	}

	/**
	* ������� ������� �� ������� �� �������
	* @param Index ������ ��������
	* @rturn true ���� ������� ���������� � ������ - ����� false
	*/
	bool         Del(unsigned int Index);
};

template<class Type>
bool TArray<Type>::Del( unsigned int Index ) {
	if ( Index < 0 || Index >= fCount ) {
		return false;
	}

	if( fItemDel )
		delete fArray[Index];
	
	fCount--;
	memcpy(fArray+Index, fArray+Index+1, sizeof(Type*)*(fCount-Index));
	return true;
}

template<class Type>
void TArray<Type>::RemoveAll( const bool ItemDel ) {
	if( ItemDel )
		for( unsigned int i = 0; i < fCount; i++ )
			delete fArray[i];
		fCount = 0;
}

template<class Type>
inline int TArray<Type>::Add(Type *p) {
	if( fCount >= fSize ) {
		int NewSize = fCount + fCount / 2;
		Type** pArray = new Type*[NewSize];
		memcpy(pArray,fArray,sizeof(Type*)*fSize);
		delete[] fArray;
		fArray = pArray;
		fCount = fSize;
		fSize = NewSize;
	}

	fArray[fCount] = p;
	return fCount++;
}

//- Chain ----------------------------------------------------------------------


template <class Type> class TChain;

/**
* ����� ��������� ������� �������� ��������� � ���������� �������
*/
template <class Type>
class TChainItem {
	friend class TChain<Type>;
private:
	/**
	* ��������� �� ���������� ������� ������
	*/
	TChainItem<Type>* Prev;

	/**
	* ��������� �� ��������� ������� ������
	*/
	TChainItem<Type>* Next;

	/**
	* ��������� �� �������� �������
	*/
	Type*             Item;
protected:

	/**
	* �����������
	* @param p ��������� �� �������� �������
	*/
	TChainItem( Type *p ): Item(p)   { Prev = Next = NULL; }

	/**
	* �����������
	* @param p ��������� �� �������� �������
	* @param p_prev ��������� �� ���������� ������� ������
	* @param p_next ��������� �� ��������� ������� ������
	*/
	TChainItem( Type *p, TChainItem<Type>* p_prev, TChainItem<Type>* p_next ): Item(p),Prev(p_prev),Next(p_next)   {}

	/**
	* ���������� ������ �� ������
	*/
	virtual ~TChainItem( void )   {}
public:

	/**
	* �������� �������� �������
	* @return �������� �������
	*/
	Type* GetItem(void)   { return Item; }
};


/**
* ����� ����������� ������.
* ����� �������� �������� ������� ������ ���������� ������ ��������� ��������� ����.
* � ������ �������� �� ������� ���������� ����, � ��������� �� ����� TChainItem �������
* �������� ��������� �� ������� ���������� ����.
* @see TChainItem
* @see TIterator
* @see TFind
*/
template <class Type>
class TChain {
private:
	/**
	* ������� �������� ��������� � �����������
	*/
	const bool      fItemDel;

	/**
	* ��������� �� ��������� ������� ������
	*/
	TChainItem<Type> *Root;

	/**
	* ��������� �� ��������� ������� ������
	*/	
	TChainItem<Type> *Last;
	
	/**
	* ���������� ��������� � ������
	*/
	int             Count;
protected:

	/**
	* �������� ����� ������� � ������. ����������� ������� �� ����������.
	* @param p ��������� �� ����� �������
	* @see TChainItem
	*/
	inline void Add(TChainItem<Type>* p) {
		if( isEmpty() )
			Root = Last = p;
		else {
			Last->Next = p;
			p->Prev = Last;
			Last = p;
		}
	}

public:
	
	/**
	* ����������� ������.
	* ��� �������� ������ �������� ��������� ItemDel �������� ��������� ��
	* ������. ���� �������� �������� ��������������� �����������, �������
	* ������� � ��������� false - ����� �������� ������ �� ����� �������.
	*
	* @param ItemDel ���� ����������� �� �������� ��������� � �����������
	*/
	TChain( const bool ItemDel = true ): fItemDel(ItemDel) {
		Root = Last = NULL;
	}
	
	/**
	* ���������� ������� �������� �� ������ ���� ������� � ���������� ������������
	*/
	virtual ~TChain( void ) {
		RemoveAll(fItemDel);
	}
	
	/**
	* ����� ������� � ������. ���� ��� ��������� � ������ ������ �������� "=="
	* ����� ����������� ����� �������� ��������� ���� �������.
	* @param item ������ �������
	* @return ��������� �� ������� ������
	* @see TChainItem
	*/
	TChainItem<Type>*  hasObject(const Type& item);

	/**
	* ��������� �������� �� ������ ��������
	* @return true ���� �������� ������ ���� ������� - ����� false
	*/
	bool               isEmpty(void)   { return (Root==NULL); }

	/**
	* �������� ������. ���� � ������ ������������ ������ �� �������������� ����������
	* ������� ������� ���������� false - ����� ������ ����� ������, �� �������� �� �������.
	* @param fItemDel ���� ����������� �� �� ��� ������ ������� ������ ���� ����� ������
	*/
	void               RemoveAll(const bool fItemDel);

	/**
	* ������� ��� �������� �� ������.
	* ������ ��������� � ��������� ��� ��� ��������. ���������� ������ RemoveAll(true).
	*/
	void               Clear() { RemoveAll(true); }

	/**
	* �������� � ������ ���������. ���� ������ �������� true �� ����� ������� ����� ���������
	* (�������� �� ������������� ������ ����� ����� ������� � ��������), ����� - ����������� 
	* ������ ��������� �� �������� ������.
	* @param Chain ������ �� �������� ���������� ��������
	* @param fCopy ���� �����������
	*/
	void               Add(TChain<Type>* Chain, const bool fCopy);

	/**
	* �������� ������� � ������ ����� ���������. ����������� ����������� ��������.
	* @param p ����� �������
	* @param p_item ��������� �� ������� ����� ������� ��������� ����� �������
	* @return ��������� �� ���������� ������� ������
	* @see TChainItem
	*/
	TChainItem<Type>*  Insert(TChainItem<Type>* p, Type* p_item);

	/**
	* �������� ������� � ����� ������. ����������� ����������� ��������.
	* @param p ��������� �� ����� �������
	* @return ��������� �� �������� ������� ������
	*/
	TChainItem<Type>*  Add(Type* p);

	/**
	* ������� ������� �� �����.
	* @param p ��������� �� ������� ��� ��������
	* @return true ���� ������� ������ � ������ - ����� fals
	*/
	bool               Del(Type* p);

	/**
	* ������� ������� �� �����.
	* @param p ��������� �� ������� ��� ��������
	* @return true ���� ������� ������ � ������ - ����� fals
	* @see TChainItem
	*/
	bool               Del(TChainItem<Type>* p);

	/**
	* ������� �������� ������� �� �����
	* @return true ���� ������� ���������� � ������ - ����� false
	*/
	bool               DelFirst();
	
	/**
	* ��������� ��� ����� ���������� ������ �������� ��������.
	* @param Iterator ������ ���������
	* @see TIterator
	*/
	void               For(TIterator<Type>& Iterator);

	/**
	* ����� ������� ������ ��������� ����� ������.
	* @param Find ����� ������
	* @return ������� ������ ��� NULL
	* @see TFind
	*/
	Type*              FindItem(TFind<Type>& Find);

	/**
	* ����� ������� ������ ��������� ����� ������.
	* @param Find ����� ������
	* @return ������� ������ ��� NULL
	* @see TFind
	*/
	TChainItem<Type>*  Find(TFind<Type>& Find);

	/**
	* ����� � �������� ������� ������ �� ��������� �� �������� ������
	* @param elem ��������� �� �������
	* @return ������� ������ ��� NULL
	* @see TChainItem
	*/
	TChainItem<Type>*  Find(const Type *elem);

	/**
	* ����� � �������� �������� ������� ������
	* @return �������� ������� ������ ��� NULL
	*/
	TChainItem<Type>*  First(void) { return Root; }
	
	/**
	* ����� � �������� ���������� (���������_ ������� ������
	* @return ���������� ������� ������ ��� NULL
	*/
	TChainItem<Type>*  FindLast(void) { return Last; }
	
	/**
	* �������� ��������� �� ��������� �������
	* @param p ������� �������
	* @return ��������� �� ��������� ������� ��� NULL
	* @see TChainItem
	*/
	TChainItem<Type>*  Next(TChainItem<Type>* p);
	
	/**
	* �������� ���������� ����� ��������� �������
	* @param p ������� �������
	* @return ���������� ����� ��������� ������� ��� NULL
	* @see TChainItem
	*/
	TChainItem<Type>*  Prev(TChainItem<Type>* p);
};

template<class Type>
bool TChain<Type>::Del(Type* Item) {
  TChainItem<Type>* p = Find(Item);
  if( p != NULL )
    return Del(p);
  return false;
}

template<class Type>
bool TChain<Type>::Del(TChainItem<Type>* p) {
  if( p == NULL )
    return false;
  if( Root == p )
    Root = p->Next;
  if( Last == p )
    Last = p->Prev;
  if( p->Prev != NULL )
    p->Prev->Next = p->Next;
  if( p->Next != NULL )
    p->Next->Prev = p->Prev;
  if( fItemDel )
    delete p->Item;
  delete p;
  return true;
}

template<class Type>
bool TChain<Type>::DelFirst() {
  return Del(First());
}

template<class Type>
inline TChainItem<Type>* TChain<Type>::Prev(TChainItem<Type>* p) {
  if( p != NULL )
    p = p->Prev;
  return p;
}

template<class Type>
inline TChainItem<Type>* TChain<Type>::Next(TChainItem<Type>* p) {
  if( p != NULL )
    p = p->Next;
  return p;
}

template<class Type>
inline TChainItem<Type>* TChain<Type>::Add(Type* p)
{
  TChainItem<Type>* p_ = new TChainItem<Type>(p);
  Add(p_);
  return p_;
}

template<class Type>
void TChain<Type>::RemoveAll( const bool fItemDel ) {
  while( Root != NULL)
  {
    TChainItem<Type>* p = Root->Next;
    if( fItemDel )
      delete Root->Item;
    delete Root;
    Root = p;
  }
  Root = Last = NULL;
}

template<class Type>
void TChain<Type>::Add(TChain<Type>* Chain, const bool fCopy) {
  TChainItem<Type>* p = Chain->First();
  while( p != NULL)
  {
    if( fCopy ) {
      Type* p_item = new Type(*(p->GetItem()));
      Add(p_item);
    } else
      Add(p->GetItem());
    p = Chain->Next(p);
  }
}

template<class Type>
inline TChainItem<Type>* TChain<Type>::Insert(TChainItem<Type>* p, Type* p_item)
{
  TChainItem<Type>* p_ = new TChainItem<Type>(p);
  if( p == NULL ) {
    p_->Next = Root;
    if( Root != NULL )
      Root->Prev = p_item;
    if( Last == Root )
      Last = p_;
    Root = p_;
  } else {
    p_->Next = p->Next;
    p->Prev = p;
    p->Next = p_;
    if( Last == p )
      Last = p;
  }
  return p;
}

template<class Type>
Type* TChain<Type>::FindItem(TFind<Type>& Find) {
  TChainItem<Type>* p = Find(Find);
  if( p != NULL )
    return p->Item;
  return NULL;
}

template<class Type>
TChainItem<Type>* TChain<Type>::Find(const Type *p_item) {
  TChainItem<Type> *p = Root;
  while( p != NULL)
  {
    if( p->Item == p_item )
      return p;
    p = p->Next;
  }
  return NULL;
}

template<class Type>
TChainItem<Type>* TChain<Type>::Find(TFind<Type>& Find) {
  TChainItem<Type> *p = Root;
  while( p != NULL)
  {
    if( Find.Is(p->Item) )
      return p;
    p = p->Next;
  }
  return NULL;
}

template<class Type>
TChainItem<Type>* TChain<Type>::hasObject(const Type& item) {
  if( isEmpty() )
    return NULL;
  else {
    TChainItem<Type> *p = Root;
    while( p != NULL )
    {
      if( *(p->Item) == item )
        return p;
      else
        p = p->Next;
    }
    return NULL;
  }
}

template<class Type>
void TChain<Type>::For(TIterator<Type>& Iterator) {
  TChainItem<Type> *p = Root;
  while( p != NULL)
  {
    Find.Do(p->Item);
    p = p->Next;
  }
}

#endif

