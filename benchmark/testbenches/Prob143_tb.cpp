#include <iostream>
#include <cstdlib>
#include <ap_int.h>

// DUT
void TopModule(
    bool        reset,
    ap_uint<3>  s,
    bool       &fr3,
    bool       &fr2,
    bool       &fr1,
    bool       &dfr
);

// Reference model
enum States { A2=0, B1=1, B2=2, C1=3, C2=4, D1=5 };

void RefModel(
    bool        reset,
    ap_uint<3>  s,
    bool       &fr3_ref,
    bool       &fr2_ref,
    bool       &fr1_ref,
    bool       &dfr_ref,
    States     &state_ref
) {
    switch (state_ref) {
      case A2: fr3_ref=1; fr2_ref=1; fr1_ref=1; dfr_ref=1; break;
      case B1: fr3_ref=0; fr2_ref=1; fr1_ref=1; dfr_ref=0; break;
      case B2: fr3_ref=0; fr2_ref=1; fr1_ref=1; dfr_ref=1; break;
      case C1: fr3_ref=0; fr2_ref=0; fr1_ref=1; dfr_ref=0; break;
      case C2: fr3_ref=0; fr2_ref=0; fr1_ref=1; dfr_ref=1; break;
      case D1: fr3_ref=0; fr2_ref=0; fr1_ref=0; dfr_ref=0; break;
      default: fr3_ref=0; fr2_ref=0; fr1_ref=0; dfr_ref=0; break;
    }

    if (reset) {
        state_ref = A2;
    } else {
        States next = state_ref;
        switch (state_ref) {
          case A2:
            if (s[0])      next = B1;
            else           next = A2;
            break;
          case B1:
            if (s[1])      next = C1;
            else if (s[0]) next = B1;
            else           next = A2;
            break;
          case B2:
            if (s[1])      next = C1;
            else if (s[0]) next = B2;
            else           next = A2;
            break;
          case C1:
            if (s[2])      next = D1;
            else if (s[1]) next = C1;
            else           next = B2;
            break;
          case C2:
            if (s[2])      next = D1;
            else if (s[1]) next = C2;
            else           next = B2;
            break;
          case D1:
            if (s[2])      next = D1;
            else           next = C2;
            break;
        }
        state_ref = next;
    }
}

int main() {
    const int TOTAL = 2000;
    bool reset, fr3, fr2, fr1, dfr;
    bool fr3_ref, fr2_ref, fr1_ref, dfr_ref;
    ap_uint<3> s;
    States state_ref = A2;
    int cycleCount = 0, errorCount = 0;

    std::srand(0);

    // Reset for 2 cycles before normal stimulus
    for (int i = 0; i < 2; i++) {
        TopModule(true, 0, fr3, fr2, fr1, dfr);
        RefModel(true, 0, fr3_ref, fr2_ref, fr1_ref, dfr_ref, state_ref);
    }

    // Deterministic sanity checks to break circular verification dependency
    // Post-reset: state=A2, outputs must be all 1
    TopModule(false, 0, fr3, fr2, fr1, dfr);
    if (fr3!=1 || fr2!=1 || fr1!=1 || dfr!=1) {
        std::cout<<"Sanity FAIL: A2 state expected fr3=fr2=fr1=dfr=1"<<std::endl;
        errorCount++;
    }
    // Drive A2->B1->C1->D1, check D1 outputs = all 0
    TopModule(false, 0x1, fr3, fr2, fr1, dfr); // A2->B1
    TopModule(false, 0x3, fr3, fr2, fr1, dfr); // B1->C1
    TopModule(false, 0x7, fr3, fr2, fr1, dfr); // C1->D1
    TopModule(false, 0x7, fr3, fr2, fr1, dfr); // D1 outputs
    if (fr3!=0 || fr2!=0 || fr1!=0 || dfr!=0) {
        std::cout<<"Sanity FAIL: D1 state expected fr3=fr2=fr1=dfr=0"<<std::endl;
        errorCount++;
    }

    // Reset before random loop
    for (int i = 0; i < 2; i++) {
        TopModule(true, 0, fr3, fr2, fr1, dfr);
        RefModel(true, 0, fr3_ref, fr2_ref, fr1_ref, dfr_ref, state_ref);
    }

    for (int i = 0; i < TOTAL; ++i) {
        reset = ((std::rand() & 15) == 0);
        s = std::rand() & 0x7;

        TopModule(reset, s, fr3, fr2, fr1, dfr);
        RefModel(reset, s, fr3_ref, fr2_ref, fr1_ref, dfr_ref, state_ref);

        if (fr3 != fr3_ref) {
            std::cout<<"Mismatch at cycle "<<cycleCount
                     <<": expected fr3="<<fr3_ref
                     <<", got="<<fr3<<std::endl; errorCount++;
        }
        if (fr2 != fr2_ref) {
            std::cout<<"Mismatch at cycle "<<cycleCount
                     <<": expected fr2="<<fr2_ref
                     <<", got="<<fr2<<std::endl; errorCount++;
        }
        if (fr1 != fr1_ref) {
            std::cout<<"Mismatch at cycle "<<cycleCount
                     <<": expected fr1="<<fr1_ref
                     <<", got="<<fr1<<std::endl; errorCount++;
        }
        if (dfr != dfr_ref) {
            std::cout<<"Mismatch at cycle "<<cycleCount
                     <<": expected dfr="<<dfr_ref
                     <<", got="<<dfr<<std::endl; errorCount++;
        }
        cycleCount++;
    }

    if (errorCount==0) {
        std::cout<<"Test Passed: All "<<cycleCount<<" test cases matched."<<std::endl;
    } else {
        std::cout<<"Test Failed: "<<errorCount
                 <<" mismatches detected out of "<<cycleCount
                 <<" cases."<<std::endl;
    }
    return 0;
}
