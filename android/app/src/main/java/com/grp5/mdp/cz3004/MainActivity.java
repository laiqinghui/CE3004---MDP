package com.grp5.mdp.cz3004;

import android.annotation.SuppressLint;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.net.Uri;
import android.os.AsyncTask;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.speech.RecognizerIntent;
import android.support.v4.app.Fragment;
import android.support.design.widget.NavigationView;
import android.support.v4.app.FragmentActivity;
import android.support.v4.app.FragmentManager;
import android.support.v4.view.GravityCompat;
import android.support.v4.widget.DrawerLayout;
import android.support.v7.app.ActionBarDrawerToggle;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.Gravity;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.view.inputmethod.EditorInfo;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.PopupWindow;
import android.widget.RelativeLayout;
import android.widget.TextView;
import android.widget.Toast;

import java.math.BigInteger;
import java.util.List;

public class MainActivity extends AppCompatActivity
        implements
        NavigationView.OnNavigationItemSelectedListener,
        BluetoothFragment.OnFragmentInteractionListener,
        SendTextFragment.OnFragmentInteractionListener,
        ArenaFragment.OnMapUpdateListener,
        MiscellaneousFragment.OnFragmentInteractionListener{

    private BluetoothAdapter BTAdapter;
    private Fragment fragment;

    BluetoothChatService mChatService;
    String arduinoAddr;
    String bluetoothStatus="Disconnected";
    /**
     * String buffer for outgoing messages
     */
    private StringBuffer mOutStringBuffer;

    public static int REQUEST_BLUETOOTH = 1;

    /**
     * Name of the connected device
     */
    private String mConnectedDeviceName = null;
    private PopupWindow mPopupWindow;
    private View mLayout;
    private String exploredBin = "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";
    private String obstacleBin = "0";

    private final int REQUEST_SPEECH_RECOGNIZER = 3000;
    private String mVoiceCommand;
    private String dirRow = "1";
    private String dirCol = "1";
    private String dirDir = "2";
    private String dirMoveOrStop = "1";
    private String exploredHex;
    private String obstacleHex;

    private static String readMessage;


    @SuppressLint("HandlerLeak")
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        BTAdapter = BluetoothAdapter.getDefaultAdapter();

        // Phone does not support Bluetooth so let the user know and exit.
        if (BTAdapter == null) {
            new AlertDialog.Builder(this)
                    .setTitle("Not compatible")
                    .setMessage("Your phone does not support Bluetooth")
                    .setPositiveButton("Exit", new DialogInterface.OnClickListener() {
                        public void onClick(DialogInterface dialog, int which) {
                            System.exit(0);
                        }
                    })
                    .setIcon(android.R.drawable.ic_dialog_alert)
                    .show();
        }

        if (!BTAdapter.isEnabled()) {
            Intent enableBT = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(enableBT, REQUEST_BLUETOOTH);
        }

        // Initialize the BluetoothChatService to perform bluetooth connections
        mChatService = new BluetoothChatService(MainActivity.this, readHandler);

        // Initialize the buffer for outgoing messages
        mOutStringBuffer = new StringBuffer("");

        // Check that the activity is using the layout version with
        // the fragment_container FrameLayout
        if (findViewById(R.id.fragment_container) != null) {

            // However, if we're being restored from a previous state,
            // then we don't need to do anything and should return or else
            // we could end up with overlapping fragments.
            if (savedInstanceState != null) {
                return;
            }

            // Create a new Fragment to be placed in the activity layout
            BluetoothFragment btFragment = BluetoothFragment.newInstance(BTAdapter, mChatService, bluetoothStatus);

            // In case this activity was started with special instructions from an
            // Intent, pass the Intent's extras to the fragment as arguments
            btFragment.setArguments(getIntent().getExtras());

            // Add the fragment to the 'fragment_container' FrameLayout
            getSupportFragmentManager().beginTransaction()
                    .add(R.id.fragment_container, btFragment, "BluetoothFragment")
                    .addToBackStack("BluetoothFragment")
                    .commit();
        }

        DrawerLayout drawer = (DrawerLayout) findViewById(R.id.drawer_layout);
        mLayout = drawer;
        ActionBarDrawerToggle toggle = new ActionBarDrawerToggle(
                this, drawer, toolbar, R.string.navigation_drawer_open, R.string.navigation_drawer_close);
        drawer.addDrawerListener(toggle);
        toggle.syncState();

        NavigationView navigationView = (NavigationView) findViewById(R.id.nav_view);
        navigationView.setNavigationItemSelectedListener(this);

    }

    @Override
    public void onBackPressed() {
        DrawerLayout drawer = (DrawerLayout) findViewById(R.id.drawer_layout);
        if (drawer.isDrawerOpen(GravityCompat.START)) {
            drawer.closeDrawer(GravityCompat.START);
        } else {
            super.onBackPressed();
        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            // Initialize a new instance of LayoutInflater service
            LayoutInflater inflater = (LayoutInflater) getSystemService(LAYOUT_INFLATER_SERVICE);

            // Inflate the custom layout/view
            View customView = inflater.inflate(R.layout.popup_layout,null);

                /*
                    public PopupWindow (View contentView, int width, int height)
                        Create a new non focusable popup window which can display the contentView.
                        The dimension of the window must be passed to this constructor.

                        The popup does not provide any background. This should be handled by
                        the content view.

                    Parameters
                        contentView : the popup's content
                        width : the popup's width
                        height : the popup's height
                */
            // Initialize a new instance of popup window
            mPopupWindow = new PopupWindow(
                    customView,
                    ViewGroup.LayoutParams.WRAP_CONTENT,
                    ViewGroup.LayoutParams.WRAP_CONTENT
            );

            // Set an elevation value for popup window
            // Call requires API level 21
            if(Build.VERSION.SDK_INT>=21){
                mPopupWindow.setElevation(5.0f);
            }

            TextView mdf_exp = (TextView) customView.findViewById(R.id.mdf_exp);
            mdf_exp.setText(exploredHex);
            TextView mdf_obs = (TextView) customView.findViewById(R.id.mdf_obs);
            mdf_obs.setText(obstacleHex);
            TextView mdf_log = (TextView) customView.findViewById(R.id.mdf_log);
            mdf_log.setText(readMessage);

            // Get a reference for the custom view close button
            ImageButton closeButton = (ImageButton) customView.findViewById(R.id.ib_close);

            // Set a click listener for the popup window close button
            closeButton.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View view) {
                    // Dismiss the popup window
                    mPopupWindow.dismiss();
                }
            });

                /*
                    public void showAtLocation (View parent, int gravity, int x, int y)
                        Display the content view in a popup window at the specified location. If the
                        popup window cannot fit on screen, it will be clipped.
                        Learn WindowManager.LayoutParams for more information on how gravity and the x
                        and y parameters are related. Specifying a gravity of NO_GRAVITY is similar
                        to specifying Gravity.LEFT | Gravity.TOP.

                    Parameters
                        parent : a parent view to get the getWindowToken() token from
                        gravity : the gravity which controls the placement of the popup window
                        x : the popup's x location offset
                        y : the popup's y location offset
                */
            // Finally, show the popup window at the center location of root relative layout
            mPopupWindow.showAtLocation(mLayout, Gravity.CENTER,0,0);
        }

        return super.onOptionsItemSelected(item);
    }

    @SuppressWarnings("StatementWithEmptyBody")
    @Override
    public boolean onNavigationItemSelected(MenuItem item) {
        String tagText;
        Fragment fragment;
        FragmentManager fm = getSupportFragmentManager();

        // Handle navigation view item clicks here.
        int id = item.getItemId();

        if (id == R.id.nav_bluetooth) {
            tagText = "BluetoothFragment";
            fragment = fm.findFragmentByTag(tagText);
            if (fragment == null) {
                fragment = BluetoothFragment.newInstance(BTAdapter, mChatService, bluetoothStatus);
            }
        } else if (id == R.id.nav_sendText) {
            tagText = "SendTextFragment";
            fragment = fm.findFragmentByTag(tagText);
            if (fragment == null) {
                fragment = SendTextFragment.newInstance(BTAdapter, mChatService, bluetoothStatus);
            }
        } else if (id == R.id.nav_arena) {
            tagText = "ArenaFragment";
            fragment = fm.findFragmentByTag(tagText);
            if (fragment == null) {
                fragment = ArenaFragment.newInstance(BTAdapter, mChatService, bluetoothStatus,
                        exploredBin, obstacleBin, dirRow, dirCol, dirDir, dirMoveOrStop);
            }
        }  else if (id == R.id.nav_misc) {
            tagText = "MiscFragment";
            fragment = fm.findFragmentByTag(tagText);
            if (fragment == null) {
                fragment = MiscellaneousFragment.newInstance(BTAdapter, mChatService, bluetoothStatus);
            }
        } else {
            fragment=null;
            tagText=null;
        }

        fm.beginTransaction().replace(R.id.fragment_container, fragment, tagText)
                .addToBackStack(tagText)
                .commit();

        DrawerLayout drawer = (DrawerLayout) findViewById(R.id.drawer_layout);
        drawer.closeDrawer(GravityCompat.START);
        return true;
    }

    @Override
    public void onFragmentInteraction(String deviceAddress) {
        arduinoAddr = deviceAddress;
    }

    public void onBluetoothStateChange(String bluetoothStatus) {
        BluetoothFragment btFrag = (BluetoothFragment) getSupportFragmentManager().findFragmentByTag("BluetoothFragment");
        if(btFrag!=null){btFrag.updateBTStatus(bluetoothStatus);}else{Log.d("BT_DEBUG", bluetoothStatus);}
        ArenaFragment arenaFrag = (ArenaFragment) getSupportFragmentManager().findFragmentByTag("ArenaFragment");
        if(arenaFrag!=null){arenaFrag.updateBTStatus(bluetoothStatus);}else{Log.d("ARENA_DEBUG", bluetoothStatus);}
    }

    public void onVoiceCommand(String command) {
        ArenaFragment arenaFrag = (ArenaFragment) getSupportFragmentManager().findFragmentByTag("ArenaFragment");
        if(arenaFrag!=null){
            arenaFrag.parseVoiceCommand(command);
        }else{
            Log.d("ARENA_DEBUG", command);
        }
    }

    public void onOrientationChanged(float x, float y, float z){
        ArenaFragment arenaFrag = (ArenaFragment) getSupportFragmentManager().findFragmentByTag("ArenaFragment");
        if(arenaFrag!=null){
            arenaFrag.tiltSteer(x, y, z);
        }else{}
    }

    /**
     * The Handler that gets information back from the BluetoothChatService
     */
    @SuppressLint("HandlerLeak")
    private final Handler readHandler = new Handler() {

        @Override
        public void handleMessage(Message msg) {
            TextView bluetooth = (TextView) findViewById(R.id.statusText);
            TextView arena = (TextView) findViewById(R.id.bluetoothStatus);
            switch (msg.what) {
                case Constants.MESSAGE_STATE_CHANGE:
                    switch (msg.arg1) {
                        case BluetoothChatService.STATE_CONNECTED:
                            bluetoothStatus=getString(R.string.title_connected_to, mConnectedDeviceName);
                            onBluetoothStateChange(bluetoothStatus);
//                            if(bluetooth != null){
//                                bluetooth.setText(getString(R.string.title_connected_to, mConnectedDeviceName));
//                            }
//                            else if (arena != null){
//                                arena.setText(getString(R.string.title_connected_to, mConnectedDeviceName));
//                            }
                            break;
                        case BluetoothChatService.STATE_CONNECTING:
                            bluetoothStatus=getResources().getString(R.string.title_connecting);
                            onBluetoothStateChange(bluetoothStatus);
//                            if(bluetooth != null){
//                                bluetooth.setText(R.string.title_connecting);
//                            }
//                            else if (arena != null){
//                                arena.setText(R.string.title_connecting);
//                            }
                            break;
                        case BluetoothChatService.STATE_NONE:
                            bluetoothStatus=getResources().getString(R.string.bluetooth_disconnected);
                            onBluetoothStateChange(bluetoothStatus);
//                            if(bluetooth != null){
//                                bluetooth.setText(R.string.bluetooth_disconnected);
//                            }
//                            else if (arena != null){
//                                arena.setText(R.string.bluetooth_disconnected);
//                            }
                            break;
                        case BluetoothChatService.STATE_LOST:
                            bluetoothStatus=getResources().getString(R.string.bluetooth_disconnected);
                            onBluetoothStateChange(bluetoothStatus);
//                            if(bluetooth != null){
//                                bluetooth.setText(R.string.bluetooth_disconnected);
//                            }
//                            else if (arena != null){
//                                arena.setText(R.string.bluetooth_disconnected);
//                            }
                            break;
                    }
                    break;
                case Constants.MESSAGE_READ:
                    byte[] readBuf = (byte[]) msg.obj;
                    // construct a string from the valid bytes in the buffer
                    String readMessage = new String(readBuf, 0, msg.arg1);
                    TextView rf = (TextView) findViewById(R.id.readField);
                    if(rf != null){
                        rf.setText(readMessage);
                    }
                    if(readMessage.startsWith("MDF") || readMessage.startsWith("DIR")){
                        Log.d("MDFFF", readMessage);
                        if(readMessage.startsWith("MDF")){MainActivity.readMessage = readMessage;}
                        onMapUpdateReceived(readMessage);
                    }
                    break;
                case Constants.MESSAGE_DEVICE_NAME:
                    // save the connected device's name
                    mConnectedDeviceName = msg.getData().getString(Constants.DEVICE_NAME);
                    Toast.makeText(MainActivity.this, "Connected to "
                            + mConnectedDeviceName, Toast.LENGTH_SHORT).show();
                    break;
                case Constants.MESSAGE_TOAST:
                    Toast.makeText(MainActivity.this, msg.getData().getString(Constants.TOAST),
                            Toast.LENGTH_SHORT).show();
                    break;
            }
        }
    };

    String hexToBinary(String hex) {
        return new BigInteger("1" + hex,16).toString(2).substring(1);
    }

    /**
     * Sends a message.
     *
     * @param message A string of text to send.
     */
    protected void sendMessage(String message) {
        EditText mOutEditText = (EditText) findViewById(R.id.writeField);

        // Check that we're actually connected before trying anything
        if (mChatService.getState() != BluetoothChatService.STATE_CONNECTED) {
            Toast.makeText(MainActivity.this, R.string.not_connected, Toast.LENGTH_SHORT).show();
            return;
        }

        // Check that there's actually something to send
        if (message.length() > 0) {
            // Get the message bytes and tell the BluetoothChatService to write
            byte[] send = message.getBytes();
            mChatService.write(send);

            if(mOutEditText != null){
                // Reset out string buffer to zero and clear the edit text field
                mOutStringBuffer.setLength(0);
                mOutEditText.setText(mOutStringBuffer);

                // Initialize the compose field with a listener for the return key
                mOutEditText.setOnEditorActionListener(mWriteListener);
            }
        }
    }

    /**
     * The action listener for the EditText widget, to listen for the return key
     */
    private TextView.OnEditorActionListener mWriteListener
            = new TextView.OnEditorActionListener() {
        public boolean onEditorAction(TextView view, int actionId, KeyEvent event) {
            // If the action is a key-up event on the return key, send the message
            if (actionId == EditorInfo.IME_NULL && event.getAction() == KeyEvent.ACTION_UP) {
                String message = view.getText().toString();
                sendMessage(message);
            }
            return true;
        }
    };

    @Override
    public void onFragmentInteraction(Uri uri) {

    }

    public void onMapUpdateReceived(String message){
        //pass the strings to arena fragment to be parsed
        ArenaFragment arenaFrag = (ArenaFragment) getSupportFragmentManager().findFragmentByTag("ArenaFragment");
        Log.d("MDF_OR_DIR", message);
        if(arenaFrag != null){
            if(message.startsWith("MDF")){
                String mdfStr = message.substring(3);
                String exploredHexStr = mdfStr.split("L")[0];
                String obstacleHexStr = mdfStr.split("L")[1];
                this.exploredHex = exploredHexStr;
                this.obstacleHex = obstacleHexStr;
                String exploredBin = hexToBinary(exploredHexStr);
                String obstacleBin = hexToBinary(obstacleHexStr);

                this.exploredBin = exploredBin.substring(2, 302);
                this.obstacleBin = obstacleBin;
                arenaFrag.updateMap(exploredBin.substring(2, 302), obstacleBin, false);
                arenaFrag.updateRobot(dirRow, dirCol, dirDir, dirMoveOrStop, false);

            } else if (message.startsWith("DIR")) {
                String dirStr = message.substring(3);
                String dirRow = dirStr.split("L")[0];
                String dirCol = dirStr.split("L")[1];
                String dirDir = dirStr.split("L")[2];
                String dirMoveOrStop = dirStr.split("L")[3];
                this.dirRow = dirRow;
                this.dirCol = dirCol;
                this.dirDir = dirDir;
                this.dirMoveOrStop = dirMoveOrStop;
                arenaFrag.updateMap(exploredBin, obstacleBin, false);
                arenaFrag.updateRobot(dirRow, dirCol, dirDir, dirMoveOrStop, false);
            }
        }
    }

    public void startSpeechRecognizer() {
        Intent intent = new Intent
                (RecognizerIntent.ACTION_RECOGNIZE_SPEECH);
        intent.putExtra(RecognizerIntent.EXTRA_LANGUAGE_MODEL,
                RecognizerIntent.LANGUAGE_MODEL_FREE_FORM);
        intent.putExtra(RecognizerIntent.EXTRA_PROMPT, "What is your command?");
        startActivityForResult(intent, REQUEST_SPEECH_RECOGNIZER);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode,
                                    Intent data) {
        super.onActivityResult(requestCode, resultCode, data);

        if (requestCode == REQUEST_SPEECH_RECOGNIZER) {
            if (resultCode == RESULT_OK) {
                List<String> results = data.getStringArrayListExtra
                        (RecognizerIntent.EXTRA_RESULTS);
                mVoiceCommand = results.get(0);

                onVoiceCommand(mVoiceCommand);

            }
        }
    }
}
