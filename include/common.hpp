#ifndef ELF_DECODER_COMMON_H
#define ELF_DECODER_COMMON_H
#include <iostream>
#include <stdlib.h>
#include <cstring>
#include <stdint.h>

#ifdef NULL
#undef NULL
#endif

#define NULL			(uint8_t) 0

#define ELF_ASSERT(cond, msg, ...)              \
if(!(cond))										\
{												\
	fprintf(stderr, msg, ##__VA_ARGS__);		\
	exit(EXIT_FAILURE);							\
}

#define ELF_LOG(with_error, msg, ...)			\
{												\
	fprintf(stdout, msg, ##__VA_ARGS__);		\
	if(with_error) exit(EXIT_FAILURE);			\
}

/* Parts of the ELF binary. */
enum class ELF_parts: uint8_t
{
	ELF_Header		= 0x0,
	ELF_PHeader 	= 0x1,
	ELF_SectionD	= 0x2, 		/* Section Data - part of segments */
	ELF_SectionHT	= 0x3,		/* Section Header Table */
};

template<typename T>
	requires (std::is_same<T, uint16_t>::value
		|| std::is_same<T, uint32_t>::value)
		&& (!std::is_same<T, uint8_t>::value)
T revert_value(T value)
{
	T old_value = value;
	value ^= value;

	switch(sizeof(T))
	{
		case 2: {
			value |= (value << 0) | ((old_value >> 0) & 0xFF);
			value = (value << 8) | ((old_value >> 8) & 0xFF);
			break;
		}
		case 4: {
			value |= (value << 0) | ((old_value >> 0) & 0xFF);
			value = (value << 8) | ((old_value >> 8) & 0xFF);
			value = (value << 8) | ((old_value >> 16) & 0xFF);
			value = (value << 8) | ((old_value >> 24) & 0xFF);
			break;
		}
		default: break;
	}

	return (T) value;
}

/* Common functionality to be found in each "step" of decoding the ELF binary file. */
class ElfDecoder
{
public:
	/* Only used for `ELF_read_binary`. */
	struct placeholder
	{
		placeholder() = default;

		~placeholder() = default;
	};
	struct placeholder *pholder = nullptr;

protected:
	FILE *bin = nullptr;
	uint8_t *ELF_binary = nullptr;
	size_t seek_pos;
	size_t backup_seek_pos;

public:
	template<typename T = placeholder>
		requires std::is_class<T>::value
	uint8_t ELF_read_binary(uint8_t &bytes, uint8_t *data, T &structure)
	{
		/* Will only read, max, 4 bytes at a time. */
		if(bytes > 4)
			bytes = 4;
			
		/* Keep track of how many bytes we are supposed to read in. */
		uint8_t bytes_to_read = bytes;

		if(bytes == NULL)
		{
			/* `nullptr` check. */
			ELF_ASSERT(!(std::is_same<T, placeholder>::value),
				"\nCannot read memory into a nullptr.\n")

			fseek(bin, seek_pos, SEEK_SET);
			ELF_ASSERT(fread(&structure, 1, sizeof(T), bin) == sizeof(T),
				"\nThere was an error reading in the needed %lX (%ld) bytes.\n",
				sizeof(T), sizeof(T))
				
			return NULL;
		}

		/* Make sure `data` is not `nullptr`. */
		ELF_ASSERT(data != nullptr,
			"\nCannot read memory into a nullptr.\n")

		/* Zero out all memory. */
		memset(data, 0, bytes);

		/* Save the "last state" of the previous seek position. */
		backup_seek_pos = seek_pos;

		while(bytes > 0)
		{
			data[bytes] = ELF_binary[seek_pos];
				
			seek_pos++;
			bytes--;
		}

		/* If we are reading in only a single byte, go ahead and make sure the first index stores the value that gets read in. */
		if(seek_pos - backup_seek_pos == 1)
		{
			data[0] = data[1];
			data[1] = 0;
		}

		/* Make sure we read in the correct amount. */
		ELF_ASSERT(seek_pos - backup_seek_pos == bytes_to_read,
			"\nA very unknown error ocurred where the seek position did not increment by %d bytes.\n",
			bytes)
		return bytes_to_read;
	}

	/* Since the above function reads in the data backwards,
	 * this function will "correct" the data.
	 *
	 * With values other than the 4-byte magic number, this will create
	 * the wrong value. For example, we can read in a value of `0x6`, and
	 * if we wanted to convert it to a 32-bit number via this function
	 * it will be `0x60000000`. I would fix this, however, the following
	 * code works miraculously with only the 4-byte magic number
	 * at the beginning of the ELF file. With that, the following code
	 * is "as-is" and the function `revert_value` exists to correct
	 * all values this function returns except the 4-byte magic number.
	 *
	 * Old Requires:
	 *	requires (std::is_same<T, uint16_t>::value
	 *		|| std::is_same<T, uint32_t>::value)
	 *		&& (!std::is_same<T, uint8_t>::value)
	 *
	 * */
	template<typename T>
		requires (std::is_integral<T>::value)
			&& (!std::is_same<T, uint8_t>::value)
	T make_into_complete_value(uint8_t bytes, uint8_t *data)
	{
		T complete_value = 0;
			
		complete_value |= data[sizeof(T)] & 0xFF;
		complete_value = (complete_value << 8) | (data[sizeof(T) - 1] & 0xFF);

		if(sizeof(T) == 2) goto end;

		complete_value = (complete_value << 8) | (data[2] & 0xFF);
		complete_value = (complete_value << 8) | (data[1] & 0xFF);

		end:
		return (T) complete_value;
	}

	template<typename T>
		requires std::is_integral<T>::value
			&& (!std::is_class<T>::value)
	void check_data(T data, T data_to_expect)
	{
		ELF_ASSERT(data == data_to_expect,
			"\nThe data retained (%X) did not match what was expected (%X)\n",
			data, data_to_expect)
	}

	ElfDecoder(FILE *f)
		: bin(f), seek_pos(0), backup_seek_pos(0)
	{
		ELF_ASSERT(bin,
			"\nThe ELF binary file does not exist.\n")
		
		fseek(bin, 0, SEEK_END);
		size_t size = ftell(bin);
		fseek(bin, 0, SEEK_SET);

		ELF_binary = new uint8_t[size];
		fread(ELF_binary, size, sizeof(*ELF_binary), bin);
	}

	template<typename T>
			requires std::is_same<T, ElfDecoder *>::value
		void delete_instance(T instance)
		{
			if(instance)
				delete instance;
			instance = nullptr;
		}

	~ElfDecoder()
	{
		if(ELF_binary) delete ELF_binary;
		ELF_binary = nullptr;
	}
};

#endif