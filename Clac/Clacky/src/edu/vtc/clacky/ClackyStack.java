package edu.vtc.clacky;

import java.util.ArrayList;

public class ClackyStack {
    
    private ArrayList<Rational> theStack;
    
    public ClackyStack()
    {
        theStack = new ArrayList<Rational>();
    }
    
    
    public void push(Rational newValue)
    {
        theStack.add(0, newValue);
    }
    
    
    public Rational pop()
    {
        return theStack.remove(0);
    }
    
    
    public int size()
    {
        return theStack.size();
    }
    
    
    public Rational get(int stackLevel)
    {
        return theStack.get(stackLevel);
    }

    
    /**
     * Extends the stack to size n. If the stack is already that size nothing is done, otherwise
     * an appropriate number of zeros are added to the stack to increase its size to n item.
     * 
     * @param newSize The desired size of the stack. Additional zeros are added if necessary.
     */
    private void extend(int newSize)
    {
        if (newSize > theStack.size()) {
            int count = newSize - theStack.size();
            for (int i = 0; i < count; ++i) theStack.add(new Rational(0, 1));
        }        
    }
    
    
    /**
     * Duplicates the item at position 0 in the stack so that it also appears in position 1. All
     * other items are moved up. For example if the stack contains 10 11 12 13 14 a call to
     * dup() produces 10 10 11 12 13 14.
     * 
     * If the stack is empty there is no effect.
     */
    public void dup()
    {
        if (theStack.size() != 0) {
            theStack.add(0, theStack.get(0));
        }
    }
    
    
    /**
     * Duplicates the items at positions 0 to n-1 in the stack. All other items are moved up.
     * For example if the stack contains 10 11 12 13 14 a call to dup(3) produces 10 11 12 10 11
     * 12 13 14.
     * 
     * If 'n' is larger than the stack size, the stack is first extended with a suitable number
     * of zeros.
     * 
     * @param n The number of stack positions to duplicate.
     */
    public void dup(int n)
    {
        extend(n);
        for (int i = 0; i < n; ++i) {
            theStack.add(0, theStack.get(n - 1));
        }
    }
    
    
    /**
     * Copies the item at position 'n' to position 0 and moves all items up. For example if the
     * stack contains 10 11 12 13 14 a call to pick(3) produces 13 10 11 12 13 14.
     * 
     * If 'n' is off the end of the stack, the stack is first extended with a suitable number
     * of zeros.
     * 
     * @param n The position of the item to be picked.
     */
    public void pick(int n)
    {
        extend(n + 1);
        theStack.add(0, theStack.get(n));
    }

    
    /**
     * The item at position 'distance' in the stack is moved to position 0. All items originally
     * in the range [0, 'distance') are moved up one place. For example if the stack contains
     * 10 11 12 13 14 a call to rollUp(3) produces 13 10 11 12 14.
     * 
     * If position 'distance' is off the end of the stack, zeros are first added as appropriate
     * to extend the stack to position 'distance.'
     * 
     * @param distance The position of the item to be brought into stack position 0.
     */
    public void rollUp(int distance)
    {
        extend(distance + 1);
        Rational item = theStack.get(distance);
        theStack.remove(distance);
        theStack.add(0, item);
    }
    
    
    /**
     * The item at position 0 in the stack is moved to position 'distance.' All items originally
     * in the range (0, 'distance'] are moved down one place. For example if the stack contains
     * 10 11 12 13 14 a call to rollDown(3) produces 11 12 13 10 14.
     * 
     * If position 'distance' is off the end of the stack, zeros are first added as appropriate
     * to extend the stack to position 'distance.'
     * 
     * @param distance The position where the item at stack position 0 will be placed.
     */
    public void rollDown(int distance)
    {
        extend(distance + 1);
        Rational item = theStack.get(0);
        theStack.remove(0);
        theStack.add(distance, item);
    }

}
