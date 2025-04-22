// Copyright Gradientspace Corp. All Rights Reserved.
#pragma once

#include "GradientspacePlatform.h"
#include "Core/dynamic_buffer.h"
#include "Core/gs_serializer.h"

namespace GS
{

template<typename ValueType>
class rle_buffer
{
private:
	dynamic_buffer<ValueType> m_buffer;
	size_t m_length = 0;
	ValueType m_constant;
	bool m_valid_constant = true;

public:
	rle_buffer(ValueType constant_value);
	explicit rle_buffer(ValueType constant_value, gs_allocator* use_allocator);
	rle_buffer(const rle_buffer& copy);
	rle_buffer(rle_buffer&& moved);
	rle_buffer& operator=(const rle_buffer& copy);

	size_t size() const;

	void clear();
	void resize(size_t num_elements, bool preserve_data = false);
	void resize_init(size_t num_elements, const ValueType& initial_value, bool preserve_data = false);

	const ValueType& operator[](int64_t index) const;

	const ValueType& get_value(int64_t index) const;
	void set_value(int64_t index, const ValueType& NewValue);

	bool Store(GS::ISerializer& Serializer, const char* custom_key = nullptr) const;
	bool Restore(GS::ISerializer& Serializer, const char* custom_key = nullptr);
	constexpr const char* SerializeVersionString() const { return "rle_buffer_Version"; }

private:
	void expand_buffer();
};



template<typename ValueType>
rle_buffer<ValueType>::rle_buffer(ValueType constant_value)
	: m_constant(constant_value)
{
}

template<typename ValueType>
rle_buffer<ValueType>::rle_buffer(ValueType constant_value, gs_allocator* use_allocator)
	: m_constant(constant_value),
	  m_buffer(use_allocator)
{

}

template<typename ValueType>
rle_buffer<ValueType>::rle_buffer(const rle_buffer& copy)
{
	*this = copy;
}

template<typename ValueType>
rle_buffer<ValueType>::rle_buffer(rle_buffer&& moved)
{
	m_buffer = std::move(moved.m_buffer);
	m_length = moved.m_length;
	m_constant = moved.m_constant;
	m_valid_constant = moved.m_valid_constant;

	moved.m_length = 0;
}


template<typename ValueType>
rle_buffer<ValueType>& rle_buffer<ValueType>::operator=(const rle_buffer& copy)
{
	m_buffer = copy.m_buffer;
	m_length = copy.m_length;
	m_constant = copy.m_constant;
	m_valid_constant = copy.m_valid_constant;
	return *this;
}


template<typename ValueType>
size_t rle_buffer<ValueType>::size() const
{
	return m_length;
}


template<typename ValueType>
const ValueType& rle_buffer<ValueType>::operator[](int64_t index) const
{
	return (m_valid_constant) ? m_constant : m_buffer[index];
}

template<typename ValueType>
const ValueType& rle_buffer<ValueType>::get_value(int64_t index) const
{
	return (m_valid_constant) ? m_constant : m_buffer.get_value(index);
}

template<typename ValueType>
void rle_buffer<ValueType>::set_value(int64_t index, const ValueType& new_value)
{
	if (m_valid_constant)
	{
		if (new_value == m_constant) return;
		expand_buffer();
	}

	m_buffer.set_value(index, new_value);
}



template<typename ValueType>
void rle_buffer<ValueType>::resize(size_t num_elements, bool preserve_data)
{
	m_length = num_elements;

	if (m_valid_constant)
	{
		return;
	}

	m_buffer.resize(num_elements, preserve_data);
}

template<typename ValueType>
void rle_buffer<ValueType>::resize_init(size_t num_elements, const ValueType& initial_value, bool preserve_data)
{
	if (m_valid_constant)
	{
		if (preserve_data && m_constant != initial_value)
		{
			gs_debug_assert(false);		// todo: implement this case
		}

		m_constant = initial_value;
		m_length = num_elements;
		return;
	}

	m_buffer.resize_init(num_elements, initial_value, preserve_data);

}


template<typename ValueType>
void rle_buffer<ValueType>::clear()
{
	if (m_valid_constant)
	{
		return;
	}
	m_buffer.clear();

	m_valid_constant = true;
}


template<typename ValueType>
void rle_buffer<ValueType>::expand_buffer()
{
	gs_debug_assert(m_valid_constant);
	gs_debug_assert(m_buffer.size() == 0);

	m_buffer.resize_init(m_length, m_constant);
	m_valid_constant = false;
}


template<typename ValueType>
bool rle_buffer<ValueType>::Store(GS::ISerializer& Serializer, const char* custom_key) const
{
	static constexpr uint32_t CurrentVersionNumber = 1;
	GS::SerializationVersion CurrentVersion(CurrentVersionNumber);
	bool bOK = Serializer.WriteVersion(SerializeVersionString(), CurrentVersion);

	bOK = bOK && Serializer.WriteBoolean("bValidConstant", m_valid_constant);
	bOK = bOK && Serializer.WriteValue<ValueType>("ConstantValue", m_constant);
	if (m_valid_constant)
	{
		bOK = bOK && Serializer.WriteValue<size_t>("Length", m_length);
	}
	else
	{
		bOK = bOK && m_buffer.Store(Serializer, custom_key);
	}
	return bOK;
}

template<typename ValueType>
bool rle_buffer<ValueType>::Restore(GS::ISerializer& Serializer, const char* custom_key)
{
	GS::SerializationVersion Version;
	bool bOK = Serializer.ReadVersion(SerializeVersionString(), Version);

	bOK = bOK && Serializer.ReadBoolean("bValidConstant", m_valid_constant);
	bOK = bOK && Serializer.ReadValue<ValueType>("ConstantValue", m_constant);
	if (m_valid_constant)
	{
		bOK = bOK && Serializer.ReadValue<size_t>("Length", m_length);
	}
	else
	{
		bOK = bOK && m_buffer.Restore(Serializer, custom_key);
		m_length = m_buffer.size();
	}
	return bOK;
}

} // end namespace GS
