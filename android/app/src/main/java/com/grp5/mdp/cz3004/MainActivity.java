package com.grp5.mdp.cz3004;

import android.annotation.SuppressLint;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.Uri;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.v4.app.Fragment;
import android.support.design.widget.NavigationView;
import android.support.v4.app.FragmentManager;
import android.support.v4.view.GravityCompat;
import android.support.v4.widget.DrawerLayout;
import android.support.v7.app.ActionBarDrawerToggle;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.TextView;
import android.widget.Toast;

public class MainActivity extends AppCompatActivity
        implements NavigationView.OnNavigationItemSelectedListener,
        BluetoothFragment.OnFragmentInteractionListener,
        SendTextFragment.OnFragmentInteractionListener{

    private BluetoothAdapter BTAdapter;
    private Fragment fragment;

    // The thread that does all the work
    BluetoothThread btt;
    String arduinoAddr;

    public static int REQUEST_BLUETOOTH = 1;
    private Handler readHandler;

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
            BluetoothFragment btFragment = BluetoothFragment.newInstance(BTAdapter, btt);

            // In case this activity was started with special instructions from an
            // Intent, pass the Intent's extras to the fragment as arguments
            btFragment.setArguments(getIntent().getExtras());

            // Add the fragment to the 'fragment_container' FrameLayout
            getSupportFragmentManager().beginTransaction()
                    .add(R.id.fragment_container, btFragment).commit();
        }

        DrawerLayout drawer = (DrawerLayout) findViewById(R.id.drawer_layout);
        ActionBarDrawerToggle toggle = new ActionBarDrawerToggle(
                this, drawer, toolbar, R.string.navigation_drawer_open, R.string.navigation_drawer_close);
        drawer.addDrawerListener(toggle);
        toggle.syncState();

        NavigationView navigationView = (NavigationView) findViewById(R.id.nav_view);
        navigationView.setNavigationItemSelectedListener(this);

        readHandler = new Handler() {

            public void handleMessage(Message message) {

                String s = (String) message.obj;

                // Do something with the message
                switch (s) {
                    case "CONNECTED": {
                        TextView tv = (TextView) findViewById(R.id.statusText);
                        tv.setText(R.string.bluetooth_connected);
                        break;
                    }
                    case "DISCONNECTED": {
                        TextView tv = (TextView) findViewById(R.id.statusText);
                        tv.setText(R.string.bluetooth_disconnected);
                        break;
                    }
                    case "CONNECTION FAILED": {
                        TextView tv = (TextView) findViewById(R.id.statusText);
                        tv.setText(R.string.bluetooth_failed);
                        btt = null;
                        break;
                    }
                    default: {
                        TextView tv = (TextView) findViewById(R.id.readField);
                        tv.setText(s);
                    }
                }
            }
        };
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
            return true;
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
                fragment = BluetoothFragment.newInstance(BTAdapter, btt);
            }
        } else if (id == R.id.nav_sendText) {
            tagText = "SendTextFragment";
            fragment = fm.findFragmentByTag(tagText);
            if (fragment == null) {
                fragment = SendTextFragment.newInstance(BTAdapter, btt, readHandler);
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
    public void onFragmentInteraction(String deviceAddress, Handler handler) {
        readHandler = handler;
        arduinoAddr = deviceAddress;
    }

    @Override
    public void onFragmentInteraction(Uri uri) {

    }
}
