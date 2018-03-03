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
    // TODO: Rename parameter arguments, choose names that match
    // the fragment initialization parameters, e.g. ARG_ITEM_NUMBER
    private static final String ARG_PARAM1 = "param1";
    private static final String ARG_PARAM2 = "param2";
    private static BluetoothAdapter mBluetoothAdapter;
    private static BluetoothChatService mChatService;

    ArrayList<GridImage> gridList = new ArrayList<GridImage>();

    // TODO: Rename and change types of parameters
    private byte exploredBin;
    private byte obstacleBin;

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
                    Toast.makeText(getContext(), "array_index:" + gridList.indexOf(ImageAdapter.getGridItem(position)) + " position: " + position + " x: " + ImageAdapter.calcRow(position)
                                    + " y: " + ImageAdapter.calcCol(position),
                            Toast.LENGTH_SHORT).show();
                }
            }
        });

        btCalibrate.setOnClickListener(
                new View.OnClickListener(){
                    public void onClick(View view) {
                        //need to implement getting value of start point and direction
                        String calibrate = "ca";
                        ((MainActivity)getActivity()).sendMessage(calibrate);
                    }
                }
        );

        btStartEx.setOnClickListener(
                new View.OnClickListener(){
                    public void onClick(View view) {
                        //need to implement getting value of start point and direction
                        String startEx = "ex"+" "+"1"+" "+"18"+" "+"4";
                        ((MainActivity)getActivity()).sendMessage(startEx);
                    }
                }
        );

        btStartFp.setOnClickListener(
                new View.OnClickListener(){
                    public void onClick(View view) {
                        //need to implement getting value of way point and direction
                        String startFp = "fp"+" "+"10"+" "+"10"+" "+"2";
                        ((MainActivity)getActivity()).sendMessage(startFp);
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

        btRefresh.setOnClickListener(new View.OnClickListener(){
                    public void onClick(View view) {
                        if(btUpdateToggle.isChecked()){
                            //implement  method to refresh grid map
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
            } else{}
        }
        GridView gridview = (GridView) getActivity().findViewById(R.id.map_grid);
        gridview.setAdapter( new ImageAdapter(getContext(), gridList));
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
        void onMapUpdateReceived(String exploredBin, String obstacleBin);
    }

    private static class ImageAdapter extends BaseAdapter {
        private Context mContext;
        private static ArrayList<GridImage> gridList;

        public ImageAdapter(Context c, ArrayList<GridImage> glist) {
            mContext = c;
            gridList = glist;
        }

        public static int calcRow(int position) {
            return getGridItem(position).getRow();
        }

        public static int calcCol(int position) {
            return getGridItem(position).getCol();
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

        GridImage(int imageId, int row, int col, int status){
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
            }
        }
    }
}
