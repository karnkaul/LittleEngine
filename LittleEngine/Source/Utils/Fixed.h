#pragma once
#include <string>

struct Fixed final {
public:
	static const int SCALE_FACTOR = 100000;

	Fixed() = default;
	explicit Fixed(double);
	explicit Fixed(float);
	Fixed(const Fixed&);
	Fixed& operator=(const Fixed&);

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
	Fixed operator+(const Fixed& rhs) const;
	Fixed operator-(const Fixed& rhs) const;
	Fixed operator*(const Fixed& rhs) const;
	Fixed operator/(const Fixed& rhs) const;
	Fixed operator%(const Fixed& rhs) const;
	
	bool operator==(const Fixed& rhs) const;
	bool operator!=(const Fixed& rhs) const;
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