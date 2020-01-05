#include "libsnark/gadgetlib1/gadget.hpp"
#include <libsnark/gadgetlib1/gadgets/hashes/sha256/sha256_gadget.hpp>

using namespace libsnark;
using namespace std;
template<typename FieldT>

/*A gadget with the following specifications-

    input- b(existing balances), x(amount to be spend), out(amouunt remaining), hx(hash of value x)
    constrains- 1)hx==sha256(x)
                2)b-x-out==0
                3)
    

*/
class gadget_model: public gadget<FieldT> {
private:
  pb_variable<FieldT> sym_1;
  digest_variable<FieldT> hash_bits;
  digest_variable<FieldT> left_bits;
  digest_variable<FieldT> right_bits;
  multipacking_gadget<FieldT> packer;
  sha256_two_to_one_hash_gadget<FieldT> hasher;

public:
  const pb_variable<FieldT> out;
  const pb_variable<FieldT> x;
  const pb_variable<FieldT> b;
  const pb_variable_array<FieldT> bv;



    

  gadget_model(protoboard<FieldT> &pb,
              const pb_variable<FieldT> &out,
              const pb_variable<FieldT> &x,
              const pb_variable<FieldT> &b) : 
    gadget<FieldT>(pb, "poly_gadget"), out(out), x(x), b(b)
  {
    // Allocate variables to protoboard
    // The strings (like "x") are only for debugging purposes
	  
    sym_1.allocate(this->pb, "sym_1"); 
    // y.allocate(this->pb, "y");
    // sym_2.allocate(this->pb, "sym_2");
    digest_variable<FieldT> hash_bits(this->pb, SHA256_digest_size, "hash_bits");
    digest_variable<FieldT> left_bits(this->pb, SHA256_digest_size, "left_bits");
    digest_variable<FieldT> right_bits(this->pb, SHA256_digest_size, "right_bits");
    multipacking_gadget<FieldT> packer(this->pb, hash_bits.bits, bv, 128, "packer");
    sha256_two_to_one_hash_gadget<FieldT> hasher(this->pb, left_bits, right_bits, hash_bits, "hash_gadget");
  }

  void generate_r1cs_constraints()
  {

    
    packer.generate_r1cs_constraints(true);

    hasher.generate_r1cs_constraints();


    
    // x_digest.digest_size(SHA256_digest_size);
    // cout<<"Sym2 size is"<<x_digest.bits;
   // sha256_two_to_one_hash_gadget<FieldT> hasher(this->pb, left_bits, right_bits, hash_bits, "hash_gadget");




    this->pb.add_r1cs_constraint(r1cs_constraint<FieldT>(b-x, 1, sym_1));


    //sym1 - out = 0
    this->pb.add_r1cs_constraint(r1cs_constraint<FieldT>(sym_1-out, 1, 0));
    cout<<"out is "<<this->pb.val(out)<<endl;
    cout<<"x is "<<this->pb.val(x)<<endl;
    cout<<"b is "<<this->pb.val(b)<<endl;

  }

  void generate_r1cs_witness()
  {

    const libff::bit_vector left_bv  = libff::int_list_to_bits({0x426bc2d8, 0x4dc86782, 0x81e8957a, 0x409ec148, 0xe6cffbe8, 0xafe6ba4f, 0x9c6f1978, 0xdd7af7e9}, 32);
    const libff::bit_vector right_bv = libff::int_list_to_bits({0x038cce42, 0xabd366b8, 0x3ede7e00, 0x9130de53, 0x72cdf73d, 0xee825114, 0x8cb48d1b, 0x9af68ad0}, 32);
    const libff::bit_vector hash_bv  = libff::int_list_to_bits({0xeffd0b7f, 0x1ccba116, 0x2ee816f7, 0x31c62b48, 0x59305141, 0x990e5c0a, 0xce40d33d, 0x0b1167d1}, 32);
    left_bits.generate_r1cs_witness(left_bv);
    right_bits.generate_r1cs_witness(right_bv);
    hasher.generate_r1cs_witness();
    packer.generate_r1cs_witness_from_bits();
    this->pb.val(sym_1) = this->pb.val(b) - this->pb.val(x);

  }
};
