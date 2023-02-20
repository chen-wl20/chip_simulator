#include "Prim.h"

PrimPacket::PrimPacket()
{
	type_name = "Null";
	this->prim_id_code = 0;
}

PrimPacket::~PrimPacket()
{

}

PrimPacket& PrimPacket::operator=(const PrimPacket& rhs)
{
	this->type_name = rhs.type_name;
	this->prim_id_code = rhs.prim_id_code;
	return *this;
}

bool PrimPacket::operator==(const PrimPacket& rhs) const
{
	return this->prim_id_code == rhs.prim_id_code;
}

ostream& operator<<(ostream& file, const PrimPacket& prim)
{
	file << "Prim code: " << prim.prim_id_code << std::endl;
	return file;
}

void sc_trace(sc_trace_file*& tf, const PrimPacket& prim, std::string nm)
{
	sc_trace(tf, prim.prim_id_code, nm + ".id_code");
}

sc_bv<PORT_WIDTH> convertPrim2Code(const PrimPacket& p)
{
	return p.prim_id_code;
}
