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

    ArrayList<GridImage> gridList = new ArrayList<GridImage>();

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

        for(int y = 0; y < 20; y++){
            for(int x = 0; x < 15; x++){
                GridImage image = new GridImage(R.drawable.blue_square, x, y, Constants.UNEXPLORED);
                gridList.add(image);
            }
        }

        gridview.setAdapter( new ImageAdapter(getContext(), gridList));

        gridview.setOnItemClickListener(new AdapterView.OnItemClickListener() {
            public void onItemClick(AdapterView<?> parent, View v, int position, long id) {
                if(ImageAdapter.getGridItem(position) != null){
                    GridView gridview = getActivity().findViewById(R.id.map_grid);

                    if(setWayPointFlag){
                        if(wayRow!=null && wayCol!=null){
                            int index = Integer.valueOf(wayRow)*15 + Integer.valueOf(wayCol);
                            gridList.get(index).setStatus(Constants.UNEXPLORED);
                        }

                        ImageAdapter.getGridItem(position).setStatus(Constants.WAYPOINT);

                        wayRow = String.valueOf(ImageAdapter.calcRow(position));
                        wayCol = String.valueOf(ImageAdapter.calcCol(position));
                        wayPos = String.valueOf(position);

                        Toast.makeText(getContext(), "Waypoint of row: " + ImageAdapter.calcRow(position)
                                        + " and col: " + ImageAdapter.calcCol(position) + " set!",
                                Toast.LENGTH_SHORT).show();
                        setWayPointFlag = false;
                        btSetWayPoint.setChecked(false);
                        gridview.setAdapter( new ImageAdapter(getContext(), gridList));
                    } else if(setStartPointFlag){
                        if(startRow!=null && startCol!=null && startPos!=null){
                            int index = Integer.valueOf(startRow)*15 + Integer.valueOf(startCol);
                            gridList.get(index).setStatus(Constants.UNEXPLORED);
                        }

                        ImageAdapter.getGridItem(position).setStatus(Constants.START);

                        startRow = String.valueOf(ImageAdapter.calcRow(position));
                        startCol = String.valueOf(ImageAdapter.calcCol(position));
                        startPos = String.valueOf(position);

                        Toast.makeText(getContext(), "Startpoint of row: " + ImageAdapter.calcRow(position)
                                        + " and col: " + ImageAdapter.calcCol(position) + " set!",
                                Toast.LENGTH_SHORT).show();
                        setStartPointFlag = false;
                        btSetStartPoint.setChecked(false);
                        gridview.setAdapter( new ImageAdapter(getContext(), gridList));
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
                                       gridList.get(index).setStatus(Constants.UNEXPLORED);
                                   case Constants.SOUTH:
                                       index = startP+15;
                                       gridList.get(index).setStatus(Constants.UNEXPLORED);
                                   case Constants.EAST:
                                       index = startP+1;
                                       gridList.get(index).setStatus(Constants.UNEXPLORED);
                                   case Constants.WEST:
                                       index = startP-1;
                                       gridList.get(index).setStatus(Constants.UNEXPLORED);
                                }
                            }

                            ImageAdapter.getGridItem(position).setStatus(Constants.STARTDIR);

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

                            Toast.makeText(getContext(), "Startdir "+ result + " of row: " + ImageAdapter.calcRow(position)
                                            + " and col: " + ImageAdapter.calcCol(position) + " set!",
                                    Toast.LENGTH_SHORT).show();
                        }
                        setStartDirFlag = false;
                        btSetStartDirection.setChecked(false);
                        dirRow=startRow;
                        dirCol=startCol;
                        dirDir=startDir;
                        Log.d("COORD_DEBUG", startRow + " " + startCol + " " + startDir);
                        updateMap(exploredBin, obstacleBin, true);
                        updateRobot(dirRow, dirCol, dirDir, "1", true);
                    } else {
                        Toast.makeText(getContext(), "array_index: "
                                        + gridList.indexOf(ImageAdapter.getGridItem(position))
                                        + " position: " + position
                                        + " row: " + ImageAdapter.calcRow(position)
                                        + " col: " + ImageAdapter.calcCol(position),
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
                            updateMap(exploredBin, obstacleBin, true);
                            updateRobot(dirRow, dirCol, dirDir, "1", true);
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
                }
            }
        );

        btRefresh.setOnClickListener(new View.OnClickListener(){
                    public void onClick(View view) {
                        if(manualUpdateFlag){
                            //implement  method to refresh grid map
                            updateMap(exploredBin, obstacleBin, true);
                            updateRobot(dirRow, dirCol, dirDir, dirMoveOrStop, true);
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

    private void resetMap() {
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

        updateMap(exploredBin, obstacleBin, true);
        updateRobot(dirRow, dirCol, dirDir, dirMoveOrStop, true);
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

    public void updateMap(String exploredBin, String obstacleBin, boolean force) {
        Log.d("EXPLORED_BIN", exploredBin);
        Log.d("OBSTACEL_BIN", obstacleBin);

        ArenaFragment.exploredBin = exploredBin;
        ArenaFragment.obstacleBin = obstacleBin;

        if(!manualUpdateFlag || force){
            int obsCount = 0;

            for (int i = 0; i < exploredBin.length(); i++){
                char c = exploredBin.charAt(i);
                //Log.d("EXP", String.valueOf(Integer.parseInt(String.valueOf(c))));
                if(Integer.parseInt(String.valueOf(c)) == 1){
                    GridImage grid = gridList.get(i);
                    if (grid != null) {
                        char x = obstacleBin.charAt(obsCount);
                        //Log.d("OBS", String.valueOf(Integer.parseInt(String.valueOf(x))));
                        if(Integer.parseInt(String.valueOf(x)) == 1){
                            grid.setStatus(Constants.OBSTACLE);
                        } else {
                            grid.setStatus(Constants.EXPLORED);
                        }
                        obsCount++;
                    }
                } else {
                    GridImage grid = gridList.get(i);
                    if (grid != null) {
                        grid.setStatus(Constants.UNEXPLORED);
                    }
                }
            }
            GridView gridview = getActivity().findViewById(R.id.map_grid);
            gridview.setAdapter( new ImageAdapter(getContext(), gridList));
        }
    }

    public void updateRobot(String dirRow, String dirCol, String dirDir,
                            String dirMoveOrStop, boolean force) {
        ArenaFragment.dirRow = dirRow;
        ArenaFragment.dirCol = dirCol;
        ArenaFragment.dirDir = dirDir;
        ArenaFragment.dirMoveOrStop = dirMoveOrStop;

        if(!manualUpdateFlag || force){

            int index = Integer.valueOf(dirRow)*15 + Integer.valueOf(dirCol);
            GridImage image1 = gridList.get(index);
            image1.setStatus(Constants.ROBOT_BODY);
            GridImage image2 = gridList.get(index+1);
            image2.setStatus(Constants.ROBOT_BODY);
            GridImage image3 = gridList.get(index-1);
            image3.setStatus(Constants.ROBOT_BODY);
            GridImage image4 = gridList.get(index-15);
            image4.setStatus(Constants.ROBOT_BODY);
            GridImage image5 = gridList.get(index+15);
            image5.setStatus(Constants.ROBOT_BODY);
            GridImage image6 = gridList.get(index-14);
            image6.setStatus(Constants.ROBOT_BODY);
            GridImage image7 = gridList.get(index+14);
            image7.setStatus(Constants.ROBOT_BODY);
            GridImage image8 = gridList.get(index-16);
            image8.setStatus(Constants.ROBOT_BODY);
            GridImage image9 = gridList.get(index+16);
            image9.setStatus(Constants.ROBOT_BODY);

            Log.d("HEADDIR", dirDir);

            switch(Integer.valueOf(dirDir)){
                case Constants.NORTH:
                    image5.setStatus(Constants.ROBOT_HEAD);
                    break;
                case Constants.SOUTH:
                    image4.setStatus(Constants.ROBOT_HEAD);
                    break;
                case Constants.EAST:
                    image2.setStatus(Constants.ROBOT_HEAD);
                    break;
                case Constants.WEST:
                    image3.setStatus(Constants.ROBOT_HEAD);
                    break;
            }

            TextView move_or_stop = getActivity().findViewById(R.id.robotStatus);
            if(Integer.valueOf(dirMoveOrStop) == 1){
                move_or_stop.setText(R.string.robot_stopped);
            } else {
                move_or_stop.setText(R.string.robot_moving);
            }

            GridView gridview = getActivity().findViewById(R.id.map_grid);
            gridview.setAdapter( new ImageAdapter(getContext(), gridList));
        }
    }

    void animateForward(){
        switch(Integer.valueOf(dirDir)){
            case Constants.NORTH:
                updateMap(exploredBin, obstacleBin, true);
                updateRobot(String.valueOf((Integer.valueOf(dirRow)+1)), dirCol, String.valueOf(Constants.NORTH), "1", true);
                break;
            case Constants.SOUTH:
                updateMap(exploredBin, obstacleBin, true);
                updateRobot(String.valueOf((Integer.valueOf(dirRow)-1)), dirCol, String.valueOf(Constants.SOUTH), "1", true);
                break;
            case Constants.EAST:
                updateMap(exploredBin, obstacleBin, true);
                updateRobot(dirRow, String.valueOf((Integer.valueOf(dirCol)+1)), String.valueOf(Constants.EAST), "1", true);
                break;
            case Constants.WEST:
                updateMap(exploredBin, obstacleBin, true);
                updateRobot(dirRow, String.valueOf((Integer.valueOf(dirCol)-1)), String.valueOf(Constants.WEST), "1", true);
                break;
        }
    }

    void animateTurnRight(){
        switch(Integer.valueOf(dirDir)){
            case Constants.NORTH:
                updateRobot(dirRow, dirCol, String.valueOf(Constants.EAST), "1", true);
                break;
            case Constants.SOUTH:
                updateRobot(dirRow, dirCol, String.valueOf(Constants.WEST), "1", true);
                break;
            case Constants.EAST:
                updateRobot(dirRow, dirCol, String.valueOf(Constants.SOUTH), "1", true);
                break;
            case Constants.WEST:
                updateRobot(dirRow, dirCol, String.valueOf(Constants.NORTH), "1", true);
                break;
        }
    }

    void animateTurnLeft(){
        switch(Integer.valueOf(dirDir)){
            case Constants.NORTH:
                updateRobot(dirRow, dirCol, String.valueOf(Constants.WEST), "1", true);
                break;
            case Constants.SOUTH:
                updateRobot(dirRow, dirCol, String.valueOf(Constants.EAST), "1", true);
                break;
            case Constants.EAST:
                updateRobot(dirRow, dirCol, String.valueOf(Constants.NORTH), "1", true);
                break;
            case Constants.WEST:
                updateRobot(dirRow, dirCol, String.valueOf(Constants.SOUTH), "1", true);
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
        void onMapUpdateReceived(String message);
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

    private static class ImageAdapter extends BaseAdapter {
        private Context mContext;
        private static ArrayList<GridImage> gridList;

        public ImageAdapter(Context c, ArrayList<GridImage> glist) {
            mContext = c;
            gridList = glist;
        }

        public static int calcCol(int position) {
            return getGridItem(position).getCol();
        }

        public static int calcRow(int position) {
            return getGridItem(position).getRow();
        }

        public int getCount() {
            return 336;
        }

        public static GridImage getGridItem(int position) {
            int y = 19-(position/16);
            int x = position%16 - 1;
            int index = y*15 + x;
            if(index >= 0 && gridList.size() > index){
                return gridList.get(index);
            } else {
                return null;
            }
        }

        @Override
        public Object getItem(int position) {
            return null;
        }

        public long getItemId(int position) {
            return 0;
        }

        // create a new ImageView for each item referenced by the Adapter
        public View getView(int position, View convertView, ViewGroup parent) {
            ImageView imageView;
            TextView textView;
            if (position == 320){
                if (convertView == null) {
                    textView = new TextView(mContext);
                    textView.setText("");
                } else {
                    return convertView;
                }
                return textView;
            } else if(position%16 == 0){
                if (convertView == null) {
                    textView = new TextView(mContext);
                    textView.setText(String.valueOf(19-(position/16)));
                } else {
                    return convertView;
                }
                return textView;
            } else if(position > 320){
                if (convertView == null) {
                    textView = new TextView(mContext);
                    textView.setText(String.valueOf(position-321));
                } else {
                    return convertView;
                }
                return textView;
            } else{
                if (convertView == null) {
                    // if it's not recycled, initialize some attributes
                    imageView = new ImageView(mContext);
                    imageView.setLayoutParams(new GridView.LayoutParams(30, 30));
                    imageView.setScaleType(ImageView.ScaleType.CENTER_CROP);
                    imageView.setPadding(1,1,1,1);
                } else {
                    return convertView;
                }
                imageView.setImageResource(getGridItem(position).getImageId());
                return imageView;
            }
        }
    }

    private class GridImage {
        int imageId;
        int row;
        int col;
        int status;

        GridImage(int imageId, int col, int row, int status){
            this.imageId = imageId;
            this.row = row;
            this.col = col;
            this.status = status;
        }

        public int getRow(){
            return this.row;
        }

        public int getCol(){
            return this.col;
        }

        public int getImageId(){
            return this.imageId;
        }

        public void setImageId(int imageId){
            this.imageId = imageId;
        }

        public int getStatus(){
            return this.status;
        }

        public void setStatus(int status){
            this.status = status;
            switch(status){
                case Constants.EXPLORED:
                    this.setImageId(R.drawable.green_square);
                    break;
                case Constants.UNEXPLORED:
                    this.setImageId(R.drawable.blue_square);
                    break;
                case Constants.OBSTACLE:
                    this.setImageId(R.drawable.red_square);
                    break;
                case Constants.ROBOT_BODY:
                    this.setImageId(R.drawable.yellow_square);
                    break;
                case Constants.ROBOT_HEAD:
                    this.setImageId(R.drawable.black_square);
                    break;
                case Constants.START:
                    this.setImageId(R.drawable.orange_square);
                    break;
                case Constants.GOAL:
                    this.setImageId(R.drawable.orange_square);
                    break;
                case Constants.WAYPOINT:
                    this.setImageId(R.drawable.orange_square);
                    break;
                case Constants.STARTDIR:
                    this.setImageId(R.drawable.black_square);
                    break;
            }
        }
    }
}
