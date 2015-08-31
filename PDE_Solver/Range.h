#ifndef RANGE_H
#define	RANGE_H


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
	virtual value_type& getValue() = 0;
	virtual ~BaseHolder() = 0;
};

template<typename Ty>
BaseHolder<Ty>::~BaseHolder(){};


template<typename Ty, typename BidirectionalIterator>
class Holder : public BaseHolder<Ty>{
	
static_assert(std::is_same<typename BidirectionalIterator::value_type, Ty>::value, "Wrong iterator value type");

	public:
		typedef Holder<Ty, BidirectionalIterator> my_Ty;
		typedef typename my_Ty::value_type value_type;		
		typedef BidirectionalIterator iterator_type;
		Holder() = delete;
		Holder(iterator_type begin, iterator_type end) 
			: m_begin(begin),
			m_end(end)
		
		{
			typename iterator_type::value_type& temp = *begin;
		};
		my_Ty* clone() const override {return new my_Ty(*this);};		
		void increment() override {++m_current;};
		void decrement() override {--m_current;};
		void to_begin() override {m_current = m_begin;};
		bool in_end() override {return m_current == m_end;};
		value_type& getValue() override {return *m_current;};
		~Holder() override {};
	private:	
		const iterator_type m_begin;
		const iterator_type m_end;	
		iterator_type m_current;		
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
};


template<typename Ty>
class Range: public IRange<Range<Ty>, Ty> {
public:
	typedef Ty value_type;
	typedef Range<Ty> my_Ty;
	
	Range(const Range& rhs){m_holder = rhs.m_holder->clone();}
	Range& operator=(const Range& rhs)
	{
		delete m_holder;
		m_holder = rhs.m_holder->clone();
		return *this;
	}
	
	my_Ty& operator++() override {m_holder->increment(); return *this;}
	my_Ty operator++(int) override {my_Ty temp(*this); m_holder->increment(); return temp;}
	my_Ty& operator--() override {m_holder->decrement(); return *this;}
	my_Ty operator--(int) override {my_Ty temp(*this); m_holder->decrement(); return temp;}
	value_type& operator*() override {return m_holder->getValue();};
	
	my_Ty& to_begin(){m_holder->to_begin(); return *this;}
	bool in_end(){return m_holder->in_end();};
	value_type getValue() const {return m_holder->getValue();}
	
	~Range(){delete m_holder;}	

protected:
	Range(){};
	BaseHolder<value_type>* m_holder = 0;
};


template<typename Ty>
class FRange: public Range<Ty>{
public:
	typedef typename Range<Ty>::value_type value_type;
	template<typename BidirectionalIterator>
	FRange(BidirectionalIterator from, BidirectionalIterator to)
	{
		Range<Ty>::m_holder = new Holder<value_type, BidirectionalIterator>(from, ++to);
	}
};


template<typename Ty>
class RRange : public Range<Ty>{
public:
	typedef typename Range<Ty>::value_type value_type;
	template<typename BidirectionalIterator>
	RRange(BidirectionalIterator from, BidirectionalIterator to)
	{
		typedef std::reverse_iterator<BidirectionalIterator> RIter;
		typename BidirectionalIterator::value_type& temp0 = *to;
		typename RIter::value_type& temp1 = *RIter(++to);
		typename RIter::value_type& temp2 = *RIter(from);
		Range<Ty>::m_holder = new Holder<value_type, RIter>(RIter(to), RIter(from));
	}
};

#endif	/* RANGE_H */

