package com.grp5.mdp.cz3004;

import android.annotation.SuppressLint;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothManager;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.Uri;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.v4.app.Fragment;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AbsListView;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.ListAdapter;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.ToggleButton;

import java.util.ArrayList;
import java.util.Set;

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

    private ArrayList<DeviceDetails> deviceItemList;

    private AbsListView mListView;

    private ArrayAdapter<DeviceDetails> mAdapter;

    private OnFragmentInteractionListener mListener;

    // Handlers used to pass data between threads
    private Handler readHandler;
    private Handler writeHandler;

    ToggleButton scan;

    // The thread that does all the work
    static BluetoothThread btt;

    public BluetoothFragment() {
        // Required empty public constructor
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
    public static BluetoothFragment newInstance(BluetoothAdapter adapter, BluetoothThread thread) {
        BluetoothFragment fragment = new BluetoothFragment();
        mBluetoothAdapter = adapter;
        btt = thread;
        return fragment;
    }

    @SuppressLint("HandlerLeak")
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

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        View view = inflater.inflate(R.layout.fragment_bluetooth, container, false);
        scan = (ToggleButton) view.findViewById(R.id.scan);
        Button discovery = view.findViewById(R.id.discovery_button);
        Button writeButton = view.findViewById(R.id.writeButton);
        // Set the adapter
        mListView = (AbsListView) view.findViewById(android.R.id.list);
        ((AdapterView<ListAdapter>) mListView).setAdapter(mAdapter);

        // Set OnItemClickListener so we can be notified on item clicks
        mListView.setOnItemClickListener(this);

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

                TextView tv = (TextView)getActivity().findViewById(R.id.writeField);
                String data = tv.getText().toString();

                Message msg = Message.obtain();
                msg.obj = data;
                writeHandler.sendMessage(msg);
            }
        });

        scan.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                IntentFilter filter = null;
                filter = new IntentFilter();
                filter.addAction(BluetoothDevice.ACTION_FOUND);
                filter.addAction(BluetoothAdapter.ACTION_DISCOVERY_FINISHED);
                if (isChecked) {
                    if(!mBluetoothAdapter.isEnabled()){
                        Intent enableBT = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
                        startActivityForResult(enableBT, REQUEST_BLUETOOTH);
                    }
                    mAdapter.clear();
                    getActivity().registerReceiver(bReciever, filter);
                    Toast.makeText(getActivity(), "Scan started!", Toast.LENGTH_SHORT).show();
                    mBluetoothAdapter.startDiscovery();
                } else {
                    getActivity().unregisterReceiver(bReciever);
                    Toast.makeText(getActivity(), "Scan stopped!", Toast.LENGTH_SHORT).show();
                    mBluetoothAdapter.cancelDiscovery();
                }
            }
        });

        return view;
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

        connectDeviceSelected(deviceItemList.get(position).getAddress());

        if (mListener != null) {
            // Notify the active callbacks interface (the activity, if the
            // fragment is attached to one) that an item has been selected.
            mListener.onFragmentInteraction(deviceItemList.get(position).getAddress(), readHandler);
        }

    }

    @SuppressLint("HandlerLeak")
    private void connectDeviceSelected(String address) {
        Log.v("BLUETOOTH", "Connect button pressed.");

        // Only one thread at a time
        if (btt != null) {
            Log.w("BLUETOOTH", "Already connected!");
            return;
        }

        // Initialize the Bluetooth thread, passing in a MAC address
        // and a Handler that will receive incoming messages
        btt = new BluetoothThread(address, new Handler() {

            public void handleMessage(Message message) {

                String s = (String) message.obj;

                // Do something with the message
                switch (s) {
                    case "CONNECTED": {
                        TextView tv = (TextView) getActivity().findViewById(R.id.statusText);
                        tv.setText(R.string.bluetooth_connected);
                        break;
                    }
                    case "DISCONNECTED": {
                        TextView tv = (TextView) getActivity().findViewById(R.id.statusText);
                        tv.setText(R.string.bluetooth_disconnected);
                        break;
                    }
                    case "CONNECTION FAILED": {
                        TextView tv = (TextView) getActivity().findViewById(R.id.statusText);
                        tv.setText(R.string.bluetooth_failed);
                        btt = null;
                        break;
                    }
                    default: {
                        TextView tv = (TextView) getActivity().findViewById(R.id.readField);
                        tv.setText(s);
                    }
                }
            }
        });

        // Run the thread
        btt.start();

        // Get the handler that is used to send messages
        writeHandler = btt.getWriteHandler();

        TextView tv = (TextView) getActivity().findViewById(R.id.statusText);
        tv.setText("Connecting...");
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
        void onFragmentInteraction(String deviceAddress, Handler handler);
    }
}
