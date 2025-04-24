// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

#include "GradientspacePlatform.h"
#include "Core/gs_debug.h"
#include "Core/gs_allocator.h"
#include "Core/gs_serializer.h"
#include "Core/buffer_view.h"

namespace GS
{

/**
 * intended to be similar std::vector or TArray without the overhead
 * 
 * TODO: for non-trivially-copyable, we default construct to m_allocated_length...does it make sense? could limit it to m_length but then every add/etc needs to do it...
 * TODO: may not be destructing properly in all cases (eg set_ref, remove, etc)
 */
template<typename ValueType>
class unsafe_vector
{
private:
	ValueType* m_storage = nullptr;
	size_t m_length = 0;
	size_t m_allocated_length = 0;
	gs_allocator* m_external_allocator = nullptr;

public:
	~unsafe_vector();
	unsafe_vector();
	explicit unsafe_vector(gs_allocator* use_allocator);
	unsafe_vector(const unsafe_vector& copy);
	unsafe_vector(unsafe_vector&& moved);
	unsafe_vector& operator=(const unsafe_vector& copy);
	unsafe_vector& operator=(unsafe_vector&& moved);

	size_t size() const;
	size_t allocated_size() const;

	void reserve(size_t num_elements);
	void resize(size_t num_elements, bool shrink_memory = false);
	int64_t grow(size_t num_new_elements);
	void clear(bool free_memory = false);

	void initialize(size_t num_elements, const ValueType& initial_value);
	void initialize(size_t num_elements, const ValueType* initial_values);
	template<typename IndexableType>
	void initialize(size_t num_elements, const IndexableType& initial_values);

	template<typename IndexType>
	ValueType& operator[](IndexType index) {
		static_assert(std::is_integral_v<IndexType> == true);
		return m_storage[index];
	}
	template<typename IndexType>
	const ValueType& operator[](IndexType index) const {
		static_assert(std::is_integral_v<IndexType> == true);
		return m_storage[index];
	}

	ValueType& last();
	const ValueType& last() const;

	void set_ref(int index, const ValueType& NewValue);
	void set_move(int index, ValueType&& NewValue);
	
	int64_t add(ValueType Element);
	int64_t add_ref(const ValueType& Element);		// internally copies, so safe to call vec.add_ref(vec[elem])
	int64_t push_back(const ValueType& Element) { return add_ref(Element); }
	int64_t add_move(ValueType&& Element);
	int64_t append(const ValueType* ElementPtr, size_t NumElements);
	int64_t add_unique(const ValueType& Element);
	int64_t set_at(ValueType Element, int index);	// will grow array so that index is valid

	bool remove_at(int64_t Index);
	bool swap_remove(int64_t Index, int64_t& SwappedIndexOut);
	bool pop_back(ValueType& ValueOut);
	void pop_back();

	ValueType* raw_pointer();
	const ValueType* raw_pointer() const;

	ValueType* raw_pointer(int64_t Index);
	const ValueType* raw_pointer(int64_t Index) const;

	bool contains(const ValueType& Element) const;
	int64_t index_of(const ValueType& Element) const;

	const_buffer_view<ValueType> get_view() const;

	bool Store(GS::ISerializer& Serializer, const char* custom_key = nullptr) const;
	bool Restore(GS::ISerializer& Serializer, const char* custom_key = nullptr);
	constexpr const char* SerializeVersionString() const { return "unsafe_vector_Version"; }

private:
	void release_memory();
	void default_construct_if_necessary(int start_index = 0);
	void destruct_if_necessary(int start_index = 0);
	//void move_allocated_values(unsigned char* from, unsigned char* to, int num_elements);

	void apply_grow_policy(int64_t new_elements);

public:

	struct iterator
	{
		explicit iterator(ValueType* init_ptr) { m_ptr = init_ptr; }
		ValueType& operator*() const { return *m_ptr; }
		ValueType* operator->() { return m_ptr; }
		iterator& operator++() { m_ptr++; return *this; }
		iterator operator++(int) { iterator tmp = *this; ++(*this); return tmp; }
		friend bool operator== (const iterator& a, const iterator& b) { return a.m_ptr == b.m_ptr; };
		friend bool operator!= (const iterator& a, const iterator& b) { return a.m_ptr != b.m_ptr; };

		// these functions are required for std::sort to be called on an unsafe_vector
		using value_type = ValueType;
		iterator& operator--() { m_ptr--; return *this; }
		iterator operator--(int) { iterator tmp = *this; --(*this); return tmp; }
		friend ptrdiff_t operator-(const iterator& lhs, const iterator& rhs) { return lhs.m_ptr - rhs.m_ptr; }
		friend iterator operator-(iterator const& lhs, ptrdiff_t rhs) { return iterator(lhs.m_ptr - rhs); }
		friend iterator operator+(iterator const& lhs, ptrdiff_t rhs) { return iterator(lhs.m_ptr + rhs); }
		friend iterator operator+(ptrdiff_t lhs, iterator const& rhs) { return iterator(lhs + rhs.m_ptr); }
		friend bool operator< (const iterator& lhs, const iterator& rhs) { return lhs.m_ptr < rhs.m_ptr; }

	private:
		ValueType* m_ptr;
	};

	iterator begin() { return iterator(m_storage); }
	iterator end() { return iterator(&m_storage[m_length]); }


	struct const_iterator
	{
		explicit const_iterator(const ValueType* init_ptr) { m_ptr = init_ptr; }
		const ValueType& operator*() const { return *m_ptr; }
		const ValueType* operator->() { return m_ptr; }
		const_iterator& operator++() { m_ptr++; return *this; }
		const_iterator operator++(int) { const_iterator tmp = *this; ++(*this); return tmp; }
		friend bool operator== (const const_iterator& a, const const_iterator& b) { return a.m_ptr == b.m_ptr; };
		friend bool operator!= (const const_iterator& a, const const_iterator& b) { return a.m_ptr != b.m_ptr; };

	private:
		const ValueType* m_ptr;
	};

	const_iterator begin() const { return const_iterator(reinterpret_cast<const ValueType*>(m_storage)); }
	const_iterator end() const { return const_iterator(&reinterpret_cast<const ValueType*>(m_storage)[m_length]); }

};

template<typename ValueType>
unsafe_vector<ValueType>::~unsafe_vector()
{
	clear();
}

template<typename ValueType>
unsafe_vector<ValueType>::unsafe_vector()
{
}

template<typename ValueType>
unsafe_vector<ValueType>::unsafe_vector(gs_allocator* use_allocator)
	: m_external_allocator(use_allocator)
{
	
}

template<typename ValueType>
unsafe_vector<ValueType>::unsafe_vector(const unsafe_vector& copy)
{
	*this = copy;
}

template<typename ValueType>
unsafe_vector<ValueType>::unsafe_vector(unsafe_vector&& moved)
{
	m_external_allocator = moved.m_external_allocator;
	m_length = moved.m_length;
	m_allocated_length = moved.m_allocated_length;
	m_storage = moved.m_storage;

	moved.m_allocated_length = moved.m_length = 0;
	moved.m_storage = nullptr;
}


template<typename ValueType>
unsafe_vector<ValueType>& unsafe_vector<ValueType>::operator=(const unsafe_vector& copy)
{
	resize(copy.size());
	size_t required_bytes = m_length * sizeof(ValueType);
	memcpy_s((void*)m_storage, required_bytes, (void*)copy.m_storage, required_bytes);
	return *this;
}

template<typename ValueType>
unsafe_vector<ValueType>& unsafe_vector<ValueType>::operator=(unsafe_vector&& moved)
{
	m_external_allocator = moved.m_external_allocator;
	m_length = moved.m_length;
	m_allocated_length = moved.m_allocated_length;
	m_storage = moved.m_storage;

	moved.m_allocated_length = moved.m_length = 0;
	moved.m_storage = nullptr;
	return *this;
}




template<typename ValueType>
size_t unsafe_vector<ValueType>::size() const
{
	return m_length;
}

template<typename ValueType>
size_t unsafe_vector<ValueType>::allocated_size() const
{
	return m_allocated_length;
}


//template<typename ValueType>
//ValueType& unsafe_vector<ValueType>::operator[](int index)
//{
//	return m_storage[index];
//}
//
//template<typename ValueType>
//const ValueType& unsafe_vector<ValueType>::operator[](int index) const
//{
//	return m_storage[index];
//}

template<typename ValueType>
void unsafe_vector<ValueType>::set_ref(int index, const ValueType& NewValue)
{
	m_storage[index] = NewValue;
}

template<typename ValueType>
void unsafe_vector<ValueType>::set_move(int index, ValueType&& NewValue)
{
	m_storage[index] = std::move(NewValue);
}

template<typename ValueType>
ValueType& unsafe_vector<ValueType>::last()
{
	return (*this)[m_length-1];
}
template<typename ValueType>
const ValueType& unsafe_vector<ValueType>::last() const
{
	return (*this)[m_length-1];
}


template<typename ValueType>
int64_t unsafe_vector<ValueType>::add(ValueType Element)
{
	apply_grow_policy(1);
	int64_t new_index = m_length;
	m_storage[new_index] = std::move(Element);
	m_length++;
	return new_index;
}

template<typename ValueType>
int64_t unsafe_vector<ValueType>::add_ref(const ValueType& Element)
{
	ValueType NewElement = Element;
	apply_grow_policy(1);
	int64_t new_index = m_length;
	m_storage[new_index] = std::move(NewElement);
	m_length++;
	return new_index;
}

template<typename ValueType>
int64_t unsafe_vector<ValueType>::add_move(ValueType&& Element)
{
	apply_grow_policy(1);
	int64_t new_index = m_length;
	m_storage[new_index] = std::move(Element);
	m_length++;
	return new_index;
}

template<typename ValueType>
int64_t unsafe_vector<ValueType>::append(const ValueType* ElementPtr, size_t NumElements)
{
	apply_grow_policy(NumElements);
	int64_t new_index = m_length;
	for (size_t k = 0; k < NumElements; ++k)
	{
		m_storage[m_length] = ElementPtr[k];
		m_length++;
	}
	return new_index;
}

template<typename ValueType>
int64_t unsafe_vector<ValueType>::add_unique(const ValueType& Element)
{
	int64_t index = index_of(Element);
	if (index >= 0)
		return index;
	return add_ref(Element);
}

template<typename ValueType>
int64_t unsafe_vector<ValueType>::set_at(ValueType Element, int index)
{
	if (m_length <= index) {
		apply_grow_policy(index - m_length + 1);
		resize(index + 1);
	}
	m_storage[index] = Element;
	return index;
}



template<typename ValueType>
bool unsafe_vector<ValueType>::remove_at(int64_t Index)
{
	if (Index < 0 || Index >= (int64_t)m_length) return false;

	// todo destruct removed element if necessary?

	m_length--;
	for (size_t k = Index; k < m_length; ++k) 
		m_storage[k] = m_storage[k+1];

	return true;
}


template<typename ValueType>
bool unsafe_vector<ValueType>::swap_remove(int64_t Index, int64_t& SwappedIndexOut)
{
	if (Index < 0 || Index >= (int64_t)m_length) return false;

	// todo destruct removed element if necessary?

	if (Index == m_length - 1)		// if last element, we do not have to swap
	{
		SwappedIndexOut = -1;
		m_length--;
		return true;
	}

	if (m_length == 1)				// if size is 1, we must be removing only element, no swap
	{
		SwappedIndexOut = -1; 
		m_length = 0;
		return true;
	}

	m_storage[Index] = std::move(m_storage[m_length-1] );
	SwappedIndexOut = m_length - 1;
	m_length--;
	return true;
}

template<typename ValueType>
bool unsafe_vector<ValueType>::pop_back(ValueType& ValueOut)
{
	// todo destruct removed element if necessary?

	if (m_length == 0)
		return false;
	ValueOut = std::move(m_storage[m_length-1]);
	m_length--;
	return true;
}

template<typename ValueType>
void unsafe_vector<ValueType>::pop_back()
{
	// todo destruct removed element if necessary?

	if (m_length > 0)
		m_length--;
}

template<typename ValueType>
ValueType* unsafe_vector<ValueType>::raw_pointer()
{
	return m_storage;
}
template<typename ValueType>
const ValueType* unsafe_vector<ValueType>::raw_pointer() const
{
	return m_storage;
}

template<typename ValueType>
ValueType* unsafe_vector<ValueType>::raw_pointer(int64_t index)
{
	return &m_storage[index];
}
template<typename ValueType>
const ValueType* unsafe_vector<ValueType>::raw_pointer(int64_t index) const
{
	return &m_storage[index];
}


template<typename ValueType>
void unsafe_vector<ValueType>::reserve(size_t num_elements)
{
	if (num_elements <= m_allocated_length) return;
	size_t reserve_bytes = num_elements * sizeof(ValueType);

	if (m_length == 0)
	{
		m_storage = (m_external_allocator) ? (ValueType*)m_external_allocator->allocate(reserve_bytes) : (ValueType*)gs_default_allocator::allocate(reserve_bytes);
		m_allocated_length = num_elements;
		default_construct_if_necessary(0);
	}
	else
	{
		unsigned char* tmp_storage = (m_external_allocator) ? m_external_allocator->allocate(reserve_bytes) : gs_default_allocator::allocate(reserve_bytes);
		size_t copy_bytes = m_length * sizeof(ValueType);
		memcpy_s(tmp_storage, copy_bytes, m_storage, copy_bytes);
		release_memory();
		m_storage = (ValueType*)tmp_storage;
		m_allocated_length = num_elements;
		default_construct_if_necessary((int)m_length);
	}
}

template<typename ValueType>
void unsafe_vector<ValueType>::resize(size_t num_elements, bool shrink_memory)
{
	if (num_elements == m_length) return;

	if (num_elements == 0)
	{
		clear(shrink_memory);
		return;
	}

	if (num_elements > m_length)
	{
		reserve(num_elements);
		//default_construct_if_necessary((int)m_length);  // currently done by reserve()
		m_length = num_elements;
		return;
	}

	if (shrink_memory == false)
	{
		m_length = num_elements;
		return;
	}

	// must be the <, shrink case
	destruct_if_necessary((int)num_elements);
	size_t required_bytes = num_elements * sizeof(ValueType);
	unsigned char* tmp_storage = (m_external_allocator) ? m_external_allocator->allocate(required_bytes) : gs_default_allocator::allocate(required_bytes);
	memcpy_s(tmp_storage, required_bytes, m_storage, required_bytes);
	release_memory();
	m_storage = (ValueType*)tmp_storage;
	m_allocated_length = m_length = num_elements;
}

template<typename ValueType>
int64_t unsafe_vector<ValueType>::grow(size_t num_new_elements)
{
	int64_t elements_start_index = (int64_t)m_length;
	if (num_new_elements > 0) {
		apply_grow_policy(num_new_elements);
		resize(m_length + num_new_elements);
	}
	return elements_start_index;
}

template<typename ValueType>
void unsafe_vector<ValueType>::initialize(size_t num_elements, const ValueType& initial_value)
{
	if (m_length != num_elements)
		resize(num_elements, false);
	for (size_t i = 0; i < num_elements; ++i)
		m_storage[i] = initial_value;
}

template<typename ValueType>
void unsafe_vector<ValueType>::initialize(size_t num_elements, const ValueType* initial_values)
{
	if (m_length != num_elements)
		resize(num_elements, false);
	for (size_t i = 0; i < num_elements; ++i)
		m_storage[i] = initial_values[i];
}

template<typename ValueType>
template<typename IndexableType>
void unsafe_vector<ValueType>::initialize(size_t num_elements, const IndexableType& initial_values)
{
	if (m_length != num_elements)
		resize(num_elements, false);
	for (size_t i = 0; i < num_elements; ++i)
		m_storage[i] = initial_values[i];
}


template<typename ValueType>
void unsafe_vector<ValueType>::clear(bool free_memory)
{
	destruct_if_necessary(0);
	if (free_memory)
	{
		release_memory();
	}
	m_length = 0;
}

template<typename ValueType>
void unsafe_vector<ValueType>::release_memory()
{
	if (m_storage != nullptr)
	{
		if (m_external_allocator)
		{
			m_external_allocator->free((unsigned char*)m_storage);
		}
		else
		{
			gs_default_allocator::free((unsigned char*)m_storage);
		}

		m_storage = nullptr;
	}
	m_allocated_length = 0;
}

template<typename ValueType>
void unsafe_vector<ValueType>::default_construct_if_necessary(int start_index)
{
	if (std::is_trivially_copyable<ValueType>()) return;

	ValueType* cur = &m_storage[start_index];
	for (size_t k = start_index; k < m_allocated_length; ++k) {
		new (cur) ValueType();
		cur++;
	}
}

template<typename ValueType>
void unsafe_vector<ValueType>::destruct_if_necessary(int start_index)
{
	if (std::is_trivially_copyable<ValueType>()) return;

	ValueType* cur = &m_storage[start_index];
	for (size_t k = start_index; k < m_allocated_length; ++k) {
		(*cur).~ValueType();
		cur++;
	}
}


template<typename ValueType>
void unsafe_vector<ValueType>::apply_grow_policy(int64_t new_elements)
{
	// will run out of 
	size_t required_length = (m_length + (size_t)new_elements);
	if (required_length >= m_allocated_length)
	{
		//uint64_t next_pow_2 = (uint64_t)required_length;
		//next_pow_2--;
		//next_pow_2 |= next_pow_2 >> 1;
		//next_pow_2 |= next_pow_2 >> 2;
		//next_pow_2 |= next_pow_2 >> 4;
		//next_pow_2 |= next_pow_2 >> 8;
		//next_pow_2 |= next_pow_2 >> 16;
		//next_pow_2 |= next_pow_2 >> 32;
		//next_pow_2++;
		//uint64_t new_size = (uint64_t)1 << next_pow_2;
		uint64_t new_size = 
			(required_length <= 16) ? (2*required_length) : (3*required_length/2);

		gs_debug_assert(new_size > required_length);

		reserve(new_size);
	}
}

//
//template<typename ValueType>
//void unsafe_vector<ValueType>::move_allocated_values(unsigned char* from_buffer, unsigned char* to_buffer, int num_elements)
//{
//	if (std::is_pod<ValueType>())
//	{
//		memcpy(from_buffer, to_buffer, num_elements * sizeof(ValueType));
//	}
//	else
//	{
//		ValueType* from = reinterpret_cast<ValueType*>(from_buffer);
//		ValueType* to = reinterpret_cast<ValueType*>(to_buffer);
//		for (int k = 0; k < num_elements; ++k)
//		{
//			from[k] = std::move(to[k]);
//		}
//	}
//}




template<typename ValueType>
bool unsafe_vector<ValueType>::contains(const ValueType& Element) const
{
	for (size_t k = 0; k < m_length; ++k) {
		if (m_storage[k] == Element) return true;
	}
	return false;
}

template<typename ValueType>
int64_t unsafe_vector<ValueType>::index_of(const ValueType& Element) const
{
	for (size_t k = 0; k < m_length; ++k) {
		if (m_storage[k] == Element) return k;
	}
	return -1;
}

template<typename ValueType>
const_buffer_view<ValueType> unsafe_vector<ValueType>::get_view() const
{
	if (m_storage == nullptr)
		return const_buffer_view<ValueType>();
	return const_buffer_view<ValueType>(m_storage, m_length);
}

template<typename ValueType>
bool unsafe_vector<ValueType>::Store(GS::ISerializer& Serializer, const char* custom_key) const
{
	static constexpr uint32_t CurrentVersionNumber = 1;
	GS::SerializationVersion CurrentVersion(CurrentVersionNumber);
	bool bOK = Serializer.WriteVersion(SerializeVersionString(), CurrentVersion);

	bOK = bOK && Serializer.WriteValue("Length", m_length);
	if (m_length > 0)
	{
		const char* use_key = (custom_key != nullptr) ? custom_key : "Data";
		bOK = bOK && Serializer.WriteData(use_key, (const void*)m_storage, sizeof(ValueType) * m_length);
	}
	return bOK;
}

template<typename ValueType>
bool unsafe_vector<ValueType>::Restore(GS::ISerializer& Serializer, const char* custom_key)
{
	GS::SerializationVersion Version(0);
	bool bOK = Serializer.ReadVersion(SerializeVersionString(), Version);

	size_t length = 0;
	bOK = bOK && Serializer.ReadValue("Length", length);
	if (bOK && length == 0)
	{
		m_length = 0;
		m_allocated_length = 0;
		m_storage = nullptr;
		return true;
	}

	if (bOK)
	{
		resize(length);
		const char* use_key = (custom_key != nullptr) ? custom_key : "Data";
		bOK = Serializer.ReadData(use_key, sizeof(ValueType) * m_length, (void*)m_storage);
	}
	return bOK;
}



} // end namespace GS
