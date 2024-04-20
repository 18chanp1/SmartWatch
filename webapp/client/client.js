"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
var utilities_js_1 = require("./utilities.js");
var views_js_1 = require("./views.js");
//dynamic rendering
window.addEventListener("load", main);
function main() {
    //initialize the views
    var homeView = new views_js_1.HomeView();
    var leaderboardView = new views_js_1.LeaderboardView();
    var registerView = new views_js_1.RegisterView();
    var loginView = new views_js_1.LoginView();
    //re-render the window when user clicks on links
    window.addEventListener("popstate", renderRoute);
    renderRoute();
    //function to render the appropriate route
    function renderRoute() {
        var hash = window.location.hash;
        var pageView = document.getElementById("page-view");
        if (hash == "#/" || hash == "") {
            (0, utilities_js_1.emptyDOM)(pageView);
            pageView.appendChild(homeView.elem);
            homeView.addWorkout({
                id: "test",
                completed: true,
                locationRecord: [{ lat: 32, lng: 23 }],
                workoutDistance: 32,
                workoutDate: new Date("Feb 9"),
                workoutTime: 32
            });
            homeView.refreshUploads();
            // homeView.addUpload({
            //     id: "01",
            //     completed: false,
            //     locationRecord: [{lat:0, long:0}],
            //     workoutDistance: -1,
            //     workoutDate: new Date("2020 Feb 14"),
            //     workoutTime: -1
            // })
        }
        else if (hash == "#/register") {
            (0, utilities_js_1.emptyDOM)(pageView);
            pageView.appendChild(registerView.elem);
        }
        else if (hash == "#/leaderboard") {
            (0, utilities_js_1.emptyDOM)(pageView);
            pageView.appendChild(leaderboardView.elem);
            leaderboardView.refreshLeaderboard();
            //leaderboardView.addEntry("john", 69, 32, new Date(30), 30);
        }
        else if (hash == "#/login") {
            (0, utilities_js_1.emptyDOM)(pageView);
            pageView.appendChild(loginView.elem);
        }
        //and other cases
    }
}
