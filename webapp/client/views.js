"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.LoginView = exports.RegisterView = exports.LeaderboardView = exports.HomeView = void 0;
var utilities_js_1 = require("./utilities.js");
var services_js_1 = require("./services.js");
//mapping stuff
var HomeView = /** @class */ (function () {
    function HomeView() {
        this.elem = (0, utilities_js_1.createDOM)("\n        <div id = \"page-view\">\n        <div class = \"content\">\n            <h1>Your workout statistics</h1>\n            <div class=\"grid-container\">\n                <div class=\"grid-item\" id = total-distance>\n                    <h2>Total Distance run</h2>\n                    <span id = total-distance-val>100</span> km\n                </div>\n                <div class=\"grid-item\" id = avg-spd>\n                    <h2>Average speed</h2>\n                    <span id = avg-spd-val>100</span> km\n                </div>\n                <div class=\"grid-item\" id = workout-time>\n                    <h2>Workout time</h2>\n                    <span id = workout-time-val>100</span> hours\n                </div>\n                <div class=\"grid-item\" id = elevation-gain>\n                    <h2>Elevation Gain</h2>\n                    <span id = elevation-gain-val>100</span> hours\n                </div>\n                \n            </div> \n\n            <h1>Your workouts history</h1>\n            <div class = \"grid-container-2\" id = \"workout-log\">\n                <div class = \"grid-item\">\n                    <H2>Workout ID</H2>\n                </div>\n                <div class = \"grid-item\">\n                    <H2>Workout Distance</H2>\n                </div>\n                <div class = \"grid-item\">\n                    <H2>Workout Date</H2>\n                </div>\n                <div class = \"grid-item\">\n                    <h2>Workout Time</h2>\n                </div>\n            </div>\n\n            <h1>Your workouts uploads</h1>\n            <div class = \"grid-container-3\" id = \"workout-log\">\n                <div class = \"grid-item\">\n                    <H2>Workout ID</H2>\n                </div>\n                <div class = \"grid-item\">\n                    <H2>Map</H2>\n                </div>\n                </div>\n            </div>\n          <div class = \"page-control\">\n          </div>\n        </div>\n        ");
        this.totalDistanceElem = document.getElementById("total-distance-val");
        this.averageSpeedElem = document.getElementById("avg-spd-val");
        this.workoutTimeElem = document.getElementById("workout-time-val");
        this.elevationGainElem = document.getElementById("elevation-gain-val");
        this.workoutLog = this.elem.querySelector(".grid-container-2");
        this.uploadLog = this.elem.querySelector(".grid-container-3");
    }
    HomeView.prototype.addWorkout = function (workout) {
        var elems = [];
        elems[0] = (0, utilities_js_1.createDOM)("\n                <span class = \"workout-log-id\"></span>\n        ");
        elems[1] = (0, utilities_js_1.createDOM)("\n                <span class = \"workout-log-dist\"></span>\n        ");
        elems[2] = (0, utilities_js_1.createDOM)("\n                <span class = \"workout-log-date\"></span>\n        ");
        elems[3] = (0, utilities_js_1.createDOM)("\n                <span class = \"workout-log-time\"></span>\n        ");
        elems[0].textContent = workout.id;
        elems[1].textContent = workout.workoutDistance.toString();
        elems[2].textContent = workout.workoutDate.toDateString();
        elems[3].textContent = workout.workoutTime.toString();
        for (var i = 0; i < elems.length; i++) {
            this.workoutLog.appendChild(elems[i]);
        }
    };
    HomeView.prototype.addUpload = function (workout) {
        var idelem = (0, utilities_js_1.createDOM)("\n            <span class = \"upload-id\">Test</span>\n        ");
        var coordelem = (0, utilities_js_1.createDOM)("\n            <span class=\"upload-coord\">Test2</span>\n         ");
        idelem.textContent = workout.id;
        coordelem.textContent = workout.locationRecord[0].lat + ", " + workout.locationRecord[0].lng;
        this.uploadLog.appendChild(idelem);
        this.uploadLog.appendChild(coordelem);
    };
    HomeView.prototype.refreshUploads = function () {
        var _this = this;
        services_js_1.Service.getUploads().then(function (result) {
            var copy = result;
            _this.clearUploads();
            for (var _i = 0, copy_1 = copy; _i < copy_1.length; _i++) {
                var e = copy_1[_i];
                console.log(e);
                _this.addUpload(e);
            }
        });
    };
    HomeView.prototype.clearUploads = function () {
        var _a;
        var arr = this.uploadLog.querySelectorAll(".upload-id, .upload-coord");
        for (var _i = 0, arr_1 = arr; _i < arr_1.length; _i++) {
            var i = arr_1[_i];
            console.log(arr);
            console.log("removed");
            (_a = i.parentNode) === null || _a === void 0 ? void 0 : _a.removeChild(i);
        }
    };
    return HomeView;
}());
exports.HomeView = HomeView;
var LeaderboardView = /** @class */ (function () {
    function LeaderboardView() {
        this.elem = (0, utilities_js_1.createDOM)("\n            <div id = \"page-view\">\n                <div class = \"content\">\n                    <h1>Leaderboard</h1>\n                    <div class=\"grid-container-1\" id=\"leaderboard\">\n                        <div class=\"grid-top\">\n                            <h2>User</h2>\n                        </div>\n                        <div class=\"grid-top\">\n                            <h2>Average speed</h2>\n                        </div>\n                        <div class=\"grid-top\">\n                            <h2>Total distance</h2>\n                        </div>\n                        <div class=\"grid-top\">\n                            <h2>Workout time</h2>\n                        </div>\n                        <div class=\"grid-top\">\n                            <h2>Elevation gain</h2>\n                        </div>\n                    </div> \n                <div class = \"page-control\">\n                </div>\n            </div>\n        ");
        this.leaderboardElem = this.elem.querySelector("#leaderboard");
    }
    LeaderboardView.prototype.addEntry = function (user, spd, distance, workoutTime, elevationGain) {
        var elems = [];
        elems[0] = (0, utilities_js_1.createDOM)("\n                <span class = \"leaderboard-entry-usr\"></span>\n        ");
        elems[1] = (0, utilities_js_1.createDOM)("\n                <span class = \"leaderboard-avg-spd\"></span>\n        ");
        elems[2] = (0, utilities_js_1.createDOM)("\n                <span class = \"leaderboard-total-dst\"></span>\n        ");
        elems[3] = (0, utilities_js_1.createDOM)("\n                <span class = \"leaderboard-total-time\"></span>\n        ");
        elems[4] = (0, utilities_js_1.createDOM)("\n                <span class = \"leaderboard-total-gain\"></span>\n        ");
        elems[0].textContent = user;
        elems[1].textContent = spd.toString();
        elems[2].textContent = distance.toString();
        //TODO fix date conversion.
        elems[3].textContent = workoutTime.toString();
        elems[4].textContent = elevationGain.toString();
        for (var i = 0; i < elems.length; i++) {
            this.leaderboardElem.appendChild(elems[i]);
        }
    };
    LeaderboardView.prototype.refreshLeaderboard = function () {
        var _this = this;
        services_js_1.Service.getLeaderboard().then(function (result) {
            var copy = result;
            _this.clearLeaderBoard();
            for (var _i = 0, copy_2 = copy; _i < copy_2.length; _i++) {
                var e = copy_2[_i];
                _this.addEntry(e.user, e.avgspd, e.dist, e.time, e.gain);
            }
        });
    };
    LeaderboardView.prototype.clearLeaderBoard = function () {
        var _a;
        var arr = this.leaderboardElem.querySelectorAll(".leaderboard-entry-usr, .leaderboard-avg-spd, .leaderboard-total-dst, .leaderboard-total-time, .leaderboard-total-gain");
        for (var _i = 0, arr_2 = arr; _i < arr_2.length; _i++) {
            var i = arr_2[_i];
            console.log(arr);
            console.log("removed");
            (_a = i.parentNode) === null || _a === void 0 ? void 0 : _a.removeChild(i);
        }
    };
    return LeaderboardView;
}());
exports.LeaderboardView = LeaderboardView;
var RegisterView = /** @class */ (function () {
    function RegisterView() {
        var _this = this;
        this.elem = (0, utilities_js_1.createDOM)("\n            <div id = \"page-view\">\n                <div class = \"content\">\n                    <h1>Register your DE1 here!</h1>\n                <div class = \"page-control\">\n                    <input type = \"text\" id=\"serial\">\n                    <button id=\"serial-submit\">Submit serial</button>\n                </div>\n                <span id = \"serial-submit-result\">Result here</span>\n            </div>\n        ");
        this.de1Serial = this.elem.querySelector("#serial");
        this.de1SerialSubmit = this.elem.querySelector("#serial-submit");
        this.resultMessage = this.elem.querySelector("#serial-submit-result");
        this.de1SerialSubmit.addEventListener("click", function () {
            var serial = _this.de1Serial.value;
            // //send the serial via xmlhttpreq to server
            _this.de1Serial.value = "";
        });
    }
    return RegisterView;
}());
exports.RegisterView = RegisterView;
var LoginView = /** @class */ (function () {
    function LoginView() {
        var _this = this;
        this.elem = (0, utilities_js_1.createDOM)("\n            <div id = \"page-view\">\n                <div class = \"content\">\n                    <h1>Register your DE1 here!</h1>\n                <div class = \"page-control\">\n                    Username\n                    <input type = \"text\" id=\"username\">\n                    Password\n                    <input type = \"password\" id=\"password\">\n                    <button id=\"login-submit\">Login</button>\n                </div>\n                <span id = \"login-result\">Result here</span>\n            </div>\n        ");
        this.username = this.elem.querySelector("#username");
        this.password = this.elem.querySelector("#password");
        this.loginSubmit = this.elem.querySelector("#login-submit");
        this.resultMessage = this.elem.querySelector("#login-result");
        this.loginSubmit.addEventListener("click", function () {
            var username = _this.username.value;
            var password = _this.password.value;
            // //send the serial via xmlhttpreq to server
            _this.username.value = "";
            _this.password.value = "";
        });
    }
    return LoginView;
}());
exports.LoginView = LoginView;
