#include "Cdecode.h"
#include "Prim.h"


void Cdecode::decode_prim() {
	while (true) {
		if (reset.read()) {

			this->prim_type_code.write(0b0000);
			mem_prim_out.write(Prim_Mem());
			memristor_prim_out.write(Prim_Memristor());


		}
		else {
			sc_bv<PORT_WIDTH> prim_temp = this->prim_code_in.read();
			short type_code = prim_temp.range(3, 0).to_uint();
			switch (type_code)
			{
			case 0b0010:
			{
				this->mem_prim_out.write(
					Prim_Load(
						prim_temp.range(19, 4).to_uint(),
						prim_temp.range(35, 20).to_uint(),
						prim_temp.range(115, 84).to_uint()
					)
				);
				this->prim_type_code.write(0b0010);

				break;
			}

			case 0b0011:
			{
				this->mem_prim_out.write(
					Prim_Store(
						prim_temp.range(19, 4).to_uint(),
						prim_temp.range(35, 20).to_uint(),
						prim_temp.range(115, 84).to_uint()
					)
				);
				this->prim_type_code.write(0b0011);

				break;
			}

			case 0b0001:
			{
				this->ctrl_prim_out.write(
					Prim_Jump(
						prim_temp.range(67, 52).to_int()
					)
				);
				this->prim_type_code.write(0b0001);
				break;
			}

			case 0b1000:
			{
				this->memristor_prim_out.write(
					Prim_Memristor(
						prim_temp.range(19, 4).to_uint(),
						prim_temp.range(35, 20).to_uint()
					)
				);
				this->prim_type_code.write(0b1000);

				break;
			}
			case 0b0101:
			{
				this->matrix_mul_prim_out.write(
					Prim_Matrix_Mul(
						prim_temp.range(19, 4).to_uint(),
						prim_temp.range(35, 20).to_uint(),
						prim_temp.range(51, 36).to_uint(),
						prim_temp.range(67, 52).to_uint(),
						prim_temp.range(83, 68).to_uint(),
						prim_temp.range(99, 84).to_uint(),
						prim_temp.range(115, 100).to_uint()
					)
				);
				this->prim_type_code.write(0b0101);
				break;
			}
			case 0b0100:
			{
				this->relu_prim_out.write(
					Prim_ReLU(
						prim_temp.range(19, 4).to_uint(),
						prim_temp.range(35, 20).to_uint(),
						prim_temp.range(67, 52).to_uint(),
						prim_temp.range(115, 100).to_uint()
					)
				);
				this->prim_type_code.write(0b0100);
				break;
			}
			case 0b0110:
			{
				this->conv2d_prim_out.write(
					Prim_Conv2d(
						prim_temp.range(19, 4).to_uint(),
						prim_temp.range(35, 20).to_uint(),
						prim_temp.range(51, 36).to_uint(),
						prim_temp.range(67, 52).to_uint(),
						prim_temp.range(72, 68).to_uint(),
						prim_temp.range(77, 73).to_uint(),
						prim_temp.range(80, 78).to_uint(),
						prim_temp.range(81, 81).to_uint(),
						prim_temp.range(82, 82).to_uint(),
						prim_temp.range(83, 83).to_uint(),
						prim_temp.range(99, 84).to_uint(),
						prim_temp.range(115, 100).to_uint()
					)
				);
				this->prim_type_code.write(0b0110);
				break;
			}
			case 0b0111:
			{
				this->pooling_prim_out.write(
					Prim_Pooling(
						prim_temp.range(19, 4).to_uint(),
						prim_temp.range(35, 20).to_uint(),
						prim_temp.range(51, 36).to_uint(),
						prim_temp.range(72, 68).to_uint(),
						prim_temp.range(77, 73).to_uint(),
						prim_temp.range(80, 78).to_uint(),
						prim_temp.range(81, 81).to_uint(),
						prim_temp.range(82, 82).to_uint(),
						prim_temp.range(83, 83).to_uint(),
						prim_temp.range(115, 100).to_uint()
					)
				);
				this->prim_type_code.write(0b0111);
				break;
			}

			default:

				this->prim_type_code.write(0b0000);
				mem_prim_out.write(Prim_Mem());
				memristor_prim_out.write(Prim_Memristor());
				matrix_mul_prim_out.write(Prim_Matrix_Mul());
				relu_prim_out.write(Prim_ReLU());
				conv2d_prim_out.write(Prim_Conv2d());
				pooling_prim_out.write(Prim_Pooling());
				break;
			}
		}
		wait();
	}
}


Cdecode::~Cdecode()
{
}
