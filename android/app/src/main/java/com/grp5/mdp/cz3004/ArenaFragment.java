package com.grp5.mdp.cz3004;

import android.bluetooth.BluetoothAdapter;
import android.content.Context;
import android.content.res.Configuration;
import android.graphics.Color;
import android.graphics.PorterDuff;
import android.icu.text.StringPrepParseException;
import android.net.Uri;
import android.os.Bundle;
import android.os.Handler;
import android.support.v4.app.Fragment;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.Surface;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.widget.AbsListView;
import android.widget.AdapterView;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.EditText;
import android.widget.GridView;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.ToggleButton;

import java.util.ArrayList;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;

/**
 * A simple {@link Fragment} subclass.
 * Activities that contain this fragment must implement the
 * {@link ArenaFragment.OnMapUpdateListener} interface
 * to handle interaction events.
 * Use the {@link ArenaFragment#newInstance} factory method to
 * create an instance of this fragment.
 */
public class ArenaFragment extends Fragment {
    private static BluetoothAdapter mBluetoothAdapter;
    private static BluetoothChatService mChatService;
    private static String btStatus;

    //static ArrayList<GridImage> gridList = new ArrayList<GridImage>();

    private static String exploredBin;
    private static String obstacleBin;
    private static String dirRow;
    private static String dirCol;
    private static String dirDir;
    private static String dirMoveOrStop;

    private String startRow = "1";
    private String startCol = "1";
    private String startPos = "290";
    private String startDir = "2";

    private String wayRow;
    private String wayCol;
    private String wayPos;

    private boolean manualUpdateFlag;
    private boolean setWayPointFlag;
    private boolean setStartPointFlag;
    private boolean setStartDirFlag;

    private OnMapUpdateListener mListener;
    private boolean rotateFlag;
    private Timer mTimer1;
    private boolean lockFlag;
    private boolean calibrateFlag;

    public ArenaFragment() {
        // Required empty public constructor
    }

    /**
     * Use this factory method to create a new instance of
     * this fragment using the provided parameters.
     *
     * @return A new instance of fragment ArenaFragment.
     */
    // TODO: Rename and change types and number of parameters
    public static ArenaFragment newInstance(BluetoothAdapter adapter,
                                            BluetoothChatService bcs,
                                            String bluetoothStatus,
                                            String exploredB,
                                            String obstacleB,
                                            String dirR,
                                            String dirC,
                                            String dirD,
                                            String dirMOrS) {
        ArenaFragment fragment = new ArenaFragment();
        btStatus = bluetoothStatus;
        mBluetoothAdapter = adapter;
        mChatService = bcs;
        exploredBin = exploredB;
        obstacleBin = obstacleB;
        dirRow = dirR;
        dirCol = dirC;
        dirDir = dirD;
        dirMoveOrStop = dirMOrS;
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
        View view = inflater.inflate(R.layout.fragment_arena, container, false);
        GridView gridview = view.findViewById(R.id.map_grid);
        Button btCalibrate = view.findViewById(R.id.calibrateButton);
        Button btStartEx = view.findViewById(R.id.startExButton);
        Button btStartFp = view.findViewById(R.id.startFpButton);
        final ToggleButton btSetStartPoint = view.findViewById(R.id.startPointButton);
        final ToggleButton btSetWayPoint = view.findViewById(R.id.wayPointButton);
        final ToggleButton btSetStartDirection = view.findViewById(R.id.directionButton);
        final ToggleButton btUpdateToggle = view.findViewById(R.id.updateToggleButton);
        Button btReturn = view.findViewById(R.id.returnButton);
        Button btRefresh = view.findViewById(R.id.refreshButton);
        TextView btS = view.findViewById(R.id.bluetoothStatus);
        Button btForward = view.findViewById(R.id.forwardButton);
        Button btTurnLeft = view.findViewById(R.id.turnLeftButton);
        Button btTurnRight = view.findViewById(R.id.turnRightButton);
        Button btVoice = view.findViewById(R.id.voice_btn);
        Button reset = view.findViewById(R.id.reset);
        final ToggleButton btRotate = view.findViewById(R.id.rotation_btn);

        btRotate.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener(){
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                rotateFlag = isChecked;
            }
        });

        btS.setText(btStatus);

        gridview.setAdapter( new MainActivity.ImageAdapter(getContext(), MainActivity.gridList));

        gridview.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            public void onItemClick(AdapterView<?> parent, View v, int position, long id) {
                if(MainActivity.ImageAdapter.getGridItem(position) != null){
                    GridView gridview = getActivity().findViewById(R.id.map_grid);

                    if(setWayPointFlag){
                        if(wayRow!=null && wayCol!=null){
                            int index = Integer.valueOf(wayRow)*15 + Integer.valueOf(wayCol);
                            MainActivity.gridList.get(index).setStatus(Constants.UNEXPLORED);
                        }

                        MainActivity.ImageAdapter.getGridItem(position).setStatus(Constants.WAYPOINT);

                        wayRow = String.valueOf(MainActivity.ImageAdapter.calcRow(position));
                        wayCol = String.valueOf(MainActivity.ImageAdapter.calcCol(position));
                        wayPos = String.valueOf(position);

                        Toast.makeText(getContext(), "Waypoint of row: " + MainActivity.ImageAdapter.calcRow(position)
                                        + " and col: " + MainActivity.ImageAdapter.calcCol(position) + " set!",
                                Toast.LENGTH_SHORT).show();
                        setWayPointFlag = false;
                        btSetWayPoint.setChecked(false);
                        gridview.setAdapter( new MainActivity.ImageAdapter(getContext(), MainActivity.gridList));
                    } else if(setStartPointFlag){
                        if(startRow!=null && startCol!=null && startPos!=null){
                            int index = Integer.valueOf(startRow)*15 + Integer.valueOf(startCol);
                            MainActivity.gridList.get(index).setStatus(Constants.UNEXPLORED);
                        }

                        MainActivity.ImageAdapter.getGridItem(position).setStatus(Constants.START);

                        startRow = String.valueOf(MainActivity.ImageAdapter.calcRow(position));
                        startCol = String.valueOf(MainActivity.ImageAdapter.calcCol(position));
                        startPos = String.valueOf(position);

                        Toast.makeText(getContext(), "Startpoint of row: " + MainActivity.ImageAdapter.calcRow(position)
                                        + " and col: " + MainActivity.ImageAdapter.calcCol(position) + " set!",
                                Toast.LENGTH_SHORT).show();
                        setStartPointFlag = false;
                        btSetStartPoint.setChecked(false);
                        gridview.setAdapter( new MainActivity.ImageAdapter(getContext(), MainActivity.gridList));
                    } else if(setStartDirFlag){
                        if(startRow==null || startCol==null || startPos==null){
                            Toast.makeText(getContext(), "Please set START POINT first!",
                                    Toast.LENGTH_SHORT).show();
                        } else{
                            if(startDir!=null){
                                int startP = Integer.parseInt(startRow)*15 + Integer.parseInt(startCol);
                                int index;
                                switch(Integer.valueOf(startDir)){
                                   case Constants.NORTH:
                                       index = startP-15;
                                       MainActivity.gridList.get(index).setStatus(Constants.UNEXPLORED);
                                   case Constants.SOUTH:
                                       index = startP+15;
                                       MainActivity.gridList.get(index).setStatus(Constants.UNEXPLORED);
                                   case Constants.EAST:
                                       index = startP+1;
                                       MainActivity.gridList.get(index).setStatus(Constants.UNEXPLORED);
                                   case Constants.WEST:
                                       index = startP-1;
                                       MainActivity.gridList.get(index).setStatus(Constants.UNEXPLORED);
                                }
                            }

                            MainActivity.ImageAdapter.getGridItem(position).setStatus(Constants.STARTDIR);

                            int startP = Integer.valueOf(startPos);

                            Log.d("STARTPOS", startPos);
                            Log.d("DIRPOS", String.valueOf(position));

                            String result = "";

                            if(startP-position == 16){
                                startDir = String.valueOf(Constants.NORTH);
                                result = "NORTH";
                            } else if(startP-position == -16){
                                startDir = String.valueOf(Constants.SOUTH);
                                result = "SOUTH";
                            } else if(startP-position == -1){
                                startDir = String.valueOf(Constants.EAST);
                                result = "EAST";
                            } else if(startP-position == 1){
                                startDir = String.valueOf(Constants.WEST);
                                result = "WEST";
                            } else {
                                Toast.makeText(getContext(), "ERROR: Try again!",
                                        Toast.LENGTH_SHORT).show();
                                return;
                            }

                            Toast.makeText(getContext(), "Startdir "+ result + " of row: " + MainActivity.ImageAdapter.calcRow(position)
                                            + " and col: " + MainActivity.ImageAdapter.calcCol(position) + " set!",
                                    Toast.LENGTH_SHORT).show();
                        }
                        setStartDirFlag = false;
                        btSetStartDirection.setChecked(false);
                        dirRow=startRow;
                        dirCol=startCol;
                        dirDir=startDir;
                        Log.d("COORD_DEBUG", startRow + " " + startCol + " " + startDir);
                        ((MainActivity)getActivity()).update(null, null,
                                dirRow, dirCol, dirDir, "1", true);
                    } else {
                        Toast.makeText(getContext(), "array_index: "
                                        + MainActivity.gridList.indexOf(MainActivity.ImageAdapter.getGridItem(position))
                                        + " position: " + position
                                        + " row: " + MainActivity.ImageAdapter.calcRow(position)
                                        + " col: " + MainActivity.ImageAdapter.calcCol(position),
                                Toast.LENGTH_SHORT).show();
                    }
                }
            }
        });

        btStartEx.setOnClickListener(
                new View.OnClickListener(){

                    public void onClick(View view) {
                        if(wayCol!=null && wayRow!=null){
                            if(calibrateFlag){
                                String startEx = "ex";
                                ((MainActivity)getActivity()).sendMessage(startEx);
                            } else{
                                Toast.makeText(getActivity(), "Please CALIBRATE!", Toast.LENGTH_SHORT).show();
                            }
                        } else {
                            Toast.makeText(getActivity(), "WAYPOINT not set!", Toast.LENGTH_SHORT).show();
                        }
                    }
                }
        );

        btStartFp.setOnClickListener(
                new View.OnClickListener(){
                    public void onClick(View view) {
                        if(wayCol!=null && wayRow!=null){
                            if(calibrateFlag){
                                String startFp = "fp";
                                ((MainActivity)getActivity()).sendMessage(startFp);
                            } else{
                                Toast.makeText(getActivity(), "Please CALIBRATE!", Toast.LENGTH_SHORT).show();
                            }
                        } else {
                            Toast.makeText(getActivity(), "WAYPOINT not set!", Toast.LENGTH_SHORT).show();
                        }
                    }
                }
        );

        btSetStartPoint.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener(){
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                setStartPointFlag = isChecked;
            }
        });

        btSetWayPoint.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener(){
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                setWayPointFlag = isChecked;
            }
        });

        btSetStartDirection.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener(){
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                setStartDirFlag = isChecked;
            }
        });

        btReturn.setOnClickListener(
                new View.OnClickListener(){
                    public void onClick(View view) {
                        String strReturn = "return";
                        ((MainActivity)getActivity()).sendMessage(strReturn);
                    }
                }
        );

        btCalibrate.setOnClickListener(
                new View.OnClickListener(){
                    public void onClick(View view) {
                        if(wayCol!=null && wayRow!=null){
                            String calibrate = "ca ";
                            calibrate += wayRow;
                            calibrate += " ";
                            calibrate += wayCol;
                            ((MainActivity)getActivity()).sendMessage(calibrate);
                            ((MainActivity)getActivity()).update(null, null,
                                    null, null, dirDir, null, true);
                            calibrateFlag = true;
                        } else {
                            Toast.makeText(getActivity(), "WAYPOINT not set!", Toast.LENGTH_SHORT).show();
                        }
                    }
                }
        );

        btUpdateToggle.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener(){
                public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                    manualUpdateFlag = isChecked;
                    ((MainActivity)getActivity()).toggleManualUpdateFlag(isChecked);
                }
            }
        );

        btRefresh.setOnClickListener(new View.OnClickListener(){
                    public void onClick(View view) {
                        if(manualUpdateFlag){
                            //implement  method to refresh grid map
                            ((MainActivity)getActivity()).update(null, null,
                                    null, null, null, null, true);
                        }
                    }
                }
        );

        btForward.setOnClickListener(
                new View.OnClickListener(){
                    public void onClick(View view) {
                        if(dirRow!=null&&dirCol!=null&&dirDir!=null){
                            EditText tvForward = getActivity().findViewById(R.id.forwardField);
                            Log.d("DEBUG", tvForward.toString());
                            String forwardDistance = tvForward.getText().toString();
                            String forward = "move"+" "+forwardDistance;
                            ((MainActivity)getActivity()).sendMessage(forward);
                            for(int i = 0; i < Integer.valueOf(forwardDistance); i++){
                                animateForward();
                            }
//                            TextView robotStatus = getActivity().findViewById(R.id.robotStatus);
//                            robotStatus.setText(R.string.robot_moving);
                        } else {
                            Toast.makeText(getContext(), "Start coordinates and direction not set!",
                                    Toast.LENGTH_SHORT).show();
                        }

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
                        for(int i = 0; i < Integer.valueOf(turnLeftDegree)/90; i++){
                            animateTurnLeft();
                        }
//                        TextView robotStatus = getActivity().findViewById(R.id.robotStatus);
//                        robotStatus.setText(R.string.robot_moving);
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
                        for(int i = 0; i < Integer.valueOf(turnRightDegree)/90; i++){
                            animateTurnRight();
                        }
//                        TextView robotStatus = getActivity().findViewById(R.id.robotStatus);
//                        robotStatus.setText(R.string.robot_moving);
                    }
                }
        );

        btVoice.setOnClickListener(
                new View.OnClickListener(){
                    public void onClick(View view) {
                        ((MainActivity)getActivity()).startSpeechRecognizer();
                    }
                }
        );

        reset.setOnClickListener(
                new View.OnClickListener(){
                    public void onClick(View view) {
                        resetMap();
                    }
                }
        );

        return view;
    }

    @Override
    public void onAttach(Context context) {
        super.onAttach(context);
        if (context instanceof OnMapUpdateListener) {
            mListener = (OnMapUpdateListener) context;
        } else {
            throw new RuntimeException(context.toString()
                    + " must implement OnMapUpdateListener");
        }
    }

    @Override
    public void onDetach() {
        super.onDetach();
        mListener = null;
    }


    void resetMap() {
        exploredBin = "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";
        obstacleBin = "0";

        dirRow="1";
        dirCol="1";
        dirDir="2";
        dirMoveOrStop="1";

        startRow = "1";
        startCol = "1";
        startPos = "290";
        startDir = "2";

        wayRow=null;
        wayCol=null;
        wayPos=null;

        setWayPointFlag=false;
        setStartPointFlag=false;
        setStartDirFlag=false;

        calibrateFlag=false;

        ((MainActivity)getActivity()).update(exploredBin, obstacleBin,
                dirRow, dirCol, dirDir, dirMoveOrStop, true);
    }

    void animateForward(){
        switch(Integer.valueOf(dirDir)){
            case Constants.NORTH:
                ((MainActivity)getActivity()).update(null, null,
                        String.valueOf((Integer.valueOf(dirRow)+1)), dirCol, String.valueOf(Constants.NORTH), "1", true);
                break;
            case Constants.SOUTH:
                ((MainActivity)getActivity()).update(null, null,
                        String.valueOf((Integer.valueOf(dirRow)-1)), dirCol, String.valueOf(Constants.SOUTH), "1", true);
                break;
            case Constants.EAST:
                ((MainActivity)getActivity()).update(null, null,
                        dirRow, String.valueOf((Integer.valueOf(dirCol)+1)), String.valueOf(Constants.EAST), "1", true);
                break;
            case Constants.WEST:
                ((MainActivity)getActivity()).update(null, null,
                        dirRow, String.valueOf((Integer.valueOf(dirCol)-1)), String.valueOf(Constants.WEST), "1", true);
                break;
        }
    }

    void animateTurnRight(){
        switch(Integer.valueOf(dirDir)){
            case Constants.NORTH:
                ((MainActivity)getActivity()).update(null, null,
                        dirRow, dirCol, String.valueOf(Constants.EAST), "1", true);
                break;
            case Constants.SOUTH:
                ((MainActivity)getActivity()).update(null, null,
                        dirRow, dirCol, String.valueOf(Constants.WEST), "1", true);
                break;
            case Constants.EAST:
                ((MainActivity)getActivity()).update(null, null,
                        dirRow, dirCol, String.valueOf(Constants.SOUTH), "1", true);
                break;
            case Constants.WEST:
                ((MainActivity)getActivity()).update(null, null,
                        dirRow, dirCol, String.valueOf(Constants.NORTH), "1", true);
                break;
        }
    }

    void animateTurnLeft(){
        switch(Integer.valueOf(dirDir)){
            case Constants.NORTH:
                ((MainActivity)getActivity()).update(null, null,
                        dirRow, dirCol, String.valueOf(Constants.WEST), "1", true);
                break;
            case Constants.SOUTH:
                ((MainActivity)getActivity()).update(null, null,
                        dirRow, dirCol, String.valueOf(Constants.EAST), "1", true);
                break;
            case Constants.EAST:
                ((MainActivity)getActivity()).update(null, null,
                        dirRow, dirCol, String.valueOf(Constants.NORTH), "1", true);
                break;
            case Constants.WEST:
                ((MainActivity)getActivity()).update(null, null,
                        dirRow, dirCol, String.valueOf(Constants.SOUTH), "1", true);
                break;
        }
    }

    public void parseVoiceCommand(String command) {
        String cmd;
        switch (command){
            case "forward":
                cmd = "move 1";
                ((MainActivity)getActivity()).sendMessage(cmd);
                animateForward();
                break;
            case "turn left":
                cmd = "rotate -90";
                ((MainActivity)getActivity()).sendMessage(cmd);
                animateTurnLeft();
                break;
            case "turn right":
                cmd = "rotate 90";
                ((MainActivity)getActivity()).sendMessage(cmd);
                animateTurnRight();
                break;
            default:
                Toast.makeText(getContext(), "VOICE: " + command,
                        Toast.LENGTH_LONG).show();
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
    public interface OnMapUpdateListener {
        void onBluetoothStateChange(String btStatus);
        void onVoiceCommand(String command);
        void onOrientationChanged(float x, float y, float z);
    }

    public void updateBTStatus(String btStatus){
        TextView tv = getActivity().findViewById(R.id.bluetoothStatus);
        tv.setText(btStatus);
    }

    public synchronized void tiltSteer(float x, float y, float z) {
        if(rotateFlag && !lockFlag) {
            TextView xText = getActivity().findViewById(R.id.x_orientation);
            TextView yText = getActivity().findViewById(R.id.y_orientation);
            TextView zText = getActivity().findViewById(R.id.z_orientation);
            xText.setText(String.valueOf(Math.round(x)));
            yText.setText(String.valueOf(Math.round(y)));
            zText.setText(String.valueOf(Math.round(z)));

            String cmd;

            if(Math.round(x)==10){
                animateTurnLeft();
                cmd = "rotate -90";
                ((MainActivity)getActivity()).sendMessage(cmd);
                startTimer();
            } else if(Math.round(x)==-9){
                animateTurnRight();
                cmd = "rotate 90";
                ((MainActivity)getActivity()).sendMessage(cmd);
                startTimer();
            } else if(Math.round(x)<2 && Math.round(x)>-2 && Math.round(y)<5 && Math.round(z)>7){
                animateForward();
                cmd = "move 1";
                ((MainActivity)getActivity()).sendMessage(cmd);
                startTimer();
            }

        }
    }

    private void stopTimer(){
        if(mTimer1 != null){
            mTimer1.cancel();
            mTimer1.purge();
        }
    }

    private void startTimer(){
        lockFlag = true;
        mTimer1 = new Timer();
        final Handler mTimerHandler = new Handler();
        TimerTask mTt1 = new TimerTask() {
            public void run() {
                mTimerHandler.post(new Runnable() {
                    public void run(){
                        lockFlag = false;
                    }
                });
            }
        };

        mTimer1.schedule(mTt1, 1000);
    }
}
