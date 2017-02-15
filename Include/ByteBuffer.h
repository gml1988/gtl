#pragma once

#include "Vector.h"
#include <assert.h>
#include <stdint.h>
#include <crtdefs.h>

namespace GTL
{
	class ByteBuffer
	{
	public:
		ByteBuffer() : _nReadPos(0)
		{
			
		}

		~ByteBuffer()
		{
			_vecStorage.Clear();
		}

		template <class T>
		void Write(T const &obj)
		{
			Write((uint8_t*)(&obj), sizeof(obj));
		}

		void Write(uint8_t const *pSrcBuf, size_t nCount)
		{
			_vecStorage.Insert(_vecStorage.End(), pSrcBuf, pSrcBuf + nCount);
		}

		template <class T>
		T Read()
		{
			T value = Read<T>(_nReadPos);
			_nReadPos += sizeof(T);
			return value;
		}

		template <class T>
		T Read(size_t nPos) const
		{
			assert(nPos + sizeof(T) <= _vecStorage.Size());
			return *((T const *)(&_vecStorage[nPos]));
		}

		template <class T>
		ByteBuffer &operator<<(T value)
		{
			Write<T>(value);
			return *this;
		}

		ByteBuffer &operator<<(bool value)
		{
			Write<char>((char)value);
			return *this;
		}

		ByteBuffer &operator<<(char const *psz)
		{
			Write((uint8_t const *)psz, psz ? strlen(psz) : 0);
			Write<uint8_t>(0);
			return *this;
		}

		ByteBuffer &operator<<(std::string const &value)
		{
			Write((uint8_t const *)value.c_str(), value.length());
			Write<uint8_t>(0);
			return *this;
		}

		ByteBuffer &operator<<(std::wstring const &value)
		{
			for (size_t nIndex = 0; nIndex < value.length(); ++nIndex)
			{
				Write<uint16_t>(value[nIndex]);
			}
			Write<uint16_t>(0);
			return *this;
		}

		template <class T>
		ByteBuffer &operator>>(T &value)
		{
			value = Read<T>();
			return *this;
		}

		ByteBuffer &operator>>(bool &value)
		{
			value = Read<char>() > 0 ? true : false;
			return *this;
		}

		ByteBuffer &operator>>(std::string &value)
		{
			return *this;
		}

		ByteBuffer &operator>>(std::wstring &value)
		{
			return *this;
		}

		size_t Size() const
		{
			return _vecStorage.Size() - _nReadPos;
		}

		void Clear()
		{
			_nReadPos = 0;
			_vecStorage.Clear();
		}

	protected:
		size_t _nReadPos;
		Vector<uint8_t> _vecStorage;
	};
}