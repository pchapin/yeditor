package edu.vtc.aclac;

import android.app.Activity;
import android.os.Bundle;
import android.text.Editable;
import android.util.Log;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.inputmethod.EditorInfo;
import android.widget.EditText;
import android.widget.TextView;
import edu.vtc.jclac.entity.Entity;
import edu.vtc.jclac.entity.IntegerEntity;
import edu.vtc.jclac.entity.OperationNotImplementedException;

import java.math.BigInteger;
import java.util.Stack;

public class MainActivity extends Activity {

    private Stack<Entity> cStack = new Stack<>();

    /**
     * This method deals with a line containing Clac words, computing the result and updating the stack accordingly.
     * @param line The line containing the words to process.
     * @param resultText The TextView into which stack level zero will be displayed.
     */
    private void processWords(String line, TextView resultText)
    {
        try {
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
            resultText.setText("[0] = " + cStack.peek().toString());
        }
        catch (OperationNotImplementedException ex) {
            // TODO: Tell the user that something went wrong!
        }
    }


    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.i("aClac", "Application Created!");
        setContentView(R.layout.main);

        final EditText streamInput = (EditText)findViewById(R.id.input_stream);
        final TextView resultText  = (TextView)findViewById(R.id.result_text);
        streamInput.setOnEditorActionListener(new TextView.OnEditorActionListener() {
            @Override
            public boolean onEditorAction(TextView v, int actionId, KeyEvent event) {
                boolean handled = false;
                // Why is it necessary to check for both IME_ACTION_DONE and an explicit press of the enter key?
                if (actionId == EditorInfo.IME_ACTION_DONE ||
                        (event.getAction() == KeyEvent.ACTION_DOWN && event.getKeyCode() == KeyEvent.KEYCODE_ENTER)) {
                    Editable streamData = streamInput.getText();
                    if (streamData != null) {
                        Log.i("aClac", "Handling word stream: " + streamData);
                        // TODO: Deal with invalid input properly!
                        processWords(streamData.toString(), resultText);
                        streamInput.setText(null);
                        handled = true;
                    }
                }
                return handled;
            }
        });
    }


    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        MenuInflater inflater = getMenuInflater();
        inflater.inflate(R.menu.main_activity_actions, menu);
        return super.onCreateOptionsMenu(menu);
    }


    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        switch (item.getItemId()) {
            case R.id.action_settings:
                // openSettings();
                return true;
            default:
                return super.onOptionsItemSelected(item);
        }
    }

}
