#ifndef _MEM_IF_H
#define _MEM_IF_H
#include "systemc.h"

enum transfer_status {TRANSFER_OK = 0, TRANSFER_ERROR};

template<class T>
class mem_read_if : virtual public sc_interface {
public:
	virtual transfer_status read(unsigned int address, T& data) = 0;
};

template<class T>
class mem_write_if : virtual public sc_interface {
public:
	virtual transfer_status write(unsigned int address, T& data) = 0;
};

class reset_if : virtual public sc_interface
{
public:
	virtual bool reset() = 0;
};

template<class T>
class ram_if : public mem_write_if<T>, public mem_read_if<T>, public reset_if {
public:
	virtual unsigned int start_address() const = 0;
	virtual unsigned int end_address() const = 0;
};


template<class T>
class rom_if : public mem_read_if<T>, public reset_if {
public:
	virtual unsigned int start_address() const = 0;
	virtual unsigned int end_address() const = 0;
};

template<class T>
class burst_ram_if : public ram_if<T> {
public:
	virtual transfer_status burst_read(unsigned int address, unsigned int length, T* &data) = 0;
	virtual transfer_status burst_write(unsigned int address, unsigned int length, T* &data) = 0;
};


#endif

