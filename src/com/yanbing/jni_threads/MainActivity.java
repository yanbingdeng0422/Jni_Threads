package com.yanbing.jni_threads;

import android.app.Activity;

import android.os.Bundle;
import android.view.View;
import android.widget.TextView;
import android.widget.Button;
import android.widget.EditText;


public class MainActivity extends Activity {

    private EditText threadsEdit;
    private EditText iterationsEdit;
    private Button startButton;
    private TextView logView;


    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("jni_threads");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        // Example of a call to a native method
        TextView tv = findViewById(R.id.sample_text);
        tv.setText(stringFromJNI());

        threadsEdit = (EditText) findViewById(R.id.threads_edit);
        iterationsEdit = (EditText) findViewById(R.id.iterations_edit);
        startButton = (Button) findViewById(R.id.start_button);
        logView = (TextView) findViewById(R.id.log_view);

        startButton.setOnClickListener(new View.OnClickListener(){
            public void onClick(View view){
                int threads = getNumber(threadsEdit,0);
                int iterations = getNumber(iterationsEdit,0);

                if(threads>0 && iterations >0){
                    startThreads(threads,iterations);
                }
            }
        });
    }

    private static int getNumber(EditText editText,int defaultValue){
        int value;
        try{
            value = Integer.parseInt(editText.getText().toString());
        }catch(NumberFormatException e){
            value = defaultValue;
        }
        return value;
    }

    private void startThreads(int threads,int iterations){

    }



    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();
}
