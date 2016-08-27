#pragma once

#include <halley/utils/utils.h>
#include <vector>
#include <gsl/gsl>
#include "halley/data_structures/flat_map.h"

namespace Halley {
	class String;

	class Serializer {
	public:
		Serializer();
		explicit Serializer(gsl::span<gsl::byte> dst);

		template <typename T>
		static Bytes toBytes(const T& value)
		{
			Serializer dry;
			value.serialize(dry);
			Bytes result(dry.getSize());
			Serializer s(gsl::as_writeable_bytes(gsl::span<Halley::Byte>(result)));
			value.serialize(dry);
			return result;
		}

		size_t getSize() const { return size; }

		Serializer& operator<<(bool val) { return serializePod(val); }
		Serializer& operator<<(char val) { return serializePod(val); }
		Serializer& operator<<(unsigned char val) { return serializePod(val); }
		Serializer& operator<<(short val) { return serializePod(val); }
		Serializer& operator<<(unsigned short val) { return serializePod(val); }
		Serializer& operator<<(int val) { return serializePod(val); }
		Serializer& operator<<(unsigned int val) { return serializePod(val); }
		Serializer& operator<<(long long val) { return serializePod(val); }
		Serializer& operator<<(size_t val) { return serializePod(val); }
		Serializer& operator<<(float val) { return serializePod(val); }
		Serializer& operator<<(double val) { return serializePod(val); }

		Serializer& operator<<(const std::string& str);
		Serializer& operator<<(const String& str);
		Serializer& operator<<(gsl::span<const gsl::byte> span);

		template <typename T>
		Serializer& operator<<(const std::vector<T>& val)
		{
			size_t sz = val.size();
			*this << sz;
			for (size_t i = 0; i < sz; i++) {
				*this << val[i];
			}
			return *this;
		}

		template <typename T, typename U>
		Serializer& operator<<(const FlatMap<T, U>& val)
		{
			size_t sz = val.size();
			*this << sz;
			for (auto& kv : val) {
				*this << kv.first;
				*this << kv.second;
			}
			return *this;
		}

		template <typename T>
		Serializer& operator<<(const T& val)
		{
			val.serialize(*this);
			return *this;
		}

	private:
		bool dryRun;
		size_t size = 0;
		gsl::span<gsl::byte> dst;

		template <typename T>
		Serializer& serializePod(T val)
		{
			if (!dryRun) {
				memcpy(dst.data() + size, &val, sizeof(T));
			}
			size += sizeof(T);
			return *this;
		}
	};

	class Deserializer {
	public:
		Deserializer(gsl::span<const gsl::byte> src);

		Deserializer& operator>>(bool& val) { return deserializePod(val); }
		Deserializer& operator>>(char& val) { return deserializePod(val); }
		Deserializer& operator>>(unsigned char& val) { return deserializePod(val); }
		Deserializer& operator>>(short& val) { return deserializePod(val); }
		Deserializer& operator>>(unsigned short& val) { return deserializePod(val); }
		Deserializer& operator>>(int& val) { return deserializePod(val); }
		Deserializer& operator>>(unsigned int& val) { return deserializePod(val); }
		Deserializer& operator>>(long long& val) { return deserializePod(val); }
		Deserializer& operator>>(size_t& val) { return deserializePod(val); }
		Deserializer& operator>>(float& val) { return deserializePod(val); }
		Deserializer& operator>>(double& val) { return deserializePod(val); }

		Deserializer& operator>>(std::string& str);
		Deserializer& operator>>(gsl::span<gsl::byte>& span);

		template <typename T>
		Deserializer& operator>>(std::vector<T>& val)
		{
			size_t sz;
			*this >> sz;
			val.clear();
			val.reserve(sz);
			for (size_t i = 0; i < sz; i++) {
				val.push_back(T());
				*this >> val[i];
			}
			return *this;
		}

	private:
		size_t pos = 0;
		gsl::span<const gsl::byte> src;

		template <typename T>
		Deserializer& deserializePod(T& val)
		{
			memcpy(&val, src.data() + pos, sizeof(T));
			pos += sizeof(T);
			return *this;
		}
	};
}
