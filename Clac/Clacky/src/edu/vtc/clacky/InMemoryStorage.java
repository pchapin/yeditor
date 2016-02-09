package edu.vtc.clacky;

import java.util.Map;
import java.util.TreeMap;

public class InMemoryStorage implements Storage {
    
    private Map<Integer, Rational> registers = new TreeMap<Integer, Rational>(); 

    @Override
    public void store(Rational value, int registerNumber) throws StorageException
    {
        if (registerNumber < 0) {
            throw new StorageException("negative register number");
        }
        registers.put(registerNumber, value);  
    }

    @Override
    public Rational recall(int registerNumber) throws StorageException
    {
        if (registerNumber < 0) {
            throw new StorageException("negative register number");
        }
        Rational value = registers.get(registerNumber);
        if (value == null) {
            value = Rational.ZERO;
        }
        return value;
    }

    @Override
    public int registerCount()
    {
        return registers.size();
    }

}
