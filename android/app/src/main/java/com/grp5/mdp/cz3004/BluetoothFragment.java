package com.grp5.mdp.cz3004;

import android.annotation.SuppressLint;
import android.app.ActionBar;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothManager;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.SharedPreferences;
import android.net.Uri;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentActivity;
import android.util.Log;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.inputmethod.EditorInfo;
import android.widget.AbsListView;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.ListAdapter;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.ToggleButton;

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.Set;

import static android.provider.Telephony.Mms.Part.FILENAME;
import static com.grp5.mdp.cz3004.MainActivity.REQUEST_BLUETOOTH;


/**
 * A simple {@link Fragment} subclass.
 * Activities that contain this fragment must implement the
 * {@link BluetoothFragment.OnFragmentInteractionListener} interface
 * to handle interaction events.
 * Use the {@link BluetoothFragment#newInstance} factory method to
 * create an instance of this fragment.
 */
public class BluetoothFragment extends Fragment implements AdapterView.OnItemClickListener {

    private static BluetoothAdapter mBluetoothAdapter = null;

    // Intent request codes
    private static final int REQUEST_CONNECT_DEVICE_SECURE = 1;
    private static final int REQUEST_CONNECT_DEVICE_INSECURE = 2;
    private static final int REQUEST_ENABLE_BT = 3;
    private static String status;

    private ArrayList<DeviceDetails> deviceItemList;

    private AbsListView mListView;

    private ArrayAdapter<DeviceDetails> mAdapter;

    private OnFragmentInteractionListener mListener;

    /**
     * Member object for the chat services
     */
    private static BluetoothChatService mChatService = null;

    ToggleButton scan;

    public BluetoothFragment() {
        // Required empty public constructor
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        Log.d("DEVICELIST", "Super called for DeviceListFragment onCreate\n");
        deviceItemList = new ArrayList<DeviceDetails>();

        Set<BluetoothDevice> pairedDevices = mBluetoothAdapter.getBondedDevices();
        if (pairedDevices.size() > 0) {
            for (BluetoothDevice device : pairedDevices) {
                DeviceDetails newDevice= new DeviceDetails(device.getName(),device.getAddress(),"false");
                deviceItemList.add(newDevice);
            }
        }

        // If there are no devices, add an item that states so. It will be handled in the view.
        if(deviceItemList.size() == 0) {
            deviceItemList.add(new DeviceDetails("No Devices", "", "false"));
        }

        Log.d("DEVICELIST", "DeviceList populated\n");

        mAdapter = new DeviceListAdapter(getActivity(), deviceItemList, mBluetoothAdapter);

        Log.d("DEVICELIST", "Adapter created\n");

    }

//    @Override
//    public void onStart() {
//        super.onStart();
//        // If BT is not on, request that it be enabled.
//        // setupChat() will then be called during onActivityResult
//        if (!mBluetoothAdapter.isEnabled()) {
//            Log.d("DEBUGG", "test2");
//            Intent enableIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
//            startActivityForResult(enableIntent, REQUEST_ENABLE_BT);
//            // Otherwise, setup the chat session
//        }
//    }

    @Override
    public void onResume() {
        super.onResume();

        // Performing this check in onResume() covers the case in which BT was
        // not enabled during onStart(), so we were paused to enable it...
        // onResume() will be called when ACTION_REQUEST_ENABLE activity returns.
        if (mChatService != null) {
            // Only if the state is STATE_NONE, do we know that we haven't started already
            if (mChatService.getState() == BluetoothChatService.STATE_NONE) {
                // Start the Bluetooth chat services
                mChatService.start();
            }
        }
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_bluetooth, container, false);
        scan = view.findViewById(R.id.scan);
        Button discovery = view.findViewById(R.id.discovery_button);
        Button writeButton = view.findViewById(R.id.writeButton);
        Button saveButton = view.findViewById(R.id.saveButton);
        EditText writeField = view.findViewById(R.id.writeField);
        TextView bluetoothStatus = view.findViewById(R.id.statusText);
        // Set the adapter
        mListView = view.findViewById(android.R.id.list);
        mListView.setAdapter(mAdapter);

        // Set OnItemClickListener so we can be notified on item clicks
        mListView.setOnItemClickListener(this);

        SharedPreferences customText = getActivity().getSharedPreferences(Constants.PREF_NAME, Context.MODE_PRIVATE);
        String custom_write_text = customText.getString(Constants.PREF_NAME, "");

        writeField.setText(custom_write_text);

        bluetoothStatus.setText(status);

        discovery.setOnClickListener(new Button.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent discoverableIntent =
                        new Intent(BluetoothAdapter.ACTION_REQUEST_DISCOVERABLE);
                discoverableIntent.putExtra(BluetoothAdapter.EXTRA_DISCOVERABLE_DURATION, 300);
                startActivity(discoverableIntent);
            }
        });

        writeButton.setOnClickListener(new Button.OnClickListener() {
            @Override
            public void onClick(View view) {
                Log.v("WRITE", "Write button pressed.");

                TextView tv = getActivity().findViewById(R.id.writeField);
                String data = tv.getText().toString();
                ((MainActivity) getActivity()).sendMessage(data);
            }
        });

        saveButton.setOnClickListener(new Button.OnClickListener() {

            @Override
            public void onClick(View view) {
                Log.v("SAVE", "Save button pressed.");
                TextView tv = getActivity().findViewById(R.id.writeField);
                String data = tv.getText().toString();

                //TODO: Save data in persistent storage
                // We need an Editor object to make preference changes.
                // All objects are from android.context.Context
                SharedPreferences customText = getActivity().getSharedPreferences(Constants.PREF_NAME, Context.MODE_PRIVATE);
                SharedPreferences.Editor editor = customText.edit();
                editor.putString(Constants.PREF_NAME, data);

                // Commit the edits!
                editor.commit();
            }
        });

        scan.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                IntentFilter filter = null;
                filter = new IntentFilter();
                filter.addAction(BluetoothDevice.ACTION_FOUND);
                filter.addAction(BluetoothAdapter.ACTION_DISCOVERY_FINISHED);
                if (isChecked) {
                    if (!mBluetoothAdapter.isEnabled()) {
                        Intent enableBT = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
                        startActivityForResult(enableBT, REQUEST_BLUETOOTH);
                        scan.setChecked(false);
                    } else {
                        mAdapter.clear();
                        getActivity().registerReceiver(bReciever, filter);
                        Toast.makeText(getActivity(), "Scan started!", Toast.LENGTH_SHORT).show();
                        mBluetoothAdapter.startDiscovery();
                    }
                } else {
                    getActivity().unregisterReceiver(bReciever);
                    Toast.makeText(getActivity(), "Scan stopped!", Toast.LENGTH_SHORT).show();
                    mBluetoothAdapter.cancelDiscovery();
                }
            }
        });

        return view;
    }

    private final BroadcastReceiver bReciever = new BroadcastReceiver() {
        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();
            if (BluetoothDevice.ACTION_FOUND.equals(action)) {
                Log.d("DEVICELIST", "Bluetooth device found\n");
                BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                // Create a new device item
                DeviceDetails newDevice = new DeviceDetails(device.getName(), device.getAddress(), "false");
                // Add it to our adapter
                mAdapter.add(newDevice);
                mAdapter.notifyDataSetChanged();
            } else if (BluetoothAdapter.ACTION_DISCOVERY_FINISHED.equals(action)) {
                Toast.makeText(context, "Scan finished!", Toast.LENGTH_SHORT).show();
                scan.setChecked(false);
            }
        }
    };

    /**
     * Use this factory method to create a new instance of
     * this fragment using the provided parameters.
     *
     */
    // TODO: Rename and change types and number of parameters
    public static BluetoothFragment newInstance(BluetoothAdapter adapter, BluetoothChatService bcs, String bluetoothStatus) {
        BluetoothFragment fragment = new BluetoothFragment();
        mBluetoothAdapter = adapter;
        mChatService = bcs;
        status = bluetoothStatus;
        return fragment;
    }

    @Override
    public void onAttach(Context context) {
        super.onAttach(context);
        if (context instanceof OnFragmentInteractionListener) {
            mListener = (OnFragmentInteractionListener) context;
        } else {
            throw new RuntimeException(context.toString()
                    + " must implement OnFragmentInteractionListener");
        }
    }

    @Override
    public void onDetach() {
        super.onDetach();
        mListener = null;
    }

    @Override
    public void onItemClick(AdapterView<?> parent, View view, int position, long id) {

        Log.d("DEVICELIST", "onItemClick position: " + position +
                " id: " + id + " name: " + deviceItemList.get(position).getDeviceName() + "\n");

        connectDevice(deviceItemList.get(position).getAddress(), true);

        if (mListener != null) {
            // Notify the active callbacks interface (the activity, if the
            // fragment is attached to one) that an item has been selected.
            mListener.updateRPiAddress(deviceItemList.get(position).getAddress());
        }

    }

    /**
     * Establish connection with other device
     *
     * @param secure Socket Security type - Secure (true) , Insecure (false)
     */
    private void connectDevice(String address, boolean secure) {
        BluetoothDevice device = mBluetoothAdapter.getRemoteDevice(address);
        // Attempt to connect to the device
        mChatService.connect(device, secure);
    }

    public void updateBTStatus(String btStatus){
        TextView tv = getActivity().findViewById(R.id.statusText);
        if(tv!=null){tv.setText(btStatus);}
    }

    /**
     * The default content for this Fragment has a TextView that is shown when
     * the list is empty. If you would like to change the text, call this method
     * to supply the text it should use.
     */
    public void setEmptyText(CharSequence emptyText) {
        View emptyView = mListView.getEmptyView();

        if (emptyView instanceof TextView) {
            ((TextView) emptyView).setText(emptyText);
        }
    }

    /**
     * This interface must be implemented by activities that contain this
     * fragment to allow an interaction in this fragment to be communicated
     * to the activity and potentially other fragments contained in that
     * activity.
     * <p>
     * See the Android Training lesson <a href=
     * "http://developer.android.com/training/basics/fragments/communicating.html"
     * >Communicating with Other Fragments</a> for more information.
     */
    public interface OnFragmentInteractionListener {
        void updateRPiAddress(String deviceAddress);
        void onBluetoothStateChange(String bluetoothStatus);
    }
}
