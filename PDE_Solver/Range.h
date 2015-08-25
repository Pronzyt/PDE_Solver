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
	virtual void to_rbegin() = 0;
	virtual bool in_end() = 0;
	virtual bool in_rend() = 0;
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
		Holder(iterator_type first, iterator_type last) : m_begin(first), 
														  m_rbegin(last),
														  m_end(++last),
														  m_rend(--first){};
		my_Ty* clone() const override {return new my_Ty(*this);};		
		void increment() override {++m_current;};
		void decrement() override {--m_current;};
		void to_begin() override {m_current = m_begin;};
		void to_rbegin() override {m_current = m_rbegin;};
		bool in_end() override {return m_current == m_end;};
		bool in_rend() override {return m_current == m_rend;}
		value_type& getValue() override {return *m_current;};
		~Holder() override {};
	private:	
		const iterator_type m_begin;
		const iterator_type m_rbegin;
		const iterator_type m_end;	
		const iterator_type m_rend;
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
	
	Range() = delete;
	Range(const Range& rhs){m_holder = rhs.m_holder->clone();}
	Range& operator=(const Range& rhs)
	{
		delete m_holder;
		m_holder = rhs.m_holder->clone();
		return *this;
	}
	
	template<typename BidirectionalIterator>
	Range(BidirectionalIterator first, BidirectionalIterator last)
	{
		m_holder = new Holder<value_type, BidirectionalIterator>(first, last);		
	}
	
	my_Ty& operator++() override {m_holder->increment(); return *this;}
	my_Ty operator++(int) override {my_Ty temp(*this); m_holder->increment(); return temp;}
	my_Ty& operator--() override {m_holder->decrement(); return *this;}
	my_Ty operator--(int) override {my_Ty temp(*this); m_holder->decrement(); return temp;}
	value_type& operator*() override {return m_holder->getValue();};
	
	my_Ty& to_begin(){m_holder->to_begin(); return *this;}
	my_Ty& to_rbegin(){m_holder->to_rbegin(); return *this;}
	bool in_end(){return m_holder->in_end();};
	bool in_rend(){return m_holder->in_rend();};	
	value_type getValue() const {return m_holder->getValue();}
	
	~Range(){delete m_holder;}	
private:
	BaseHolder<value_type>* m_holder = 0;
};



#endif	/* RANGE_H */

