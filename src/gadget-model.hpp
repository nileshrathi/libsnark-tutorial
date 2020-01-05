#include "libsnark/gadgetlib1/gadget.hpp"

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
//   digest_variable<FieldT> hash_bits(pb, SHA256_digest_size, "hash_bits");
//   digest_variable<FieldT> left_bits(pb, SHA256_digest_size, "left_bits");
//   pb_variable<FieldT> y;
//   pb_variable<FieldT> sym_2;
public:
  const pb_variable<FieldT> out;
  const pb_variable<FieldT> x;
  const pb_variable<FieldT> b;
  //const libff::bit_vector hx;

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
  }

  void generate_r1cs_constraints()
  {
    // // x*x = sym_1
    // this->pb.add_r1cs_constraint(r1cs_constraint<FieldT>(x, x, sym_1));

    // // sym_1 * x = y
    // this->pb.add_r1cs_constraint(r1cs_constraint<FieldT>(sym_1, x, y));

    // // y + x = sym_2
    // this->pb.add_r1cs_constraint(r1cs_constraint<FieldT>(y + x, 1, sym_2));

    // // sym_2 + 5 = ~out
    // this->pb.add_r1cs_constraint(r1cs_constraint<FieldT>(sym_2 + 5, 1, out));



    //b-x=sym_1
    
    this->pb.add_r1cs_constraint(r1cs_constraint<FieldT>(b-x, 1, sym_1));


    //sym1 - out = 0
    this->pb.add_r1cs_constraint(r1cs_constraint<FieldT>(sym_1-out, 1, 0));
    cout<<"out is "<<this->pb.val(out)<<endl;
    cout<<"x is "<<this->pb.val(x)<<endl;
    cout<<"b is "<<this->pb.val(b)<<endl;

  }

  void generate_r1cs_witness()
  {



    // this->pb.val(sym_1) = this->pb.val(x) * this->pb.val(x);
    // this->pb.val(y) = this->pb.val(sym_1) * this->pb.val(x);
    // this->pb.val(sym_2) = this->pb.val(y) + this->pb.val(x);
    this->pb.val(sym_1) = this->pb.val(b) - this->pb.val(x);


  }
};
