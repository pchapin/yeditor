package edu.vtc.clacky;

import java.util.Scanner;

/**
 * Clacky is a simple calculator program that provides a number of useful features. It is
 * primarily a demonstration application for Vermont Technical College's Java Programming
 * course (CIS-2271).
 * 
 * @author Peter C. Chapin &lt;PChapin@vtc.vsc.edu&gt;
 *
 */
public class Clacky {

    public static void main(String[] args)
    {
        Scanner input = new Scanner(System.in);
        
        // Create the parameter stack and initialize it.
        ClackyStack numbers = new ClackyStack();
        
        while (true) {
            try {
                // Display the current numbers.
                for (int i = numbers.size() - 1; i >= 0; --i) {
                    System.out.printf("%2d: %s\n", i, numbers.get(i).toString());
                }
                
                // Read a command line. Check if the user wants to quit.
                System.out.print("> ");
                String commandLine = input.nextLine();
                if (commandLine.equals("quit")) {
                    input.close();
                    System.exit(0);
                }
                
                // Process each word on the command line.
                String[] commandWords = commandLine.split("\\s+");
                for (String word : commandWords) {
                    switch (word) {
                    case "+":
                        Rational sum = numbers.get(1).add(numbers.get(0));
                        numbers.pop();
                        numbers.pop();
                        numbers.push(sum);
                        break;
                        
                    case "-":
                        Rational difference = numbers.get(1).subtract(numbers.get(0));
                        numbers.pop();
                        numbers.pop();
                        numbers.push(difference);
                        break;
                        
                    case "*":
                        Rational product = numbers.get(1).multiply(numbers.get(0));
                        numbers.pop();
                        numbers.pop();
                        numbers.push(product);
                        break;
                        
                    case "/":
                        Rational quotient = numbers.get(1).divide(numbers.get(0));
                        numbers.pop();
                        numbers.pop();
                        numbers.push(quotient);
                        break;
                        
                    case "dup":
                        numbers.dup();
                        break;
                        
                    case "dupn":
                        numbers.dup(numbers.pop().getNumerator().intValue());
                        break;
                        
                    case "pick":
                        numbers.pick(numbers.pop().getNumerator().intValue());
                        break;
                        
                    case "roll":
                        numbers.rollUp(numbers.pop().getNumerator().intValue());
                        break;
                        
                    case "rolld":
                        numbers.rollDown(numbers.pop().getNumerator().intValue());
                        break;
                        
                    default:
                        Rational value = new Rational(word);
                        numbers.push(value);
                    }
                }
            }
            catch (ArithmeticException ex) {
                System.out.println("ArithmeticException: " + ex.getMessage());
            }
            catch (NumberFormatException ex) {
                System.out.println("NumberFormatException: " + ex.getMessage());
            }
        }
    }

}
