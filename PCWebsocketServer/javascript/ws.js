var host = window.location.host;
var ws = new WebSocket('ws://'+host+'/ws');

var map = [];
const ROWS = 20, COLUMNS = 15;
for (var i = 0; i < ROWS; i++){
  map[i] = [];
  for (var j = 0; j < COLUMNS; j++){
    map[i][j] = 0;
  }
}
ws.onopen = function(){
  console.log("connected to websocket server");
};
ws.onmessage = function(ev){

  let json = JSON.parse(ev.data);
  let option = json.data.substr(0,3);
  let option_data = json.data.substr(3).split('L').slice(0,-1);

  console.log(option);
  console.log(option_data);

  if(option == 'MDF'){
    let explore_mdf = option_data[0];
    let obstacle_mdf = option_data[1];

    explore_convert(explore_mdf);
    obstacle_convert(obstacle_mdf);


  }
  if(option == 'DIR'){
    let robot_row = 19-(parseInt(option_data[0]));
    let robot_col = parseInt(option_data[1]);
    let robot_dir = parseInt(option_data[2]);
    let robot_status = option_data[3];


    map[robot_row][robot_col] = "5";
    map[robot_row][robot_col+1] ="5";
    map[robot_row][robot_col-1] ="5";
    map[robot_row+1][robot_col] ="5";
    map[robot_row+1][robot_col+1] ="5";
    map[robot_row+1][robot_col-1] ="5";
    map[robot_row-1][robot_col] ="5";
    map[robot_row-1][robot_col+1] ="5";
    map[robot_row-1][robot_col-1] ="5";

    robot_head(robot_dir,robot_row,robot_col);


  }

  /*for (var i = 0; i < ROWS; i++){
    for (var j = 0; j < COLUMNS; j++){
      map[i][j] = binary.substr(0,1);
      binary= binary.substr(1);}
    }*/
  draw(map);



}

function sendMsg() {
            ws.send(document.getElementById('msg').value);
        }
ws.onclose = function(ev){
  console.log('websocket connection closed');
};
ws.onerror = function(ev){
  console.log('websocket connection error');
};

  var canvas = document.getElementById('canvas');
	var context = canvas.getContext('2d');
	var roboPath = [];




	function getStyle(cell) {
	    switch(cell) {
	        case "0": return "#000000"; // unexplored
	        case "1": return "#a1bce2"; // explored
	        case "2": return "#d80e00"; // obstacle
	        case "3": return "#30807d"; // start
	        case "4": return "#00f908"; // goal
	        case "5": return "#32CD32"; // robot
	        case "6": return "#0300e8"; // path
	        case "7": return "#673ab7"; // way-point
	        default: return "#1a1e24";
	    }
	}

  function draw(map) {

    context.save();
    context.strokeStyle = "#252a33";
    context.lineWidth = 3;

    // Filling explored and unexplored cells
    for (var i = 0; i < ROWS; i++){
    for (var j = 0; j < COLUMNS; j++){
    context.beginPath();
    context.fillStyle = getStyle(map[i][j]);
    context.rect(30 * j, 30 * i, 30, 30);
    context.fill();
    context.stroke();
    context.closePath();}
  }


}


draw(map);


function hex2bin(hex){
  switch(hex) {
    case "0": return "0000";
    case "1": return "0001";
    case "2": return "0010";
    case "3": return "0011";
    case "4": return "0100";
    case "5": return "0101";
    case "6": return "0110";
    case "7": return "0111";
    case "8": return "1000";
    case "9": return "1001";
    case "A": return "1010";
    case "B": return "1011";
    case "C": return "1100";
    case "D": return "1101";
    case "E": return "1110";
    case "F": return "1111";
  }
}
function explore_convert(explore_mdf){
  var binary_explore_mdf = "";
  for (var i = 0; i < 76; i++) {
    var temp = hex2bin(explore_mdf.substr(0,1));
    binary_explore_mdf = binary_explore_mdf + temp;
    explore_mdf = explore_mdf.substr(1);
  }
  /*console.log(explore_mdf);
  console.log(binary_explore_mdf);*/
  var binary_array= binary_explore_mdf.slice(2,-2).split('');
  var n = 0
  for (var i = 19; i > -1; i--){
    for (var j = 0; j < 15; j++){
      map[i][j] = binary_array[n];
      n = n + 1;}
  }
}

function obstacle_convert(obstacle_mdf){
  var binary_obstacle_mdf = "";
  while (obstacle_mdf.length !=0) {
    var temp = hex2bin(obstacle_mdf.substr(0,1));
    binary_obstacle_mdf = binary_obstacle_mdf + temp;
    obstacle_mdf = obstacle_mdf.substr(1);
  }


  for (var i = 19; i > -1; i--){
    for (var j = 0; j < 15; j++){
      if (map[i][j] == "1"){
        if(binary_obstacle_mdf.substr(0,1) == "1"){
          map[i][j] = "2";
        }
        binary_obstacle_mdf = binary_obstacle_mdf.substr(1);}
    }
  }

}

function robot_head(robot_dir,robot_row,robot_col){

  switch(robot_dir) {

    case 1: return map[robot_row-1][robot_col] ="6";
    case 2: return map[robot_row][robot_col+1] ="6";
    case 3: return map[robot_row+1][robot_col] ="6";
    case 4: return map[robot_row][robot_col-1] ="6";

  }

}
