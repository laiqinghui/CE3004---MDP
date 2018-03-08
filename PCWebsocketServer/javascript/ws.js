var host = window.location.host;
var ws = new WebSocket('ws://'+host+'/ws');

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
  }
  if(option == 'DIR'){
    let robot_row = option_data[0];
    let robot_col = option_data[1];
    let robot_dir = option_data[2];
    let robot_status = option_data[3];
  }


};
ws.onclose = function(ev){
  console.log('websocket connection closed');
};
ws.onerror = function(ev){
  console.log('websocket connection error');
};
