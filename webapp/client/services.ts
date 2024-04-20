let Service = 
{
    getLeaderboard: () => {
        let request = new Promise((resolve, reject)=> {
            let xhr = new XMLHttpRequest();
            xhr.open("GET", window.location.origin + "/leaderboard");
            xhr.send(null);

            xhr.timeout = 2000;
            xhr.ontimeout = ()=>
            {
                reject( new Error("timed out"));
            }

            xhr.onload = () => 
            {
                if(xhr.status == 200)
                {
                    resolve(JSON.parse(xhr.response));
                } 
                else
                {
                    reject( new Error(xhr.response));
                }
            }

            xhr.onerror = (err) =>
            {
                reject(err);
            }
        });

        return request

        
    },

    getUploads: () => {
        let request = new Promise((resolve, reject)=> {
            let xhr = new XMLHttpRequest();
            xhr.open("GET", window.location.origin + "/uploads");
            xhr.send(null);

            xhr.timeout = 2000;
            xhr.ontimeout = ()=>
            {
                reject( new Error("timed out"));
            }

            xhr.onload = () => 
            {
                if(xhr.status == 200)
                {
                    resolve(JSON.parse(xhr.response));
                } 
                else
                {
                    reject( new Error(xhr.response));
                }
            }

            xhr.onerror = (err) =>
            {
                reject(err);
            }
        });

        return request;
    },


    //temp mod to test map generator
    submitSerial: (submittedSerial : string) => 
    {
        let request = new Promise((resolve, reject) => {
            let xhr = new XMLHttpRequest();

            xhr.open("POST", window.location.origin + "/submitserial", true);
            xhr.setRequestHeader("Content-type", "application/json");


            /**
             * Test code for debugging
             */
            // let splitted = submittedSerial.split(",")
            // let coord = JSON.stringify({latitude: parseFloat(splitted[0]), longitude: parseFloat(splitted[1])})

            // let testWorkout = 
            // {
            //     id: "1",
            //     de1Serial:"365",
            //     workoutDistance: 35,
            //     workoutTime: 120
            // }

            //xhr.send(JSON.stringify(testWorkout));

            xhr.send(JSON.stringify({serial: submittedSerial}));


            xhr.timeout = 2000;
            xhr.ontimeout = () => 
            {
                reject(new Error("Timed out"));
            }

            xhr.onload = () =>
            {
                if(xhr.status == 200)
                {
                    resolve(xhr.response);
                }
                else 
                {
                    reject(new Error(xhr.response));
                }
            }

            xhr.onerror = (err) =>
            {
                reject(err);
            }

        });

        return request;
    },

    getSerial: () => {
        let request = new Promise((resolve, reject)=> {
            let xhr = new XMLHttpRequest();
            xhr.open("GET", window.location.origin + "/serial");
            xhr.send(null);

            xhr.timeout = 2000;
            xhr.ontimeout = ()=>
            {
                reject( new Error("timed out"));
            }

            xhr.onload = () => 
            {
                if(xhr.status == 200)
                {
                    resolve(xhr.response);
                } 
                else
                {
                    reject( new Error(xhr.response));
                }
            }

            xhr.onerror = (err) =>
            {
                reject(err);
            }
        });

        return request;
    },
    getStats: () => 
    {
        let request = new Promise((resolve, reject)=> {
            let xhr = new XMLHttpRequest();
            xhr.open("GET", window.location.origin + "/stats");
            xhr.send(null);

            xhr.timeout = 2000;
            xhr.ontimeout = ()=>
            {
                reject( new Error("timed out"));
            }

            xhr.onload = () => 
            {
                if(xhr.status == 200)
                {
                    resolve(JSON.parse(xhr.response));
                } 
                else
                {
                    reject( new Error(xhr.response));
                }
            }

            xhr.onerror = (err) =>
            {
                reject(err);
            }
        });

        return request

        
    },

    getWorkouts: () => 
    {
        let request = new Promise((resolve, reject)=> {
            let xhr = new XMLHttpRequest();
            xhr.open("GET", window.location.origin + "/workouts");
            xhr.send(null);

            xhr.timeout = 2000;
            xhr.ontimeout = ()=>
            {
                reject( new Error("timed out"));
            }

            xhr.onload = () => 
            {
                if(xhr.status == 200)
                {
                    resolve(JSON.parse(xhr.response));
                } 
                else
                {
                    reject( new Error(xhr.response));
                }
            }

            xhr.onerror = (err) =>
            {
                reject(err);
            }
        });

        return request

        
    },
    getPublicWorkouts: (usr) => 
    {
        let request = new Promise((resolve, reject)=> {
            let xhr = new XMLHttpRequest();
            let url = new URL(window.location.origin + "/publicworkouts");

            url.searchParams.set("username", usr)

            xhr.open("GET", url);
            xhr.send(null);

            xhr.timeout = 2000;
            xhr.ontimeout = ()=>
            {
                reject( new Error("timed out"));
            }

            xhr.onload = () => 
            {
                if(xhr.status == 200)
                {
                    resolve(JSON.parse(xhr.response));
                } 
                else
                {
                    reject( new Error(xhr.response));
                }
            }

            xhr.onerror = (err) =>
            {
                reject(err);
            }
        });

        return request;  
    },

    logout: () =>
    {
        let request = new Promise((resolve, reject)=> {
            let xhr = new XMLHttpRequest();
            let url = new URL(window.location.origin + "/logout");
            
            xhr.open("POST", url);
            xhr.send(null);

            xhr.timeout = 2000;
            xhr.ontimeout = ()=>
            {
                reject( new Error("timed out"));
            }

            xhr.onload = () => 
            {
                if(xhr.status == 200)
                {
                    resolve(xhr.response);
                } 
                else
                {
                    reject( new Error(xhr.response));
                }
            }

            xhr.onerror = (err) =>
            {
                reject(err);
            }
        });

        return request;
    },

    changePerms: (id: string, changeToPublic: boolean) => 
    {
        let request = new Promise((resolve, reject)=> {
            let xhr = new XMLHttpRequest();
            let url = new URL(window.location.origin + "/changePerms");

            url.searchParams.set("id", id);
            url.searchParams.set("changeToPublic", changeToPublic.toString());

            xhr.open("POST", url);
            xhr.send(null);

            xhr.timeout = 2000;
            xhr.ontimeout = ()=>
            {
                reject( new Error("timed out"));
            }

            xhr.onload = () => 
            {
                if(xhr.status == 200 || xhr.status == 304)
                {
                    resolve(xhr);
                } 
                else
                {
                    reject( new Error(xhr.response));
                }
            }

            xhr.onerror = (err) =>
            {
                reject(err);
            }
        });

        return request;  
    },
    deleteWorkout: (id: string) =>
    {
        let request = new Promise((resolve, reject)=> {
            let xhr = new XMLHttpRequest();
            let url = new URL(window.location.origin + "/deleteWorkout");

            url.searchParams.set("id", id);

            xhr.open("POST", url);
            xhr.send(null);

            xhr.timeout = 2000;
            xhr.ontimeout = ()=>
            {
                reject( new Error("timed out"));
            }

            xhr.onload = () => 
            {
                if(xhr.status == 200 || xhr.status == 304)
                {
                    resolve(xhr);
                } 
                else
                {
                    reject( new Error(xhr.response));
                }
            }

            xhr.onerror = (err) =>
            {
                reject(err);
            }
        });

        return request; 

    }

    
}

export {Service};