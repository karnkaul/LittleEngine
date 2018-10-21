#pragma once
#include <string>

namespace GameUtils {
	// \brief Fixed point POD using constant scale factor
	struct Fixed final {
	public:
		static const int SCALE_FACTOR = 100000;
		static const Fixed Zero;
		static const Fixed One;
		static const Fixed Two;
		static const Fixed Three;
		static const Fixed Ten;
		static const Fixed OneHalf;
		static const Fixed OneThird;
		static const Fixed OneTenth;

		Fixed() = default;
		explicit Fixed(double);
		explicit Fixed(float);
		Fixed(const Fixed&);
		Fixed(Fixed&&) = default;
		Fixed& operator=(const Fixed&);
		Fixed& operator=(Fixed&&) = default;

		Fixed(int numerator, int denominator = 1);
		int GetInt() const;
		float GetFloat() const;
		double GetDouble() const;

		Fixed& operator+=(const Fixed& rhs);
		Fixed& operator-=(const Fixed& rhs);
		Fixed& operator*=(const Fixed& rhs);
		Fixed& operator/=(const Fixed& rhs);
		Fixed& operator%=(const Fixed& rhs);
		Fixed& operator++();
		Fixed operator++(int);
		Fixed& operator--();
		Fixed operator--(int);

		Fixed Abs() const;
		Fixed Power(int exponent) const;
		Fixed Sqrt() const;
		Fixed Inverse() const;
		// TODO: Need lookup tables for determinism
		Fixed Sin() const;
		Fixed Cos() const;
		Fixed Tan() const;

		bool operator==(const Fixed& rhs) const;
		bool operator>(const Fixed& rhs) const;
		bool operator<(const Fixed& rhs) const;
		bool operator<=(const Fixed& rhs) const;
		bool operator>=(const Fixed& rhs) const;

		std::string ToString() const;

	private:
		int m_value;
		struct RawInit {};

		friend std::ostream& operator<<(std::ostream&, Fixed);
		friend Fixed operator-(const Fixed& rhs);

		Fixed(RawInit, int rawFixedValue) : m_value(rawFixedValue) {}
	};

	Fixed operator+(const Fixed& lhs, const Fixed& rhs);
	Fixed operator-(const Fixed& lhs, const Fixed& rhs);
	Fixed operator*(const Fixed& lhs, const Fixed& rhs);
	Fixed operator/(const Fixed& lhs, const Fixed& rhs);
	Fixed operator%(const Fixed& lhs, const Fixed& rhs);
	bool operator!=(const Fixed& lhs, const Fixed& rhs);
}
