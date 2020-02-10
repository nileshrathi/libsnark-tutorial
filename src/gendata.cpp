#include <libff/common/default_types/ec_pp.hpp>
#include <libsnark/common/default_types/r1cs_gg_ppzksnark_pp.hpp>
#include "libsnark/common/default_types/r1cs_ppzksnark_pp.hpp"
#include <libsnark/relations/constraint_satisfaction_problems/r1cs/examples/r1cs_examples.hpp>
#include <libsnark/zk_proof_systems/ppzksnark/r1cs_gg_ppzksnark/r1cs_gg_ppzksnark.hpp>

#include <libsnark/gadgetlib1/pb_variable.hpp>
#include <libsnark/gadgetlib1/gadgets/basic_gadgets.hpp>

#include <cassert>
#include <memory>

#include <libsnark/gadgetlib1/gadget.hpp>






using namespace libsnark;
using namespace libff;
#include<iostream>
using namespace std;


typedef libff::default_ec_pp ppT;


vector<vector<size_t>> nodes_coeff;


int random_number(int min,int max)
{
    int randNum = rand()%(max-min + 1) + min;
    return randNum;
}


/* Global Variables Defining properties of the system */

size_t number_of_chains=45;
size_t number_of_users_per_shard=90;


#define ONE pb_variable<libff::Fr<ppT>>(0)

int main () {

    typedef libff::Fr<ppT> field_T ;
    default_r1cs_gg_ppzksnark_pp::init_public_params();
    //test_r1cs_gg_ppzksnark<default_r1cs_gg_ppzksnark_pp>(1000, 100);

    // Create protoboard
    

    protoboard<field_T> pb;
  
  pb_variable<field_T> x;
  pb_variable<field_T> sym_1;
  pb_variable<field_T> y;
  pb_variable<field_T> sym_2;
  pb_variable<field_T> out;



    vector<pb_variable_array<field_T> > ledger_array;
    vector<pb_variable_array<field_T> > blocks_array;
    pb_variable_array<field_T> coefficients;
    pb_variable_array<field_T> ledger_array_encoded;
    vector<shared_ptr<inner_product_gadget<field_T> > > lip;
    vector<shared_ptr<inner_product_gadget<field_T> > > bip;  
    pb_variable_array<field_T> blocks_array_encoded;
    pb_variable_array<field_T> result_array_encoded;






  // Allocate variables to protoboard
  // The strings (like "x") are only for debugging purposes
  
  out.allocate(pb, "out");
  x.allocate(pb, "x");
  sym_1.allocate(pb, "sym_1");
  y.allocate(pb, "y");
  sym_2.allocate(pb, "sym_2");



    //resizing the ledger_aay to number of shards
    ledger_array.resize(number_of_users_per_shard);
    //allocating each ledger of ledger_array with num_of_users_per_shard elements
    for(size_t i=0;i<number_of_users_per_shard;i++)
    {
        ledger_array[i].allocate(pb,number_of_chains,"every element contain and individual ledger");
    }

    //resizing the ledger_aay to number of shards
    blocks_array.resize(number_of_users_per_shard);
    //allocating each ledger of ledger_array with num_of_users_per_shard elements
    for(size_t i=0;i<number_of_users_per_shard;i++)
    {
        blocks_array[i].allocate(pb,number_of_chains,"every element contain and individual ledger");
    }




    //Allocating the coefficients to be equal to number of chains is system
    coefficients.allocate(pb,number_of_chains,"Coefficients for chain i");

    //allocating encoded ledger
    ledger_array_encoded.allocate(pb,number_of_users_per_shard,"encoded ledger for node i");

    //allocating encoded ledger
    blocks_array_encoded.allocate(pb,number_of_users_per_shard,"encoded ledger for node i");

    //allocate the result array
    result_array_encoded.allocate(pb,number_of_users_per_shard,"encoded_results");
    

    lip.resize(number_of_users_per_shard);

    for(int i=0;i<number_of_users_per_shard;i++)
    {
        lip[i].reset(new inner_product_gadget<field_T>(pb, ledger_array[i], coefficients, ledger_array_encoded[i], "ip"));
    }


   bip.resize(number_of_users_per_shard);

    for(int i=0;i<number_of_users_per_shard;i++)
    {
        bip[i].reset(new inner_product_gadget<field_T>(pb, blocks_array[i], coefficients, blocks_array_encoded[i], "ip"));
    }









  // This sets up the protoboard variables
  // so that the first one (out) represents the public
  // input and the rest is private input
  pb.set_input_sizes(pb.num_variables());

  // Add R1CS constraints to protoboard

  // x*x = sym_1
  pb.add_r1cs_constraint(r1cs_constraint<field_T>(x, x, sym_1));

  // sym_1 * x = y
  pb.add_r1cs_constraint(r1cs_constraint<field_T>(sym_1, x, y));

  // y + x = sym_2
  pb.add_r1cs_constraint(r1cs_constraint<field_T>(y + x, 1, sym_2));

  // sym_2 + 5 = ~out
  pb.add_r1cs_constraint(r1cs_constraint<field_T>(sym_2 + 5, 1, out));






  for(int i=0;i<number_of_users_per_shard;i++)
  {
      lip[i]->generate_r1cs_constraints();
  }

   for(int i=0;i<number_of_users_per_shard;i++)
  {
      bip[i]->generate_r1cs_constraints();
  }

  for(int i=0;i<number_of_users_per_shard;i++)
  {
     pb.add_r1cs_constraint(r1cs_constraint<field_T>(ledger_array_encoded[i] - blocks_array_encoded[i], 1, result_array_encoded[i]));
  }


  
  // Add witness values

  pb.val(x) = 3;
  pb.val(out) = 35;
  pb.val(sym_1) = 9;
  pb.val(y) = 27;
  pb.val(sym_2) = pb.val(y)+pb.val(x);


  for(int i=0;i<number_of_users_per_shard;i++)
  {
      for(int j=0;j<number_of_chains;j++)
      {
          pb.val(ledger_array[i][j])= random_number(1000,10000);
          pb.val(blocks_array[i][j])= random_number(1000,10000);
      }
  }

  for(int j=0;j<number_of_chains;j++)
      {
          pb.val(coefficients[j])= random_number(1,50);
      }



for(int i=0;i<number_of_users_per_shard;i++)
  {
      lip[i]->generate_r1cs_witness();
  }

for(int i=0;i<number_of_users_per_shard;i++)
  {
      bip[i]->generate_r1cs_witness();
  }

for(int i=0;i<number_of_users_per_shard;i++)
{
    pb.val(result_array_encoded[i])=pb.val(ledger_array_encoded[i])-pb.val(blocks_array_encoded[i]);
}

  





    

    


    const r1cs_constraint_system<field_T> constraint_system = pb.get_constraint_system();

    libff::print_header("R1CS GG-ppzkSNARK Generator");
    r1cs_gg_ppzksnark_keypair<ppT> keypair = r1cs_gg_ppzksnark_generator<ppT>(constraint_system);
    printf("\n"); libff::print_indent(); libff::print_mem("after generator");

    libff::print_header("Preprocess verification key");
    r1cs_gg_ppzksnark_processed_verification_key<ppT> pvk = r1cs_gg_ppzksnark_verifier_process_vk<ppT>(keypair.vk);


    libff::print_header("R1CS GG-ppzkSNARK Prover");
    r1cs_gg_ppzksnark_proof<ppT> proof = r1cs_gg_ppzksnark_prover<ppT>(keypair.pk, pb.primary_input(), pb.auxiliary_input());
    printf("\n"); libff::print_indent(); libff::print_mem("after prover");




    libff::print_header("R1CS GG-ppzkSNARK Verifier");
    const bool ans = r1cs_gg_ppzksnark_verifier_strong_IC<ppT>(keypair.vk, pb.primary_input(), proof);
    printf("\n"); libff::print_indent(); libff::print_mem("after verifier");
    printf("* The verification result is: %s\n", (ans ? "PASS" : "FAIL"));

    libff::print_header("R1CS GG-ppzkSNARK Online Verifier");
    const bool ans2 = r1cs_gg_ppzksnark_online_verifier_strong_IC<ppT>(pvk, pb.primary_input(), proof);
    assert(ans == ans2);



  cout << "Number of R1CS constraints: " << constraint_system.num_constraints() << endl;
  //cout << "Primary (public) input: " << pb.primary_input() << endl;
  cout << "Auxiliary (private) input: " << pb.auxiliary_input() << endl;
  cout << "Verification status: " << ans << endl;






    cout<<"The verifier answer is "<<ans<<" "<<ans2<<"\n";



    return 0;
}
