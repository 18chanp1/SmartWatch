import {emptyDOM, createDOM} from "./utilities.js";
import {HomeView, LeaderboardView, RegisterView, LoginView, UserView} from "./views.js";
import { Service } from "./services.js";



//dynamic rendering
window.addEventListener("load", main);


function main()
{
    //initialize the views
    let homeView = new HomeView();
    let leaderboardView = new LeaderboardView();
    let registerView = new RegisterView();
    let loginView = new LoginView();
    let userView = new UserView();

    //re-render the window when user clicks on links
    window.addEventListener("popstate", renderRoute);
    renderRoute();

    //function to render the appropriate route
    function renderRoute() {
        let hash = window.location.hash;
        let pageView = document.getElementById("page-view");

        if (hash == "#/" || hash == "")
        {
            emptyDOM(pageView);
            pageView!.appendChild(homeView.elem!);

            // homeView.addWorkout({
            //     id: "test",
            //     completed: true,
            //     locationRecord: [{lat: 32, lng: 23}],
            //     workoutDistance: 32,
            //     workoutDate: new Date("Feb 9"),
            //     workoutTime: 32,
            //     nearestTown: "Nigeria",
            // });

            homeView.refreshUploads();
            homeView.refreshStats();
            homeView.refreshWorkout();
            
        }
        
        else if (hash == "#/register")
        {
            emptyDOM(pageView);
            pageView!.appendChild(registerView.elem!);
        }

        else if(hash == "#/leaderboard")
        {
            emptyDOM(pageView);
            pageView!.appendChild(leaderboardView.elem!);
            leaderboardView.refreshLeaderboard();
            
            //leaderboardView.addEntry("john", 69, 32, new Date(30), 30);
        }

        else if(hash == "#/login")
        {
            emptyDOM(pageView);
            pageView!.appendChild(loginView.elem!);
        }

        else if (hash.slice(0,6) == "#/user")
        {
            emptyDOM(pageView);
            pageView?.appendChild(userView.elem);
            userView.refreshStats();
        }

        else if (hash = "#/logout")
        {
            Service.logout().then((success) =>
            {
                window.location.replace(window.location.origin);
            }, (err) =>
            {
                alert("Not logged out!");
                history.back();
            })
        }

        //and other cases

    }
}

