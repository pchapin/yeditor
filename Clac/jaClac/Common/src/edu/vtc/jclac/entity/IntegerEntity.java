package edu.vtc.jclac.entity;

import java.math.BigInteger;

public class IntegerEntity extends AbstractEntity {

    private BigInteger value;

    public IntegerEntity(BigInteger value)
    {
        this.value = value;
    }

    @Override
    public String toString()
    {
        // TODO: Need to support bases other than 10 at some point.
        return value.toString(10);
    }

    @Override
    public Entity duplicate()
    {
        // Since BigIntegers are immutable there are no problems having the new IntegerEntity share the old value.
        return new IntegerEntity(value);
    }

    // Unary operations
    // ----------------

    @Override
    public Entity acos() throws OperationNotImplementedException
    {
        Entity converted = this.toFloatEntity();
        return converted.acos();
    }

    @Override
    public Entity asin() throws OperationNotImplementedException
    {
        Entity converted = this.toFloatEntity();
        return converted.asin();
    }

    @Override
    public Entity atan() throws OperationNotImplementedException
    {
        Entity converted = this.toFloatEntity();
        return converted.atan();
    }

    @Override
    public Entity cos() throws OperationNotImplementedException
    {
        Entity converted = this.toFloatEntity();
        return converted.cos();
    }

    @Override
    public Entity exp() throws OperationNotImplementedException
    {
        Entity converted = this.toFloatEntity();
        return converted.exp();
    }

    @Override
    public Entity exp10() throws OperationNotImplementedException
    {
        Entity converted = this.toFloatEntity();
        return converted.exp10();
    }

    @Override
    public Entity ln() throws OperationNotImplementedException
    {
        Entity converted = this.toFloatEntity();
        return converted.ln();
    }

    @Override
    public Entity log() throws OperationNotImplementedException
    {
        Entity converted = this.toFloatEntity();
        return converted.log();
    }

    @Override
    public Entity sin() throws OperationNotImplementedException
    {
        Entity converted = this.toFloatEntity();
        return converted.sin();
    }

    @Override
    public Entity sqrt() throws OperationNotImplementedException
    {
        Entity converted = this.toFloatEntity();
        return converted.sqrt();
    }

    @Override
    public Entity tan() throws OperationNotImplementedException
    {
        Entity converted = this.toFloatEntity();
        return converted.tan();
    }

    // Binary operations
    // -----------------

    @Override
    public Entity divide(Entity other)
    {
        IntegerEntity otherIntegerEntity = (IntegerEntity)other;
        return new IntegerEntity(value.divide(otherIntegerEntity.value));
    }

    @Override
    public Entity minus(Entity other)
    {
        IntegerEntity otherIntegerEntity = (IntegerEntity)other;
        return new IntegerEntity(value.subtract(otherIntegerEntity.value));
    }

    @Override
    public Entity multiply(Entity other)
    {
        IntegerEntity otherIntegerEntity = (IntegerEntity)other;
        return new IntegerEntity(value.multiply(otherIntegerEntity.value));
    }

    @Override
    public Entity plus(Entity other)
    {
        IntegerEntity otherIntegerEntity = (IntegerEntity)other;
        return new IntegerEntity(value.add(otherIntegerEntity.value));
    }

}
