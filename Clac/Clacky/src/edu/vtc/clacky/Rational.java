package edu.vtc.clacky;

import java.math.BigInteger;

/**
 * This class models rational numbers with arbitrary precision numerators and denominators. It
 * provides the usual arithmetic operations similarly in style to class BigInteger. Internally
 * Rationals are always kept in reduced form so getNumerator() and getDenominator() always
 * return reduced values. Negative Rationals are permitted; the sign is always on the numerator.
 * 
 * @author Peter Chapin
 *
 */
public class Rational implements Comparable<Rational> {
    
    public static Rational ZERO = new Rational(0, 1);
    public static Rational ONE = new Rational(1, 1);

    // TODO: Use list of prime factors for n and d. (Faster?)
    private BigInteger n;
    private BigInteger d;
    
    private void reduce()
    {
        // Reduce rational as much as possible.
        BigInteger greatestCommonDivisor = n.gcd(d);
        n = n.divide(greatestCommonDivisor);
        d = d.divide(greatestCommonDivisor);        
    }
    
    
    private void initialize(BigInteger numerator, BigInteger denominator)
    {
        n = numerator;
        d = denominator;
        
        if (d.compareTo(BigInteger.ZERO) == 0) {
            throw new ArithmeticException("zero denominator");
        }
        
        reduce();
        
        if (d.compareTo(BigInteger.ZERO) < 0) {
            n = n.negate();
            d = d.negate();
        }
    }

    
    /**
     * Constructs a Rational from two BigInteger values. It is permitted to use negative values
     * for either or both of the numerator and denominator. The final sign (if any) is always
     * carried on the numerator.
     * 
     * @param numerator The value to use as the numerator of the Rational.
     * @param denominator The value to use as the denominator of the Rational.
     */
    public Rational(BigInteger numerator, BigInteger denominator)
    {
        initialize(numerator, denominator);
    }
    
    
    /**
     * Constructs a Rational from two long int values. It is permitted to use negative values
     * for either or both of the numerator and denominator. The final sign (if any) is always
     * carried on the numerator.
     * 
     * @param numerator The value to use as the numerator of the Rational.
     * @param denominator The value to use as the denominator of the Rational.
     */
    public Rational(long numerator, long denominator)
    {
        initialize(BigInteger.valueOf(numerator), BigInteger.valueOf(denominator));
    }
    
    
    /**
     * Constructs a Rational from a string representation using the "obvious" syntax. This
     * method also allows just strings of digits to be used in which case the denominator is
     * set to one. 
     * 
     * @param text The text to convert.
     */
    public Rational(String text)
    {
        int separatorIndex = text.indexOf('/');
        if (separatorIndex == -1) {
            String numerator = text.trim();
            initialize(new BigInteger(numerator), BigInteger.ONE);
        }
        else {
            String numerator = text.substring(0, separatorIndex).trim();
            String denominator = text.substring(separatorIndex + 1).trim();
            initialize(new BigInteger(numerator), new BigInteger(denominator));
        }
    }

    
    /**
     * Accessor method.
     * 
     * @return The numerator of this Rational (might be negative or zero).
     */
    public BigInteger getNumerator()
        { return n; }

    
    /**
     * Accessor method.
     * 
     * @return The denominator of this Rational (always positive).
     */
    public BigInteger getDenominator()
        { return d; }

    
    /**
     * Computes the sum of this Rational and the other Rational.
     * 
     * @param other The value to add.
     * @return The resulting Rational.
     */
    public Rational add(Rational other)
    {
        BigInteger commonDenominator = d.multiply(other.d);
        BigInteger temp1 = n.multiply(other.d);
        BigInteger temp2 = other.n.multiply(d);
        return new Rational(temp1.add(temp2), commonDenominator);
    }

    
    /**
     * Computes the difference of this Rational and the other Rational.
     * 
     * @param other The value to use as the subtrahend.
     * @return The resulting Rational.
     */
    public Rational subtract(Rational other)
    {
        BigInteger commonDenominator = d.multiply(other.d);
        BigInteger temp1 = n.multiply(other.d);
        BigInteger temp2 = other.n.multiply(d);
        return new Rational(temp1.subtract(temp2), commonDenominator);
    }

    
    /**
     * Computes the product of this Rational and the other Rational.
     * 
     * @param other The value to multiply.
     * @return The resulting Rational.
     */
    public Rational multiply(Rational other)
    {
        return new Rational(n.multiply(other.n), d.multiply(other.d));
    }

    
    /**
     * Computes the quotient of this Rational and the other Rational.
     * 
     * @param other The value to use as a divisor.
     * @return The resulting Rational.
     */
    public Rational divide(Rational other)
    {
        return new Rational(n.multiply(other.d), d.multiply(other.n));
    }
    
 
    @Override
    public String toString()
    {
        if (d.compareTo(BigInteger.ONE) == 0) {
            return n.toString();
        }
        else {
            return n.toString() + "/" + d.toString();
        }
    }


    @Override
    public int hashCode() {
        final int prime = 31;
        int result = 1;
        result = prime * result + ((d == null) ? 0 : d.hashCode());
        result = prime * result + ((n == null) ? 0 : n.hashCode());
        return result;
    }


    @Override
    public boolean equals(Object obj) {
        if (this == obj)
            return true;
        if (obj == null)
            return false;
        if (getClass() != obj.getClass())
            return false;
        Rational other = (Rational) obj;
        if (d == null) {
            if (other.d != null)
                return false;
        }
        else if (!d.equals(other.d))
            return false;
        if (n == null) {
            if (other.n != null)
                return false;
        }
        else if (!n.equals(other.n))
            return false;
        return true;
    }


    @Override
    public int compareTo(Rational other) {
        // n1/d1  n2/d2  =>   (n1*d2)/(d1*d2)   (n2*d1)/(d1*d2)
        BigInteger scaledN1 = n.multiply(other.d);
        BigInteger scaledN2 = (other.n).multiply(d);
        return scaledN1.compareTo(scaledN2);
    }

}
