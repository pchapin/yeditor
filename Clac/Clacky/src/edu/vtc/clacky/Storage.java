package edu.vtc.clacky;

/**
 * The interface to a register storage facility. Classes implementing this interface can save
 * and recall Rational numbers into/from registers identified using integers. The number of
 * registers is unspecified by this interface but it is intended to be "large."
 * 
 * @author Peter C. Chapin
 *
 */
public interface Storage {

    /**
     * Store the given value into the specified register. Negative register numbers are not
     * allowed.
     * 
     * @param value The value to store.
     * @param registerNumber The register into which the value is placed.
     */
    public void store(Rational value, int registerNumber) throws StorageException;
    
    /**
     * Recall the value stored in the specified register. Negative register numbers are not
     * allowed. If the register is uninitialized the value zero is returned (and the register
     * remains uninitialized.
     * 
     * @param registerNumber The register to recall.
     * @return The value stored in that register or zero if the register was uninitialized.
     */
    public Rational recall(int registerNumber) throws StorageException;
    
    /**
     * @return The number of registers in use (that have been initialized).
     */
    public int registerCount();
    
}
