package com.grp5.mdp.cz3004;

import android.bluetooth.BluetoothAdapter;
import android.content.Context;
import android.net.Uri;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.v4.app.Fragment;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.TextView;


/**
 * A simple {@link Fragment} subclass.
 * Activities that contain this fragment must implement the
 * {@link SendTextFragment.OnFragmentInteractionListener} interface
 * to handle interaction events.
 * Use the {@link SendTextFragment#newInstance} factory method to
 * create an instance of this fragment.
 */
public class SendTextFragment extends Fragment {

    // The thread that does all the work
    static BluetoothThread btt;

    private static BluetoothAdapter mBluetoothAdapter = null;

    // Handler for writing messages to the Bluetooth connection
    Handler writeHandler;
    static Handler readHandler;

    Button b;

    // Tag for logging
    private static final String TAG = "BluetoothActivity";

    private OnFragmentInteractionListener mListener;

    public SendTextFragment() {
        // Required empty public constructor
    }

    /**
     * Use this factory method to create a new instance of
     * this fragment using the provided parameters.
     *
     * @return A new instance of fragment SendTextFragment.
     */
    // TODO: Rename and change types and number of parameters
    public static SendTextFragment newInstance(BluetoothAdapter adapter, BluetoothThread thread, Handler handler) {
        SendTextFragment fragment = new SendTextFragment();
        mBluetoothAdapter = adapter;
        btt = thread;
        readHandler = handler;
        return fragment;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        b = (Button) getActivity().findViewById(R.id.writeButton);
    }

    /**
     * Send a message using the Bluetooth thread's write handler.
     */
    public void writeButtonPressed(View v) {
        Log.v(TAG, "Write button pressed.");

        TextView tv = (TextView)getActivity().findViewById(R.id.writeField);
        String data = tv.getText().toString();

        Message msg = Message.obtain();
        msg.obj = data;
        writeHandler.sendMessage(msg);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        return inflater.inflate(R.layout.fragment_send_text, container, false);
    }

    // TODO: Rename method, update argument and hook method into UI event
    public void onButtonPressed(Uri uri) {
        if (mListener != null) {
            mListener.onFragmentInteraction(uri);
        }
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
        // TODO: Update argument type and name
        void onFragmentInteraction(Uri uri);
    }
}
