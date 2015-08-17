#ifndef RANGE_H
#define	RANGE_H

template<typename Ty>
class BaseHolder{
public:
	virtual Ty pref_increment() = 0;
	virtual Ty post_increment() = 0;
	virtual Ty pref_decrement() = 0;
	virtual Ty post_decrement() = 0;
	virtual void to_begin() = 0;
	virtual void to_end() = 0;
	virtual ~BaseHolder() = 0;
};

template<typename Ty>
BaseHolder<Ty>::~BaseHolder(){};


template<typename Ty, typename BidirectionalIterator>
class Holder : public BaseHolder<Ty>{
static_assert(std::is_same<typename BidirectionalIterator::value_type, Ty>::value, "Wrong iterator value type");
	public:
		Holder() = delete;
		Holder(BidirectionalIterator begin, BidirectionalIterator end) : m_begin(begin), m_end(end) {};
		Ty pref_increment() override {return *++m_current;};
		Ty post_increment() override {return *m_current++;};
		Ty pref_decrement() override {return *--m_current;};
		Ty post_decrement() override {return *m_current--;};
		void to_begin() override {m_current = m_begin;};
		void to_end() override {m_current = m_end;};
		~Holder() override {};
	private:	
		BidirectionalIterator m_begin;
		BidirectionalIterator m_end;
		BidirectionalIterator m_current;
};


template<typename Ty>
class Range{
public:
	typedef typename Ty valye_type;
	template<typename BidirectionalIterator>
	void setRange(BidirectionalIterator begin, BidirectionalIterator end)
	{
		m_holder = new Holder<Ty, BidirectionalIterator>(begin, end);		
	};
	
	~Range()
	{
		delete m_holder;
		m_holder = 0;
	}
	
private:
	BaseHolder<Ty>* m_holder = 0;
};



#endif	/* RANGE_H */

