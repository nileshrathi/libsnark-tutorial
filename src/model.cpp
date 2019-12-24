#include <stdlib.h>
#include <iostream>

#include "libff/algebra/fields/field_utils.hpp"
#include "libsnark/zk_proof_systems/ppzksnark/r1cs_ppzksnark/r1cs_ppzksnark.hpp"
#include "libsnark/common/default_types/r1cs_ppzksnark_pp.hpp"
#include "libsnark/gadgetlib1/pb_variable.hpp"

#include "gadget-model.hpp"
#include "util.hpp"

using namespace libsnark;
using namespace std;




vector<int> convert_to_binary(int x) {
  vector<int> ret;
  int count=0;
  while(x) {
    if (x&1)
      ret.push_back(1);
    else
      ret.push_back(0);
    x>>=1;
    count++;  
  }
  
  while(count<512)
  {
    ret.push_back(0);
    count++;
  }
  reverse(ret.begin(),ret.end());


  return ret;
}








int main()
{
  // Initialize the curve parameters

  default_r1cs_ppzksnark_pp::init_public_params();

  typedef libff::Fr<default_r1cs_ppzksnark_pp> FieldT;




  
  // Create protoboard

  protoboard<FieldT> pb;
  pb_variable<FieldT> out;
  pb_variable<FieldT> x;

  pb_variable<FieldT> b;

  // Allocate variables

  out.allocate(pb, "out");
  x.allocate(pb, "x");
  b.allocate(pb,"b");

  // This sets up the protoboard variables
  // so that the first one (out) represents the public
  // input and the rest is private input




  pb.set_input_sizes(3);

  // Initialize gadget

  gadget_model<FieldT> g(pb, out, x, b);
  g.generate_r1cs_constraints();
  
  // Add witness values

  pb.val(out) = 7;
  pb.val(x) = 3;
  pb.val(b)=9;





  vector<int> vx=convert_to_binary(7);
  

  printf("Vector print starn\n");
   for (auto i = vx.begin(); i != vx.end(); ++i) 
        cout << *i << " ";
  cout<<"\nvx size is "<<vx.size()<<"\n";
  // libff::bit_vector hx=

  g.generate_r1cs_witness();
  
  const r1cs_constraint_system<FieldT> constraint_system = pb.get_constraint_system();

  const r1cs_ppzksnark_keypair<default_r1cs_ppzksnark_pp> keypair = r1cs_ppzksnark_generator<default_r1cs_ppzksnark_pp>(constraint_system);

  const r1cs_ppzksnark_proof<default_r1cs_ppzksnark_pp> proof = r1cs_ppzksnark_prover<default_r1cs_ppzksnark_pp>(keypair.pk, pb.primary_input(), pb.auxiliary_input());

  bool verified = r1cs_ppzksnark_verifier_strong_IC<default_r1cs_ppzksnark_pp>(keypair.vk, pb.primary_input(), proof);

  cout << "Number of R1CS constraints: " << constraint_system.num_constraints() << endl;
  cout << "Primary (public) input: " << pb.primary_input() << endl;
  cout << "Auxiliary (private) input: " << pb.auxiliary_input() << endl;
  cout << "Verification status: " << verified << endl;

  const r1cs_ppzksnark_verification_key<default_r1cs_ppzksnark_pp> vk = keypair.vk;

  print_vk_to_file<default_r1cs_ppzksnark_pp>(vk, "../build/vk_data");
  print_proof_to_file<default_r1cs_ppzksnark_pp>(proof, "../build/proof_data");

  return 0;
}
