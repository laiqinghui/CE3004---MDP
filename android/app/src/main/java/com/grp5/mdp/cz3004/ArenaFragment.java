package com.grp5.mdp.cz3004;

import android.bluetooth.BluetoothAdapter;
import android.content.Context;
import android.graphics.Color;
import android.graphics.PorterDuff;
import android.net.Uri;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AbsListView;
import android.widget.AdapterView;
import android.widget.BaseAdapter;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.GridView;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.ToggleButton;

import java.util.ArrayList;

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

    ArrayList<GridImage> gridList = new ArrayList<GridImage>();

    private String exploredBin;
    private String obstacleBin;

    private String dirRow;
    private String dirCol;
    private String dirDir;
    private String dirMoveOrStop;

    private String startRow;
    private String startCol;
    private String startPos;
    private String startDir;

    private String wayRow;
    private String wayCol;
    private String wayPos;

    private boolean manualUpdateFlag;
    private boolean setWayPointFlag;
    private boolean setStartPointFlag;
    private boolean setStartDirFlag;

    private OnMapUpdateListener mListener;

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
    public static ArenaFragment newInstance(BluetoothAdapter adapter, BluetoothChatService bcs) {
        ArenaFragment fragment = new ArenaFragment();
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
        View view = inflater.inflate(R.layout.fragment_arena, container, false);
        GridView gridview = (GridView) view.findViewById(R.id.map_grid);
        Button btCalibrate = view.findViewById(R.id.calibrateButton);
        Button btStartEx = view.findViewById(R.id.startExButton);
        Button btStartFp = view.findViewById(R.id.startFpButton);
        Button btSetStartPoint = view.findViewById(R.id.startPointButton);
        Button btSetWayPoint = view.findViewById(R.id.wayPointButton);
        Button btSetStartDirection = view.findViewById(R.id.directionButton);
        final ToggleButton btUpdateToggle = view.findViewById(R.id.updateToggleButton);
        Button btReturn = view.findViewById(R.id.returnButton);
        Button btRefresh = view.findViewById(R.id.refreshButton);

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
                    GridView gridview = (GridView) getActivity().findViewById(R.id.map_grid);

                    if(setWayPointFlag){
                        if(wayRow!=null && wayCol!=null){
                            int index = Integer.valueOf(wayRow)*15 + Integer.valueOf(wayCol);
                            gridList.get(index).setStatus(Constants.UNEXPLORED);
                        }

                        ImageAdapter.getGridItem(position).setStatus(Constants.WAYPOINT);

                        wayRow = String.valueOf(ImageAdapter.calcRow(position));
                        wayCol = String.valueOf(ImageAdapter.calcCol(position));

                        Toast.makeText(getContext(), "Waypoint of row" + ImageAdapter.calcRow(position)
                                        + " and col " + ImageAdapter.calcCol(position) + "set!",
                                Toast.LENGTH_SHORT).show();
                        setWayPointFlag = false;
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

                        Toast.makeText(getContext(), "Startpoint of row" + ImageAdapter.calcRow(position)
                                        + " and col " + ImageAdapter.calcCol(position) + "set!",
                                Toast.LENGTH_SHORT).show();
                        setStartPointFlag = false;
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

                            int startP = Integer.parseInt(startPos);

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

                            Toast.makeText(getContext(), "Startdir "+ result + " of row " + ImageAdapter.calcRow(position)
                                            + " and col " + ImageAdapter.calcCol(position) + " set!",
                                    Toast.LENGTH_SHORT).show();
                        }
                        setStartDirFlag = false;
                        gridview.setAdapter( new ImageAdapter(getContext(), gridList));
                    } else {
                        Toast.makeText(getContext(), "array_index:" + gridList.indexOf(ImageAdapter.getGridItem(position)) + " position: " + position + " row: " + ImageAdapter.calcRow(position)
                                        + " col: " + ImageAdapter.calcCol(position),
                                Toast.LENGTH_SHORT).show();
                    }
                }
            }
        });

        btStartEx.setOnClickListener(
                new View.OnClickListener(){

                    public void onClick(View view) {
                        //ex <robot_start_row> <robot_start_col> <starting_direction>
                        if(startRow!=null && startCol!=null && startDir!=null){
                            String startEx = "ex"+" "+startRow+" "+startCol+" "+startDir;
                            ((MainActivity)getActivity()).sendMessage(startEx);
                        } else {
                            Toast.makeText(getActivity(), "Coordinates or direction not set!", Toast.LENGTH_SHORT).show();
                        }
                    }
                }
        );

        btStartFp.setOnClickListener(
                new View.OnClickListener(){
                    public void onClick(View view) {
                        //fp <waypoint_row> <waypoint_col> <starting_direction>
                        if(wayRow!=null && wayCol!=null && startDir!=null){
                            String startFp = "fp"+" "+wayRow+" "+wayCol+" "+startDir;
                            ((MainActivity)getActivity()).sendMessage(startFp);
                        } else {
                            Toast.makeText(getActivity(), "Coordinates or direction not set!", Toast.LENGTH_SHORT).show();
                        }
                    }
                }
        );

        btSetStartPoint.setOnClickListener(
                new View.OnClickListener(){
                    public void onClick(View view) {
                        setStartPointFlag = true;
                        Toast.makeText(getActivity(), "Set your START POINT.", Toast.LENGTH_SHORT).show();
                    }
                }
        );

        btSetWayPoint.setOnClickListener(
                new View.OnClickListener(){
                    public void onClick(View view) {
                        setWayPointFlag = true;
                        Toast.makeText(getActivity(), "Set your WAY POINT.", Toast.LENGTH_SHORT).show();
                    }
                }
        );

        btSetStartDirection.setOnClickListener(
                new View.OnClickListener(){
                    public void onClick(View view) {
                        setStartDirFlag = true;
                        Toast.makeText(getActivity(), "Set your START DIRECTION.", Toast.LENGTH_SHORT).show();
                    }
                }
        );

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
                        //need to implement getting value of start point and direction
                        String calibrate = "ca";
                        ((MainActivity)getActivity()).sendMessage(calibrate);
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
                            updateMap(exploredBin, obstacleBin);
                            updateRobot(dirRow, dirCol, dirDir, dirMoveOrStop);
                        }
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

    public void updateMap(String exploredBin, String obstacleBin) {
        Log.d("EXPLORED_BIN", exploredBin);
        Log.d("OBSTACEL_BIN", obstacleBin);

        this.exploredBin = exploredBin;
        this.obstacleBin = obstacleBin;

        if(!manualUpdateFlag){
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
            GridView gridview = (GridView) getActivity().findViewById(R.id.map_grid);
            gridview.setAdapter( new ImageAdapter(getContext(), gridList));
        }
    }

    public void updateRobot(String dirRow, String dirCol, String dirDir, String dirMoveOrStop) {
        if(exploredBin!=null && obstacleBin!=null){
            updateMap(exploredBin, obstacleBin);
        };
        this.dirRow = dirRow;
        this.dirCol = dirCol;
        this.dirDir = dirDir;
        this.dirMoveOrStop = dirMoveOrStop;

        if(!manualUpdateFlag){
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

            switch(Integer.valueOf(dirDir)){
                case Constants.NORTH:
                    image4.setStatus(Constants.ROBOT_HEAD);
                    break;
                case Constants.SOUTH:
                    image5.setStatus(Constants.ROBOT_HEAD);
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

            GridView gridview = (GridView) getActivity().findViewById(R.id.map_grid);
            gridview.setAdapter( new ImageAdapter(getContext(), gridList));
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
        // TODO: Update argument type and name
        void onMapUpdateReceived(String message);
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
