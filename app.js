'use strict'

/*
Backend Code for GPX parser
Written by: Lorent Aliu
*/

// C library API
const ffi = require('ffi-napi');

// Express App (Routes)
const express = require("express");
const app     = express();
const path    = require("path");
const fileUpload = require('express-fileupload');

app.use(fileUpload());
app.use(express.static(path.join(__dirname+'/uploads')));

// Minimization
const fs = require('fs');
const JavaScriptObfuscator = require('javascript-obfuscator');
const { stringify } = require('querystring');

// Important, pass in port as in `npm run dev 1234`, do not change
const portNum = process.argv[2];

// Send HTML at root, do not change
app.get('/',function(req,res){
  res.sendFile(path.join(__dirname+'/public/index.html'));
});

// Send Style, do not change
app.get('/style.css',function(req,res){
  //Feel free to change the contents of style.css to prettify your Web app
  res.sendFile(path.join(__dirname+'/public/style.css'));
});

// Send obfuscated JS, do not change
app.get('/index.js',function(req,res){
  fs.readFile(path.join(__dirname+'/public/index.js'), 'utf8', function(err, contents) {
    const minimizedContents = JavaScriptObfuscator.obfuscate(contents, {compact: true, controlFlowFlattening: true});
    res.contentType('application/javascript');
    res.send(minimizedContents._obfuscatedCode);
  });
});

//Respond to POST requests that upload files to uploads/ directory
app.post('/upload', function(req, res) {
  if(!req.files) {
    return res.status(400).send('No files were uploaded.');
  }
 
  let uploadFile = req.files.uploadFile;
 
  // Use the mv() method to place the file somewhere on your server
  uploadFile.mv('uploads/' + uploadFile.name, function(err) {
    if(err) {
      return res.status(500).send(err);
    }

    res.redirect('/');
  });
});

//Respond to GET requests for files in the uploads/ directory
app.get('/uploads/:name', function(req , res){
  fs.stat('uploads/' + req.params.name, function(err, stat) {
    if(err == null) {
      res.sendFile(path.join(__dirname+'/uploads/' + req.params.name));
    } else {
      console.log('Error in file downloading route: '+err);
      res.send('');
    }
  });
});

//******************** Your code goes here ******************** 

let sharedLib = ffi.Library('./libgpxparser', {

  'createGPXJSON': ['string', ['string' , 'string']],
  'fileToRouteList': ['string', ['string']],
  'fileToTrackList': ['string', ['string']],
  'renameTrack':['void',['string', 'string', 'string']],
  'renameRoute':['void',['string', 'string', 'string']],
  'fileFromJSON':['void', ['string', 'string']],
  'writeAddRoute':['void', ['string', 'string']],
  'writeAddWaypoint':['void', ['string', 'string', 'string']],
  'fileToDataList':['string', ['string', 'string', 'string']],
  'tracksBetweenList':['string', ['string', 'string', 'double', 'double', 'double', 'double', 'double']],
  'routesBetweenList':['string', ['string', 'string', 'double', 'double', 'double', 'double', 'double']],
  'fileRouteToWPList':['string', ['string', 'string']]

});

app.get('/endpoint1', function(req , res){
  let retStr = req.query.data1 + " " + req.query.data2;
  res.send(
    {
      somethingElse: retStr
    }
  );
});
app.get('/endpoint2', function(req , res){

  let i = 0;
  let files = fs.readdirSync('./uploads/');
  let gpxJSON = [];
  let failValid = [];
  let numValid = 0;
  files.forEach(file => {
    gpxJSON[i] = sharedLib.createGPXJSON("uploads/" + file, "./parser/gpx.xsd");
    failValid[i] = 0;
    if(gpxJSON[i] === "{}"){
      failValid[i] = 1;
      console.log("Invalid File detected: " + file);
      numValid--;
    }
    numValid++;
    i++;
  })
  res.send({
    filename: files,
    jsonArray: gpxJSON,
    validity: failValid,
    numFiles: numValid
  });
});
app.get('/endpoint3', function(req , res){

  let i = 0;
  let files = fs.readdirSync('./uploads/');
  let gpxJSON = [];
  let failValid = [];
  let numValid = 0;
  files.forEach(file => {
    gpxJSON[i] = sharedLib.createGPXJSON("uploads/" + file, "./parser/gpx.xsd");
    failValid[i] = 0;
    if(gpxJSON[i] === "{}"){
      failValid[i] = 1;
      numValid--;
    }
    numValid++;
    i++;
  })
  res.send({
    filename: files,
    jsonArray: gpxJSON,
    validity: failValid,
    numFiles: numValid
  });
});

app.get('/endpoint4', function(req , res){

  let filename = req.query.file
  let gpx;
  gpx = sharedLib.createGPXJSON("uploads/" + filename, "./parser/gpx.xsd");
  let trackList;
  trackList = sharedLib.fileToTrackList("uploads/" + filename);
  let routeList;
  routeList = sharedLib.fileToRouteList("uploads/" + filename);

  res.send({
    routes: routeList,
    tracks: trackList,
    doc: gpx,
    file: filename
  });
});

async function renameDB(user, pass, name, rname, newName, fileName){
  let connection;
  try{
    connection = await mysql.createConnection({
      host : 'dursley.socs.uoguelph.ca',
      user : user,
      password : pass,
      database : name
    })
    await connection.execute("UPDATE ROUTE SET route_name = '" + newName + "' WHERE route_name = '" + rname + "'");

  }
  catch(e){
    console.log('Connection failed: ' + e);
  }
  finally{
    if(connection && connection.end()) connection.end();
  }

}

app.get('/endpoint5', function(req , res){

  let fileName = req.query.fileName;
  let type = req.query.compType;
  let name = req.query.compName;
  let newName = req.query.newName;

  if(type==='track'){
    sharedLib.renameTrack("uploads/" + fileName, name, newName);
  }
  if(type==='route'){
    //.log('its a route!');
    sharedLib.renameRoute("uploads/" + fileName, name, newName);
  }
  res.send({
  });
});

async function dbCreateDoc(user, pass, name, file, creator, version){

  let connection;
  try{
    connection = await mysql.createConnection({
      host : 'dursley.socs.uoguelph.ca',
      user : user,
      password : pass,
      database : name
    })
    await connection.execute("INSERT INTO FILE (file_name, ver, creator) VALUES ('" + file + "', '" + version + "', '" + creator + "')");
  }
  catch(e){
    console.log('Connection failed: ' + e);
  }
  finally{
    if(connection && connection.end()) await connection.end();
  }

}

app.get('/endpoint6', function(req , res){

  let fileName = req.query.fileName;
  let creator = req.query.creator;
  let version = req.query.version;

  let jsonObj = {version: Number(version), creator: creator};
  let jsonString = JSON.stringify(jsonObj);
  sharedLib.fileFromJSON("uploads/" + fileName, jsonString);

  dbCreateDoc(req.query.user, req.query.pass, req.query.name, fileName, creator, version);

  res.send({
  });
});

async function addRouteDB(user, pass, name, fileName, wpList, rname){

  let list = JSON.parse(wpList);
  console.log(list);
  let connection;
  let leng = 0;
  try{
    connection = await mysql.createConnection({
      host : 'dursley.socs.uoguelph.ca',
      user : user,
      password : pass,
      database : name
    })
    let [rows, fields] = await connection.execute("SELECT gpx_id FROM FILE WHERE file_name = '" + fileName + "'");
    await connection.execute("INSERT INTO ROUTE (route_name, route_len, gpx_id) VALUES ('" + rname + "', '" + leng + "', " + rows[0].gpx_id + ")");
    for(let loop1=0;loop1<list.length;loop1++){
      let waypoint = await list[loop1];
      let [rows2, fields2] = await connection.execute("SELECT MAX(route_id) as LAST from ROUTE");
      await connection.execute("INSERT INTO POINT (point_name, latitude, longitude, point_index, route_id) VALUES ('" + waypoint.name + "', '" + waypoint.lat + "', '" + waypoint.lon + "', '" + loop1 + "', " + rows2[0].LAST + ")");
    }
  }
  catch(e){
    console.log('Connection failed: ' + e);
  }
  finally{
    if(connection && connection.end()) await connection.end();
  }

}

app.get('/endpoint7', function(req , res){

  let fileName = req.query.file;
  let nameObj = req.query.name;
  let wpList = req.query.wpList;
  let name = req.query.dbname;
  let user = req.query.dbuser;
  let pass = req.query.dbpass;

  sharedLib.writeAddRoute("uploads/" + fileName, JSON.stringify(nameObj));
  wpList.forEach(item=>{
    sharedLib.writeAddWaypoint("uploads/" + fileName, nameObj.name, item);
  });

  addRouteDB(user, pass, name, fileName, wpList, nameObj.name);

  res.send({
  });
});

app.get('/endpoint8', function(req , res){

  let file = req.query.file;
  let type = req.query.type;
  let name = req.query.name;

  let jsonStr = sharedLib.fileToDataList("uploads/" + file, name, type);
  jsonStr.replace('\n', '');
  jsonStr.replace('\t', '');
  let jsonObj = JSON.parse(jsonStr);
  res.send({
    dataList: jsonObj
  });
});

app.get('/endpoint9', function(req , res){

  let latOne = req.query.latOne;
  let latTwo = req.query.latTwo;
  let lonOne = req.query.lonOne;
  let lonTwo = req.query.lonTwo;
  let delta = req.query.delta;
  let files = fs.readdirSync('./uploads/');
  let object = [];
  let object2 = [];
  let tempString;
  let i = 0;
  files.forEach(file => {
    tempString = sharedLib.tracksBetweenList("uploads/" + file, "./parser/gpx.xsd", latOne, latTwo, lonOne, lonTwo, delta);
    object[i] = JSON.parse(tempString);
    tempString = sharedLib.routesBetweenList("uploads/" + file, "./parser/gpx.xsd", latOne, latTwo, lonOne, lonTwo, delta);
    object2[i] = JSON.parse(tempString);
    i++;
  })

  res.send({
    object: object,
    object2: object2
  });
});

const mysql = require('mysql2/promise');
const e = require('express');
const { connect } = require('http2');

app.get('/endpoint10', async function(req , res){

  let user = req.query.user;
  let pass = req.query.pass;
  let name = req.query.name;
  let connection;
  let errorString ='';
  try{
    connection = await mysql.createConnection({
      host : 'dursley.socs.uoguelph.ca',
      user : user,
      password : pass,
      database : name
    })
    console.log('successfully connected');
    await connection.execute("create table IF NOT EXISTS FILE (gpx_id INT AUTO_INCREMENT, file_name VARCHAR(60) NOT NULL, ver DECIMAL(2,1) NOT NULL, creator VARCHAR(256) NOT NULL, primary key(gpx_id))");
    await connection.execute("create table IF NOT EXISTS ROUTE (route_id INT AUTO_INCREMENT, route_name VARCHAR(256), route_len FLOAT(15,7) NOT NULL, gpx_id INT NOT NULL, primary key(route_id), foreign key (gpx_id) references FILE (gpx_id) ON DELETE CASCADE)");
    await connection.execute("create table IF NOT EXISTS POINT (point_id INT AUTO_INCREMENT, point_index INT NOT NULL, latitude DECIMAL(11,7) NOT NULL, longitude DECIMAL(11,7) NOT NULL, point_name VARCHAR(256),  route_id INT NOT NULL, primary key(point_id), foreign key (route_id) references ROUTE (route_id) ON DELETE CASCADE)");

  }
  catch(e){
    console.log('Connection failed: ' + e);
    errorString = e.message;
  }
  finally{
    if(connection && connection.end()) connection.end();
  }

  res.send({
    error: errorString
  });
});

app.get('/endpoint11', async function(req , res){

  let user = req.query.user;
  let pass = req.query.pass;
  let name = req.query.name;
  let connection;
  let errorString ='';
  try{
    connection = await mysql.createConnection({
      host : 'dursley.socs.uoguelph.ca',
      user : user,
      password : pass,
      database : name
    })
    await connection.execute("DELETE FROM POINT");
    await connection.execute("DELETE FROM ROUTE");
    await connection.execute("DELETE FROM FILE");
    await connection.execute("ALTER TABLE POINT AUTO_INCREMENT = 1");
    await connection.execute("ALTER TABLE ROUTE AUTO_INCREMENT = 1");
    await connection.execute("ALTER TABLE FILE AUTO_INCREMENT = 1");
  }
  catch(e){
    console.log('Connection failed: ' + e);
    errorString = e.message;
  }
  finally{
    if(connection && connection.end()) connection.end();
  }

  res.send({
    error: errorString
  });
});

app.get('/endpoint12', async function(req , res){

  let user = req.query.user;
  let pass = req.query.pass;
  let name = req.query.name;
  let connection;
  let errorString ='';
  let tableStats = {files: 0, routes: 0, points: 0};
  try{
    connection = await mysql.createConnection({
      host : 'dursley.socs.uoguelph.ca',
      user : user,
      password : pass,
      database : name
    })
    let [rows, fields] = await connection.execute("SELECT * FROM FILE");
    tableStats.files = rows.length;
    let [rows2, fields2] = await connection.execute("SELECT * FROM ROUTE");
    tableStats.routes = rows2.length;
    let [rows3, fields3] = await connection.execute("SELECT * FROM POINT");
    tableStats.points = rows3.length;
  }
  catch(e){
    console.log('Connection failed: ' + e);
    errorString = e.message;
  }
  finally{
    if(connection && connection.end()) connection.end();
  }

  res.send({
    error: errorString,
    stats: tableStats
  });
});

async function insertFile(object, filename, user, pass, name){
  let connection;
  try{
    connection = await mysql.createConnection({
      host : 'dursley.socs.uoguelph.ca',
      user : user,
      password : pass,
      database : name
    })
    await connection.execute("INSERT INTO FILE (file_name, ver, creator) VALUES ('" + filename + "', '" + object.version + "', '" + object.creator + "')");
  }
  catch(e){
    console.log('Connection failed: ' + e);
  }
  finally{
    if(connection && connection.end()) await connection.end();
  }
}

async function insertRoute(item2, user, pass, name){
  let connection;
  try{
    connection = await mysql.createConnection({
      host : 'dursley.socs.uoguelph.ca',
      user : user,
      password : pass,
      database : name
    })
    let [rows, fields] = await connection.execute("SELECT MAX(gpx_id) as LAST from FILE");
    await connection.execute("INSERT INTO ROUTE (route_name, route_len, gpx_id) VALUES ('" + item2.name + "', '" + item2.len + "', " + rows[0].LAST + ")");
  }
  catch(e){
    console.log('Connection failed: ' + e);
  }
  finally{
    if(connection && connection.end()) await connection.end();
  }
}
async function insertWp(item3, user, pass, name, index){
  let connection;
  try{
    connection = await mysql.createConnection({
      host : 'dursley.socs.uoguelph.ca',
      user : user,
      password : pass,
      database : name
    })
    let [rows2, fields2] = await connection.execute("SELECT MAX(route_id) as LAST from ROUTE");
    console.log(rows2[0].LAST);
    await connection.execute("INSERT INTO POINT (point_name, latitude, longitude, point_index, route_id) VALUES ('" + item3.name + "', '" + item3.lat + "', '" + item3.lon + "', '" + index + "', " + rows2[0].LAST + ")");
  }
  catch(e){
    console.log('Connection failed: ' + e);
  }
  finally{
    if(connection && connection.end()) await connection.end();
  }
}

async function createData(objects, filenames, user, pass, name){
  let k =0;
  let loop1 = 0;
  let loop2 = 0;
  let loop3 = 0;
  let tempIndex = 0;
  console.log(objects);
  console.log(filenames);
  console.log('Length of obj: ' + objects.length + ' Length of files : ' + filenames.length);
  for(loop1 = 0; loop1 < objects.length; loop1++){

    let item = objects[loop1];

    insertFile(item, filenames[k], user, pass, name);
    console.log(item);
    console.log(filenames[k]);
    let routeList = await sharedLib.fileToRouteList("uploads/" + filenames[k]);
    let routes = await JSON.parse(routeList);
    for(loop2 = 0;loop2<routes.length;loop2++){
      let item2 = routes[loop2];
      insertRoute(item2, user, pass, name);
      let wpList = await sharedLib.fileRouteToWPList("uploads/" + filenames[k], item2.name);
      let waypoints = await JSON.parse(wpList);
      tempIndex = 0;
      for(loop3 = 0;loop3<waypoints.length;loop3++){
        let item3 = waypoints[loop3];
        insertWp(item3, user, pass, name, tempIndex);
        console.log(item3);
        tempIndex++;
      }
    }
    k++;
  }

}

async function fileLogDB(objects, filenames, user, pass, name){
  await console.log(objects);
  await console.log(filenames);

  let connection;
  try{
    //Setup connection
    connection = await mysql.createConnection({
      host : 'dursley.socs.uoguelph.ca',
      user : user,
      password : pass,
      database : name
    })
    
    for(let loop1 = 0; loop1 < objects.length; loop1++){
      //Insert file normally
      console.log(filenames[loop1]);
      let object = await objects[loop1];
      let filename = await filenames[loop1];
      await connection.execute("INSERT INTO FILE (file_name, ver, creator) VALUES ('" + filename + "', '" + object.version + "', '" + object.creator + "')");
      //Create Routelist
      let routeList = await sharedLib.fileToRouteList("uploads/" + filenames[loop1]);
      let routes = await JSON.parse(routeList);
      console.log(routes);
      for(let loop2 = 0;loop2 < routes.length;loop2++){
        //Insert Each route to matching GPX id
        let route = await routes[loop2];

        let [rows, fields] = await connection.execute("SELECT MAX(gpx_id) as LAST from FILE");
        await connection.execute("INSERT INTO ROUTE (route_name, route_len, gpx_id) VALUES ('" + route.name + "', '" + route.len + "', " + rows[0].LAST + ")");
        //Create WPList
        let wpList = await sharedLib.fileRouteToWPList("uploads/" + filenames[loop1], route.name);
        let waypoints = await JSON.parse(wpList);
        console.log(waypoints);

        for(let loop3 = 0;loop3 < waypoints.length;loop3++){
          //Insert each waypoint to matching Route ID
          let waypoint = await waypoints[loop3];

          let [rows2, fields2] = await connection.execute("SELECT MAX(route_id) as LAST from ROUTE");
          await connection.execute("INSERT INTO POINT (point_name, latitude, longitude, point_index, route_id) VALUES ('" + waypoint.name + "', '" + waypoint.lat + "', '" + waypoint.lon + "', '" + loop3 + "', " + rows2[0].LAST + ")");

        }
      }
    }
  }
  catch(e){
    console.log('Connection failed: ' + e);
  }
  finally{
    if(connection && connection.end()) await connection.end();
  }

}

app.get('/endpoint13', function(req , res){

  let i = 0;
  let j = 0;
  let files = fs.readdirSync('./uploads/');
  let gpxJSON = [];
  let failValid = [];
  let numValid = 0;
  let user = req.query.user;
  let pass = req.query.pass;
  let name = req.query.name;
  let objects =[];
  let filenames = [];
  files.forEach(file => {
    gpxJSON[i] = sharedLib.createGPXJSON("uploads/" + file, "./parser/gpx.xsd");
    failValid[i] = 0;
    if(gpxJSON[i] === "{}"){
      failValid[i] = 1;
      numValid--;
    }
    else{
      objects[j] = JSON.parse(gpxJSON[i]);
      filenames[j] = file;
      j++;

    }
    numValid++;
    i++;
  })

  fileLogDB(objects, filenames, user, pass, name);

  res.send({
  });
});

app.get('/endpoint14', async function(req , res){

  let user = req.query.user;
  let pass = req.query.pass;
  let name = req.query.name;
  let connection;
  let list;
  try{
    connection = await mysql.createConnection({
      host : 'dursley.socs.uoguelph.ca',
      user : user,
      password : pass,
      database : name
    })
    let [rows, fields] = await connection.execute("SELECT * FROM FILE");
    list = rows;
  }
  catch(e){
    console.log('Connection failed: ' + e);
  }
  finally{
    if(connection && connection.end()) connection.end();
  }
  res.send({
    list: list
  });
});

app.get('/endpoint15', async function(req , res){

  let user = req.query.user;
  let pass = req.query.pass;
  let name = req.query.name;
  let connection;
  let list;
  try{
    connection = await mysql.createConnection({
      host : 'dursley.socs.uoguelph.ca',
      user : user,
      password : pass,
      database : name
    })
    let [rows, fields] = await connection.execute("SELECT * FROM ROUTE WHERE gpx_id IN (SELECT gpx_id FROM FILE WHERE file_name = '" + req.query.file + "')");
    list = rows;
  }
  catch(e){
    console.log('Connection failed: ' + e);
  }
  finally{
    if(connection && connection.end()) connection.end();
  }
  res.send({
    list: list
  });
});

app.get('/endpoint16', async function(req , res){

  let user = req.query.user;
  let pass = req.query.pass;
  let name = req.query.name;
  let connection;
  let list;
  try{
    connection = await mysql.createConnection({
      host : 'dursley.socs.uoguelph.ca',
      user : user,
      password : pass,
      database: name
    })
    if(req.query.sort == 'Name'){
      let [rows, fields] = await connection.execute("SELECT * FROM ROUTE ORDER BY route_name");
      list = rows;
    }
    if(req.query.sort == 'Length'){
      let [rows, fields] = await connection.execute("SELECT * FROM ROUTE ORDER BY route_len");
      list = rows;
    }
  }
  catch(e){
    console.log('Connection failed: ' + e);
  }
  finally{
    if(connection && connection.end()) connection.end();
  }
  res.send({
    list: list
  });
});

app.get('/endpoint17', async function(req , res){

  let user = req.query.user;
  let pass = req.query.pass;
  let name = req.query.name;
  let connection;
  let list;
  try{
    connection = await mysql.createConnection({
      host : 'dursley.socs.uoguelph.ca',
      user : user,
      password : pass,
      database: name
    })
    if(req.query.sort == 'Name'){
      let [rows, fields] = await connection.execute("SELECT * FROM ROUTE WHERE gpx_id IN (SELECT gpx_id FROM FILE WHERE file_name = '" + req.query.file + "') ORDER BY route_name");
    list = rows;
    }
    if(req.query.sort == 'Length'){
      let [rows, fields] = await connection.execute("SELECT * FROM ROUTE WHERE gpx_id IN (SELECT gpx_id FROM FILE WHERE file_name = '" + req.query.file + "') ORDER BY route_len");
    list = rows;
    }
    
  }
  catch(e){
    console.log('Connection failed: ' + e);
  }
  finally{
    if(connection && connection.end()) connection.end();
  }
  res.send({
    list: list
  });
});

app.get('/endpoint18', async function(req , res){

  let user = req.query.user;
  let pass = req.query.pass;
  let name = req.query.name;
  let connection;
  let list;
  try{
    connection = await mysql.createConnection({
      host : 'dursley.socs.uoguelph.ca',
      user : user,
      password : pass,
      database: name
    })
    let [rows, fields] = await connection.execute("SELECT * FROM POINT WHERE route_id IN (SELECT route_id FROM ROUTE WHERE route_name = '" + req.query.route + "')  ORDER BY point_index");
    list = rows;
  }
  catch(e){
    console.log('Connection failed: ' + e);
  }
  finally{
    if(connection && connection.end()) connection.end();
  }
  res.send({
    list: list
  });
});

app.get('/endpoint19', async function(req , res){

  let user = req.query.user;
  let pass = req.query.pass;
  let name = req.query.name;
  let connection;
  let list;
  let nameList;
  let lengthList;
  try{
    connection = await mysql.createConnection({
      host : 'dursley.socs.uoguelph.ca',
      user : user,
      password : pass,
      database: name
    })
    let [rows, fields] = await connection.execute("SELECT * FROM POINT WHERE route_id IN (SELECT route_id FROM ROUTE WHERE gpx_id IN (SELECT gpx_id FROM FILE WHERE file_name = '" + req.query.file + "'))   ORDER BY route_id, point_index");
    list = rows;
  }
  catch(e){
    console.log('Connection failed: ' + e);
  }
  finally{
    if(connection && connection.end()) connection.end();
  }
  res.send({
    list: list,
    nameList: nameList,
    lengthList: lengthList
  });
});

app.get('/endpoint20', async function(req , res){

  let user = req.query.user;
  let pass = req.query.pass;
  let name = req.query.name;
  let connection;
  let list;
  let N = req.query.NNumber;
  let sorting;
  try{
    connection = await mysql.createConnection({
      host : 'dursley.socs.uoguelph.ca',
      user : user,
      password : pass,
      database: name
    })
    if(req.query.sort2 == 'Longest'){
      sorting = 'DESC'
    }
    if(req.query.sort2 == 'Shortest'){
      sorting = 'ASC'
    }
    if(req.query.sort == 'Name'){
      console.log("SELECT * FROM ROUTE WHERE gpx_id IN (SELECT gpx_id FROM FILE WHERE file_name = '" + req.query.file + "') ORDER BY route_len " + sorting + " LIMIT " + N);
      let [rows, fields] = await connection.execute("SELECT * FROM ROUTE WHERE gpx_id IN (SELECT gpx_id FROM FILE WHERE file_name = '" + req.query.file + "') ORDER BY route_len " + sorting + " LIMIT " + N);
    list = rows;
    }
    if(req.query.sort == 'Length'){
      console.log("SELECT * FROM ROUTE WHERE gpx_id IN (SELECT gpx_id FROM FILE WHERE file_name = '" + req.query.file + "') ORDER BY route_len " + sorting + " LIMIT " + N);
      let [rows, fields] = await connection.execute("SELECT * FROM ROUTE WHERE gpx_id IN (SELECT gpx_id FROM FILE WHERE file_name = '" + req.query.file + "') ORDER BY route_len " + sorting + " LIMIT " + N);
    list = rows;
    }
  }
  catch(e){
    console.log('Connection failed: ' + e);
  }
  finally{
    if(connection && connection.end()) connection.end();
  }
  res.send({
    list: list
  });
});

app.listen(portNum);
console.log('Running app at localhost: ' + portNum);