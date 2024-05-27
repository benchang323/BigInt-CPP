#include <cassert>
#include "bigint.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>

using std::cout;
using std::endl;
using std::hex;
using std::stringstream;
using std::vector;

/* DEFAULT CONSTRUCTOR */
BigInt::BigInt()
{
  magnitude.push_back(0);
  isNeg = false;
}

/* CONSTRUCTOR */
BigInt::BigInt(uint64_t val, bool negative)
{
  magnitude.push_back(val);
  isNeg = negative;
}

/* CONSTRUCTOR */
BigInt::BigInt(std::initializer_list<uint64_t> vals, bool negative)
    : magnitude(vals), isNeg(negative) // Initializer
{
  // No code needed
}

/* COPY CONSTRUCTOR */
BigInt::BigInt(const BigInt &other)
    : magnitude(other.magnitude), isNeg(other.isNeg)
{
  // No code needed
}

/* DESTRUCTOR */
BigInt::~BigInt()
{
  // No code needed
}

/* GET VECTOR */
const std::vector<uint64_t> &BigInt::get_bit_vector() const
{
  return magnitude;
}

/* GET BITS */
uint64_t BigInt::get_bits(unsigned index) const
{
  // Edge Case: OOB
  if (index >= magnitude.size())
  {
    return 0;
  }

  return this->magnitude[index];
}

/* GET NEGATIVITY */
bool BigInt::is_negative() const
{
  return this->isNeg; // isNeg is true if negative
}

/* UNARY MINUS */
BigInt BigInt::operator-() const
{
  // Edge Case: 0
  if (magnitude.size() == 1 && magnitude[0] == 0)
  {
    return BigInt(*this);
  }

  BigInt temp = BigInt(*this);
  temp.isNeg = !temp.isNeg;
  return temp;
}

/* CONVERT VECTOR TO HEX */
std::string BigInt::to_hex() const
{
  stringstream ss;

  // ss in hex mode
  ss << hex << std::setfill('0');

  // Check negativity
  if (isNeg)
  {
    ss << '-';
  }

  // First non-zero tracker
  bool found = false;

  // Edge Case: BigInt is 0
  if (magnitude.empty() || (magnitude.size() == 1 && magnitude[0] == 0))
  {
    return "0";
  }

  // Iterate vector
  for (auto i = magnitude.rbegin(); i != magnitude.rend(); ++i)
  {
    // Skip leading 0s
    if (!found)
    {
      if (*i == 0)
      {
        continue;
      }
      found = true;
    }

    // First element doesn't need padding
    if (i == magnitude.rbegin())
    {
      ss << *i;
    }
    else
    {
      ss << std::setw(16) << *i;
    }
  }

  // Return stream as string
  return ss.str();
}

///////////////////////////////////////////////////////////////////
/////////////////////////* MILESTONE 2 *//////////////////////////
//////////////////////////////////////////////////////////////////

/* ADDITION */
BigInt BigInt::operator+(const BigInt &rhs) const
{

  const vector<uint64_t> lmagnitude = this->magnitude; // storing lhs vector

  bool lneg = this->isNeg;
  BigInt sum;

  if (!lneg && !rhs.isNeg) // same positive operands -> a+b
  {
    sum.isNeg = this->isNeg;

    sum.magnitude = add(lmagnitude, rhs.magnitude);

    return sum;
  }
  if (!lneg && rhs.isNeg) // opposite sign operands -> a+-b = a-b
  {
    // TODO: depends on which magnitude is bigger
    sum.magnitude = subtract(lmagnitude, rhs.magnitude, sum.isNeg);
    return sum;
  }
  if (lneg && !rhs.isNeg) // opposite sign operands -> -a+b = b-a
  {
    // TODO: depends on which magnitude is bigger
    sum.magnitude = subtract(rhs.magnitude, lmagnitude, sum.isNeg);
    return sum;
  }
  // lneg && rhs.isNeg
  // both negative operands -> a+b and set to negative
  sum.isNeg = this->isNeg;
  sum.magnitude = add(lmagnitude, rhs.magnitude);
  return sum;
}

/* ADDITION - HELPER */
vector<uint64_t> BigInt::add(const vector<uint64_t> leftMag, const vector<uint64_t> rightMag) const
{

  vector<uint64_t> sumMag;
  const vector<uint64_t> &smallMag = rightMag.size() < leftMag.size() ? rightMag : leftMag; // determine smaller magnitude
  const vector<uint64_t> &largeMag = rightMag.size() < leftMag.size() ? leftMag : rightMag; // determine larger magnitude
  uint64_t carry = 0UL;

  for (int i = 0; i < (int)largeMag.size(); ++i) // iterate fully through largeMag to ensure everything is added
  {
    uint64_t largeMagVal = largeMag[i];
    uint64_t smallMagVal = i < (int)smallMag.size() ? smallMag[i] : 0;
    uint64_t totalSum = largeMagVal + smallMagVal + carry;
    if (totalSum < largeMagVal) // overflow detected, need to carry to next operation
    {
      carry = 1UL;
    }
    else // no overflow, no need to carry
    {
      carry = 0UL;
    }
    sumMag.push_back(totalSum);
  }
  if (carry) // add the remaining carry
  {
    sumMag.push_back(carry);
  }

  return sumMag;
}

/* SUBTRACTION */
BigInt BigInt::operator-(const BigInt &rhs) const
{
  const vector<uint64_t> lmagnitude = this->magnitude; // storing lhs magnitude

  bool lneg = this->isNeg; // storing lhs leg
  BigInt difference;

  if (!lneg && !rhs.isNeg)
  {
    difference.magnitude = subtract(lmagnitude, rhs.magnitude, difference.isNeg); // same positive operands -> a-b
    return difference;
  }
  if (!lneg and rhs.isNeg)
  {
    difference.magnitude = add(lmagnitude, rhs.magnitude); // opposite sign operands so we add -> a-(-b) = a+b
    difference.isNeg = false;
    return difference;
  }
  if (lneg and rhs.isNeg)
  {
    difference.magnitude = subtract(rhs.magnitude, lmagnitude, difference.isNeg); // same negative operands -> -a-(-b) = b-a
    return difference;
  }

  // lneg and !rhs.isNeg

  difference.magnitude = add(lmagnitude, rhs.magnitude); // opposite sign operands so we add -> -a - b
  difference.isNeg = true;
  return difference;
}

/* SUBTRACTION - HELPER */
vector<uint64_t> BigInt::subtract(const vector<uint64_t> leftMag, const vector<uint64_t> rightMag, bool &isNeg) const
{
  vector<uint64_t> diffMag;
  if (compare_mag(leftMag, rightMag) == 1) // use compare mag to determine whether or not the operation will end up with a negative value
  {
    isNeg = false;
  }
  else if (compare_mag(leftMag, rightMag) == -1)
  {
    isNeg = true;
  }
  else if (compare_mag(leftMag, rightMag) == 0)
  {
    isNeg = false;
  }

  // assigning vectors based on isNeg, as isNeg is true if the left magnitude is smaller
  const vector<uint64_t> &smallMag = isNeg ? leftMag : rightMag;
  const vector<uint64_t> &largeMag = isNeg ? rightMag : leftMag;

  uint64_t borrow = 0;
  for (size_t i = 0; i < largeMag.size(); ++i) // iterate fully through largeMag to ensure everything is subtracted
  {
    uint64_t smallMagVal = i < smallMag.size() ? smallMag[i] : 0; // if we have subtracted out all of the smaller magnitude, give default value of 0
    uint64_t subtrahend = smallMagVal + borrow;

    // If we need to borrow and the current digit of largeMag is 0 or subtrahend is larger than the current digit,
    // perform the borrow operation.
    if (largeMag[i] < subtrahend)
    {
      borrow = 1;
      // Use uint64_t max value + 1 to simulate borrowing in unsigned arithmetic
      diffMag.push_back(largeMag[i] + (UINT64_MAX - subtrahend) + 1);
    }
    else
    {
      borrow = 0;
      diffMag.push_back(largeMag[i] - subtrahend);
    }
  }
  return diffMag;
}

/* IS BIT SET */
bool BigInt::is_bit_set(unsigned n) const
{
  // Calculate the index for the array and the bit position within that index.
  unsigned index = n / 64;  // vector index
  unsigned bitPos = n % 64; // position of bit

  // Check if the index is within the bounds of the internal array.
  if (index >= magnitude.size())
  {
    return false; // Out of bounds, the bit is not set.
  }
  // finding byte
  uint64_t byte = magnitude[index];

  // Check if the bit at bitPos is set in the byte.
  // Shift a 1 to the correct bit position and perform bitwise AND.
  uint64_t testingbit = 1;
  return (byte & (testingbit << bitPos)) != 0;
}

/* LEFT SHIFT */
BigInt BigInt::operator<<(unsigned n) const
{
  if (this->isNeg)
  {
    throw std::invalid_argument("Left shifting a negative value is not allowed");
  }
  int shiftAmount = n / 64; // amount to increase vector size
  int leftOver = n % 64;    // need to handle the leftover bits if number is not a multiple of 64
  BigInt shiftedResult;

  shiftedResult.magnitude.resize(magnitude.size() + shiftAmount, 0);
  for (int i = shiftAmount; i < (int)shiftedResult.magnitude.size(); i++) // copying old magnitude values into new shifted magnitude
  {
    shiftedResult.magnitude[i] = magnitude[i - shiftAmount];
  }

  if (leftOver) // if the number of bits shifted is not a multiple of 64
  {
    uint64_t carry = 0;
    for (int i = 0; i < (int)shiftedResult.magnitude.size(); i++)
    {
      uint64_t nextCarry = shiftedResult.magnitude[i] >> (64 - leftOver);            // the overflowed bits that we have to add to the next most significant bits
      shiftedResult.magnitude[i] = (shiftedResult.magnitude[i] << leftOver) | carry; // shift by the leftover including the carry
      carry = nextCarry;                                                             // updating carry
    }
    if (carry) // add the left over carry
    {
      shiftedResult.magnitude.push_back(carry);
    }
  }
  return shiftedResult;
}

/* ASSIGNMENT */
BigInt &BigInt::operator=(const BigInt &rhs)
{
  // Deep copy rhs into current object
  if (this != &rhs)
  {
    this->magnitude = rhs.magnitude;
    this->isNeg = rhs.isNeg;
  }
  return *this;
}

/* COMPARISON */
int BigInt::compare(const BigInt &rhs) const
{
  // Check negativity - 4 cases: sign/size comparison

  if (this->isNeg != rhs.isNeg)
  {
    return this->isNeg ? -1 : 1;
  }
  int compared_val = compare_mag(this->magnitude, rhs.magnitude);
  if (this->isNeg)
  {
    compared_val *= -1;
  }
  return compared_val;
}

/* COMPARISON - HELPER */
int BigInt::compare_mag(const vector<uint64_t> lhs, const vector<uint64_t> rhs) const
{
  vector<uint64_t> leftVec;
  vector<uint64_t> rightVec;
  for (int i = 0; i <(int)  lhs.size(); i++)
  {
    leftVec.push_back(lhs[i]);
  }
  for (int i = 0; i < (int) rhs.size(); i++)
  {
    rightVec.push_back(rhs[i]);
  }
  while (leftVec[leftVec.size() - 1] == 0)
  {
    leftVec.pop_back();
  }
  while (rightVec[rightVec.size() - 1] == 0)
  {
    rightVec.pop_back();
  }
  if (leftVec.size() > rightVec.size())
  {
    return 1;
  }
  else if (leftVec.size() < rightVec.size())
  {
    return -1;
  }
  else
  { // Sizes are equal; compare each block from most to least significant
    for (int i = leftVec.size() - 1; i >= 0; --i)
    {
      if (leftVec[i] > rightVec[i])
      {
        return 1;
      }
      else if (leftVec[i] < rightVec[i])
      {
        return -1;
      }
    }
    return 0; // Magnitudes are equal
  }
}

/* MULTIPLICATION */
BigInt BigInt::operator*(const BigInt &rhs) const
{
  // Edge Case
  if (magnitude.empty() || (magnitude.size() == 1 && magnitude[0] == 0) || rhs.magnitude.empty() || (rhs.magnitude.size() == 1 && rhs.magnitude[0] == 0))
  {
    return BigInt(0);
  }

  BigInt product;

  // Set negativity
  product.isNeg = (isNeg != rhs.isNeg);

  // Multiplication
  product.magnitude = multiply(*this, rhs).magnitude;

  return product;
}

/* MULTIPLICATION - HELPER */
BigInt BigInt::multiply(const BigInt &left, const BigInt &right) const
{
  BigInt product;

  // Drop signs
  BigInt leftCopy = left;
  leftCopy.isNeg = false;
  BigInt rightCopy = right;

  // Break right BigInt into powers of 2 components
  for (int i = 0; i < (int)right.magnitude.size() * 64; i++)
  {
    if (rightCopy.is_bit_set(i))
    {
      // Multiply left BigInt by 2^i
      BigInt powOf2 = leftCopy << i;

      // Add to product
      product = product + powOf2;
    }
  }

  return product;
}

/* DIVISION - HELPER */
BigInt BigInt::divideByTwo(const BigInt &val) const
{
    // Variables for iteration
    BigInt res = val;
    uint64_t carry = 0;

    // Iterate through magnitude to shift right
    for (int i = res.magnitude.size() - 1; i >= 0; --i)
    {
        // Save least significant bit
        uint64_t nextCarry = res.magnitude[i] & 1;

        // Right shift
        res.magnitude[i] >>= 1;

        // Add carry to most significant bit
        res.magnitude[i] |= (carry << 63);

        // Update carry
        carry = nextCarry;
    }

    return res;
}

/* DIVSION */
BigInt BigInt::operator/(const BigInt &rhs) const
{
  // Edge Case
  if (rhs.magnitude.empty() || (rhs.magnitude.size() == 1 && rhs.magnitude[0] == 0))
  {
    throw std::invalid_argument("Division by zero");
  }

  // Variables for binary search + drop signs
  BigInt dividend = *this;
  dividend.isNeg = false;
  BigInt divisor = rhs;
  BigInt low(0);
  divisor.isNeg = false;
  BigInt high = dividend + BigInt(1);

  // Binary Search
  while (low + BigInt(1) < high)
  {
    // Midpoint and current product
    BigInt mid = divideByTwo(low + high);
    BigInt product = divisor * mid;

    if (product > dividend)
    {
      // Update high
      high = mid;
    }
    else if (product < dividend)
    {
      // Avoid infinite loop
      if (low == mid)
      {
        break;
      }
      // Update low
      low = mid;
    }
    else // Found match
    {
      low = mid;
      break;
    }
  }

  // Assign negativity
  BigInt result = low;
  result.isNeg = isNeg != rhs.isNeg;
  if (result.magnitude.size() == 1 && result.magnitude[0] == 0)
  {
    result.isNeg = false;
  }
  return result;
}

/* TO DECIMAL */
std::string BigInt::to_dec() const
{
  // Edge Case
  if (magnitude.size() == 1 && magnitude[0] == 0)
  {
    return "0";
  }

  // String stream
  std::stringstream ss;

  // Variables
  std::string result;
  std::string temp;

  // Copy BigInt to modify
  BigInt copy = *this;

  // Set negativity and drop sign
  bool isNeg = copy.isNeg;
  copy.isNeg = false;

  BigInt zero(0);

  // Convert to base 10
  while (copy > zero)
  {
    // Repeatedly divide by 10
    BigInt base10 = copy / 10;

    // Append remainder
    temp = std::to_string((copy - (base10 * 10)).get_bits(0));

    // Store to stream and update copy
    ss << temp;
    copy = base10;
  }

  // Reverse string
  result = ss.str();
  std::reverse(result.begin(), result.end());

  // Assign negativity
  if (isNeg)
  {
    result.insert(result.begin(), '-');
  }

  return result;
}
