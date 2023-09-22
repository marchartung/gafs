#pragma once

#include <cstdint>

#include "utils/macros.hpp"
#include "utils/types.hpp"

class Morton64 {
 public:
  using Coord = uint32_t;
  using Coords = const Array<Coord, 3>;
  using morton = uint64_t;

  Morton64() = default;
  INLINE Morton64(const morton morton) : value_(morton) {}
  INLINE Morton64(const Coords coords) : Morton64(Encode(coords)) {}

  INLINE Morton64(const Coord x, const Coord y, const Coord z)
      : Morton64(std::array<Coord, 3>{x, y, z}) {}

  INLINE Coords coords() const { return Decode(value_); }
  INLINE morton value() const { return value_; }

  INLINE bool operator<(const Morton64 m) const { return value_ < m.value_; }
  INLINE bool operator==(const Morton64 m) const { return value_ == m.value_; }
  INLINE Morton64 operator>>(const int s) const {
    return Morton64(value_ >> s);
  }
  INLINE Morton64 operator<<(const int s) const {
    return Morton64(value_ << s);
  }
  INLINE Morton64 operator|(const morton s) const {
    return Morton64(value_ | s);
  }
  INLINE Morton64 operator&(const morton s) const {
    return Morton64(value_ & s);
  }
  INLINE Morton64 operator|(const Morton64 m) const {
    return Morton64(value_ | m.value_);
  }

  INLINE uint32_t GetLast3Bits() const { return value_ & 7; }

 private:
  INLINE static morton SplitThird(const Coord a) {
    static constexpr std::array<morton, 6> magic_bits = {
        0x1fffff,           0x1f00000000ffff,   0x1f0000ff0000ff,
        0x100f00f00f00f00f, 0x10c30c30c30c30c3, 0x1249249249249249};
    morton x = static_cast<morton>(a) & magic_bits[0];
    x = (x | x << 32) & magic_bits[1];
    x = (x | x << 16) & magic_bits[2];
    x = (x | x << 8) & magic_bits[3];
    x = (x | x << 4) & magic_bits[4];
    x = (x | x << 2) & magic_bits[5];
    return x;
  }

  INLINE static morton Encode(const Coords c) {
    return SplitThird(c[0]) | (SplitThird(c[1]) << 1) | (SplitThird(c[2]) << 2);
  }

  INLINE static Coord GetThird(const morton m) {
    static constexpr std::array<morton, 6> magic_bits = {
        0x1fffff,           0x1f00000000ffff,   0x1f0000ff0000ff,
        0x100f00f00f00f00f, 0x10c30c30c30c30c3, 0x1249249249249249};
    morton x = m & magic_bits[5];
    x = (x ^ (x >> 2)) & magic_bits[4];
    x = (x ^ (x >> 4)) & magic_bits[3];
    x = (x ^ (x >> 8)) & magic_bits[2];
    x = (x ^ (x >> 16)) & magic_bits[1];
    x = (x ^ (x >> 32)) & magic_bits[0];
    return static_cast<Coord>(x);
  }

  INLINE static Coords Decode(const morton m) {
    const Coord x = GetThird(m);
    const Coord y = GetThird(m >> 1);
    const Coord z = GetThird(m >> 2);
    return Coords{x, y, z};
  }

  morton value_;
};

class Morton32 {
 public:
  using Coord = uint16_t;
  using Coords = const Vector3T<Coord>;
  using morton = uint32_t;

  Morton32() = default;
  INLINE Morton32(const morton morton) : value_(morton) {}
  INLINE Morton32(const Coords coords) : Morton32(Encode(coords)) {}

  INLINE Coords coords() const { return Decode(value_); }
  INLINE morton value() const { return value_; }
  INLINE bool operator<(const Morton32 m) const { return value_ < m.value_; }
  INLINE bool operator==(const Morton32 m) const { return value_ == m.value_; }

  INLINE Morton32 operator>>(const int s) const {
    return Morton32(value_ >> s);
  }
  INLINE Morton32 operator<<(const int s) const {
    return Morton32(value_ << s);
  }
  INLINE Morton32 operator|(const morton s) const {
    return Morton32(value_ | s);
  }
  INLINE Morton32 operator&(const morton s) const {
    return Morton32(value_ & s);
  }
  INLINE Morton32 operator|(const Morton32 m) const {
    return Morton32(value_ | m.value_);
  }

  INLINE uint32_t GetLast3Bits() const { return value_ & 7; }

 private:
  INLINE static morton SplitBy3(const Coord a) {
    static constexpr std::array<morton, 5> magic_bits = {
        0x000003ff, 0x30000ff, 0x0300f00f, 0x30c30c3, 0x9249249};
    morton x = static_cast<morton>(a) & magic_bits[0];
    x = (x | x << 16) & magic_bits[1];
    x = (x | x << 8) & magic_bits[2];
    x = (x | x << 4) & magic_bits[3];
    x = (x | x << 2) & magic_bits[4];
    return x;
  }

  INLINE static morton EncodeBetter(const Coords c) {
    static constexpr std::array<morton, 5> magic_bits = {
        0x000003ff, 0x30000ff, 0x0300f00f, 0x30c30c3, 0x9249249};
    morton x = static_cast<morton>(c[0]) & magic_bits[0];
    morton y = static_cast<morton>(c[1]) & magic_bits[0];
    morton z = static_cast<morton>(c[2]) & magic_bits[0];
    for (morton i = 16, j = 1; j < 5; ++j, i = i >> 1) {
      const morton mb = magic_bits[j];
      x = (x | x << i) & mb;
      y = (y | y << i) & mb;
      z = (z | z << i) & mb;
    }
    // x = (x | x << 16) & magic_bits[1];
    // y = (y | y << 16) & magic_bits[1];
    // z = (z | z << 16) & magic_bits[1];
    // x = (x | x << 8) & magic_bits[2];
    // y = (y | y << 8) & magic_bits[2];
    // z = (z | z << 8) & magic_bits[2];
    // x = (x | x << 4) & magic_bits[3];
    // y = (y | y << 4) & magic_bits[3];
    // z = (z | z << 4) & magic_bits[3];
    // x = (x | x << 2) & magic_bits[4];
    // y = (y | y << 2) & magic_bits[4];
    // z = (z | z << 2) & magic_bits[4];

    return x | (y << 1) | (z << 2);
  }

  INLINE static morton Encode(const Coords c) {
    return SplitBy3(c[0]) | (SplitBy3(c[1]) << 1) | (SplitBy3(c[2]) << 2);
  }

  INLINE static Coord GetThird(const morton m) {
    static constexpr std::array<uint64_t, 5> magic_bits = {
        0x000003ff, 0x30000ff, 0x0300f00f, 0x30c30c3, 0x9249249};
    morton x = m & magic_bits[4];
    x = (x ^ (x >> 2)) & magic_bits[3];
    x = (x ^ (x >> 4)) & magic_bits[2];
    x = (x ^ (x >> 8)) & magic_bits[1];
    x = (x ^ (x >> 16)) & magic_bits[0];
    return static_cast<Coord>(x);
  }

  INLINE static Coords Decode(const morton m) {
    const Coord x = GetThird(m);
    const Coord y = GetThird(m >> 1);
    const Coord z = GetThird(m >> 2);
    return Coords{x, y, z};
  }

  morton value_;
};

template <typename morton_type = Morton64>
struct MortIdx {
  using size_type = uint32_t;
  morton_type morton;
  size_type idx;
  DEVICE bool operator==(const MortIdx<morton_type> in) const {
    return morton == in.morton;
  }
  DEVICE bool operator<(const MortIdx<morton_type> in) const {
    return morton < in.morton;
  }
};
