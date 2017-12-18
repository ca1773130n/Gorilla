#define HALF_NRM_MIN   6.10351562e-05   // Smallest positive normalized half
#define HALF_MAX      65504.0         // Largest positive half
typedef unsigned char  uchar_t;
typedef unsigned int   uint32_t;
typedef unsigned short uint16_t;

namespace simplehalf
{
	static uint16_t positive_normalized(const uint32_t val) {
		enum {
			mantissa_width   = 23,
			mantissa_width_h = 10,
			mantissa_mask = (1<<mantissa_width) - 1,
			adjust = mantissa_width - mantissa_width_h,
			exponent_fix = uchar_t(-(127-15)) << mantissa_width
		};

		const uint32_t
			exp = val + exponent_fix,
			round = (val + 0xFFFu) + ((val >> adjust) & 1u),
			result = (exp & ~mantissa_mask) | (round & mantissa_mask),
			scaled = result >> adjust;
		return scaled;
	}

	static uint16_t half(const float val) {
		union { float f; uint32_t u; } bits = { val > HALF_MAX ? HALF_MAX : val };
		uint16_t h = positive_normalized(bits.u); 
		return val >= HALF_NRM_MIN ? h : 0;
	}
}
