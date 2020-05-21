#include <fc/uint256.hpp>
#include <fc/variant.hpp>
#include <fc/crypto/bigint.hpp>
#include <boost/multiprecision/cpp_int.hpp>

#include <stdexcept>
#include "byteswap.hpp"

namespace fc
{
   typedef boost::multiprecision::uint128_t  m128;
   typedef boost::multiprecision::uint256_t  m256;

   template <typename T>
   static void divide(const T &numerator, const T &denominator, T &quotient, T &remainder)
   {
      static const int bits = sizeof(T) * 8;//CHAR_BIT;

      if(denominator == 0) {
         throw std::domain_error("divide by zero");
      } else {
         T n      = numerator;
         T d      = denominator;
         T x      = 1;
         T answer = 0;

         while((n >= d) && (((d >> (bits - 1)) & 1) == 0)) {
            x <<= 1;
            d <<= 1;
         }

         while(x != 0) {
            if(n >= d) {
               n -= d;
               answer |= x;
            }

            x >>= 1;
            d >>= 1;
         }

         quotient = answer;
         remainder = n;
      }
   }

   uint256::uint256(const std::string &sz)
         :lo(0), hi(0)
   {
      // do we have at least one character?
      if(!sz.empty()) {
         // make some reasonable assumptions
         int radix = 10;
         bool minus = false;

         std::string::const_iterator i = sz.begin();

         // check for minus sign, i suppose technically this should only apply
         // to base 10, but who says that -0x1 should be invalid?
         if(*i == '-') {
            ++i;
            minus = true;
         }

         // check if there is radix changing prefix (0 or 0x)
         if(i != sz.end()) {
            if(*i == '0') {
               radix = 8;
               ++i;
               if(i != sz.end()) {
                  if(*i == 'x') {
                     radix = 16;
                     ++i;
                  }
               }
            }

            while(i != sz.end()) {
               unsigned int n = 0;
               const char ch = *i;

               if(ch >= 'A' && ch <= 'Z') {
                  if(((ch - 'A') + 10) < radix) {
                     n = (ch - 'A') + 10;
                  } else {
                     break;
                  }
               } else if(ch >= 'a' && ch <= 'z') {
                  if(((ch - 'a') + 10) < radix) {
                     n = (ch - 'a') + 10;
                  } else {
                     break;
                  }
               } else if(ch >= '0' && ch <= '9') {
                  if((ch - '0') < radix) {
                     n = (ch - '0');
                  } else {
                     break;
                  }
               } else {
                  /* completely invalid character */
                  break;
               }

               (*this) *= radix;
               (*this) += n;

               ++i;
            }
         }

         // if this was a negative number, do that two's compliment madness :-P
         if(minus) {
            *this = -*this;
         }
      }
   }

   uint256::operator std::string ()const
   {
      if(*this == 0) { return "0"; }

      // at worst it will be size digits (base 2) so make our buffer
      // that plus room for null terminator
      static char sz [256 + 1];
      sz[sizeof(sz) - 1] = '\0';

      uint256 ii(*this);
      int i = 256 - 1;

      while (ii != 0 && i) {

         uint256 remainder;
         divide(ii, uint256(10), ii, remainder);
         sz [--i] = "0123456789abcdefghijklmnopqrstuvwxyz"[remainder.to_integer()];
      }

      return &sz[i];
   }

   constexpr uint256& uint256::operator<<=(const uint32_t& rhs)
   {
      if(rhs >= 256)
      {
         hi = 0;
         lo = 0;
      }
      else
      {
         auto self = (m256((__uint128_t)hi) << 128) + m256((__uint128_t)lo);
         self <<= rhs;
         hi = __uint128_t(static_cast<__uint128_t>(self >> 128));
         lo = __uint128_t(static_cast<__uint128_t>((self << 128 ) >> 128));
      }

      return *this;
   }

   constexpr uint256 & uint256::operator>>=(const uint32_t& rhs)
   {
      if(rhs >= 256)
      {
         hi = 0;
         lo = 0;
      }
      else
      {
         auto self = (m256((__uint128_t)hi) << 128) + m256((__uint128_t)lo);
         self >>= rhs;
         hi = __uint128_t(static_cast<__uint128_t>(self >> 128));
         lo = __uint128_t(static_cast<__uint128_t>((self << 128) >> 128));
      }
      return *this;
   }

   uint256& uint256::operator/=(const uint256 &b)
   {
      auto self = (m256((__uint128_t)hi) << 128) + m256((__uint128_t)lo);
      auto other = (m256((__uint128_t)b.hi) << 128) + m256((__uint128_t)b.lo);
      self /= other;
      hi = __uint128_t(static_cast<__uint128_t>(self >> 128));
      lo = __uint128_t(static_cast<__uint128_t>((self << 128 ) >> 128));

      return *this;
   }

   uint256& uint256::operator%=(const uint256 &b)
   {
      uint256 quotient;
      divide(*this, b, quotient, *this);
      return *this;
   }

   uint256& uint256::operator*=(const uint256 &b)
   {
      auto self = (m256((__uint128_t)hi) << 128) + m256((__uint128_t)lo);
      auto other = (m256((__uint128_t)b.hi) << 128) + m256((__uint128_t)b.lo);
      self *= other;
      hi = __uint128_t(static_cast<__uint128_t>(self >> 128));
      lo = __uint128_t(static_cast<__uint128_t>((self << 128 ) >> 128));

      return *this;
   }

   static uint8_t _popcount_64( uint64_t x )
   {
      static const uint64_t m[] = {
            0x5555555555555555ULL,
            0x3333333333333333ULL,
            0x0F0F0F0F0F0F0F0FULL,
            0x00FF00FF00FF00FFULL,
            0x0000FFFF0000FFFFULL,
            0x00000000FFFFFFFFULL
      };
      // TODO future optimization:  replace slow, portable version
      // with fast, non-portable __builtin_popcountll intrinsic
      // (when available)

      for( int i=0, w=1; i<6; i++, w+=w )
      {
         x = (x & m[i]) + ((x >> w) & m[i]);
      }
      return uint8_t(x);
   }

   uint8_t uint256::popcount()const
   {
      return _popcount_64( lo & 0xFFFFFFFFFFFFFFFFull ) + _popcount_64( (lo >> 64) ) + _popcount_64( hi & 0xFFFFFFFFFFFFFFFFull ) + _popcount_64( (hi >> 64) );
   }

   void to_variant( const uint256& var,  variant& vo )  { vo = std::string(var);         }
   void from_variant( const variant& var,  uint256& vo ){ vo = uint256(var.as_string()); }
} // namespace fc
