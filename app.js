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
    item.classList.add("napkn-num");

    let tableNum = document.createElement('li');
    tableNum.classList.add("napkn-num");

    // napkin title
    let div1 = document.createElement('div')
    div1.classList.add("col-lg-4");
    div1.innerText = "Napkn #" + i + " ";
    div1.classList.add("napkn-title");
    tableList.appendChild(div1);

    let div2 = document.createElement('div')
    div2.classList.add("col-lg-4");
    div2.innerText = "Status: " + childSnapshot.child("DeviceStatus").val() + " ";
    div2.classList.add("napkn-status");
    tableList.appendChild(div2);

    let div3 = document.createElement('div')
    div3.classList.add("col-lg-4");
    div3.classList.add("napkn-timeLeft");
    tableList.appendChild(div3);


    let status = document.createElement('li');
    status.classList.add("napkn-status");
    let timeLeft = document.createElement('li');
    timeLeft.classList.add("napkn-timeLeft");

    let space = document.createElement('div');

    // Print out table number
    // tableNum.innerHTML = "Napkn #" + i + " ";
    // item.appendChild(tableNum);
    // tableList.appendChild(item);
    i++;

    // Display device status (open or occupied)
    // status.innerHTML = "Status: " + childSnapshot.child("DeviceStatus").val() + " ";
    // tableList.appendChild(status);



    // Display minutes remaining
    var statusStr = childSnapshot.child("DeviceStatus").val().toString();
    if (statusStr == "Open"){
      div3.innerText = "Time remaining: N/A ";
      numOpenTables++;
    }else{ // otherwise, occupied
      div3.innerText = "Time remaining: " + childSnapshot.child("TimeRemaining").val();
    }
    tableList.appendChild(timeLeft);
    tableList.appendChild(space);
  });

  openTables.innerHTML = "Tables Open: " + numOpenTables; // Display num open tables
});
