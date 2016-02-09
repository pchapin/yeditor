package edu.vtc.clacky;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;

public class OnDiskStorage implements Storage {

    @Override
    public void store(Rational value, int registerNumber) throws StorageException
    {
        if (registerNumber < 0)
            throw new StorageException("negative register numbers forbidden");
        
        try (PrintWriter out = new PrintWriter(new BufferedWriter(new FileWriter("foo.out")))) {
            // Reminder: value.toString() gives you the string version of the Rational value.
        }
        catch (IOException ex) {
            throw new StorageException("failed to write register file: " + ex.getMessage());
        }
    }

    
    @Override
    public Rational recall(int registerNumber) throws StorageException
    {
        if (registerNumber < 0)
            throw new StorageException("negative register numbers forbidden");

        try (BufferedReader in = new BufferedReader(new FileReader("foo.in"))) {
            // Reminder: There is a constructor in class Rational that takes a string.
        }
        catch (IOException ex) {
            return new Rational(0, 1);
        }
        return null;
    }

    
    @Override
    public int registerCount() {
        // Count the number of *.crg files and return that count.
        File currentFolder = new File(".");
        String[] fileNames = currentFolder.list();
        int count = 0;
        for (String name : fileNames) {
            if (name.endsWith(".crg")) {
                count++;
            }
        }
        return count;
    }

}
