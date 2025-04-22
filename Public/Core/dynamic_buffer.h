// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

#include "GradientspacePlatform.h"
#include "Core/gs_debug.h"
#include "Core/gs_allocator.h"
#include "Core/gs_serializer.h"
#include "Core/buffer_view.h"


// TODO need 64-bit int...

namespace GS
{

/**
 * Simple array-like class, only supports POD types
 * No default-initialization
 * Buffer can be resized, but does not support add/etc (no concept of slack/reserve)
 */
template<typename ValueType>
class dynamic_buffer
{
private:
	ValueType* m_storage = nullptr;
	size_t m_length = 0;
	gs_allocator* m_external_allocator = nullptr;

public:
	~dynamic_buffer();
	dynamic_buffer();
	explicit dynamic_buffer(gs_allocator* use_allocator);
	dynamic_buffer(const dynamic_buffer& copy);
	dynamic_buffer(dynamic_buffer&& moved);
	dynamic_buffer& operator=(const dynamic_buffer& copy);
	dynamic_buffer& operator=(dynamic_buffer&& copy);

	size_t size() const;

	void clear();
	void resize(size_t num_elements, bool preserve_data = false);
	void resize_init(size_t num_elements, const ValueType& initial_value, bool preserve_data = false);

	ValueType& operator[](int64_t index);
	const ValueType& operator[](int64_t index) const;

	const ValueType& get_value(int64_t index) const;
	ValueType& get_value_ref(int64_t index);
	void set_value(int64_t index, const ValueType& NewValue);

	ValueType& last();
	const ValueType& last() const;

	ValueType* raw_pointer();

	const_buffer_view<ValueType> get_view() const;

	bool Store(GS::ISerializer& Serializer, const char* custom_key = nullptr) const;
	bool Restore(GS::ISerializer& Serializer, const char* custom_key = nullptr);
	constexpr const char* SerializeVersionString() const { return "dynamic_buffer_Version"; }

private:
	void release_memory();


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

	const_iterator begin() const { return const_iterator(m_storage); }
	const_iterator end() const { return const_iterator(&m_storage[m_length]); }
};

template<typename ValueType>
dynamic_buffer<ValueType>::~dynamic_buffer()
{
	clear();
}

template<typename ValueType>
dynamic_buffer<ValueType>::dynamic_buffer()
{
	static_assert(sizeof(size_t) == 8, "64-bit size_t is required");
	//static_assert(std::is_default_constructible_v<ValueType>, "Data structure requires default-constructible elements");
	//static_assert(std::is_copy_constructible_v<ValueType>, "requires copying");
	//static_assert(std::is_nothrow_copy_constructible_v<ValueType> && std::is_nothrow_copy_assignable_v<ValueType>, "requires nothrow copy/assign");
}

template<typename ValueType>
dynamic_buffer<ValueType>::dynamic_buffer(gs_allocator* use_allocator)
	: m_external_allocator(use_allocator)
{

}

template<typename ValueType>
dynamic_buffer<ValueType>::dynamic_buffer(const dynamic_buffer& copy)
{
	*this = copy;
}

template<typename ValueType>
dynamic_buffer<ValueType>::dynamic_buffer(dynamic_buffer&& moved)
{
	m_external_allocator = moved.m_external_allocator;
	m_length = moved.m_length;
	m_storage = moved.m_storage;

	moved.m_length = 0;
	moved.m_storage = nullptr;
}


template<typename ValueType>
dynamic_buffer<ValueType>& dynamic_buffer<ValueType>::operator=(const dynamic_buffer& copy)
{
	resize(copy.size());
	memcpy((void*)m_storage, (void*)copy.m_storage, m_length * sizeof(ValueType));
	return *this;
}

template<typename ValueType>
dynamic_buffer<ValueType>& dynamic_buffer<ValueType>::operator=(dynamic_buffer&& moved)
{
	m_external_allocator = moved.m_external_allocator;
	m_length = moved.m_length;
	m_storage = moved.m_storage;

	moved.m_length = 0;
	moved.m_storage = nullptr;
	return *this;
}


template<typename ValueType>
size_t dynamic_buffer<ValueType>::size() const
{
	return m_length;
}


template<typename ValueType>
ValueType& dynamic_buffer<ValueType>::operator[](int64_t index)
{
	return m_storage[index];
}

template<typename ValueType>
const ValueType& dynamic_buffer<ValueType>::operator[](int64_t index) const
{
	return m_storage[index];
}

template<typename ValueType>
const ValueType& dynamic_buffer<ValueType>::get_value(int64_t index) const
{
	return m_storage[index];
}

template<typename ValueType>
ValueType& dynamic_buffer<ValueType>::get_value_ref(int64_t index)
{
	return m_storage[index];
}


template<typename ValueType>
void dynamic_buffer<ValueType>::set_value(int64_t index, const ValueType& new_value)
{
	m_storage[index] = new_value;
}


template<typename ValueType>
ValueType& dynamic_buffer<ValueType>::last()
{
	return m_storage[m_length-1];
}
template<typename ValueType>
const ValueType& dynamic_buffer<ValueType>::last() const
{
	return m_storage[m_length-1];
}




template<typename ValueType>
ValueType* dynamic_buffer<ValueType>::raw_pointer()
{
	return m_storage;
}


template<typename ValueType>
void dynamic_buffer<ValueType>::resize(size_t num_elements, bool preserve_data)
{
	if (num_elements == m_length) return;

	if (num_elements == 0)
	{
		clear();
		return;
	}

	size_t required_bytes = num_elements * sizeof(ValueType);
	unsigned char* new_storage = (m_external_allocator) ? m_external_allocator->allocate(required_bytes) : gs_default_allocator::allocate(required_bytes);
	gs_runtime_assert(new_storage != nullptr); // fatal

	if (preserve_data && m_length > 0)
	{
		size_t num_to_copy = (num_elements < m_length) ? num_elements : m_length;
		memcpy( (void*)new_storage, (void*)m_storage, num_to_copy * sizeof(ValueType) ) ;
	}

	clear();

	m_storage = reinterpret_cast<ValueType*>(new_storage);
	m_length = num_elements;
}

template<typename ValueType>
void dynamic_buffer<ValueType>::resize_init(size_t num_elements, const ValueType& initial_value, bool preserve_data)
{
	size_t previous_length = m_length;
	resize(num_elements, preserve_data);

	size_t start_index = (preserve_data) ? previous_length : 0;
	for (size_t k = start_index; k < m_length; ++k)
	{
		m_storage[k] = initial_value;
	}
}


template<typename ValueType>
void dynamic_buffer<ValueType>::clear()
{
	release_memory();
}

template<typename ValueType>
void dynamic_buffer<ValueType>::release_memory()
{
	if (m_storage != nullptr)
	{
		if (m_external_allocator)
		{
			m_external_allocator->free( reinterpret_cast<unsigned char*>(m_storage) );
		}
		else
		{
			gs_default_allocator::free( reinterpret_cast<unsigned char*>(m_storage) );
		}

		m_storage = nullptr;
	}
	m_length = 0;
}


template<typename ValueType>
const_buffer_view<ValueType> dynamic_buffer<ValueType>::get_view() const
{
	if (m_storage == nullptr)
		return const_buffer_view<ValueType>();
	return const_buffer_view<ValueType>(m_storage, m_length);
}


template<typename ValueType>
bool dynamic_buffer<ValueType>::Store(GS::ISerializer& Serializer, const char* custom_key) const
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
bool dynamic_buffer<ValueType>::Restore(GS::ISerializer& Serializer, const char* custom_key)
{
	GS::SerializationVersion Version(0);
	bool bOK = Serializer.ReadVersion(SerializeVersionString(), Version);

	size_t length = 0;
	bOK = bOK && Serializer.ReadValue("Length", length);
	if (bOK && length == 0)
	{
		m_length = 0;
		m_storage = nullptr;
		return true;
	}

	if (bOK)
	{
		resize(length);
		const char* use_key = (custom_key != nullptr) ? custom_key : "Data";
		bOK = Serializer.ReadData(use_key, sizeof(ValueType) * m_length , (void*)m_storage);
	}
	return bOK;
}


} // end namespace GS
