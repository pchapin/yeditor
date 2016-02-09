package edu.vtc.jclac.entity;

/**
 * Interface to Clac entities. Each entity type is intended to be immutable. The various operations always return a
 * new entity holding the result of the operation. No operands are changed. Note that the runtime type of the value
 * returned by an operation might be different than the runtime types of the operands. Exceptions are thrown if the
 * operation fails.
 */
public interface Entity {

    /**
     * Creates a clone of 'this' entity.
     * @return The duplicate entity.
     */
    Entity duplicate() throws OperationNotImplementedException;

    // Unary operations.
    //
    // The runtime type of the returned Entity might be different than the runtime type of the operand. If applying one
    // of these methods to a particular Entity does not make sense, a suitable exception is thrown.

    Entity abs             ( ) throws OperationNotImplementedException;
    Entity acos            ( ) throws OperationNotImplementedException;
    Entity asin            ( ) throws OperationNotImplementedException;
    Entity atan            ( ) throws OperationNotImplementedException;
    Entity complexConjugate( ) throws OperationNotImplementedException;
    Entity cos             ( ) throws OperationNotImplementedException;
    Entity exp             ( ) throws OperationNotImplementedException;
    Entity exp10           ( ) throws OperationNotImplementedException;
    Entity fractionalPart  ( ) throws OperationNotImplementedException;
    Entity imaginaryPart   ( ) throws OperationNotImplementedException;
    Entity integerPart     ( ) throws OperationNotImplementedException;
    Entity inv             ( ) throws OperationNotImplementedException;
    Entity ln              ( ) throws OperationNotImplementedException;
    Entity log             ( ) throws OperationNotImplementedException;
    Entity neg             ( ) throws OperationNotImplementedException;
    Entity logicalNot      ( ) throws OperationNotImplementedException;
    Entity realPart        ( ) throws OperationNotImplementedException;
    Entity rotateLeft      ( ) throws OperationNotImplementedException;
    Entity rotateRight     ( ) throws OperationNotImplementedException;
    Entity shiftLeft       ( ) throws OperationNotImplementedException;
    Entity shiftRight      ( ) throws OperationNotImplementedException;
    Entity sign            ( ) throws OperationNotImplementedException;
    Entity sin             ( ) throws OperationNotImplementedException;
    Entity sq              ( ) throws OperationNotImplementedException;
    Entity sqrt            ( ) throws OperationNotImplementedException;
    Entity tan             ( ) throws OperationNotImplementedException;
    Entity transpose       ( ) throws OperationNotImplementedException;

    // Conversion methods.

    Entity toBinaryEntity   ( ) throws OperationNotImplementedException;
    Entity toComplexEntity  ( ) throws OperationNotImplementedException;
    Entity toDirectoryEntity( ) throws OperationNotImplementedException;
    Entity toFloatEntity    ( ) throws OperationNotImplementedException;
    Entity toIntegerEntity  ( ) throws OperationNotImplementedException;
    Entity toLabeledEntity  ( ) throws OperationNotImplementedException;
    Entity toListEntity     ( ) throws OperationNotImplementedException;
    Entity toMatrixEntity   ( ) throws OperationNotImplementedException;
    Entity toProgramEntity  ( ) throws OperationNotImplementedException;
    Entity toRationalEntity ( ) throws OperationNotImplementedException;
    Entity toStringEntity   ( ) throws OperationNotImplementedException;
    Entity toVectorEntity   ( ) throws OperationNotImplementedException;

    // Binary Operations.
    //
    // The 'other' entity must have the same runtime type as 'this' entity. If a conversion is sensible, it must have
    // already been performed before these methods are used. An exception is thrown if the runtime types are different.
    // Note, however, that the runtime type of the returned entity might be different than that of the operands.

    Entity cross     (Entity other) throws OperationNotImplementedException;
    Entity divide    (Entity other) throws OperationNotImplementedException;
    Entity dot       (Entity other) throws OperationNotImplementedException;
    Entity logicalAnd(Entity other) throws OperationNotImplementedException;
    Entity logicalOr (Entity other) throws OperationNotImplementedException;
    Entity logicalXor(Entity other) throws OperationNotImplementedException;
    Entity minus     (Entity other) throws OperationNotImplementedException;
    Entity modulo    (Entity other) throws OperationNotImplementedException;
    Entity multiply  (Entity other) throws OperationNotImplementedException;
    Entity plus      (Entity other) throws OperationNotImplementedException;
    Entity power     (Entity other) throws OperationNotImplementedException;

    // Relational operations.
    Entity isEqual         (Entity other) throws OperationNotImplementedException;
    Entity isNotEqual      (Entity other) throws OperationNotImplementedException;
    Entity isLess          (Entity other) throws OperationNotImplementedException;
    Entity isLessOrEqual   (Entity other) throws OperationNotImplementedException;
    Entity isGreater       (Entity other) throws OperationNotImplementedException;
    Entity isGreaterOrEqual(Entity other) throws OperationNotImplementedException;
}
