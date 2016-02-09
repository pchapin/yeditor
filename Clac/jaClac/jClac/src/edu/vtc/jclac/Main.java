package edu.vtc.jclac;

import edu.vtc.jclac.entity.Entity;
import edu.vtc.jclac.entity.IntegerEntity;
import edu.vtc.jclac.entity.OperationNotImplementedException;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.math.BigInteger;
import java.util.Stack;

public class Main {

    static public void main(String[] args) throws IOException
    {
        Stack<Entity> cStack = new Stack<>();
        BufferedReader input = new BufferedReader(new InputStreamReader(System.in));
        boolean done = false;

        while (!done) {
            try {

                // Print the stack.
                int stackLevel = cStack.size();
                for (Entity stackItem : cStack) {
                    System.out.printf("%4d: %s\n", stackLevel, stackItem.toString());
                    stackLevel--;
                }

                // Get a line of text from the user.
                System.out.print("> ");
                String line = input.readLine();

                // Has the user had enough yet?
                if (line.equals("quit")) {
                    done = true;
                    continue;
                }

                String[] words = line.split("\\s+");
                Entity operand1;
                Entity operand2;
                for (String word : words) {
                    switch (word) {
                        case "+":
                            operand2 = cStack.pop();
                            operand1 = cStack.pop();
                            cStack.push(operand1.plus(operand2));
                            break;

                        case "-":
                            operand2 = cStack.pop();
                            operand1 = cStack.pop();
                            cStack.push(operand1.minus(operand2));
                            break;

                        case "*":
                            operand2 = cStack.pop();
                            operand1 = cStack.pop();
                            cStack.push(operand1.multiply(operand2));
                            break;

                        case "/":
                            operand2 = cStack.pop();
                            operand1 = cStack.pop();
                            cStack.push(operand1.divide(operand2));
                            break;

                        default:
                            cStack.push(new IntegerEntity(new BigInteger(word)));
                            break;
                    }
                }
            }
            catch (OperationNotImplementedException ex) {
                System.out.println(ex.getMessage());
            }
        }
    }

}
