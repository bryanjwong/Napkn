var database = firebase.database();
var table = database.ref('Napkn');

var ref = firebase.database().ref('Napkn');
var tableList = document.getElementById("TableList");
var openTables = document.getElementById("TablesOpen");

var i = 1;
var numOpenTables = 0;

ref.once("value", function(snapshot) {
  snapshot.forEach(function(childSnapshot) {
    console.log(childSnapshot.child("DeviceStatus").val());
    let item = document.createElement('li');
    let tableNum = document.createElement('p');
    let status = document.createElement('p');
    let timeLeft = document.createElement('p');
    
    // Print out table number
    tableNum.innerHTML = "Table " + i;
    item.appendChild(tableNum);
    tableList.appendChild(item);
    i++;
    
    // Display device status (open or occupied)
    status.innerHTML = "Status: " + childSnapshot.child("DeviceStatus").val();
    tableList.appendChild(status);

    
    
    // Display minutes remaining
    var statusStr = childSnapshot.child("DeviceStatus").val().toString();
    if (statusStr == "Open"){
      timeLeft.innerHTML = "Time remaining: N/A";
      numOpenTables++; 
    }else{ // otherwise, occupied
      timeLeft.innerHTML = "Time remaining: " + childSnapshot.child("TimeRemaining").val();
    }
    tableList.appendChild(timeLeft);
  });

  openTables.innerHTML = "Tables Open: " + numOpenTables; // Display num open tables
});

