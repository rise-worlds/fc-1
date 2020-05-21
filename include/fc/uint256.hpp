#pragma once
#include <limits>
#include <stdint.h>
#include <string>

#include <fc/exception/exception.hpp>
#include <fc/crypto/city.hpp>
#include <fc/uint128.hpp>

#ifdef _MSC_VER
#pragma warning (push)
  #pragma warning (disable : 4244)
#endif //// _MSC_VER

namespace fc
{
   class bigint;
   class uint128;
   /**
    *  @brief an implementation of 256 bit unsigned integer
    *
    */
   class uint256
   {
   public:
      constexpr uint256():lo(0),hi(0){}
      constexpr uint256( int32_t l ):lo(l),hi(-(l<0)){}
      constexpr uint256( uint32_t l ):lo(l),hi(0){}
      constexpr uint256( int64_t l ):lo(l),hi(-(l<0)){}
      constexpr uint256( uint64_t l ):lo(l),hi(0){}
      constexpr uint256( __uint128_t l ):lo(l),hi(0){}
      constexpr uint256( __uint128_t _h, __uint128_t _l ):lo(_l),hi(_h){}
      constexpr uint256( const uint256& i ):lo( i.lo ), hi(i.hi){ }
      uint256( const std::string& s );

      explicit operator std::string()const;

      bool     operator == ( const uint256& o )const{ return hi == o.hi && lo == o.lo;             }
      bool     operator != ( const uint256& o )const{ return hi != o.hi || lo != o.lo;             }
      bool     operator < ( const uint256& o )const { return (hi == o.hi) ? lo < o.lo : hi < o.hi; }
      bool     operator < ( const __uint128_t& o )const { return *this < uint256(o); }
      bool     operator < ( const int64_t& o )const { return *this < uint256(o); }
      bool     operator !()const                    { return !(hi !=0 || lo != 0);                 }
      uint256  operator -()const                    { return ++uint256( ~hi, ~lo );                }
      uint256  operator ~()const                    { return uint256( ~hi, ~lo );                  }

      uint256& operator++()    {  hi += (++lo == 0); return *this; }
      uint256& operator--()    {  hi -= (lo-- == 0); return *this; }
      uint256  operator++(int) { auto tmp = *this; ++(*this); return tmp; }
      uint256  operator--(int) { auto tmp = *this; --(*this); return tmp; }

      constexpr uint256& operator |= ( const uint256& u ) { hi |= u.hi; lo |= u.lo; return *this; }
      constexpr uint256& operator &= ( const uint256& u ) { hi &= u.hi; lo &= u.lo; return *this; }
      constexpr uint256& operator ^= ( const uint256& u ) { hi ^= u.hi; lo ^= u.lo; return *this; }
      constexpr uint256& operator <<= ( const uint32_t& u );
      constexpr uint256& operator >>= ( const uint32_t& u );

      uint256& operator += ( const uint256& u ) { const __uint128_t old = lo; lo += u.lo;  hi += u.hi + (lo < old); return *this; }
      uint256& operator -= ( const uint256& u ) { return *this += -u; }
      uint256& operator *= ( const uint256& u );
      uint256& operator /= ( const uint256& u );
      uint256& operator %= ( const uint256& u );


      friend uint256 operator + ( const uint256& l, const uint256& r )   { return uint256(l)+=r;   }
      friend uint256 operator - ( const uint256& l, const uint256& r )   { return uint256(l)-=r;   }
      friend uint256 operator * ( const uint256& l, const uint256& r )   { return uint256(l)*=r;   }
      friend uint256 operator / ( const uint256& l, const uint256& r )   { return uint256(l)/=r;   }
      friend uint256 operator % ( const uint256& l, const uint256& r )   { return uint256(l)%=r;   }
      friend uint256 operator | ( const uint256& l, const uint256& r )   { return uint256(l)=(r);  }
      friend uint256 operator | ( const uint256& l, const uint64_t& r )  { return uint256(l)=uint256(r);    }
      friend uint256 operator & ( const uint256& l, const uint256& r )   { return uint256(l)&=r;   }
      friend uint256 operator & ( const uint256& l, const uint64_t& r )  { return uint256(l)&=uint256(r);   }
      friend uint256 operator ^ ( const uint256& l, const uint256& r )   { return uint256(l)^=r;   }
      friend constexpr uint256 operator << ( const uint256& l, const uint32_t& r ) { return uint256(l)<<=r;  }
      friend constexpr uint256 operator >> ( const uint256& l, const uint32_t& r ) { return uint256(l)>>=r;  }
      friend bool    operator >  ( const uint256& l, const uint256& r )  { return r < l;           }
      friend bool    operator >  ( const uint256& l, const uint128& r )  { return uint256(r) < l;           }
      friend bool    operator >  ( const uint128& l, const uint256& r )  { return r < uint256(l);           }
      friend bool    operator >  ( const uint256& l, const int64_t& r )  { return uint256(r) < l;           }
      friend bool    operator >  ( const int64_t& l, const uint256& r )  { return r < uint256(l);           }

      friend bool    operator >=  ( const uint256& l, const uint256& r ) { return l == r || l > r; }
      friend bool    operator >=  ( const uint256& l, const __uint128_t& r ) { return l >= uint256(r); }
      friend bool    operator >=  ( const __uint128_t& l, const uint256& r ) { return uint256(l) >= r; }
      friend bool    operator >=  ( const uint256& l, const uint64_t& r ) { return l >= uint256(r); }
      friend bool    operator >=  ( const uint64_t& l, const uint256& r ) { return uint256(l) >= r; }
      friend bool    operator >=  ( const uint256& l, const int64_t& r ) { return l >= uint256(r); }
      friend bool    operator >=  ( const int64_t& l, const uint256& r ) { return uint256(l) >= r; }
      friend bool    operator <=  ( const uint256& l, const uint256& r ) { return l == r || l < r; }
      friend bool    operator <=  ( const uint256& l, const __uint128_t& r ) { return l <= uint256(r); }
      friend bool    operator <=  ( const __uint128_t& l, const uint256& r ) { return uint256(l) <= r; }
      friend bool    operator <=  ( const uint256& l, const int64_t& r ) { return l <= uint256(r); }
      friend bool    operator <=  ( const int64_t& l, const uint256& r ) { return uint256(l) <= r; }
      friend bool    operator <=  ( const uint256& l, const uint64_t& r ) { return l <= uint256(r); }
      friend bool    operator <=  ( const uint64_t& l, const uint256& r ) { return uint256(l) <= r; }

      friend std::size_t hash_value( const uint256& v ) { return city_hash_size_t((const char*)&v, sizeof(v)); }

      uint32_t to_integer()const
      {
         FC_ASSERT( hi == 0 );
         uint32_t lo32 = (uint32_t) lo & 0xFFFFFFFF;
         FC_ASSERT( lo == lo32 );
         return lo32;
      }
      uint64_t to_uint64()const
      {
         FC_ASSERT( hi == 0 );
         uint32_t lo64 = (uint32_t) lo & 0xFFFFFFFFFFFFFFFFULL;
         FC_ASSERT( lo == lo64 );
         return lo64;
      }
      uint128 to_uint128()const
      {
         FC_ASSERT( hi == 0 );
         return uint128(lo);
      }
      uint32_t low_32_bits()const { return (uint32_t) ((lo << 96) >> 96) & 0xFFFFFFFF; }
      uint64_t low_64_bits()const { return (uint64_t) ((lo << 64) >> 64) & 0xFFFFFFFFFFFFFFFFULL; }
      uint128 low_bits()const  { return uint128(lo); }
      uint128 high_bits()const { return uint128(hi); }

      static uint256 max_value() {
         const __uint128_t max128 = std::numeric_limits<__uint128_t>::max();
         return uint256( max128, max128 );
      }
      static constexpr uint256 min() noexcept {
         const __uint128_t min128 = std::numeric_limits<__uint128_t>::min();
         return uint256( min128, min128 );
      }
      static constexpr uint256 max() noexcept {
         const __uint128_t max128 = std::numeric_limits<__uint128_t>::max();
         return uint256( max128, max128 );
      }

      uint8_t popcount() const;

      // fields must be public for serialization
      __uint128_t lo;
      __uint128_t hi;
   };
   static_assert( sizeof(uint256) == 4*sizeof(uint64_t), "validate packing assumptions" );

   typedef uint256 uint256_t;

   class variant;

   void to_variant( const uint256& var,  variant& vo );
   void from_variant( const variant& var,  uint256& vo );

   namespace raw
   {
      template<typename Stream>
      inline void pack( Stream& s, const uint256& u ) { s.write( (char*)&u, sizeof(u) ); }
      template<typename Stream>
      inline void unpack( Stream& s, uint256& u ) { s.read( (char*)&u, sizeof(u) ); }
   }

   size_t city_hash_size_t(const char *buf, size_t len);
} // namespace fc

namespace std
{
   template<>
   struct hash<fc::uint256>
   {
      size_t operator()( const fc::uint256& s )const
      {
         return fc::city_hash_size_t((char*)&s, sizeof(s));
      }
   };

   template<>
   class numeric_limits<fc::uint256>
   {
   public:
      static constexpr bool is_integer = false;
      static constexpr fc::uint256 min() noexcept { return fc::uint256::min(); }
      static constexpr fc::uint256 max() noexcept { return fc::uint256::max(); }
      static constexpr fc::uint256 lowest() noexcept { return fc::uint256::min(); }

      static constexpr int  min_exponent = 0;
      static constexpr int  min_exponent10 = 0;
      static constexpr int  max_exponent = 0;
      static constexpr int  max_exponent10 = 0;
   };
}

FC_REFLECT( fc::uint256, (lo)(hi) )

#ifdef _MSC_VER
#pragma warning (pop)
#endif ///_MSC_VER