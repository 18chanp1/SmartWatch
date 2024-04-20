import {emptyDOM, createDOM} from "./utilities.js";
import {Workout} from "./workout.js"
import {Service} from "./services.js"
import { LeaderboardEntry, LeaderboardObject } from "./leaderboard.js";

//mapping stuff

class HomeView 
{
    elem: HTMLElement| null;
    totalDistanceElem: HTMLInputElement;
    averageSpeedElem: HTMLElement;
    workoutTimeElem: HTMLElement;
    workoutLog: HTMLElement;
    uploadLog: HTMLElement;

    constructor() 
    {
        this.elem = <HTMLElement> createDOM
        (`
        <div id = "page-view">
        <div class = "content">
            <h1>Your workout statistics</h1>
            <div class="grid-container">
                <div class="grid-item" id = total-distance>
                    <h2>Total Distance run </h2>
                    <span id = total-distance-val>100</span> m
                </div>
                <div class="grid-item" id = avg-spd>
                    <h2>Average speed</h2>
                    <span id = avg-spd-val>100</span> m/s
                </div>
                <div class="grid-item" id = workout-time>
                    <h2>Workout time</h2>
                    <span id = workout-time-val>100</span> seconds
                </div>
                <div class="grid-item" id = elevation-gain>
                    <h2>Reserved for future features</h2>
                    <span id = elevation-gain-val></span> Reserved
                </div>
                
            </div> 

            <h1>Your workouts history</h1>
            <div class = "grid-container-2" id = "workout-log">
                <div class = "grid-item">
                    <H2>Workout ID</H2>
                </div>
                <div class = "grid-item">
                    <H2>Workout Distance (M)</H2>
                </div>
                <div class = "grid-item">
                    <H2>Workout Date</H2>
                </div>
                <div class = "grid-item">
                    <h2>Workout Time (seconds) </h2>
                </div>

                <div class="grid-item">
                    <h2>Make Public</h2>
                </div>
            </div>

            <h1>Your workouts uploads</h1>
            <div class = "grid-container-3" id = "workout-log">
                <div class = "grid-item">
                    <H2>Workout ID</H2>
                </div>
                <div class = "grid-item">
                    <H2>Location</H2>
                </div>
                <div class="grid-item">
                    <h2>Delete Upload</h2>
                </div>
                </div>
            </div>
          <div class = "page-control">
          </div>
        </div>
        `)

        this.totalDistanceElem =(<HTMLElement> this.elem).querySelector("#total-distance-val")!;
        this.averageSpeedElem = (<HTMLElement> this.elem).querySelector("#avg-spd-val")!;
        this.workoutTimeElem = (<HTMLElement> this.elem).querySelector("#workout-time-val")!;
        this.workoutLog = (<HTMLElement> this.elem).querySelector("#workout-log")!;
        this.uploadLog = (<HTMLElement> this.elem).querySelector(".grid-container-3")!;
        

    }

    addWorkout(workout: Workout)
    {
        let elems : HTMLInputElement [] = [];
        elems[0] = <HTMLInputElement> createDOM
        (`
                <a class = "workout-log-id"></span>
        `);

        elems[1] = <HTMLInputElement> createDOM
        (`
                <span class = "workout-log-dist"></span>
        `);

        elems[2] = <HTMLInputElement> createDOM
        (`
                <span class = "workout-log-date"></span>
        `);

        elems[3] = <HTMLInputElement> createDOM
        (`
                <span class = "workout-log-time"></span>
        `);

        elems[4] = <HTMLInputElement> createDOM
        (`
            <button class = "change-perms mybutton"></button>
        `)


        elems[0].textContent = workout.id;
        
        elems[0].setAttribute("href", "")

        let url = new URL(window.location.origin + "/routedisplay");
        url.searchParams.set("workoutID", workout.id);
        url.searchParams.set("type", "workout");

        let date = new Date(workout.workoutDate);
        let datestring = date.getFullYear() + "." + date.getMonth() + "." + date.getDate() + " "
        + (date.getHours() < 10 ? date.getHours() + "0": date.getHours())  + ":" + (date.getMinutes() < 10 ? "0" + date.getMinutes() : date.getMinutes());

        elems[0].setAttribute("href", url.toString()); 

        elems[1].textContent = workout.workoutDistance.toFixed(2).toString();
        elems[2].textContent = datestring;
        elems[3].textContent = workout.workoutTime.toFixed(2).toString();

        elems[4].textContent = workout.isPublic ? "Make Private" : "Make Public";
        
        elems[4].addEventListener("click", (ev) =>
        {
            Service.changePerms(workout.id, !workout.isPublic).then((response) =>
            {
                let resp = <XMLHttpRequest> response;
                if(resp.status == 200)
                {
                    elems[4].textContent = !workout.isPublic ? "Make Private" : "Make Public";
                    workout.isPublic = !workout.isPublic;
                }
            }, (err) => 
            {
                window.alert("Please try changing the permissions later");
            })
        })

        for(let i = 0; i < elems.length; i++)
        {
            this.workoutLog.appendChild(elems[i]);
        } 
    }

    clearWorkout()
    {
        let arr = this.workoutLog.querySelectorAll(`.workout-log-id, .workout-log-dist, .workout-log-date, .workout-log-time, .change-perms`);
        console.error(arr);

        for (const i of arr)
        {
            i.parentNode?.removeChild(i);
        }

    }

    refreshWorkout()
    {
        this.clearWorkout();
        Service.getWorkouts().then((workouts)=>
        {
            for(let e of <Workout[]> workouts)
            {
                this.addWorkout(e);
            }
        });
    }

    addUpload(workout: Workout)
    {
        let idelem = <HTMLInputElement> createDOM(`
            <a class = "upload-id">Test</span>
        `);

        let coordelem = <HTMLInputElement> createDOM(`
            <span class="upload-coord">Test2</span>
         `);

        let deleteButton = <HTMLInputElement> createDOM(`
        <button class = "change-perms mybutton">Delete workout</button>
        `);
        
        idelem.textContent = workout.id;

        let url = new URL(window.location.origin + "/routedisplay");
        url.searchParams.set("workoutID", workout.id);
        url.searchParams.set("type", "upload");

        idelem.setAttribute("href", url.toString()); 

        deleteButton.addEventListener("click", (event) =>
        {
            Service.deleteWorkout(workout.id).then((success) => 
            {
                idelem.parentNode?.removeChild(idelem);
                coordelem.parentNode?.removeChild(coordelem);
                deleteButton.parentNode?.removeChild(deleteButton);

                //update corresponding ids
                let list = this.uploadLog.querySelectorAll(".upload-id")

                for(let p of list)
                {
                    if(parseInt(<string> (p.textContent)) >= parseInt(workout.id))
                    {
                        p.textContent = (parseInt(<string> p.textContent) - 1 ).toString();
                    }
                }

            }, (err) =>
            {
                console.error(err);
                window.alert("Could not delete, please try again later");
            })
        })


        
        coordelem.textContent = workout.nearestTown;

        this.uploadLog.appendChild(idelem);
        this.uploadLog.appendChild(coordelem);
        this.uploadLog.appendChild(deleteButton);

    }

    refreshUploads()
    {
        Service.getUploads().then((result) =>
        {
            let copy : Workout[] = <Workout[]> result
            this.clearUploads();
            for(let e of copy){
                this.addUpload(e);
            }
        }) 
    }

    clearUploads()
    {
        let arr = this.uploadLog.querySelectorAll(`.upload-id, .upload-coord, .change-perms`);

        for (const i of arr)
        {
            i.parentNode?.removeChild(i);
        }

    }

    refreshStats()
    {
        Service.getStats().then((result) =>
        {
            this.totalDistanceElem.textContent = (<LeaderboardEntry>result).dist.toFixed(2).toString();
            this.averageSpeedElem.textContent = (<LeaderboardEntry>result).avgspd.toFixed(2).toString();
            this.workoutTimeElem.textContent = (<LeaderboardEntry>result).time.toFixed(2).toString();

        }, (err)=>
        {
            console.error("Could not get stats");
        })
    }

}

class LeaderboardView
{
    elem: ChildNode;
    leaderboardElem: HTMLElement;

    constructor()
    {
        this.elem = createDOM
        (`
            <div id = "page-view">
                <div class = "content">
                    <h1>Leaderboard</h1>
                    <div class="grid-container-1" id="leaderboard">
                        <div class="grid-top">
                            <h2>User</h2>
                        </div>
                        <div class="grid-top">
                            <h2>Average speed (m/s) </h2>
                        </div>
                        <div class="grid-top">
                            <h2>Total distance (m) </h2>
                        </div>
                        <div class="grid-top">
                            <h2>Workout time (hours)</h2>
                        </div>
                    </div> 
                <div class = "page-control">
                </div>
            </div>
        `)!

        this.leaderboardElem = (<HTMLElement>this.elem).querySelector("#leaderboard")!;
        
    }

    addEntry(user: string, spd: number, distance:number, workoutTime:number)
    {
        let elems : HTMLInputElement [] = [];
        elems[0] = <HTMLInputElement> createDOM
        (`
                <a class = "leaderboard-entry-usr"></span>
        `);

        elems[1] = <HTMLInputElement> createDOM
        (`
                <span class = "leaderboard-avg-spd"></span>
        `);

        elems[2] = <HTMLInputElement> createDOM
        (`
                <span class = "leaderboard-total-dst"></span>
        `);

        elems[3] = <HTMLInputElement> createDOM
        (`
                <span class = "leaderboard-total-time"></span>
        `);



        elems[0].textContent = user;
        elems[0].setAttribute("href", "/#/user/" + user);
        elems[1].textContent = spd.toFixed(2).toString();
        elems[2].textContent = distance.toFixed(2).toString();
        //TODO fix date conversion.
        elems[3].textContent = workoutTime.toFixed(2).toString();


        for(let i = 0; i < elems.length; i++)
        {
            this.leaderboardElem.appendChild(elems[i]);
        }
    }

    refreshLeaderboard()
    {
        Service.getLeaderboard().then((result) =>
        {
            let copy : LeaderboardEntry[] = <LeaderboardEntry[]> result
            this.clearLeaderBoard();
            for(let e of copy){
                this.addEntry(e.user, e.avgspd, e.dist, e.time);
            }
        })
    }

    clearLeaderBoard()
    {
        let arr = this.leaderboardElem.querySelectorAll(`.leaderboard-entry-usr, .leaderboard-avg-spd, .leaderboard-total-dst, .leaderboard-total-time, .leaderboard-total-gain`);

        for (const i of arr)
        {
            i.parentNode?.removeChild(i);
        }

    }

  
    

    //TODO fetch from server DB

    


}

class RegisterView
{
    elem: ChildNode| null;
    de1Serial: HTMLInputElement;
    de1SerialSubmit: HTMLElement;
    resultMessage: HTMLInputElement;


    constructor() 
    {
        this.elem = createDOM
        (`
            <div id = "page-view">
                <div class = "content">
                    <h1>Register your DE1 here!</h1>
                <div class = "page-control">
                    <input type = "text" id="serial">
                    <br>
                    <button id="serial-submit" class = "mybutton">Submit serial</button>
                </div>

                <h2>Your registered DE1 </h2>
                <span id = "serial-submit-result"></span>
            </div>
        `)

        this.de1Serial = (<HTMLInputElement> this.elem).querySelector("#serial")!;
        this.de1SerialSubmit = (<HTMLElement> this.elem).querySelector("#serial-submit")!;
        this.resultMessage = (<HTMLInputElement> this.elem).querySelector("#serial-submit-result")!;
        
        this.getSerial()

        this.de1SerialSubmit.addEventListener("click", () => {
            let serial : string = this.de1Serial.value;
            // //send the serial via xmlhttpreq to server

            

            Service.submitSerial(serial).then(
                (result) => {
                    this.getSerial();
                },
                (reject) => {
                    this.resultMessage.textContent = reject;
                });

            this.de1Serial.value = "";

        });

    }

    getSerial()
    {
        Service.getSerial().then(
            (result) => 
            {
                this.resultMessage.textContent = <string> result;
            },
            (err) => 
            {
                this.resultMessage.textContent = `You have not registered your DE1
                or there has been an unfortunate error.`
            }
        )
    }
}

class LoginView
{
    elem: ChildNode| null;
    username: HTMLInputElement;
    password: HTMLInputElement;
    loginSubmit: HTMLElement;
    resultMessage: HTMLElement;


    constructor() 
    {
        this.elem = createDOM
        (`
            <div id = "page-view">
                <div class = "content">
                    <h1>Register your DE1 here!</h1>
                <div class = "page-control">
                    Username
                    <input type = "text" id="username">
                    Password
                    <input type = "password" id="password">
                    <button id="login-submit">Login</button>
                </div>
                <span id = "login-result">Result here</span>
            </div>
        `)

        this.username = (<HTMLInputElement> this.elem).querySelector("#username")!;
        this.password = (<HTMLInputElement> this.elem).querySelector("#password")!;
        this.loginSubmit = (<HTMLElement> this.elem).querySelector("#login-submit")!;
        this.resultMessage = (<HTMLElement> this.elem).querySelector("#login-result")!;
        
        this.loginSubmit.addEventListener("click", () => {
            let username : String = this.username.value;
            let password: String = this.password.value;
            // //send the serial via xmlhttpreq to server
            this.username.value = "";
            this.password.value = "";
        });

    }
}

class UserView
{
    elem: HTMLInputElement;
    avgSpdElem: HTMLInputElement;
    totalDistElem: HTMLInputElement;
    workoutTimeElem: HTMLInputElement;
    usernameElem: HTMLInputElement;
    workoutLog: HTMLInputElement;

    constructor() 
    {
        this.elem = <HTMLInputElement> createDOM
        (`
        <div id = "page-view">
            <div class = "content">
                <H1 class = "username">Test User</H1>
                <div class = "stats-grid">
                    <h2 class="title">Average Speed</h2> 
                    <h2 class="title">Total distance</h2> 
                    <h2 class="title">Workout time</h2>
                    <p><span id="avg-spd"></span> m/s</p>
                    <p><span id="total-dist"></span> m</p>
                    <p><span id="workout-time"></span> Seconds</p>
                </div>
                <H2>Shared workouts </h2>
                <div id = "shared-workouts">
                    <h3>Workout ID</h3>
                    <h3>Location</h3>
                </div>
            </div>
        </div>
        `)

        this.usernameElem = <HTMLInputElement> this.elem.querySelector(".username");
        this.avgSpdElem = <HTMLInputElement> this.elem.querySelector("#avg-spd");
        this.totalDistElem = <HTMLInputElement> this.elem.querySelector("#total-dist");
        this.workoutTimeElem = <HTMLInputElement> this.elem.querySelector("#workout-time");
        this.workoutLog = <HTMLInputElement> this.elem.querySelector("#shared-workouts");
    }

    refreshStats()
    {
        Service.getLeaderboard().then((leaderboard) =>
        {
            let username = window.location.hash.slice(7);
            let result = (<Array<LeaderboardEntry>> leaderboard).find(item => item.user = username);

            this.usernameElem.textContent = "About " + username;
            this.totalDistElem.textContent = (<LeaderboardEntry>result).dist.toFixed(2).toString();
            this.avgSpdElem.textContent = (<LeaderboardEntry>result).avgspd.toFixed(2).toString();
            this.workoutTimeElem.textContent = (<LeaderboardEntry>result).time.toFixed(2).toString();
        }, (err) =>
        {
            console.log(err);
        })

        Service.getPublicWorkouts(window.location.hash.slice(7)).then((workouts) =>
        {
            this.clearWorkout();

            for (let workout of <Workout[]> workouts)
            {
                this.addWorkout(workout);
            }

        }, (err)=>
        {
            console.log(err);
        })
    }

    addWorkout(workout: Workout)
    {
        let idelem = <HTMLInputElement> createDOM(`
            <a class = "workout-id">Test</span>
        `);

        let coordelem = <HTMLInputElement> createDOM(`
            <span class="upload-coord">Test2</span>
        `);
        
        let url = new URL(window.location.origin + "/routedisplay");
        url.searchParams.set("workoutID", workout.id);
        url.searchParams.set("type", "workout");

        idelem.setAttribute("href", url.toString()); 
        idelem.textContent = workout.id;
        coordelem.textContent = workout.nearestTown;

        this.workoutLog.appendChild(idelem);

        this.workoutLog.appendChild(coordelem);
    }

    clearWorkout()
    {
        let arr = this.workoutLog.querySelectorAll(`.workout-id, .upload-coord`);
        console.error(arr);

        for (const i of arr)
        {
            i.parentNode?.removeChild(i);
        }
    }
}


export {HomeView, LeaderboardView, RegisterView, LoginView, UserView}