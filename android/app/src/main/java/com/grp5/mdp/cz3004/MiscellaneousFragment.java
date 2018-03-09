package com.grp5.mdp.cz3004;

import android.bluetooth.BluetoothAdapter;
import android.content.Context;
import android.net.Uri;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;


/**
 * A simple {@link Fragment} subclass.
 * Activities that contain this fragment must implement the
 * {@link MiscellaneousFragment.OnFragmentInteractionListener} interface
 * to handle interaction events.
 * Use the {@link MiscellaneousFragment#newInstance} factory method to
 * create an instance of this fragment.
 */
public class MiscellaneousFragment extends Fragment {
    // TODO: Rename parameter arguments, choose names that match
    // the fragment initialization parameters, e.g. ARG_ITEM_NUMBER
    private static final String ARG_PARAM1 = "param1";
    private static final String ARG_PARAM2 = "param2";
    private static BluetoothAdapter mBluetoothAdapter;
    private static BluetoothChatService mChatService;

    // TODO: Rename and change types of parameters
    private String mParam1;
    private String mParam2;

    private OnFragmentInteractionListener mListener;

    public MiscellaneousFragment() {
        // Required empty public constructor
    }

    /**
     * Use this factory method to create a new instance of
     * this fragment using the provided parameters.
     *
     * @return A new instance of fragment MiscellaneousFragment.
     */
    // TODO: Rename and change types and number of parameters
    public static MiscellaneousFragment newInstance(BluetoothAdapter adapter, BluetoothChatService bcs, String bluetoothStatus) {
        MiscellaneousFragment fragment = new MiscellaneousFragment();
        mBluetoothAdapter = adapter;
        mChatService = bcs;
        return fragment;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        View view = inflater.inflate(R.layout.fragment_miscellaneous, container, false);
        Button btForward = view.findViewById(R.id.forwardButton);
        Button btTurnLeft = view.findViewById(R.id.turnLeftButton);
        Button btTurnRight = view.findViewById(R.id.turnRightButton);



        btForward.setOnClickListener(
                new View.OnClickListener(){
                    public void onClick(View view) {
                        //need to implement getting value of start point and direction
                        EditText tvForward = getActivity().findViewById(R.id.forwardField);
                        Log.d("DEBUG", tvForward.toString());
                        String forwardDistance = tvForward.getText().toString();
                        String forward = "move"+" "+forwardDistance;
                        ((MainActivity)getActivity()).sendMessage(forward);
                    }
                }
        );


        btTurnLeft.setOnClickListener(
                new View.OnClickListener(){
                    public void onClick(View view) {
                        //need to implement getting value of start point and direction
                        EditText tvTurnLeft = getActivity().findViewById(R.id.turnLeftField);
                        String turnLeftDegree = tvTurnLeft.getText().toString();
                        String turnLeft = "rotate"+" "+"-"+turnLeftDegree;
                        ((MainActivity)getActivity()).sendMessage(turnLeft);
                    }
                }
        );

        btTurnRight.setOnClickListener(
                new View.OnClickListener(){
                    public void onClick(View view) {
                        //need to implement getting value of start point and direction
                        EditText tvTurnRight = getActivity().findViewById(R.id.turnRightField);
                        String turnRightDegree = tvTurnRight.getText().toString();
                        String turnRight = "rotate"+" "+turnRightDegree;
                        ((MainActivity)getActivity()).sendMessage(turnRight);
                    }
                }
        );

        return view;
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
