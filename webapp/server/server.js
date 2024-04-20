"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
var path_1 = require("path");
var express_1 = require("express");
var user_js_1 = require("./user.js");
var crypto_1 = require("crypto");
//replace dirname since now it is module
var __dirname = path_1.default.resolve();
var app = (0, express_1.default)();
var port = 3000;
var sessionManager_js_1 = require("./sessionManager.js");
//create instance of SessionManager
var session = new sessionManager_js_1.SessionManager();
app.use(express_1.default.json());
app.use(express_1.default.urlencoded({ extended: true }));
app.use(logRequest);
var LeaderboardObject = [];
//initialize leaderboard with some values
LeaderboardObject.push({ user: "testUser", avgspd: 1, dist: 1, time: 1, gain: 1 });
//initialize users with some values
var users = {};
//add salting and hashing later
users["alice"] = (new user_js_1.default("alice", "secret", null));
users["Joe"] = (new user_js_1.default("Joe", "", null));
//create a set of users. 
//serve static files to client
var clientApp = path_1.default.join(__dirname, "compiled", "client");
//protecting endpoints
// console.log(clientApp + "/index.html");
//open ports for getting the leaderboard
app.get("/leaderboard", session.middleware, function (req, res) {
    res.status(200).send(JSON.stringify(LeaderboardObject));
});
app.get("/uploads", session.middleware, function (req, res) {
    res.status(200).send(JSON.stringify(users[req.username].workouts));
});
app.post("/routeupload", session.middleware, function (req, res) {
    var user = req.username;
    users[user].addWorkout({
        id: crypto_1.default.randomUUID(),
        completed: false,
        locationRecord: req.body,
        workoutDistance: 1,
        workoutDate: -1,
        workoutTime: -1,
    });
    console.log(users[user]);
    res.status(200).send("testing for now");
});
app.post("/login", function (req, res) {
    var user = req.body.username;
    if (user == null) {
        res.redirect("/login");
        res.send();
    }
    else {
        if (user in users && users[user].comparePassword(req.body.password)) {
            session.createSession(res, user);
            res.redirect("/");
            res.send();
        }
        else {
            res.status(401).send("User password combination does not exist, please try again. <br><a href=\"/login\"> Go back to home page <a>");
            res.send();
        }
    }
});
app.use('/login', express_1.default.static(clientApp + "/login", { extensions: ['html'] }));
app.use('/', session.middleware, express_1.default.static(clientApp, { extensions: ['html'] }));
app.use('/map', session.middleware, express_1.default.static(clientApp + "/map", { extensions: ['html'] }));
//listen to incoming requests
app.listen(port, function () {
    console.log("Example app listening on port ".concat(port));
});
//handle errors
app.use(function (err, req, res, next) {
    //console.log(err);
    if (err instanceof session.Error) {
        if (req.headers.accept == "application/json") {
            res.status(401).send(err);
        }
        else {
            res.redirect("/login");
            req.greatSuccess = false;
        }
    }
    else {
        console.log(err);
        res.status(500).send("Not SessionError Object");
    }
});
//function to log incoming requests
function logRequest(req, res, next) {
    console.log("".concat(new Date(), "  ").concat(req.ip, " : ").concat(req.method, " ").concat(req.path));
    next();
}
var parseCookie = function (str) {
    return str
        .split(';')
        .map(function (v) { return v.split('='); })
        .reduce(function (acc, v) {
        acc[decodeURIComponent(v[0].trim())] = decodeURIComponent(v[1].trim());
        return acc;
    }, {});
};
