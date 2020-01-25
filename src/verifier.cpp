#include <libff/common/default_types/ec_pp.hpp>
#include <libsnark/common/default_types/r1cs_gg_ppzksnark_pp.hpp>
#include "libsnark/common/default_types/r1cs_ppzksnark_pp.hpp"
#include <libsnark/relations/constraint_satisfaction_problems/r1cs/examples/r1cs_examples.hpp>
#include <libsnark/zk_proof_systems/ppzksnark/r1cs_gg_ppzksnark/r1cs_gg_ppzksnark.hpp>

#include <libsnark/gadgetlib1/pb_variable.hpp>






using namespace libsnark;
using namespace libff;
#include<iostream>
using namespace std;

/**
 * The code below provides an example of all stages of running a R1CS GG-ppzkSNARK.
 *
 * Of course, in a real-life scenario, we would have three distinct entities,
 * mangled into one in the demonstration below. The three entities are as follows.
 * (1) The "generator", which runs the ppzkSNARK generator on input a given
 *     constraint system CS to create a proving and a verification key for CS.
 * (2) The "prover", which runs the ppzkSNARK prover on input the proving key,
 *     a primary input for CS, and an auxiliary input for CS.
 * (3) The "verifier", which runs the ppzkSNARK verifier on input the verification key,
 *     a primary input for CS, and a proof.
 */
// template<typename ppT>
// bool run_r1cs_gg_ppzksnark(const r1cs_example<libff::Fr<ppT> > &example)
// {
//     libff::print_header("R1CS GG-ppzkSNARK Generator");
//     r1cs_gg_ppzksnark_keypair<ppT> keypair = r1cs_gg_ppzksnark_generator<ppT>(example.constraint_system);
//     printf("\n"); libff::print_indent(); libff::print_mem("after generator");

//     libff::print_header("Preprocess verification key");
//     r1cs_gg_ppzksnark_processed_verification_key<ppT> pvk = r1cs_gg_ppzksnark_verifier_process_vk<ppT>(keypair.vk);

//     libff::print_header("R1CS GG-ppzkSNARK Prover");
//     r1cs_gg_ppzksnark_proof<ppT> proof = r1cs_gg_ppzksnark_prover<ppT>(keypair.pk, example.primary_input, example.auxiliary_input);
//     printf("\n"); libff::print_indent(); libff::print_mem("after prover");

//     libff::print_header("R1CS GG-ppzkSNARK Verifier");
//     const bool ans = r1cs_gg_ppzksnark_verifier_strong_IC<ppT>(keypair.vk, example.primary_input, proof);
//     printf("\n"); libff::print_indent(); libff::print_mem("after verifier");
//     printf("* The verification result is: %s\n", (ans ? "PASS" : "FAIL"));

//     libff::print_header("R1CS GG-ppzkSNARK Online Verifier");
//     const bool ans2 = r1cs_gg_ppzksnark_online_verifier_strong_IC<ppT>(pvk, example.primary_input, proof);
//     assert(ans == ans2);

//     return ans;
// }

// template<typename ppT>
// void test_r1cs_gg_ppzksnark(size_t num_constraints, size_t input_size)
// {
//     r1cs_example<libff::Fr<ppT> > example = generate_r1cs_example_with_binary_input<libff::Fr<ppT> >(num_constraints, input_size);
//     const bool bit = run_r1cs_gg_ppzksnark<ppT>(example);
//     assert(bit);
// }
typedef libff::default_ec_pp ppT;


vector<vector<size_t>> nodes_coeff;


size_t a[]={0,5,6,7,8,9,10,11,12,13};
size_t w[]={0,1,2,3};

vector< vector < size_t > > lagrange_coeff(size_t a[] , size_t w[], int len, int len_len)
{
    
    vector< vector <size_t> > nodes_coeff(len);
    //cout<<"here"<<len<<" "<<len_len;
    for(int i=1;i<len;i++)
    {
        for(int j=1;j<len_len;j++)
        {
            size_t coeff=1;
            for(int k=1;k<len_len;k++)
            {
                if(j!=k)
                coeff=coeff * ((a[i]-w[k])/(w[j]-w[k]));
            }
            //cout<<coeff<<" ";
            nodes_coeff[i].push_back(coeff);
        }
    }

    return nodes_coeff;
}




int main () {
    
    typedef libff::Fr<ppT> field_T ;
    default_r1cs_gg_ppzksnark_pp::init_public_params();
    //test_r1cs_gg_ppzksnark<default_r1cs_gg_ppzksnark_pp>(1000, 100);

    // Create protoboard

    protoboard<field_T> pb;

    // Define variables

    // pb_variable<field_T> x;
    // pb_variable<field_T> sym_1;
    // pb_variable<field_T> y;
    // pb_variable<field_T> sym_2;
    // pb_variable<field_T> out;


    //VAriables for verification
    pb_variable_array<field_T> alpha;
    pb_variable_array<field_T> w;
    pb_variable_array<field_T> coeff;
    pb_variable_array<field_T> ledger;
    pb_variable_array<field_T> blocks;
    pb_variable_array<field_T> encoded_ledger_ubu;
    pb_variable_array<field_T> encoded_blocks_ubu;
    pb_variable_array<field_T> encoded_results;






    // Allocate variables to protoboard
    // The strings (like "x") are only for debugging purposes
    
    // out.allocate(pb, "out");
    // x.allocate(pb, "x");
    // sym_1.allocate(pb, "sym_1");
    // y.allocate(pb, "y");
    // sym_2.allocate(pb, "sym_2");



    // Allocating the verification variables
    size_t number_of_chains=3;
    size_t number_of_nodes=9;
    alpha.allocate(pb,number_of_nodes,"Nodes coefficients");
    w.allocate(pb,number_of_chains,"Evaluation points to recover original chains");
    coeff.allocate(pb,number_of_chains*number_of_nodes,"Holding all coeff in single array");
    ledger.allocate(pb,number_of_chains*number_of_nodes,"Holding all coeff in single array");
    blocks.allocate(pb,number_of_chains*number_of_nodes,"Holding all coeff in single array");
    encoded_ledger_ubu.allocate(pb,number_of_chains*number_of_nodes,"Holding all coeff in single array");
    encoded_blocks_ubu.allocate(pb,number_of_chains*number_of_nodes,"Holding all coeff in single array");
    encoded_results.allocate(pb,number_of_chains*number_of_nodes,"holding the coded verification results");

    // This sets up the protoboard variables
    // so that the first one (out) represents the public
    // input and the rest is private input
    // pb.set_input_sizes(1);

    // // Add R1CS constraints to protoboard

    // // x*x = sym_1
    // pb.add_r1cs_constraint(r1cs_constraint<field_T>(x, x, sym_1));

    // // sym_1 * x = y
    // pb.add_r1cs_constraint(r1cs_constraint<field_T>(sym_1, x, y));

    // // y + x = sym_2
    // pb.add_r1cs_constraint(r1cs_constraint<field_T>(y + x, 1, sym_2));

    // // sym_2 + 5 = ~out
    // pb.add_r1cs_constraint(r1cs_constraint<field_T>(sym_2 + 5, 1, out));


    for(int i=0;i<number_of_chains*number_of_nodes;i++)
    {
        pb.add_r1cs_constraint(r1cs_constraint<field_T>(coeff[i],ledger[i],encoded_ledger_ubu[i]));
    }

    for(int i=0;i<number_of_chains*number_of_nodes;i++)
    {
        pb.add_r1cs_constraint(r1cs_constraint<field_T>(coeff[i],blocks[i],encoded_blocks_ubu[i]));
    }

    for(int i=0;i<number_of_chains*number_of_nodes;i++)
    {
        pb.add_r1cs_constraint(r1cs_constraint<field_T>(encoded_ledger_ubu[i]-encoded_blocks_ubu[i],1,encoded_results[i]));
    }



    





    // Add witness values

    // pb.val(x)=3;
    // pb.val(out) = 35;
    // pb.val(sym_1) = 9;
    // pb.val(y) = 27;
    // pb.val(sym_2) = 30;


    //setting the verifier variable values;
    int coeff_vector[] ={3,-8,6,6,-15,10,10,-24,15,15,-35,21,21,-48,28,28,-63,36,36,-80,45,45,-99,55,55,-120,66};
    int ledger_vector[] ={13,15,17,13,15,17,13,15,17,13,15,17,13,15,17,13,15,17,13,15,17,13,15,17,13,15,17};
    int blocks_vector[]={3,2,3,3,2,3,3,2,3,3,2,3,3,2,3,3,2,3,3,2,3,3,2,3,3,2,3};

    vector<int> encoded_ledger_ubu_vector(number_of_chains*number_of_nodes);
    vector<int> encoded_blocks_ubu_vector(number_of_chains*number_of_nodes);
    vector<int> encoded_results_vector(number_of_chains*number_of_nodes);

    for(int i=0;i<27;i++)
    {
        pb.val(coeff[i])=coeff_vector[i];
    
    }

    for(int i=0;i<27;i++)
    {
        pb.val(ledger[i])=ledger_vector[i];
    }

    for(int i=0;i<number_of_chains*number_of_nodes;i++)
    {
        pb.val(blocks[i])=blocks_vector[i];
    }

    for(int i=0;i<number_of_chains*number_of_nodes;i++)
    {
        encoded_ledger_ubu_vector[i]=coeff_vector[i]*ledger_vector[i];
        pb.val(encoded_ledger_ubu[i])=encoded_ledger_ubu_vector[i];
    }

    for(int i=0;i<number_of_chains*number_of_nodes;i++)
    {
        encoded_blocks_ubu_vector[i]=coeff_vector[i]*blocks_vector[i];
        pb.val(encoded_blocks_ubu[i])=encoded_blocks_ubu_vector[i];
    }

    for(int i=0;i<number_of_chains*number_of_nodes;i++)
    {
        encoded_results_vector[i]=encoded_ledger_ubu_vector[i]-encoded_blocks_ubu_vector[i];
        pb.val(encoded_results[i])=encoded_results_vector[i];
    }
    
    pb.val(encoded_results[2])=encoded_results_vector[6];



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
  cout << "Primary (public) input: " << pb.primary_input() << endl;
  cout << "Auxiliary (private) input: " << pb.auxiliary_input() << endl;
  cout << "Verification status: " << ans << endl;






    cout<<"The verifier answer is "<<ans<<" "<<ans2<<"\n";



    return 0;
}
