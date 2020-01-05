#include <stdlib.h>
#include <iostream>
#include "libff/algebra/fields/field_utils.hpp"
#include "libsnark/zk_proof_systems/ppzksnark/r1cs_ppzksnark/r1cs_ppzksnark.hpp"
#include "libsnark/common/default_types/r1cs_ppzksnark_pp.hpp"
#include "libsnark/gadgetlib1/pb_variable.hpp"
#include <libsnark/gadgetlib1/gadgets/hashes/sha256/sha256_gadget.hpp>
#include <initializer_list>
#include "util.hpp"

#include <algorithm>
#include <cassert>
#include <cstdarg>
#include <cstdint>

#include <libff/common/utils.hpp>

using namespace libsnark;
using namespace std;


bit_vector convert(unsigned long x) {
  bit_vector ret;
  int count=0;
  while(x!=0) {
    if (x&1)
      ret.push_back(1);
    else
      ret.push_back(0);
    x>>=1;
    count++;  
  }
  
  while(count<256)
  {
    ret.push_back(0);
    count++;
  }
  reverse(ret.begin(),ret.end());


  return ret;
}

libff::bit_vector get_hash(const libff::bit_vector &input1,const libff::bit_vector &input2)
{

    default_r1cs_ppzksnark_pp::init_public_params();
    typedef libff::Fr<default_r1cs_ppzksnark_pp> FieldT;

    protoboard<FieldT> pb;

    digest_variable<FieldT> input_variable1(pb, SHA256_block_size, "input1");
    digest_variable<FieldT> input_variable2(pb, SHA256_block_size, "input2");
    digest_variable<FieldT> output_variable(pb, SHA256_digest_size, "output");
    sha256_two_to_one_hash_gadget<FieldT> f(pb, input_variable1, input_variable2, output_variable, "f");

    input_variable1.generate_r1cs_witness(input1);
    input_variable2.generate_r1cs_witness(input2);
    f.generate_r1cs_witness();

    return output_variable.get_digest();
}


int main()
{
  typedef libff::Fr<default_r1cs_ppzksnark_pp> FieldT;

  // Initialize the curve parameters

  default_r1cs_ppzksnark_pp::init_public_params();
  
  // Create protoboard

  protoboard<FieldT> pb;

  // Define variables

  pb_variable<FieldT> x;
  pb_variable<FieldT> sym_1;
  pb_variable<FieldT> y;
  pb_variable<FieldT> sym_2;
  pb_variable<FieldT> out;



  //declaring the hash variable
  pb_variable_array<FieldT> hash_packed;
  pb_variable<FieldT> ledger;
  pb_variable<FieldT> block;
  pb_variable<FieldT> rem_balance;
  pb_variable<FieldT> l_minus_b;
  // Allocate variables to protoboard
  // The strings (like "x") are only for debugging purposes
  
  out.allocate(pb, "out");
  x.allocate(pb, "x");
  sym_1.allocate(pb, "sym_1");
  y.allocate(pb, "y");
  sym_2.allocate(pb, "sym_2");


  //allocating hash variable
  hash_packed.allocate(pb,2,"hash_packed");
  ledger.allocate(pb,"ledger");
  block.allocate(pb,"block");
  rem_balance.allocate(pb,"rem_balance");
  l_minus_b.allocate(pb,"l_minus_b");
  // This sets up the protoboard variables
  // so that the first one (out) represents the public
  // input and the rest is private input
  pb.set_input_sizes(9);


   //intermediate variable 
  digest_variable<FieldT> hash_bits(pb, SHA256_digest_size, "hash_bits");
  digest_variable<FieldT> left_bits(pb, SHA256_digest_size, "left_bits");
  digest_variable<FieldT> right_bits(pb, SHA256_digest_size, "right_bits");


  multipacking_gadget<FieldT> packer(pb, hash_bits.bits, hash_packed, 128, "packer");
  sha256_two_to_one_hash_gadget<FieldT> hasher(pb, left_bits, right_bits, hash_bits, "hash_gadget");


  

  // Add R1CS constraints to protoboard

  // x*x = sym_1
  pb.add_r1cs_constraint(r1cs_constraint<FieldT>(x, x, sym_1));

  // sym_1 * x = y
  pb.add_r1cs_constraint(r1cs_constraint<FieldT>(sym_1, x, y));

  // y + x = sym_2
  pb.add_r1cs_constraint(r1cs_constraint<FieldT>(y + x, 1, sym_2));

  // sym_2 + 5 = ~out
  pb.add_r1cs_constraint(r1cs_constraint<FieldT>(sym_2 + 5, 1, out));



  //Add constraints
  packer.generate_r1cs_constraints(true);
  hasher.generate_r1cs_constraints();
  pb.add_r1cs_constraint(r1cs_constraint<FieldT>(ledger-block, 1, l_minus_b));
  pb.add_r1cs_constraint(r1cs_constraint<FieldT>(l_minus_b-rem_balance,1, 0));

  
  // Add witness values

  pb.val(x) = 3;
  pb.val(out) = 35;
  pb.val(sym_1) = 9;
  pb.val(y) = 27;
  pb.val(sym_2) = 30;

  pb.val(ledger)=10;
  pb.val(block)=2;
  pb.val(rem_balance)=8;
  pb.val(l_minus_b)=pb.val(ledger)-pb.val(block);


//   const libff::bit_vector left_bv  = libff::int_list_to_bits({0x426bc2d8, 0x4dc86782, 0x81e8957a, 0x409ec148, 0xe6cffbe8, 0xafe6ba4f, 0x9c6f1978, 0xdd7af7e9}, 32);
//   const libff::bit_vector right_bv = libff::int_list_to_bits({0x038cce42, 0xabd366b8, 0x3ede7e00, 0x9130de53, 0x72cdf73d, 0xee825114, 0x8cb48d1b, 0x9af68ad0}, 32);
//   const libff::bit_vector hash_bv  = libff::int_list_to_bits({0xeffd0b7f, 0x1ccba116, 0x2ee816f7, 0x31c62b48, 0x59305141, 0x990e5c0a, 0xce40d33d, 0x0b1167d1}, 32);
  const libff::bit_vector left_bv  = convert(0);
  const libff::bit_vector right_bv = convert(9);
  const libff::bit_vector hash_bv  = get_hash(left_bv,right_bv);



  left_bits.generate_r1cs_witness(left_bv);
  right_bits.generate_r1cs_witness(right_bv);
  hasher.generate_r1cs_witness();
  packer.generate_r1cs_witness_from_bits();


  const r1cs_constraint_system<FieldT> constraint_system = pb.get_constraint_system();

  const r1cs_ppzksnark_keypair<default_r1cs_ppzksnark_pp> keypair = r1cs_ppzksnark_generator<default_r1cs_ppzksnark_pp>(constraint_system);

  const r1cs_ppzksnark_proof<default_r1cs_ppzksnark_pp> proof = r1cs_ppzksnark_prover<default_r1cs_ppzksnark_pp>(keypair.pk, pb.primary_input(), pb.auxiliary_input());

  bool verified = r1cs_ppzksnark_verifier_strong_IC<default_r1cs_ppzksnark_pp>(keypair.vk, pb.primary_input(), proof);

  cout << "Number of R1CS constraints: " << constraint_system.num_constraints() << endl;
  cout << "Primary (public) input: " << pb.primary_input() << endl;
  //cout << "Auxiliary (private) input: " << pb.auxiliary_input() << endl;
  cout << "Verification status: " << verified << endl;
  //libff::serialize_bit_vector(cout,left_bv);

  const r1cs_ppzksnark_verification_key<default_r1cs_ppzksnark_pp> vk = keypair.vk;

  print_vk_to_file<default_r1cs_ppzksnark_pp>(vk, "../build/vk_data");
  print_proof_to_file<default_r1cs_ppzksnark_pp>(proof, "../build/proof_data");

  return 0;
}
