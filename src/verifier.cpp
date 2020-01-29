#include <libff/common/default_types/ec_pp.hpp>
#include <libsnark/common/default_types/r1cs_gg_ppzksnark_pp.hpp>
#include "libsnark/common/default_types/r1cs_ppzksnark_pp.hpp"
#include <libsnark/relations/constraint_satisfaction_problems/r1cs/examples/r1cs_examples.hpp>
#include <libsnark/zk_proof_systems/ppzksnark/r1cs_gg_ppzksnark/r1cs_gg_ppzksnark.hpp>

#include <libsnark/gadgetlib1/pb_variable.hpp>
#include <libsnark/gadgetlib1/gadgets/basic_gadgets.hpp>
#include <bits/stdc++.h> 





using namespace libsnark;
using namespace libff;
#include<iostream>
using namespace std;


typedef libff::default_ec_pp ppT;


vector<vector<size_t>> nodes_coeff;


// size_t a[]={0,5,6,7,8,9,10,11,12,13};
// size_t w[]={0,1,2,3};

vector< vector < int > > lagrange_coeff(vector<int> a , vector<int> w, int len, int len_len)
{
    
    vector< vector <int> > nodes_coeff(len);
    //cout<<"here"<<len<<" "<<len_len;
    for(int i=1;i<len;i++)
    {
        for(int j=1;j<len_len;j++)
        {
            int coeff=1;
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
size_t number_of_users_per_shard=9000;
size_t node_number=1;


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

    vector<pb_variable_array<field_T> > blocks_array;
    pb_variable_array<field_T> blocks_array_encoded;
    vector<pb_variable_array<field_T> > SB;

    pb_variable_array<field_T> result_array_encoded;



    //calculation of nodes coefficients

    vector<int> alpha;
    vector<int> beta;
    alpha.resize(number_of_nodes+1);
    beta.resize(number_of_chains+1);

    for(int i=0;i<beta.size();i++)
    {
        beta[i]=i;
    }
    
    for(int i=0;i<alpha.size();i++)
    {
        if(i==0)
        {
            alpha[i]=0;
        }
        else
        {
            alpha[i]=number_of_chains+1+i;
        }
        
    }

    // for(int i=0;i<beta.size();i++)
    // {
    //     cout<<beta[i]<<" ";
    // }
    // cout<<"\n end \n";

    // for(int i=0;i<alpha.size();i++)
    // {
    //     cout<<alpha[i]<<" ";
    // }
    // cout<<"\n end \n";
    vector<vector <int> > coeff_vector = lagrange_coeff(alpha,beta,alpha.size(),beta.size());







    
    //resizing the ledger_aay to number of shards
    ledger_array.resize(number_of_users_per_shard);
    //allocating each ledger of ledger_array with num_of_users_per_shard elements
    for(size_t i=0;i<number_of_users_per_shard;i++)
    {
        ledger_array[i].allocate(pb,number_of_chains,"every element contain and individual ledger");
    }

    //resizing the blocks_array to number of shards
    blocks_array.resize(number_of_users_per_shard);
    //allocating each block of blocks_array with num_of_users_per_shard elements
    for(size_t i=0;i<number_of_users_per_shard;i++)
    {
        blocks_array[i].allocate(pb,number_of_chains,"every element contain and individual block");
    }



    //Allocating the coefficients to be equal to number of chains is system
    coefficients.allocate(pb,number_of_chains,"Coefficients for chain i");

    //allocating encoded ledger
    ledger_array_encoded.allocate(pb,number_of_users_per_shard,"encoded ledger for node i");


    //allocating encoded block
    blocks_array_encoded.allocate(pb,number_of_users_per_shard,"encoded ledger for node i");


    S.resize(number_of_users_per_shard);
    for(int i=0;i<number_of_users_per_shard;i++)
    {
        S[i].allocate(pb,number_of_chains-1,"S");

    }

    SB.resize(number_of_users_per_shard);
    for(int i=0;i<number_of_users_per_shard;i++)
    {
        SB[i].allocate(pb,number_of_chains-1,"SB");

    }

    result_array_encoded.allocate(pb,number_of_users_per_shard,"encoded_results");


    pb.set_input_sizes(pb.num_variables());
    cout<<"number of variables are "<<pb.num_variables()<<"\n";
    

    //GENERATING R1CS CONSTRAINTS
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


        for(size_t j=0;j<number_of_users_per_shard;j++)
    {
        // compute_inner_product[i](pb,ledger_array[i],coefficients,dummy, "f");
        // compute_inner_product[i].generate_r1cs_constraints();

        for (size_t i = 0; i < blocks_array[j].size(); ++i)
    {
        pb.add_r1cs_constraint(
            r1cs_constraint<field_T>(blocks_array[j][i], coefficients[i],
                                    (i == blocks_array[j].size()-1 ? blocks_array_encoded[j] : SB[j][i]) + (i == 0 ? 0 * ONE : -SB[j][i-1])),
            "gtv");
    }
    }


    for(size_t i=0;i<number_of_users_per_shard;i++)
    {
        pb.add_r1cs_constraint(r1cs_constraint<field_T>(ledger_array_encoded[i]-blocks_array_encoded[i],1,result_array_encoded[i]));

    }







    
    //Filling aup the ledger_array

// int dummy_ledger_array[number_of_chains][number_of_users_per_shard]
// {
//   { 1, 2, 3, 4, 5, 6, 7, 8, 9 }, // row 0
//   { 10, 11, 12,13,14,15,16,17,18 }, // row 1
//   { 19,20,21,22,23,24,25,26,27 } // row 2
// };


//Create a dummy ledger for testing purpose
vector<vector<int> >dummy_ledger_array;
dummy_ledger_array.resize(number_of_chains);
for(int i=0;i<number_of_chains;i++)
{
    dummy_ledger_array[i].resize(number_of_users_per_shard);
}

for(int i=0;i<number_of_chains;i++)
{
    for(int j=0;j<number_of_users_per_shard;j++)
    {
        dummy_ledger_array[i][j]=random_number(10,500);
    }
}

//Create a dummy block for testing purpose
vector<vector<int> >dummy_blocks_array;
dummy_blocks_array.resize(number_of_chains);
for(int i=0;i<number_of_chains;i++)
{
    dummy_blocks_array[i].resize(number_of_users_per_shard);
}

for(int i=0;i<number_of_chains;i++)
{
    for(int j=0;j<number_of_users_per_shard;j++)
    {
        dummy_blocks_array[i][j]=random_number(10,500);
    }
}


//Creating encoded_ledger array and encoded_block array so as to add pb.va(encoded_result)

vector<int> ledger_array_encoded_temp;
vector<int> blocks_array_encoded_temp;
ledger_array_encoded_temp.resize(number_of_users_per_shard);
blocks_array_encoded_temp.resize(number_of_users_per_shard);


//temporary replica of ledger_array so as to compute S, since I dont know how to use pb.val(ledger_array) directly.
vector<vector<int> > ledger_array_temp;
ledger_array_temp.resize(number_of_users_per_shard);
for(int i=0;i<number_of_users_per_shard;i++)
{
    ledger_array_temp[i].resize(number_of_chains);
}


//temporary replica of blocks_array so as to compute S, since I dont know how to use pb.val(blocks_array) directly.
vector<vector<int> > blocks_array_temp;
blocks_array_temp.resize(number_of_users_per_shard);
for(int i=0;i<number_of_users_per_shard;i++)
{
    blocks_array_temp[i].resize(number_of_chains);
}



//setting the values of pb.val(ledger array)
    for(int i=0;i<number_of_users_per_shard;i++)
    {
        for(int j=0;j<number_of_chains;j++)
        {
            ledger_array_temp[i][j]=dummy_ledger_array[j][i];
            pb.val(ledger_array[i][j])=dummy_ledger_array[j][i];
        }
    }

//setting the values of pb.val(blocks array)
    for(int i=0;i<number_of_users_per_shard;i++)
    {
        for(int j=0;j<number_of_chains;j++)
        {
            blocks_array_temp[i][j]=dummy_blocks_array[j][i];
            pb.val(blocks_array[i][j])=dummy_blocks_array[j][i];
        }
    }


    for(int i=0;i<number_of_chains;i++)
    {
        // coefficients_array[i]=coeff_vector[1][i];
         pb.val(coefficients[i])=coeff_vector[node_number][i];
    }





    //filling up the encoded_ledger
    

    for(int i=0;i<number_of_users_per_shard;i++)
    {
        int sum=0;
        for(int j=0;j<number_of_chains;j++)
        {
            sum=sum+(coeff_vector[node_number][j]*ledger_array_temp[i][j]);

        }
        ledger_array_encoded_temp[i]=sum;
        pb.val(ledger_array_encoded[i])=sum;

    }



    //filling up the encoded_blocks

    for(int i=0;i<number_of_users_per_shard;i++)
    {
        int sum=0;
        for(int j=0;j<number_of_chains;j++)
        {
            sum=sum+(coeff_vector[node_number][j]*blocks_array_temp[i][j]);

        }
        blocks_array_encoded_temp[i]=sum;
        pb.val(blocks_array_encoded[i])=sum;

    }

    // addinng witness for the encoded result array

    for(int i=0;i<number_of_users_per_shard;i++)
    {
        pb.val(result_array_encoded[i])=ledger_array_encoded_temp[i]-blocks_array_encoded_temp[i];
    }



    for(size_t j=0;j<number_of_users_per_shard;j++)
    {
        // compute_inner_product[i](pb,ledger_array[i],coefficients,dummy, "f");
        // compute_inner_product[i].generate_r1cs_constraints();

    int total=0;
        for (size_t i = 0; i < number_of_chains; ++i)
    {
            total=total+ledger_array_temp[j][i]*coeff_vector[node_number][i];
            if(i==number_of_chains-1)
            {
                pb.val(ledger_array_encoded[j])=total;
            }
            else
            {
                pb.val(S[j][i])=total;
            }
               
    }

    }





      for(size_t j=0;j<number_of_users_per_shard;j++)
    {
        // compute_inner_product[i](pb,ledger_array[i],coefficients,dummy, "f");
        // compute_inner_product[i].generate_r1cs_constraints();

    int total=0;
        for (size_t i = 0; i < number_of_chains; ++i)
    {
            total=total+blocks_array_temp[j][i]*coeff_vector[node_number][i];
            if(i==number_of_chains-1)
            {
                pb.val(blocks_array_encoded[j])=total;
            }
            else
            {
                pb.val(SB[j][i])=total;
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

    stringstream proof_stream;
    proof_stream<<proof;
    ofstream fileOut;
    fileOut.open("verifier_proof");
    fileOut<<proof_stream.rdbuf();
    fileOut.close();


    ifstream fileIn("verifier_proof");
    stringstream proof_from_file;
    if (fileIn) {
       proof_from_file << fileIn.rdbuf();
       fileIn.close();
    }
    r1cs_gg_ppzksnark_proof<ppT> proof2;
    proof_from_file>>proof2;




    stringstream verifierKey;
    verifierKey << keypair.vk;

    ofstream fileOut1;
    fileOut1.open("verifierKey");

    fileOut1 << verifierKey.rdbuf();
    fileOut1.close();
   
    cout << "verifierKey before" << endl;
   // cout << keypair.vk << endl;

    ifstream fileIn1("verifierKey");
    stringstream verifierKeyFromFile;
    if (fileIn1) {
       verifierKeyFromFile << fileIn1.rdbuf();
       fileIn1.close();
    }
   
    verifierKeyFromFile >> keypair.vk;
   
    cout << "verifierKey after" << endl;
    //cout << keypair.vk << endl;


    



    libff::print_header("R1CS GG-ppzkSNARK Verifier");
    const bool ans = r1cs_gg_ppzksnark_verifier_strong_IC<ppT>(keypair.vk, pb.primary_input(), proof2);
    printf("\n"); libff::print_indent(); libff::print_mem("after verifier");
    printf("* The verification result is: %s\n", (ans ? "PASS" : "FAIL"));

    libff::print_header("R1CS GG-ppzkSNARK Online Verifier");
    const bool ans2 = r1cs_gg_ppzksnark_online_verifier_strong_IC<ppT>(pvk, pb.primary_input(), proof2);
    assert(ans == ans2);



  cout << "Number of R1CS constraints: " << constraint_system.num_constraints() << endl;
//   cout << "Primary (public) input: " << pb.primary_input() << endl;
//   cout << "Auxiliary (private) input: " << pb.auxiliary_input() << endl;
  cout << "Verification status: " << ans << endl;
  cout<<"The verifier answer is "<<ans<<" "<<ans2<<"\n";

  cout<<"Saving primary input\n";
    stringstream primary_input;
    primary_input<<pb.primary_input();
    ofstream fileOut2;
    fileOut2.open("verifier_primary_input");
    fileOut2<<primary_input.rdbuf();
    fileOut2.close();




    return 0;
}
