#include "PPintrin.h"

// implementation of absSerial(), but it is vectorized using PP intrinsics
void absVector(float *values, float *output, int N)
{
  __pp_vec_float x;
  __pp_vec_float result;
  __pp_vec_float zero = _pp_vset_float(0.f);
  __pp_mask maskAll, maskIsNegative, maskIsNotNegative;

  //  Note: Take a careful look at this loop indexing.  This example
  //  code is not guaranteed to work when (N % VECTOR_WIDTH) != 0.
  //  Why is that the case?
  for (int i = 0; i < N; i += VECTOR_WIDTH)
  {

    // All ones
    maskAll = _pp_init_ones();

    // All zeros
    maskIsNegative = _pp_init_ones(0);

    // Load vector of values from contiguous memory addresses
    _pp_vload_float(x, values + i, maskAll); // x = values[i];

    // Set mask according to predicate
    _pp_vlt_float(maskIsNegative, x, zero, maskAll); // if (x < 0) {

    // Execute instruction using mask ("if" clause)
    _pp_vsub_float(result, zero, x, maskIsNegative); //   output[i] = -x;

    // Inverse maskIsNegative to generate "else" mask
    maskIsNotNegative = _pp_mask_not(maskIsNegative); // } else {

    // Execute instruction ("else" clause)
    _pp_vload_float(result, values + i, maskIsNotNegative); //   output[i] = x; }

    // Write results back to memory
    _pp_vstore_float(output + i, result, maskAll);
  }
}

void clampedExpVector(float *values, int *exponents, float *output, int N)
{
  //
  // PP STUDENTS TODO: Implement your vectorized version of
  // clampedExpSerial() here.
  //
  // Your solution should work for any value of
  // N and VECTOR_WIDTH, not just when VECTOR_WIDTH divides N
  //
  __pp_vec_float x;
  __pp_vec_int y;
  __pp_vec_int zero = _pp_vset_int(0);
  __pp_vec_int intone = _pp_vset_int(1);
  __pp_vec_int count;
  __pp_vec_float result;
  __pp_vec_float floatone = _pp_vset_float(1.f);
  __pp_vec_float clamped = _pp_vset_float(9.999999f);
  __pp_mask maskAll, maskIsZero, maskIsNotZero, greater, maskIsClamped;
  
  int n=0;
  int limit=N;
  if(N%VECTOR_WIDTH!=0){
    limit = N-VECTOR_WIDTH;
    n=N%VECTOR_WIDTH;
  }

  for (int i = 0; i < limit; i += VECTOR_WIDTH)
  {
    maskAll = _pp_init_ones();
    maskIsZero = _pp_init_ones(0);
    greater = _pp_init_ones(0);
    maskIsClamped = _pp_init_ones(0);
    _pp_vload_float(x, values + i, maskAll);
    _pp_vload_int(y, exponents + i, maskAll);
    _pp_veq_int(maskIsZero, y, zero, maskAll);
    _pp_vstore_float(output + i, floatone, maskIsZero);

    maskIsNotZero = _pp_mask_not(maskIsZero);
    _pp_vload_float(result, values + i, maskIsNotZero);
    _pp_vsub_int(count, y, intone, maskIsNotZero);


    _pp_vgt_int(greater, count, zero, maskIsNotZero);
    while(_pp_cntbits(greater)){
      _pp_vmult_float(result, result, x, greater);
      _pp_vsub_int(count, count, intone, maskAll);
      _pp_vgt_int(greater, count, zero, maskIsNotZero);
    }
    _pp_vgt_float(maskIsClamped, result, clamped, maskIsNotZero);
    _pp_vstore_float(output + i, result, maskIsNotZero);
    _pp_vstore_float(output + i, clamped, maskIsClamped);
  }
  if(n==0)
    return;

  for (int i = N-n; i < N; i++)
  {
    float x = values[i];
    int y = exponents[i];
    if (y == 0)
    {
      output[i] = 1.f;
    }
    else
    {
      float result = x;
      int count = y - 1;
      while (count > 0)
      {
        result *= x;
        count--;
      }
      if (result > 9.999999f)
      {
        result = 9.999999f;
      }
      output[i] = result;
    }
  }
}

// returns the sum of all elements in values
// You can assume N is a multiple of VECTOR_WIDTH
// You can assume VECTOR_WIDTH is a power of 2
float arraySumVector(float *values, int N)
{
  //
  // PP STUDENTS TODO: Implement your vectorized version of arraySumSerial here
  //
  __pp_vec_float result = _pp_vset_float(0.f);
  __pp_vec_float tmp;
  __pp_mask maskAll = _pp_init_ones();

  for (int i = 0; i < N; i += VECTOR_WIDTH)
  {
    _pp_vload_float(tmp, values+i, maskAll);
    _pp_vadd_float(result, result, tmp, maskAll);
  }
  int w=VECTOR_WIDTH;
  while(w!=1){
    _pp_hadd_float(result, result);
    _pp_vmove_float(tmp, result, maskAll);
    _pp_interleave_float(result, tmp);
    w=w>>1;
  }
  return result.value[0];
}