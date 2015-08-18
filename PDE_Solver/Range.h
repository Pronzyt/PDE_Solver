#ifndef RANGE_H
#define	RANGE_H

template<typename Ty>
class BaseHolder{
public:
	typedef Ty value_type;
	virtual void increment() = 0;
	virtual void decrement() = 0;
	virtual void to_begin() = 0;
	virtual void to_rbegin() = 0;
	virtual bool in_end() = 0;
	virtual bool in_rend() = 0;
	virtual value_type getValue() const = 0;
	virtual ~BaseHolder() = 0;
};

template<typename Ty>
BaseHolder<Ty>::~BaseHolder(){};


template<typename Ty, typename BidirectionalIterator>
class Holder : public BaseHolder<Ty>{
	
static_assert(std::is_same<typename BidirectionalIterator::value_type, Ty>::value, "Wrong iterator value type");

	public:
		typedef typename BaseHolder<Ty>::value_type value_type;
		typedef BidirectionalIterator iterator_type;
		//Clone!!
		Holder() = delete;
		Holder(iterator_type first, iterator_type last) : m_begin(first), 
														  m_rbegin(last),
														  m_end(++last),
														  m_rend(--first){};
		
		void increment() override {++m_current;};
		void decrement() override {--m_current;};
		void to_begin() override {m_current = m_begin;};
		void to_rbegin() override {m_current = m_rbegin;};
		bool in_end() override {return m_current == m_end;};
		bool in_rend() override {return m_current == m_rend;}
		value_type getValue() const override {return *m_current;};
		~Holder() override {};
	private:	
		const iterator_type m_begin;
		const iterator_type m_rbegin;
		const iterator_type m_end;	
		const iterator_type m_rend;
		iterator_type m_current;		
};


template<typename Ty>
class Range{
public:
	typedef Ty value_type;
	typedef Range<Ty> my_Ty;
	
	Range() = delete;
	Range(const Range& rhs){};
	
	template<typename BidirectionalIterator>
	Range(BidirectionalIterator first, BidirectionalIterator last)
	{
		m_holder = new Holder<value_type, BidirectionalIterator>(first, last);		
	};
	
	my_Ty& operator++(){m_holder->increment(); return *this;}
	my_Ty& operator++(int){my_Ty temp(*this); m_holder->increment(); return temp;};
	my_Ty& operator--(){m_holder->decrement(); return *this;};
	my_Ty& operator--(int){my_Ty temp(*this); m_holder->decrement(); return temp;};
	
	my_Ty& to_begin(){m_holder->to_begin(); return *this;};
	bool in_end(){return m_holder->in_end();};
	
	value_type getValue() const {return m_holder->getValue();}
	
	
	~Range()
	{
		delete m_holder;
		m_holder = 0;
	}
	
private:
	BaseHolder<value_type>* m_holder = 0;
};



#endif	/* RANGE_H */

