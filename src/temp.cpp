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


bit_vector int_list_to_bits(const std::vector<unsigned long> l, const size_t wordsize)
{
    bit_vector res(wordsize*l.size());
    for (size_t i = 0; i < l.size(); ++i)
    {
        for (size_t j = 0; j < wordsize; ++j)
        {
            res[i*wordsize + j] = (*(l.begin()+i) & (1ul<<(wordsize-1-j)));
        }
    }
    return res;
}


int main()
{

    libff::bit_vector int_bv=convert(7);
    for(unsigned int i=0;i<int_bv.size();i++)
    {
        cout<<int_bv[i];
    }

    cout<<endl;
    cout<<int_bv.size();


    // //const std::initializer_list<unsigned long> data = ;//{0,1,1,1};
    // const libff::bit_vector left_bv  = int_list_to_bits(int_bv,1);


    // cout<<left_bv.size();

    // libff::serialize_bit_vector(cout,left_bv);


    return 0;

}