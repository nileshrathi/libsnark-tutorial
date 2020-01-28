#include <libff/common/default_types/ec_pp.hpp>
#include <libsnark/common/default_types/r1cs_gg_ppzksnark_pp.hpp>
#include "libsnark/common/default_types/r1cs_ppzksnark_pp.hpp"
#include <libsnark/relations/constraint_satisfaction_problems/r1cs/examples/r1cs_examples.hpp>
#include <libsnark/zk_proof_systems/ppzksnark/r1cs_gg_ppzksnark/r1cs_gg_ppzksnark.hpp>

#include <libsnark/gadgetlib1/pb_variable.hpp>
#include <libsnark/gadgetlib1/gadgets/basic_gadgets.hpp>






using namespace libsnark;
using namespace libff;
#include<iostream>
using namespace std;


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


int random_number(int min,int max)
{
    int randNum = rand()%(max-min + 1) + min;
    return randNum;
}


/* Global Variables Defining properties of the system */

size_t number_of_chains=3;
size_t number_of_nodes=9;
size_t number_of_users_per_shard=9;


#define ONE pb_variable<libff::Fr<ppT>>(0)

int main () {

    typedef libff::Fr<ppT> field_T ;
    default_r1cs_gg_ppzksnark_pp::init_public_params();
    //test_r1cs_gg_ppzksnark<default_r1cs_gg_ppzksnark_pp>(1000, 100);

    // Create protoboard

    protoboard<field_T> pb;

    // Define variables
    
    vector<pb_variable_array<field_T> > ledger_array;
    pb_variable_array<field_T> coefficients;
    pb_variable_array<field_T> ledger_array_encoded;
    vector<pb_variable_array<field_T> > S;







    
    //resizing the ledger_aay to number of shards
    ledger_array.resize(number_of_users_per_shard);
    //allocating each ledger of ledger_array with num_of_users_per_shard elements
    for(size_t i=0;i<number_of_users_per_shard;i++)
    {
        ledger_array[i].allocate(pb,number_of_chains,"num of users per shard");
    }

    //Allocating the coefficients to be equal to number of chains is system
    coefficients.allocate(pb,number_of_chains,"Coefficits for chain i");

    //allocating encoded ledger

    ledger_array_encoded.allocate(pb,number_of_users_per_shard,"encoded ledger for node i");


    S.resize(number_of_users_per_shard);
    for(int i=0;i<number_of_users_per_shard;i++)
    {
        S[i].allocate(pb,number_of_chains-1,"S");

    }

    pb.set_input_sizes(1);
    
    
    // compute_inner_product.resize(number_of_users_per_shard);

    for(size_t j=0;j<number_of_users_per_shard;j++)
    {
        // compute_inner_product[i](pb,ledger_array[i],coefficients,dummy, "f");
        // compute_inner_product[i].generate_r1cs_constraints();

        for (size_t i = 0; i < ledger_array[j].size(); ++i)
    {
        pb.add_r1cs_constraint(
            r1cs_constraint<field_T>(ledger_array[j][i], coefficients[i],
                                    (i == ledger_array[j].size()-1 ? ledger_array_encoded[j] : S[j][i]) + (i == 0 ? 0 * ONE : -S[j][i-1])),
            "gtv");
    }
    }







    
    //Filling aup the ledger_array

    int dummy_ledger_array[number_of_chains][number_of_users_per_shard]
    {
  { 1, 2, 3, 4, 5, 6, 7, 8, 9 }, // row 0
  { 10, 11, 12,13,14,15,16,17,18 }, // row 1
  { 19,20,21,22,23,24,25,26,27 } // row 2
};

int ledger_array_temp[number_of_users_per_shard][number_of_chains];

    for(int i=0;i<number_of_users_per_shard;i++)
    {
        for(int j=0;j<number_of_chains;j++)
        {
            ledger_array_temp[i][j]=dummy_ledger_array[j][i];
            pb.val(ledger_array[i][j])=dummy_ledger_array[j][i];
        }
    }

    //filling up the coefficients;
    int coefficients_array[number_of_chains];
    for(int i=0;i<number_of_chains;i++)
    {
        coefficients_array[i]=i+1;
        pb.val(coefficients[i])=coefficients_array[i];
    }

    //filling up the encoded_ledger

    for(int i=0;i<number_of_users_per_shard;i++)
    {
        int sum=0;
        for(int j=0;j<number_of_chains;j++)
        {
            sum=sum+(coefficients_array[j]*ledger_array_temp[i][j]);

        }
        pb.val(ledger_array_encoded[i])=sum;

    }

    




    for(size_t j=0;j<number_of_users_per_shard;j++)
    {
        // compute_inner_product[i](pb,ledger_array[i],coefficients,dummy, "f");
        // compute_inner_product[i].generate_r1cs_constraints();

    int total=0;
        for (size_t i = 0; i < 3; ++i)
    {
            total=total+ledger_array_temp[j][i]*coefficients_array[i];
            if(i==2)
            {
                pb.val(ledger_array_encoded[j])=total;
            }
            else
            {
                pb.val(S[j][i])=total;
            }
               
    }

    }

    //pb.val(ledger_array_encoded[1])=45;

    



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
