package edu.vtc.jclac.entity;

public abstract class AbstractEntity implements Entity {

    public Entity duplicate() throws OperationNotImplementedException
    {
        throw new OperationNotImplementedException("Unable to duplicate object");
    }

    // Unary operations
    // ----------------

    public Entity abs( ) throws OperationNotImplementedException
    {
        throw new OperationNotImplementedException("Unable to take absolute value of object");
    }

    public Entity acos( ) throws OperationNotImplementedException
    {
        throw new OperationNotImplementedException("Unable to take arccosine of object");
    }

    public Entity asin( ) throws OperationNotImplementedException
    {
        throw new OperationNotImplementedException("Unable to take arcsine of object");
    }

    public Entity atan( ) throws OperationNotImplementedException
    {
        throw new OperationNotImplementedException("Unable to take arctangent of object");
    }

    public Entity complexConjugate( ) throws OperationNotImplementedException
    {
        throw new OperationNotImplementedException("Unable to take complex conjugate of object");
    }

    public Entity cos( ) throws OperationNotImplementedException
    {
        throw new OperationNotImplementedException("Unable to take cosine of object");
    }

    public Entity exp( ) throws OperationNotImplementedException
    {
        throw new OperationNotImplementedException("Unable to exponentiate object");
    }

    public Entity exp10( ) throws OperationNotImplementedException
    {
        throw new OperationNotImplementedException("Unable to exponentiate object");
    }

    public Entity fractionalPart( ) throws OperationNotImplementedException
    {
        throw new OperationNotImplementedException("Object has no fractional part");
    }

    public Entity imaginaryPart( ) throws OperationNotImplementedException
    {
        throw new OperationNotImplementedException("Object has no imaginary part");
    }

    public Entity integerPart( ) throws OperationNotImplementedException
    {
        throw new OperationNotImplementedException("Object has no integer part");
    }

    public Entity inv( ) throws OperationNotImplementedException
    {
        throw new OperationNotImplementedException("Unable to invert object");
    }

    public Entity ln( ) throws OperationNotImplementedException
    {
        throw new OperationNotImplementedException("Unable to take natural logarithm of object");
    }

    public Entity log( ) throws OperationNotImplementedException
    {
        throw new OperationNotImplementedException("Unable to take logarithm of object");
    }

    public Entity neg( ) throws OperationNotImplementedException
    {
        throw new OperationNotImplementedException("Unable to negate object");
    }

    public Entity logicalNot( ) throws OperationNotImplementedException
    {
        throw new OperationNotImplementedException("Unable to logically negate object");
    }

    public Entity realPart( ) throws OperationNotImplementedException
    {
        throw new OperationNotImplementedException("Object has no real part");
    }

    public Entity rotateLeft( ) throws OperationNotImplementedException
    {
        throw new OperationNotImplementedException("Unable to rotate object to the left");
    }

    public Entity rotateRight( ) throws OperationNotImplementedException
    {
        throw new OperationNotImplementedException("Unable to rotate object to the right");
    }

    public Entity shiftLeft( ) throws OperationNotImplementedException
    {
        throw new OperationNotImplementedException("Unable to shift object to the left");
    }

    public Entity shiftRight( ) throws OperationNotImplementedException
    {
        throw new OperationNotImplementedException("Unable to shift object to the right");
    }

    public Entity sign( ) throws OperationNotImplementedException
    {
        throw new OperationNotImplementedException("Unable to find sign of object");
    }

    public Entity sin( ) throws OperationNotImplementedException
    {
        throw new OperationNotImplementedException("Unable to take sine of object");
    }

    public Entity sq( ) throws OperationNotImplementedException
    {
        throw new OperationNotImplementedException("Unable to square object");
    }

    public Entity sqrt( ) throws OperationNotImplementedException
    {
        throw new OperationNotImplementedException("Unable to take square root of object");
    }

    public Entity tan( ) throws OperationNotImplementedException
    {
        throw new OperationNotImplementedException("Unable to take tangent of object");

    }
    public Entity transpose( ) throws OperationNotImplementedException
    {
        throw new OperationNotImplementedException("Unable to transpose object");
    }

    // Conversion methods
    // ------------------

    public Entity toBinaryEntity( ) throws OperationNotImplementedException
    {
        throw new OperationNotImplementedException("Unable to convert object to a binary");
    }

    public Entity toComplexEntity( ) throws OperationNotImplementedException
    {
        throw new OperationNotImplementedException("Unable to convert object to a complex");
    }

    public Entity toDirectoryEntity( ) throws OperationNotImplementedException
    {
        throw new OperationNotImplementedException("Unable to convert object to a directory");
    }

    public Entity toFloatEntity( ) throws OperationNotImplementedException
    {
        throw new OperationNotImplementedException("Unable to convert object to a float");
    }

    public Entity toIntegerEntity( ) throws OperationNotImplementedException
    {
        throw new OperationNotImplementedException("Unable to convert object to an integer");
    }

    public Entity toLabeledEntity( ) throws OperationNotImplementedException
    {
        throw new OperationNotImplementedException("Unable to convert object to a labeled");
    }

    public Entity toListEntity( ) throws OperationNotImplementedException
    {
        throw new OperationNotImplementedException("Unable to convert object to a list");
    }

    public Entity toMatrixEntity( ) throws OperationNotImplementedException
    {
        throw new OperationNotImplementedException("Unable to convert object to a matrix");
    }

    public Entity toProgramEntity( ) throws OperationNotImplementedException
    {
        throw new OperationNotImplementedException("Unable to convert object to a program");
    }

    public Entity toRationalEntity( ) throws OperationNotImplementedException
    {
        throw new OperationNotImplementedException("Unable to convert object to a rational number");
    }

    public Entity toStringEntity( ) throws OperationNotImplementedException
    {
        throw new OperationNotImplementedException("Unable to convert object to a string");
    }

    public Entity toVectorEntity( ) throws OperationNotImplementedException
    {
        throw new OperationNotImplementedException("Unable to convert object to a vector");
    }

    // Binary Operations
    // -----------------

    public Entity cross(Entity other) throws OperationNotImplementedException
    {
        throw new OperationNotImplementedException("Unable to take cross product of these objects");
    }

    public Entity divide(Entity other) throws OperationNotImplementedException
    {
        throw new OperationNotImplementedException("Unable to divide these objects");
    }

    public Entity dot(Entity other) throws OperationNotImplementedException
    {
        throw new OperationNotImplementedException("Unable to take dot product of these objects");
    }

    public Entity logicalAnd(Entity other) throws OperationNotImplementedException
    {
        throw new OperationNotImplementedException("Unable to logically AND these objects");
    }

    public Entity logicalOr(Entity other) throws OperationNotImplementedException
    {
        throw new OperationNotImplementedException("Unable to logically OR these objects");
    }

    public Entity logicalXor(Entity other) throws OperationNotImplementedException
    {
        throw new OperationNotImplementedException("Unable to logically XOR these objects");
    }

    public Entity minus(Entity other) throws OperationNotImplementedException
    {
        throw new OperationNotImplementedException("Unable to subtract these objects");
    }

    public Entity modulo(Entity other) throws OperationNotImplementedException
    {
        throw new OperationNotImplementedException("Unable to modulo these objects");
    }

    public Entity multiply(Entity other) throws OperationNotImplementedException
    {
        throw new OperationNotImplementedException("Unable to multiply these objects");
    }

    public Entity plus(Entity other) throws OperationNotImplementedException
    {
        throw new OperationNotImplementedException("Unable to add these objects");
    }

    public Entity power(Entity other) throws OperationNotImplementedException
    {
        throw new OperationNotImplementedException("Unable to exponentiate these objects");
    }

    // Relational operations
    // ---------------------

    public Entity isEqual(Entity other) throws OperationNotImplementedException
    {
        throw new OperationNotImplementedException("Unable to compare these objects");
    }

    public Entity isNotEqual(Entity other) throws OperationNotImplementedException
    {
        throw new OperationNotImplementedException("Unable to compare these objects");
    }

    public Entity isLess(Entity other) throws OperationNotImplementedException
    {
        throw new OperationNotImplementedException("Unable to compare these objects");
    }

    public Entity isLessOrEqual(Entity other) throws OperationNotImplementedException
    {
        throw new OperationNotImplementedException("Unable to compare these objects");
    }

    public Entity isGreater(Entity other) throws OperationNotImplementedException
    {
        throw new OperationNotImplementedException("Unable to compare these objects");
    }

    public Entity isGreaterOrEqual(Entity other) throws OperationNotImplementedException
    {
        throw new OperationNotImplementedException("Unable to compare these objects");
    }
}
