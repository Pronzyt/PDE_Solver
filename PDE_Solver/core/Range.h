#ifndef RANGE_H
#define	RANGE_H

namespace core{

	/*Класс Range абстрагирует досптуп к состояниям, 
	для того, чтобы была возможность использовать любые итераторы
	...*/

template<typename Ty>
class BaseHolder{
public:
	typedef Ty value_type;
	typedef BaseHolder<Ty> my_Ty;
	virtual my_Ty* clone() const = 0;
	virtual void increment() = 0;
	virtual void decrement() = 0;
	virtual void to_begin() = 0;
	virtual bool in_end() = 0;
	/**/
	virtual void hold() = 0;
	virtual void reset() = 0;
	/**/
	virtual value_type& getReference() = 0;
	virtual value_type* getPointer() = 0;
	virtual ~BaseHolder() = 0;
};

template<typename Ty>
BaseHolder<Ty>::~BaseHolder(){};


template<typename BidirectionalIterator>
class Holder : public BaseHolder<typename BidirectionalIterator::value_type>{
public:
	typedef Holder<BidirectionalIterator> my_Ty;
	typedef typename my_Ty::value_type value_type;
	typedef BidirectionalIterator iterator_type;
	Holder(iterator_type begin, iterator_type end)
		: m_begin(begin),
		m_end(end)
	{};
	my_Ty* clone() const override { return new my_Ty(*this); };
	void increment() override { ++m_current; };
	void decrement() override { --m_current; };
	void to_begin() override { m_current = m_begin; };
	bool in_end() override { return m_current == m_end; };
	/**/
	void hold() override { m_held = m_current; };
	void reset() override { m_current = m_held; };
	/**/
	value_type& getReference() override { return *m_current; };
	value_type* getPointer() override { return &(*m_current); };

	~Holder() override {};
private:
	Holder() = delete;

	const iterator_type m_begin;
	const iterator_type m_end;
	iterator_type m_current;
	iterator_type m_held;
};


template<typename Derived, typename my_Val>
class IRange{
public:
	typedef my_Val value_type;
	virtual Derived& operator++() = 0;
	virtual Derived operator++(int) = 0;
	virtual Derived& operator--() = 0;
	virtual Derived operator--(int) = 0;
	virtual value_type& operator*() = 0;
	virtual value_type* operator->() = 0;
	virtual ~IRange() = 0;
};


template<typename Derived, typename my_Val>
IRange<Derived, my_Val>::~IRange(){};


template<typename Ty>
class Range : public IRange<Range<Ty>, Ty> {
public:
	typedef Ty value_type;
	typedef Range<Ty> my_Ty;

	Range(const Range& rhs){ m_holder = rhs.m_holder->clone(); }
	Range& operator=(const Range& rhs)
	{
		delete m_holder;
		m_holder = rhs.m_holder->clone();
		return *this;
	};

	my_Ty& operator++() override { m_holder->increment(); return *this; };
	my_Ty operator++(int) override { my_Ty temp(*this); m_holder->increment(); return temp; };
	my_Ty& operator--() override { m_holder->decrement(); return *this; };
	my_Ty operator--(int) override { my_Ty temp(*this); m_holder->decrement(); return temp; };
	value_type& operator*() override { return m_holder->getReference(); };
	value_type* operator->() override { return m_holder->getPointer(); };

	my_Ty& to_begin(){ m_holder->to_begin(); return *this; };
	bool in_end(){ return m_holder->in_end(); };
	/**/
	my_Ty& hold(){ m_holder->hold(); return *this; };
	my_Ty& reset(){ m_holder->reset(); return *this; };
	/**/
	~Range(){ delete m_holder; }

protected:
	Range(){};
	BaseHolder<value_type>* m_holder = 0;
};

/**/
template<typename Ty>
class FRange : public Range<Ty>{
public:
	typedef typename Range<Ty>::value_type value_type;
	

	template<typename BidirectionalIterator>
	FRange(BidirectionalIterator from, BidirectionalIterator to, int offset)
	{
		std::advance(from, offset);
		std::advance(to, offset);
		Range<Ty>::m_holder = new Holder<BidirectionalIterator>(from, ++to);
	};
};

/**/
template<typename Ty>
class RRange : public Range<Ty>{
public:
	typedef typename Range<Ty>::value_type value_type;

	template<typename BidirectionalIterator>
	RRange(BidirectionalIterator from, BidirectionalIterator to, int offset)
	{
		std::advance(from, -offset);
		std::advance(to, -offset);
		typedef std::reverse_iterator<BidirectionalIterator> RIter;
		Range<Ty>::m_holder = new Holder<RIter>(RIter(++to), RIter(from));
	};
};

};

#endif	/* RANGE_H */
