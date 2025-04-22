// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

#include "GradientspacePlatform.h"
#include "Core/gs_debug.h"

namespace GS
{

/**
 * minimal version of something like a range
 */
template<typename ValueType>
class const_buffer_view
{
protected:
	const ValueType* m_buffer = nullptr;
	size_t m_length = 0;
public:
	const_buffer_view()
		: m_buffer(nullptr), m_length(0)
	{
	}

	const_buffer_view(const ValueType* buffer, size_t length)
		: m_buffer(buffer), m_length(length)
	{
		gs_debug_assert(buffer != nullptr);
	}

	~const_buffer_view() {
		m_buffer = nullptr;
		m_length = 0;
	}

	const_buffer_view(const const_buffer_view& copy) = default;
	const_buffer_view& operator=(const const_buffer_view& copy) = default;

	const_buffer_view(const_buffer_view&& moved) = delete;
	const_buffer_view& operator=(const_buffer_view&& moved) = delete;

	size_t size() const { return m_length; }
	bool is_empty() const { return m_buffer == nullptr || m_length == 0; }
	bool is_valid() const { return m_buffer != nullptr; }

	template<typename IndexType>
	const ValueType& operator[](IndexType index) const {
		static_assert(std::is_integral_v<IndexType> == true);
		return m_buffer[index];
	}

	bool contains(const ValueType& Element) const {
		for (size_t k = 0; k < m_length; ++k) 
			if (m_buffer[k] == Element) 
				return true;
		return false;
	}
	int64_t index_of(const ValueType& Element) const {
		for (size_t k = 0; k < m_length; ++k)
			if (m_buffer[k] == Element)
				return k;
		return -1;
	}



// iterators

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

	const_iterator begin() const { return const_iterator(m_buffer); }
	const_iterator end() const { return const_iterator(&m_buffer[m_length]); }

};


} // end namespace GS
