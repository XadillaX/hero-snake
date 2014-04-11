/**
* @file m_list.h
* @brief Файл описания классов списков
*/

#ifndef M_LISTH
#define M_LISTH

#include <stdlib.h>
#include <string.h>

//- List -----------------------------------------------------------------------

template <class Type> class TList;

/**
* Класс описывает единицу хранения элементов в односвязных списках
*/
template <class Type>
class TListItem {
	friend class TList<Type>;
private:
	/**
	* Указатель на следующий лемент в списке
	*/
	TListItem<Type>* Next;
	
	/**
	* Элемент списка
	*/
	Type*            Item;

protected:
	/**
	* Конструктор
	* @param p указатель на добавляемый элемент
	*/
	TListItem( Type *p ): Item(p)   { Next = NULL; }
	
	/**
	* Конструктор
	* @param p указатель на добавляемый элемент
	* @param p_next указатель на следующий элемент
	*/
	TListItem( Type *p, TListItem<Type>* p_next ): Item(p),Next(p_next)   {}
	
	/**
	* Деструктор не выполняет никаких действий
	*/
	virtual ~TListItem( void )   {}
	
public:
	/**
	* Получить хранимый элемент
	* @return указатель на элемент
	*/
	Type* GetItem(void)   { return Item; }
};

//------------------------------------------------------------------

/**
* Базовый класс для поиска в списках и массивах
*/
template <class Type>
class TFind {
public:
	/**
	* Метод сравнения
	* @param elem элемент для сравнения
	* @return true если элемент найден - иначе false
	*/
	virtual bool Is(Type*elem) = NULL;
};

/**
* Базовый класс для прохода списков и массивов
*/
template <class Type>
class TIterator {
public:
	/**
	* Метод оперирования каждым элементом
	* @param elem элемент для проведения операции
	*/
	virtual void Do(Type*elem) = NULL;
};


/**
* Класс односвязного списка
* Класс является шаблоном которых хранит односвязнай список элементов заданного типа.
* В списке хранятся не объекты указанного типа, а указатели на класс TListItem которые
* содержат указатели на объекты указанного типа.
* @see TListItem
* @see TIterator
* @see TFind
*/
template <class Type>
class TList {
private:
	/**
	* Признак удаления элементов в деструкторе
	*/
	const bool      fItemDel;

	/**
	* Указатель на кореневой элемент списка
	*/
	TListItem<Type> *Root;
	
	/**
	* Указатель на последний элемент списка
	*/	
	TListItem<Type> *Last;

	/**
	* Набор указателей для доступа к элементам списка как к массиву
	*/
	TListItem<Type> **p_List;

	/**
	* Количество элементов в списке
	*/
	int             p_ListCount;
protected:
	/**
	* Пересчитать и получить количество элементов в списке
	* @return количество элементов в списке
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
	* Инициализация списка
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
	* Сброс состояния списка. Удаляются внутрение элементы. Сбрасывается счетчик элементов.
	*/
	void Reset(void) {
		if( p_List != NULL ) {
			delete[] p_List;
			p_List = NULL;
			p_ListCount = 0;
		}
	}

	/**
	* Добавить новый элемент в список. Копирования объекта не происходит.
	* @param p указатель на новый элемент
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
	* Конструктор списка.
	* При удалении списка согласно параметру ItemDel элементы удаляются из
	* списка. Если элементы являются автоматическими переменными, следует
	* указать в параметре false - тогда элементы списка не будут удалены.
	*
	* @param ItemDel флаг указывающий на удаление элементов в деструкторе
	*/
	TList( const bool ItemDel = true ): fItemDel(ItemDel), p_List(NULL), p_ListCount(0) {
		Root = Last = NULL;
	}

	/**
	* Деструктор удаляет элементы из списка если указано в параметрах конструктора
	*/
	virtual ~TList( void ) {
		Reset();
		RemoveAll(fItemDel);
	}
	
	/**
	* Найти элемент в списке. Если для элементов в списке описан оператор "=="
	* можно производить поиск элеметов пользуясь этим методом.
	* @param item ссылка элемент
	* @return указатель на элемент списка
	* @see TListItem
	*/
	TListItem<Type>*  hasObject(const Type& item);

	/**
	* Проверить содержит ли список элементы
	* @return true если содержит хотябы один элемент - иначе false
	*/
	bool              isEmpty(void)   { return (Root==NULL); }

	/**
	* Очистить список. Если в списке используются ссылки на автоматические переменные
	* следует указать параметром false - тогда список будет очищен, но элементы не удалены.
	* @param fItemDel флаг указывающий на то что каждый элемент должен быть также удален
	*/
	void              RemoveAll(const bool fItemDel);

	/**
	* Удалить все элементы из списка.
	* Список очищается и удаляются все его элементы. Аналогично вызову RemoveAll(true).
	*/
	void              Clear() { RemoveAll(true); }

	/**
	* Добавить в список подсписок. Если второй параметр true то будет создана копия элементов
	* (элементы из оригинального списка можно будет удалять и изменять), иначе - скопируются 
	* только указатели на элементы списка.
	* @param List список из которого копировать элементы
	* @param fCopy флаг копирования
	*/
	void              Add(TList<Type>* List, const bool fCopy);

	/**
	* Вставить элемент в список перед указанным. Производтся копирование элемента.
	* @param p новый элемент
	* @param p_item указатель на элемент перед которым вставлять новый элемент
	* @return указатель на вставленый элемент списка
	* @see TListItem
	*/
	TListItem<Type>*  Insert(TListItem<Type>* p, Type* p_item);

	/**
	* Добавить элемент в конец списка. Производтся копирование элемента.
	* @param p указатель на новый элемент
	* @return указатель на созданый элемент списка
	*/
	TListItem<Type>*  Add(Type* p);

	/**
	* Удалить элемент из спика.
	* @param p указатель на элемент для удаления
	* @return true если элемент найден и удален - иначе fals
	*/
	bool              Del(Type* p);

	/**
	* Удалить элемент из спика.
	* @param p указатель на элемент для удаления
	* @return true если элемент найден и удален - иначе fals
	* @see TListItem
	*/
	bool              Del(TListItem<Type>* p);

	/**
	* Получить количество элементов в списке
	* @return количество элементов в списке
	*/
	int               Count(void);

	/**
	* Получить элемент по числовому индексу. К списку можно также обращаться как к массиву.
	* При этом индекс элементов совпадает с порядком их добавления в список.
	* @param Index индекс элемента (начиная с 0)
	* @return элемент списка или NULL
	*/
	Type*             Item(unsigned int Index);
	
	/**
	* Выполнить над всеми элементами списка указаное действие.
	* @param Iterator объект итератора
	* @see TIterator
	*/
	void              For(TIterator<Type>& Iterator);

	/**
	* Найти элемент списка используя класс поиска.
	* @param Find класс поиска
	* @return элемент списка или NULL
	* @see TFind
	*/
	Type*             FindItem(TFind<Type>& Find);

	/**
	* Найти элемнт списка по указателю.
	* @param elem указатель на элемент
	* @return элемент списка или NULL
	* @see TListItem
	*/
	TListItem<Type>*  Find(const Type *elem);

	/**
	* Найти элемент списка используя класс поиска.
	* @param Find класс поиска
	* @return элемент списка или NULL
	* @see TFind
	* @see TListItem
	*/
	TListItem<Type>*  Find(TFind<Type>& Find);

	/**
	* Получить первый элемент списка
	* @return корневой элемент списка или NULL
	*/
	TListItem<Type>*  First(void) { return Root; }

	/**
	* Получить последний элемент списка
	* @return последний элемент списка или NULL
	*/
	TListItem<Type>*  FindLast(void) { return Last; }

	/**
	* Получить следующий за указанным элемент
	* @param p текущий элемент
	* @return следующий за указанным элемент или NULL
	* @see TListItem
	*/
	TListItem<Type>*  Next(TListItem<Type>* p);
	
	/**
	* Оператор индекстрования.
	* Аналогичен вызову метода Item(idx).
	* @param idx индекс элемента
	* @return элемент списка или NULL
	*/
	Type* operator [](const int idx) const     { return Item(idx); }

	/**
	* Оператор индекстрования.
	* Аналогичен вызову метода Item(idx).
	* @param idx индекс элемента
	* @return элемент списка или NULL
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
* Класс является шаблоном для нумерованных массивов указанного типа
*/
template <class Type>
class TArray
{
private:
	/**
	* Флаг автоматического удаления элеметов из массива
	*/
	const bool fItemDel;

	/**
	* Массив элементов указанного типа
	*/
	Type** fArray;

	/**
	* Количестов элементов в массиве
	*/
	unsigned int fCount;

	/**
	* Размер массива (увеличивается автоматически при надобности)
	*/
	unsigned int fSize;
protected:
public:

	/**
	* Конструктор массива создает пустой массив исходя из начальных данных
	* @param Size внутренний размер массива для быстрой втсавки элементов (1000 по умолчанию)
	* @param ItemDel флаг удаления элементов в деструкторе (по умолчанию false)
	*/
	TArray( const int Size = 1000, const bool ItemDel = true ): fItemDel(ItemDel)   { fArray = new Type*[fSize=Size]; fCount = 0; }

	/**
	* Деструктор удаляет элементы массива если это сказано в конструкторе
	*/
	virtual ~TArray( void )   { RemoveAll(fItemDel); delete[] fArray; }
	
	/**
	* Удалить все элементы массива. В зависимости от параметра элементы будут удалены (true)
	* либо массив просто будет обнулен (false).
	* @param ItemDel
	*/
	void         RemoveAll(const bool ItemDel);

	/**
	* Добавить новый элемент в конец массива. Элемент не копируется.
	* @return номер добавленного элемента в массиве
	*/
	inline int   Add(Type* p);

	/**
	* Получить количество элементов в массиве
	*/
	int          Count(void) { return fCount; }

	/**
	* Получить элемент по индексу.
	* @param Index номер элемента
	* @return элемент массива или NULL если указаный индекс вне диапазона значений
	*/
	Type*        Item(unsigned int Index) { 
		if ( Index < 0 || Index >= fCount ) {
			return NULL;
		}
		return fArray[Index];
	}

	/**
	* Удалить элемент из массива по индексу
	* @param Index индекс элемента
	* @rturn true если элемент существует и удален - иначе false
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
* Класс описывает единицу хранения элементов в двусвязных списках
*/
template <class Type>
class TChainItem {
	friend class TChain<Type>;
private:
	/**
	* Указатель на предидущий элемент списка
	*/
	TChainItem<Type>* Prev;

	/**
	* Указатель на следующий элемент списка
	*/
	TChainItem<Type>* Next;

	/**
	* Указатель на хранимый элемент
	*/
	Type*             Item;
protected:

	/**
	* Конструктор
	* @param p указатель на хранимый элемент
	*/
	TChainItem( Type *p ): Item(p)   { Prev = Next = NULL; }

	/**
	* Конструктор
	* @param p указатель на хранимый элемент
	* @param p_prev указатель на предидущий элемент списка
	* @param p_next указатель на следующий элемент списка
	*/
	TChainItem( Type *p, TChainItem<Type>* p_prev, TChainItem<Type>* p_next ): Item(p),Prev(p_prev),Next(p_next)   {}

	/**
	* Деструктор ничего не делает
	*/
	virtual ~TChainItem( void )   {}
public:

	/**
	* Получить хранимый элемент
	* @return хранимый элемент
	*/
	Type* GetItem(void)   { return Item; }
};


/**
* Класс двусвязного списка.
* Класс является шаблоном которых хранит двусвязный список элементов заданного типа.
* В списке хранятся не объекты указанного типа, а указатели на класс TChainItem которые
* содержат указатели на объекты указанного типа.
* @see TChainItem
* @see TIterator
* @see TFind
*/
template <class Type>
class TChain {
private:
	/**
	* Признак удаления элементов в деструкторе
	*/
	const bool      fItemDel;

	/**
	* Указатель на кореневой элемент списка
	*/
	TChainItem<Type> *Root;

	/**
	* Указатель на последний элемент списка
	*/	
	TChainItem<Type> *Last;
	
	/**
	* Количество элементов в списке
	*/
	int             Count;
protected:

	/**
	* Добавить новый элемент в список. Копирования объекта не происходит.
	* @param p указатель на новый элемент
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
	* Конструктор списка.
	* При удалении списка согласно параметру ItemDel элементы удаляются из
	* списка. Если элементы являются автоматическими переменными, следует
	* указать в параметре false - тогда элементы списка не будут удалены.
	*
	* @param ItemDel флаг указывающий на удаление элементов в деструкторе
	*/
	TChain( const bool ItemDel = true ): fItemDel(ItemDel) {
		Root = Last = NULL;
	}
	
	/**
	* Деструктор удаляет элементы из списка если указано в параметрах конструктора
	*/
	virtual ~TChain( void ) {
		RemoveAll(fItemDel);
	}
	
	/**
	* Найти элемент в списке. Если для элементов в списке описан оператор "=="
	* можно производить поиск элеметов пользуясь этим методом.
	* @param item ссылка элемент
	* @return указатель на элемент списка
	* @see TChainItem
	*/
	TChainItem<Type>*  hasObject(const Type& item);

	/**
	* Проверить содержит ли список элементы
	* @return true если содержит хотябы один элемент - иначе false
	*/
	bool               isEmpty(void)   { return (Root==NULL); }

	/**
	* Очистить список. Если в списке используются ссылки на автоматические переменные
	* следует указать параметром false - тогда список будет очищен, но элементы не удалены.
	* @param fItemDel флаг указывающий на то что каждый элемент должен быть также удален
	*/
	void               RemoveAll(const bool fItemDel);

	/**
	* Удалить все элементы из списка.
	* Список очищается и удаляются все его элементы. Аналогично вызову RemoveAll(true).
	*/
	void               Clear() { RemoveAll(true); }

	/**
	* Добавить в список подсписок. Если второй параметр true то будет создана копия элементов
	* (элементы из оригинального списка можно будет удалять и изменять), иначе - скопируются 
	* только указатели на элементы списка.
	* @param Chain список из которого копировать элементы
	* @param fCopy флаг копирования
	*/
	void               Add(TChain<Type>* Chain, const bool fCopy);

	/**
	* Вставить элемент в список перед указанным. Производтся копирование элемента.
	* @param p новый элемент
	* @param p_item указатель на элемент перед которым вставлять новый элемент
	* @return указатель на вставленый элемент списка
	* @see TChainItem
	*/
	TChainItem<Type>*  Insert(TChainItem<Type>* p, Type* p_item);

	/**
	* Добавить элемент в конец списка. Производтся копирование элемента.
	* @param p указатель на новый элемент
	* @return указатель на созданый элемент списка
	*/
	TChainItem<Type>*  Add(Type* p);

	/**
	* Удалить элемент из спика.
	* @param p указатель на элемент для удаления
	* @return true если элемент найден и удален - иначе fals
	*/
	bool               Del(Type* p);

	/**
	* Удалить элемент из спика.
	* @param p указатель на элемент для удаления
	* @return true если элемент найден и удален - иначе fals
	* @see TChainItem
	*/
	bool               Del(TChainItem<Type>* p);

	/**
	* Удалить корневой элемент из спика
	* @return true если элемент существует и удален - иначе false
	*/
	bool               DelFirst();
	
	/**
	* Выполнить над всеми элементами списка указаное действие.
	* @param Iterator объект итератора
	* @see TIterator
	*/
	void               For(TIterator<Type>& Iterator);

	/**
	* Найти элемент списка используя класс поиска.
	* @param Find класс поиска
	* @return элемент списка или NULL
	* @see TFind
	*/
	Type*              FindItem(TFind<Type>& Find);

	/**
	* Найти элемент списка используя класс поиска.
	* @param Find класс поиска
	* @return элемент списка или NULL
	* @see TFind
	*/
	TChainItem<Type>*  Find(TFind<Type>& Find);

	/**
	* Найти и получить элемент списка по указателю на хранимый объект
	* @param elem указатель на элемент
	* @return элемент списка или NULL
	* @see TChainItem
	*/
	TChainItem<Type>*  Find(const Type *elem);

	/**
	* Найти и получить корневой элемент списка
	* @return корневой элемент списка или NULL
	*/
	TChainItem<Type>*  First(void) { return Root; }
	
	/**
	* Найти и получить замыкающий (последний_ элемент списка
	* @return замыкающий элемент списка или NULL
	*/
	TChainItem<Type>*  FindLast(void) { return Last; }
	
	/**
	* Получить следующий за указанным элемент
	* @param p текущий элемент
	* @return следующий за указанным элемент или NULL
	* @see TChainItem
	*/
	TChainItem<Type>*  Next(TChainItem<Type>* p);
	
	/**
	* Получить предидущий перед указанным элемент
	* @param p текущий элемент
	* @return предидущий перед указанным элемент или NULL
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

