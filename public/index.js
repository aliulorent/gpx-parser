/*
Frontend Code for GPX parser
Written by: Lorent Aliu
*/

// Put all onload AJAX calls here, and event listeners
jQuery(document).ready(function() {
    // On page-load AJAX Example

        jQuery.ajax({
            type: 'get',            //Request type
            dataType: 'json',       //Data type - we will use JSON for almost everything 
            url: '/endpoint2',   //The server endpoint we are connecting to
            data: {
            },
            success: function (data) {
                let i =0;
                //console.log('Successfully called function');
                let table = document.getElementById('table1');
                
                if(data.numFiles === 0){
                    let row = table.insertRow(-1);
                    let emptyCell = row.insertCell(0);
                    emptyCell.innerHTML = 'No files found';
                }
                else{
                    data.validity.forEach(file =>{
                        if(file === 0){
                            let row = table.insertRow(-1);
                            let cell1 = row.insertCell(0);
                            cell1.innerHTML = '<a href="' + data.filename[i] + '" download >' + data.filename[i] + "<\a>";
                            let doc = JSON.parse(data.jsonArray[i]);
                            let cell2 = row.insertCell(-1);
                            cell2.innerHTML = doc.version;
                            let cell3 = row.insertCell(-1);
                            cell3.innerHTML = doc.creator;
                            let cell4 = row.insertCell(-1);
                            cell4.innerHTML = doc.numWaypoints;
                            let cell5 = row.insertCell(-1);
                            cell5.innerHTML = doc.numRoutes;
                            let cell6 = row.insertCell(-1);
                            cell6.innerHTML = doc.numTracks;
                        }
                        if(file===1){
                            console.log('Invalid File detected: ' + data.filename[i]);
                        }
                        i++;
                    })
                }
            },
            fail: function(error) {
                console.log('Error while loading File View Panel!');
                alert('Something went wrong! Refresh the page!\n' + error);
                console.log(error); 
            }
        });


        jQuery.ajax({
            type: 'get',            //Request type
            dataType: 'json',       //Data type - we will use JSON for almost everything 
            url: '/endpoint3',   //The server endpoint we are connecting to
            data: {
            },
            success: function (data) {
                let i =0;
                let table = document.getElementById('table1');
                
                var x = document.getElementById("currentDoc");
                var x2 = document.getElementById("editDoc");
                data.validity.forEach(item =>{
                    if(item===0){
                        let option = document.createElement('option');
                        let option2 = document.createElement('option');

                        option.text = data.filename[i];
                        option2.text = data.filename[i];
                        x.add(option);
                        x2.add(option2);
                    }
                    else{

                    }
                    i++;
                })

    
            },
            fail: function(error) {
                console.log('Error when refreshing GPX selector!');
                alert('Something went wrong! Refresh the page!\n' + error);
                console.log(error); 
            }
        });

        //Refresh button
        document.getElementById('currentDoc').onchange = function(){ 
            let selectlist = document.getElementById("currentDoc");
            let filename = selectlist.selectedOptions[0].text;
            jQuery.ajax({
                type: 'get',            //Request type
                dataType: 'json',       //Data type - we will use JSON for almost everything 
                url: '/endpoint4',   //The server endpoint we are connecting to
                data: {
                    file: filename
                },
                success: function (data) {
                    let currDoc = JSON.parse(data.doc);
                    var currFileName = data.file;
                    var routes = JSON.parse(data.routes);
                    var tracks = JSON.parse(data.tracks);
                    let gpxtable = document.getElementById('table2');
                    gpxtable.innerHTML = "";
                    let row = gpxtable.insertRow(-1);
                    let cell1 = row.insertCell(0);
                    cell1.innerHTML = "<th><b>Component<b><th>"
                    let cell2 = row.insertCell(-1);
                    cell2.innerHTML = "<th><b>Name<b><th>";
                    let cell3 = row.insertCell(-1);
                    cell3.innerHTML = "<th><b>Number of Points<b><th>"
                    let cell4 = row.insertCell(-1);
                    cell4.innerHTML = "<th><b>Length<b><th>"
                    let cell5 = row.insertCell(-1);
                    cell5.innerHTML = "<th><b>Loop<b><th>"
                    $('#currentComponent').empty();
                    let i = 1;
                    routes.forEach(item =>{
                        let trackRow = gpxtable.insertRow(-1);
                        let tCell1 = trackRow.insertCell(-1);
                        tCell1.innerHTML = "Route " + i;
                        let tCell2 = trackRow.insertCell(-1);
                        tCell2.innerHTML = item.name;
                        let tCell3 = trackRow.insertCell(-1);
                        tCell3.innerHTML = item.numPoints;
                        let tCell4 = trackRow.insertCell(-1);
                        tCell4.innerHTML = item.len + "m";
                        let tCell5 = trackRow.insertCell(-1);
                        tCell5.innerHTML = item.loop;
                        let option = document.createElement('option');
                        option.text = item.name;
                        option.value = 'route';
                        let selecter = document.getElementById('currentComponent');
                        selecter.add(option);
                        i++;
                    })
                    i = 1;
                    tracks.forEach(item =>{
                        let trackRow = gpxtable.insertRow(-1);
                        let tCell1 = trackRow.insertCell(-1);
                        tCell1.innerHTML = "Track " + i;
                        let tCell2 = trackRow.insertCell(-1);
                        tCell2.innerHTML = item.name;
                        let tCell3 = trackRow.insertCell(-1);
                        tCell3.innerHTML = item.numPoints;
                        let tCell4 = trackRow.insertCell(-1);
                        tCell4.innerHTML = item.len + "m";
                        let tCell5 = trackRow.insertCell(-1);
                        tCell5.innerHTML = item.loop;
                        let option = document.createElement('option');
                        option.text = item.name;
                        option.value = 'track';
                        let selecter = document.getElementById('currentComponent');
                        selecter.add(option);
                        i++;
                    })

                },
                fail: function(error) {
                    console.log('Error when building GPX view table!');
                    alert('Something went wrong! Refresh the page!\n' + error);
                    console.log(error); 
                }
            });

        };

        document.getElementById('submission').onclick = function(){
            let selection = document.getElementById('currentComponent');
            let selection2 = document.getElementById('currentDoc');
            let typed = document.getElementById('rename');
            if(typed.value.length < 1){
                let divvy = document.getElementById('renamestatus');
                console.log('Name field cannot be empty!');
                divvy.innerHTML = '';
                divvy.appendChild(document.createTextNode('Missing Field!'));
                alert('Missing name field!');
            }
            else{
            //RENAMING A COMPONENT!!
                jQuery.ajax({
                    type: 'get',            //Request type
                    dataType: 'json',       //Data type - we will use JSON for almost everything 
                    url: '/endpoint5',   //The server endpoint we are connecting to
                    data: {
                        fileName: selection2.selectedOptions[0].text,
                        compType: selection.selectedOptions[0].value,
                        compName: selection.selectedOptions[0].text,
                        newName: typed.value,
                        //rnuser = dbUser,
                        //rnpass = dbPass,
                        //rndbname = dbName
                    },
                    success: function (data) {
                        let div = document.getElementById('renamestatus');
                        div.innerHTML ='';
                        let renameText = document.createTextNode('Success! (Refresh table)');
                        div.appendChild(renameText);
                    },
                    fail: function(error) {
                        console.log('Error when renaming route!');
                        alert('Something went wrong! Refresh the page!\n' + error);
                        console.log(error); 
                    }
                });
            }
        }

        document.getElementById('createGPX').onclick = function(){
        
            let name = document.getElementById('freshname').value;
            let creator = document.getElementById('freshCreator').value;
            let version = document.getElementById('freshVersion').value;
            let div = document.getElementById('newError');
            let text = document.createTextNode('Error: File name is empty!');
            let text2 = document.createTextNode('Error: Creator is empty!');
            let text3 = document.createTextNode('Error: Version is empty!');
            div.innerHTML ='';
            if(name.length < 1){
                div.appendChild(text);
                div.appendChild(document.createElement('br'));
                alert('Missing file name!');
            }
            if(creator.length < 1){
                div.appendChild(text2);
                div.appendChild(document.createElement('br'));
                alert('Missing creator name!');
            }
            if(version.length < 1){
                div.appendChild(text3);
                div.appendChild(document.createElement('br'));
                alert('Missing version number!');
            }
            if(name.length > 0 && creator.length > 0 && version.length > 0){

                jQuery.ajax({
                    type: 'get',            //Request type
                    dataType: 'json',       //Data type - we will use JSON for almost everything 
                    url: '/endpoint6',   //The server endpoint we are connecting to
                    data: {
                        
                        fileName: name,
                        creator: creator,
                        version: version,
                        user: dbUser,
                        pass: dbPass,
                        name: dbName

                    },
                    success: function (data) {
                    
                        div.innerHTML = '';
                        div.appendChild(document.createTextNode('Success!'));

                    },
                    fail: function(error) {
                        console.log('Error when creating new GPX');
                        alert('Something went wrong! Refresh the page!\n' + error);
                        console.log(error); 
                    }
                });

            }
        };
        let j = 0;
        let wpList = [];
        let addWp = document.getElementById('addWp');
        addWp.onclick = function(){
        
            let lat = document.getElementById('latitude').value;
            let lon = document.getElementById('longitude').value;
            if(lat.length < 1 || lon.elngth < 1){
                console.log('Missing field!');
                let text43 = document.createTextNode('Missing Field!');
                document.getElementById('wpAdded').innerHTML = '';
                document.getElementById('wpAdded').appendChild(text43);
            }
            else{
                let tempObj = {lat:Number(lat), lon:Number(lon)};
                wpList[j] = JSON.stringify(tempObj);
                let text = document.createTextNode('Added: ' + lat + ', ' + lon);
                document.getElementById('wpAdded').innerHTML = '';
                document.getElementById('wpAdded').appendChild(text);
                j++;
            }
        };
        let addRoute = document.getElementById('addRoute');
        addRoute.onclick = function(){
            let name = {name:document.getElementById('newRoute').value};
            let file = document.getElementById('editDoc').selectedOptions[0].text;
            if(name.length < 1){
                document.getElementById('routeAdded').innerHTML = '';
                document.getElementById('routeAdded').appendChild(document.createTextNode('Error: Empty name!'));
                alert("Empty name field!");
            }
            else{

                jQuery.ajax({
                    type: 'get',            //Request type
                    dataType: 'json',       //Data type - we will use JSON for almost everything 
                    url: '/endpoint7',   //The server endpoint we are connecting to
                    data: {
                        wpList: wpList,
                        name: name,
                        file: file,
                        dbuser: dbUser,
                        dbpass: dbPass,
                        dbname: dbName
                    },
                    success: function (data) {
                    
                        document.getElementById('routeAdded').innerHTML = '';
                        document.getElementById('routeAdded').appendChild(document.createTextNode('Added Route!'));

                    },
                    fail: function(error) {
                        console.log('Error when adding new route!');
                        alert('Something went wrong! Refresh the page!\n' + error);
                        console.log(error); 
                    }
                });

            }
        };

        document.getElementById('otherData').onclick = function(){
        
            let file = document.getElementById('currentDoc').selectedOptions[0].text;
            let type = document.getElementById('currentComponent').selectedOptions[0].value;
            let name = document.getElementById('currentComponent').selectedOptions[0].text;

            jQuery.ajax({
                type: 'get',            //Request type
                dataType: 'json',       //Data type - we will use JSON for almost everything 
                url: '/endpoint8',   //The server endpoint we are connecting to
                data: {
                    file: file,
                    type: type,
                    name: name
                },
                success: function (data) {
                
                    let dataList = data.dataList;
                    let div = document.getElementById('extraData');
                    div.innerHTML = '';
                    let bold = document.createElement('strong');
                    let header = document.createTextNode('Other Data for: ' + name);
                    let br = document.createElement('br');
                    bold.appendChild(header);
                    div.appendChild(bold);
                    div.appendChild(br);
                    if(dataList.length < 1){
                        let text = document.createTextNode('No other data');
                        div.appendChild(text);
                    }
                    else{
                        dataList.forEach(item =>{
                            let nameText = item.name;
                            let valueText = item.value;
                            let fullString = nameText + ": " + valueText;
                            let br2 = document.createElement('br');
                            div.appendChild(document.createTextNode(fullString));
                            div.appendChild(br2);
                        });
                    }

                },
                fail: function(error) {
                    console.log('Error when displaying other data!');
                    alert('Something went wrong! Refresh the page!\n' + error);
                    console.log(error); 
                }
            });

        };

        document.getElementById('findPaths').onclick = function(){
        
            let latOne = document.getElementById('latOne').value;
            let latTwo = document.getElementById('latTwo').value;
            let lonOne = document.getElementById('lonOne').value;
            let lonTwo = document.getElementById('lonTwo').value;
            let delta = document.getElementById('delta').value;
            let divPath = document.getElementById('missingPath');
            if(latOne.length < 1 || latTwo.length < 1 || lonOne.length < 1 || lonTwo.length < 1){
                divPath.innerHTML ='';
                divPath.appendChild(document.createTextNode('Missing field!'));
                alert('Missing field!');
            } 
            else{

                jQuery.ajax({
                    type: 'get',            //Request type
                    dataType: 'json',       //Data type - we will use JSON for almost everything 
                    url: '/endpoint9',   //The server endpoint we are connecting to
                    data: {
                        latOne: latOne,
                        latTwo: latTwo,
                        lonOne: lonTwo,
                        lonTwo: lonTwo,
                        delta: delta
                    },
                    success: function (data) {
                        let object = data.object;
                        let object2 = data.object2;
                        console.log(object);
                        console.log(object);
                        let table = document.getElementById('pathsTable');
                        table.innerHTML = '';
                        let rrow = table.insertRow(-1);
                        let rcell1 = rrow.insertCell(0);
                        rcell1.innerHTML = "<th><b>Component<b><th>"
                        let rcell2 = rrow.insertCell(-1);
                        rcell2.innerHTML = "<th><b>Name<b><th>";
                        let rcell3 = rrow.insertCell(-1);
                        rcell3.innerHTML = "<th><b>Number of Points<b><th>"
                        let rcell4 = rrow.insertCell(-1);
                        rcell4.innerHTML = "<th><b>Length<b><th>"
                        let rcell5 = rrow.insertCell(-1);
                        rcell5.innerHTML = "<th><b>Loop<b><th>"
                        let i = 1;
                        let totalCount = 0;
                        object2.forEach(list=>{
                            list.forEach(path=>{
                                let row = table.insertRow(-1);
                                let cell1 = row.insertCell(-1);
                                cell1.innerHTML = 'Route ' + i;
                                let cell2 = row.insertCell(-1);
                                cell2.innerHTML = path.name;
                                let cell3 = row.insertCell(-1);
                                cell3.innerHTML = path.numPoints;
                                let cell4 = row.insertCell(-1);
                                cell4.innerHTML = path.len;
                                let cell5 = row.insertCell(-1);
                                cell5.innerHTML = path.loop;
                                totalCount++;
                            });
                        });
                        i = 1;
                        object.forEach(list=>{
                            list.forEach(path=>{
                                let row = table.insertRow(-1);
                                let cell1 = row.insertCell(-1);
                                cell1.innerHTML = 'Route ' + i;
                                let cell2 = row.insertCell(-1);
                                cell2.innerHTML = path.name;
                                let cell3 = row.insertCell(-1);
                                cell3.innerHTML = path.numPoints;
                                let cell4 = row.insertCell(-1);
                                cell4.innerHTML = path.len;
                                let cell5 = row.insertCell(-1);
                                cell5.innerHTML = path.loop;
                                totalCount++;
                            });
                        });
                        if(totalCount == 0){
                            let row = table.insertRow(-1);
                                let cell1 = row.insertCell(-1);
                                cell1.innerHTML = 'No paths';
                        }
                    },
                    fail: function(error) {
                        console.log('Error when building paths table!');
                        console.log(error); 
                    }
                });
            }
        };
        let dbUser;
        let dbPass;
        let dbName;
        let dbConnect = document.getElementById('dbConnect');
        let loginCheck = 0;
        dbConnect.onclick = function(){
            console.log('Attempting to connect to database');
            let tempUser = document.getElementById('dbUser').value;
            let tempPass = document.getElementById('dbPass').value;
            let tempName = document.getElementById('dbName').value;

            jQuery.ajax({
                type: 'get',            //Request type
                dataType: 'json',       //Data type - we will use JSON for almost everything 
                url: '/endpoint10',   //The server endpoint we are connecting to
                data: {
                    user: tempUser,
                    pass: tempPass,
                    name: tempName
                },
                success: function (data) {
                    dbUser = tempUser;
                    dbPass = tempPass;
                    dbName = tempName;
                    div = document.getElementById('dbStatus');
                    if(data.error.length > 0){
                        alert('Failed to login!\n' + data.error);
                        div.innerHTML = '';
                        div.appendChild(document.createTextNode('Log in failed!'));
                        loginCheck = 0;
                    }
                    else{
                        div.innerHTML = '';
                        div.appendChild(document.createTextNode('Log in successful!'));
                        loginCheck = 1;
                        if(loginCheck == 1){
                            jQuery.ajax({
                                type: 'get',            //Request type
                                dataType: 'json',       //Data type - we will use JSON for almost everything 
                                url: '/endpoint14',   //The server endpoint we are connecting to
                                data: {
                                    user: dbUser,
                                    pass: dbPass,
                                    name: dbName
                                },
                                success: function (data) {
                                    data.list.forEach(item =>{
                                        let selection = document.getElementById('dbFileSelect');
                                        let option = document.createElement('option');
                                        option.text = item.file_name;
                                        selection.add(option);
                                    });
                                },
                                fail: function(error) {
                                    alert('Failed to connect to database!\n' + error);
                                    console.log(error);
                                }
                            });
                        }
                    }
                },
                fail: function(error) {
                    alert('Failed to login!\n' + error);
                    console.log(error);
                }
            });

        };

        document.getElementById('clearDb').onclick = function(){
            
            let dbDiv = document.getElementById('dbButtons');
            if(loginCheck===0){
                dbDiv.innerHTML = '';
                dbDiv.appendChild(document.createTextNode('Must log in first!'));
            }
            else if(loginCheck===1){
                dbDiv.innerHTML = '';
                jQuery.ajax({
                    type: 'get',            //Request type
                    dataType: 'json',       //Data type - we will use JSON for almost everything 
                    url: '/endpoint11',   //The server endpoint we are connecting to
                    data: {
                        user: dbUser,
                        pass: dbPass,
                        name: dbName
                    },
                    success: function (data) {
                        document.getElementById('dbButtons').innerHTML = '';
                        document.getElementById('dbButtons').appendChild(document.createTextNode('Purged data from database!'));
                        jQuery.ajax({
                            type: 'get',            //Request type
                            dataType: 'json',       //Data type - we will use JSON for almost everything 
                            url: '/endpoint12',   //The server endpoint we are connecting to
                            data: {
                                user: dbUser,
                                pass: dbPass,
                                name: dbName
                            },
                            success: function (data) {
                                let string = 'Database has ' + data.stats.files + ' files, ' + data.stats.routes + ' routes and ' + data.stats.points + ' points';
                                alert(string);
                            },
                            fail: function(error) {
                                alert('Failed to login!\n' + error);
                                console.log(error);
                            }
                        });
                    },
                    fail: function(error) {
                        alert('Failed to login!\n' + error);
                        console.log(error);
                    }
                });
            }

        };
        document.getElementById('storeFiles').onclick = function(){
            
            let dbDiv = document.getElementById('dbButtons');
            if(loginCheck===0){
                dbDiv.innerHTML = '';
                dbDiv.appendChild(document.createTextNode('Must log in first!'));
            }
            else if(loginCheck===1){
                dbDiv.innerHTML = '';
                
                jQuery.ajax({
                    type: 'get',            //Request type
                    dataType: 'json',       //Data type - we will use JSON for almost everything 
                    url: '/endpoint13',   //The server endpoint we are connecting to
                    data: {
                        user: dbUser,
                        pass: dbPass,
                        name: dbName
                    },
                    success: function (data) {
                        dbDiv.innerHTML = '';
                        dbDiv.appendChild(document.createTextNode('Success!'));
                        jQuery.ajax({
                            type: 'get',            //Request type
                            dataType: 'json',       //Data type - we will use JSON for almost everything 
                            url: '/endpoint12',   //The server endpoint we are connecting to
                            data: {
                                user: dbUser,
                                pass: dbPass,
                                name: dbName
                            },
                            success: function (data2) {
                                let string = 'Database has ' + data2.stats.files + ' files, ' + data2.stats.routes + ' routes and ' + data2.stats.points + ' points';
                                alert(string + "\nNote: If the number seems wrong, click Display DB Status again for a more accurate count!");
                            },
                            fail: function(error2) {
                                alert('Failed to login!\n' + error2);
                                console.log(error2);
                            }
                        });
                    },
                    fail: function(error) {
                        console.log(error);
                    }
                });
            }
        };
        document.getElementById('displayStatus').onclick = function(){
            
            let dbDiv = document.getElementById('dbButtons');
            if(loginCheck===0){
                dbDiv.innerHTML = '';
                dbDiv.appendChild(document.createTextNode('Must log in first!'));
            }
            else if(loginCheck===1){
                dbDiv.innerHTML = '';
                jQuery.ajax({
                    type: 'get',            //Request type
                    dataType: 'json',       //Data type - we will use JSON for almost everything 
                    url: '/endpoint12',   //The server endpoint we are connecting to
                    data: {
                        user: dbUser,
                        pass: dbPass,
                        name: dbName
                    },
                    success: function (data) {
                        let string = 'Database has ' + data.stats.files + ' files, ' + data.stats.routes + ' routes and ' + data.stats.points + ' points';
                        document.getElementById('dbButtons').innerHTML = '';
                        document.getElementById('dbButtons').appendChild(document.createTextNode(string));
                        alert(string);
                    },
                    fail: function(error) {
                        alert('Failed to login!\n' + error);
                        console.log(error);
                    }
                });
            }
        };

        document.getElementById('dbFileSelect').onchange = function(){
            jQuery.ajax({
                type: 'get',            //Request type
                dataType: 'json',       //Data type - we will use JSON for almost everything 
                url: '/endpoint15',   //The server endpoint we are connecting to
                data: {
                    user: dbUser,
                    pass: dbPass,
                    name: dbName,
                    file: document.getElementById('dbFileSelect').selectedOptions[0].text
                },
                success: function (data) {
                    $('#dbRouteSelect').empty();
                    data.list.forEach(item =>{
                        let selection = document.getElementById('dbRouteSelect');
                        let option = document.createElement('option');
                        option.text = item.route_name;
                        selection.add(option);
                    });
                },
                
                fail: function(error) {
                    alert('Failed to login!\n' + error);
                    console.log(error);
                }
            });
        };
        document.getElementById('allRoutes').onclick = function(){
            let mode = document.getElementById('dbSort').selectedOptions[0].text;
            jQuery.ajax({
                type: 'get',            //Request type
                dataType: 'json',       //Data type - we will use JSON for almost everything 
                url: '/endpoint16',   //The server endpoint we are connecting to
                data: {
                    user: dbUser,
                    pass: dbPass,
                    name: dbName,
                    sort: mode
                },
                success: function (data) {
                    console.log(data.list);
                    let dbtable = document.getElementById('dbTable');
                    dbtable.innerHTML = '';
                    let row = dbtable.insertRow(-1);
                    let cell1 = row.insertCell(-1);
                    cell1.innerHTML = 'route_id';
                    let cell2 = row.insertCell(-1);
                    cell2.innerHTML = 'route_name';
                    let cell3 = row.insertCell(-1);
                    cell3.innerHTML = 'route_len';
                    let cell4 = row.insertCell(-1);
                    cell4.innerHTML = 'gpx_id';
                    let i =0;
                    data.list.forEach(item=>{
                        let rowx = dbtable.insertRow(-1);
                        let cellx1 = rowx.insertCell(-1);
                        cellx1.innerHTML = item.route_id;
                        let cellx2 = rowx.insertCell(-1);
                        cellx2.innerHTML = item.route_name;
                        let cellx3 = rowx.insertCell(-1);
                        cellx3.innerHTML = item.route_len;
                        let cellx4 = rowx.insertCell(-1);
                        cellx4.innerHTML = item.gpx_id;
                    });
                    
                },
                fail: function(error) {
                    alert('Failed to connect to DB!\n' + error);
                    console.log(error);
                }
            });
            

        };
        document.getElementById('specificRoute').onclick = function(){
            let mode = document.getElementById('dbSort').selectedOptions[0].text;
            let file = document.getElementById('dbFileSelect').selectedOptions[0].text;
            jQuery.ajax({
                type: 'get',            //Request type
                dataType: 'json',       //Data type - we will use JSON for almost everything 
                url: '/endpoint17',   //The server endpoint we are connecting to
                data: {
                    user: dbUser,
                    pass: dbPass,
                    name: dbName,
                    sort: mode,
                    file: file
                },
                success: function (data) {
                    console.log(data.list);
                    let dbtable = document.getElementById('dbTable');
                    dbtable.innerHTML = '';
                    let row = dbtable.insertRow(-1);
                    let cell1 = row.insertCell(-1);
                    cell1.innerHTML = 'route_id';
                    let cell2 = row.insertCell(-1);
                    cell2.innerHTML = 'route_name';
                    let cell3 = row.insertCell(-1);
                    cell3.innerHTML = 'route_len';
                    let cell4 = row.insertCell(-1);
                    cell4.innerHTML = 'gpx_id';
                    let cell5 = row.insertCell(-1);
                    cell5.innerHTML = 'filename';
                    let i =0;
                    data.list.forEach(item=>{
                        let rowx = dbtable.insertRow(-1);
                        let cellx1 = rowx.insertCell(-1);
                        cellx1.innerHTML = item.route_id;
                        let cellx2 = rowx.insertCell(-1);
                        cellx2.innerHTML = item.route_name;
                        let cellx3 = rowx.insertCell(-1);
                        cellx3.innerHTML = item.route_len;
                        let cellx4 = rowx.insertCell(-1);
                        cellx4.innerHTML = item.gpx_id;
                        let cellx5 = rowx.insertCell(-1);
                        cellx5.innerHTML = file;
                    });
                    
                },
                fail: function(error) {
                    alert('Failed to connect to DB!\n' + error);
                    console.log(error);
                }
            });
            

        };

        document.getElementById('specificPoints').onclick = function(){
            let mode = document.getElementById('dbSort').selectedOptions[0].text;
            let file = document.getElementById('dbFileSelect').selectedOptions[0].text;
            let route = document.getElementById('dbRouteSelect').selectedOptions[0].text;
            jQuery.ajax({
                type: 'get',            //Request type
                dataType: 'json',       //Data type - we will use JSON for almost everything 
                url: '/endpoint18',   //The server endpoint we are connecting to
                data: {
                    user: dbUser,
                    pass: dbPass,
                    name: dbName,
                    sort: mode,
                    file: file,
                    route: route
                },
                success: function (data) {
                    console.log(data.list);
                    let dbtable = document.getElementById('dbTable');
                    dbtable.innerHTML = '';
                    let row = dbtable.insertRow(-1);
                    let cell1 = row.insertCell(-1);
                    cell1.innerHTML = 'point_id';
                    let cell2 = row.insertCell(-1);
                    cell2.innerHTML = 'point_name';
                    let cell3 = row.insertCell(-1);
                    cell3.innerHTML = 'latitude';
                    let cell4 = row.insertCell(-1);
                    cell4.innerHTML = 'longitude';
                    let cell5 = row.insertCell(-1);
                    cell5.innerHTML = 'point_index';
                    let cell6 = row.insertCell(-1);
                    cell6.innerHTML = 'route_id';
                    let i =0;
                    data.list.forEach(item=>{
                        let rowx = dbtable.insertRow(-1);
                        let cellx1 = rowx.insertCell(-1);
                        cellx1.innerHTML = item.point_id;
                        let cellx2 = rowx.insertCell(-1);
                        cellx2.innerHTML = item.point_name;
                        let cellx3 = rowx.insertCell(-1);
                        cellx3.innerHTML = item.latitude;
                        let cellx4 = rowx.insertCell(-1);
                        cellx4.innerHTML = item.longitude;
                        let cellx5 = rowx.insertCell(-1);
                        cellx5.innerHTML = item.point_index;
                        let cellx6 = rowx.insertCell(-1);
                        cellx6.innerHTML = item.route_id;
                    });
                    
                },
                fail: function(error) {
                    alert('Failed to connect to DB!\n' + error);
                    console.log(error);
                }
            });
        };

        document.getElementById('allPointsFile').onclick = function(){
            let mode = document.getElementById('dbSort').selectedOptions[0].text;
            let file = document.getElementById('dbFileSelect').selectedOptions[0].text;
            let route = document.getElementById('dbRouteSelect').selectedOptions[0].text;
            jQuery.ajax({
                type: 'get',            //Request type
                dataType: 'json',       //Data type - we will use JSON for almost everything 
                url: '/endpoint19',   //The server endpoint we are connecting to
                data: {
                    user: dbUser,
                    pass: dbPass,
                    name: dbName,
                    sort: mode,
                    file: file,
                    route: route
                },
                success: function (data) {
                    console.log(data.list);
                    let dbtable = document.getElementById('dbTable');
                    dbtable.innerHTML = '';
                    let row = dbtable.insertRow(-1);
                    let cell1 = row.insertCell(-1);
                    cell1.innerHTML = 'point_id';
                    let cell2 = row.insertCell(-1);
                    cell2.innerHTML = 'point_name';
                    let cell3 = row.insertCell(-1);
                    cell3.innerHTML = 'latitude';
                    let cell4 = row.insertCell(-1);
                    cell4.innerHTML = 'longitude';
                    let cell5 = row.insertCell(-1);
                    cell5.innerHTML = 'point_index';
                    let cell6 = row.insertCell(-1);
                    cell6.innerHTML = 'route_id';
                    let i =0;
                    data.list.forEach(item=>{
                        let rowx = dbtable.insertRow(-1);
                        let cellx1 = rowx.insertCell(-1);
                        cellx1.innerHTML = item.point_id;
                        let cellx2 = rowx.insertCell(-1);
                        cellx2.innerHTML = item.point_name;
                        let cellx3 = rowx.insertCell(-1);
                        cellx3.innerHTML = item.latitude;
                        let cellx4 = rowx.insertCell(-1);
                        cellx4.innerHTML = item.longitude;
                        let cellx5 = rowx.insertCell(-1);
                        cellx5.innerHTML = item.point_index;
                        let cellx6 = rowx.insertCell(-1);
                        cellx6.innerHTML = item.route_id;
                        i++;
                    });
                    
                },
                fail: function(error) {
                    alert('Failed to connect to DB!\n' + error);
                    console.log(error);
                }
            });
        };

        document.getElementById('displayN').onclick = function(){
            let mode = document.getElementById('dbSort').selectedOptions[0].text;
            let file = document.getElementById('dbFileSelect').selectedOptions[0].text;
            let mode2 = document.getElementById('NSort').selectedOptions[0].text;
            let NNumber = document.getElementById('dbN').value;
            jQuery.ajax({
                type: 'get',            //Request type
                dataType: 'json',       //Data type - we will use JSON for almost everything 
                url: '/endpoint20',   //The server endpoint we are connecting to
                data: {
                    user: dbUser,
                    pass: dbPass,
                    name: dbName,
                    sort: mode,
                    sort2: mode2,
                    NNumber: NNumber,
                    file: file
                },
                success: function (data) {
                    console.log(data.list);
                    let dbtable = document.getElementById('dbTable');
                    dbtable.innerHTML = '';
                    let row = dbtable.insertRow(-1);
                    let cell1 = row.insertCell(-1);
                    cell1.innerHTML = 'route_id';
                    let cell2 = row.insertCell(-1);
                    cell2.innerHTML = 'route_name';
                    let cell3 = row.insertCell(-1);
                    cell3.innerHTML = 'route_len';
                    let cell4 = row.insertCell(-1);
                    cell4.innerHTML = 'gpx_id';
                    let cell5 = row.insertCell(-1);
                    cell5.innerHTML = 'filename';
                    let i =0;
                    data.list.forEach(item=>{
                        let rowx = dbtable.insertRow(-1);
                        let cellx1 = rowx.insertCell(-1);
                        cellx1.innerHTML = item.route_id;
                        let cellx2 = rowx.insertCell(-1);
                        cellx2.innerHTML = item.route_name;
                        let cellx3 = rowx.insertCell(-1);
                        cellx3.innerHTML = item.route_len;
                        let cellx4 = rowx.insertCell(-1);
                        cellx4.innerHTML = item.gpx_id;
                        let cellx5 = rowx.insertCell(-1);
                        cellx5.innerHTML = file;
                    });
                    
                },
                fail: function(error) {
                    alert('Failed to connect to DB!\n' + error);
                    console.log(error);
                }
            });
            

        };

    // Event listener form example , we can use this instead explicitly listening for events
    // No redirects if possible
    $('#someform').submit(function(e){
        $('#blah').html("Form has data: "+$('#entryBox').val());
        e.preventDefault();
        //Pass data to the Ajax call, so it gets passed to the server
        $.ajax({
            //Create an object for connecting to another waypoint
        });
    });
});
