let i = 0;
let numOpenTables = 0;

firebase.database().ref('Napkn').once("value", function(snapshot) {
    snapshot.forEach(function(childSnapshot) {
        
        let listItem = document.createElement('li');

        // show table number
        let tableNum = document.createElement('h2');
        tableNum.innerHTML = "table " + (i+1);
        listItem.appendChild(tableNum);
        
        // show device status (open or occupied)
        let deviceStatus = document.createElement('h3');
        let status = childSnapshot.child("DeviceStatus").val();
        deviceStatus.innerHTML = status.toLowerCase();
        listItem.appendChild(deviceStatus);

        
        if(status == "Occupied"){
            listItem.classList.add('occupied');
            listItem.appendChild(document.createElement('br'));
            
            let remaining = document.createElement('h3');
            remaining.innerHTML = "remaining:";
            listItem.appendChild(remaining);

            // Display minutes remaining
            let timeLeft = document.createElement('h2');
            timeLeft.innerHTML = childSnapshot.child("TimeRemaining").val();
            listItem.appendChild(timeLeft);
        }
        else
            numOpenTables++;

        let tableList = document.getElementById("TableList");
        tableList.appendChild(listItem);
        i++;
      });

    // Display number of open tables
    let openTables = document.getElementById("TablesOpen");
    openTables.innerHTML = "tables open: " + numOpenTables + "/" + i; // Display num open tables
});