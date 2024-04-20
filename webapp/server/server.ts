import path from "path"
import express from "express";
import fitnessUser from "./user.js";
import https from "https"

import crypto from "crypto"
import nearbyCities from "nearby-cities/index.js"
import countryCode from "country-code-lookup"

import StaticMap from "staticmaps"
import {PNG} from "pngjs"
import fs from "fs"

import Database from "./Database.js"
//let userDataDb = new Database();
let usersDb = new Database();

await usersDb.connect("mongodb://127.0.0.1:27017", "cpen391-project","users");

//replace dirname since now it is module
const __dirname = path.resolve();

const app = express();
const port = 3000;

//create instance of SessionManager
import {SessionManager, isCorrectPassword, createSaltedHash} from "./sessionManager.js"
let session = new SessionManager();

app.use(express.json());
app.use(express.urlencoded({extended: true}));
app.use(logRequest);

/**
 * Middleware that blocks non https requests. 
 */
app.use((req, res, next) => {

	try
	{
		if((req.originalUrl == "/requestImage") || req.originalUrl == "/receiveWorkout"|| (req.secure))
		{
			next();
		} 
		else
		{
			res.redirect('https://' + req.headers.host + req.url)
		}
	}
	catch(e){

		if(!res.headersSent)
		{
			res.redirect('https://' + req.headers.host + req.url)
		}
		
	}

	
})

/**
 * HTTPS server creation
 */
https
https.createServer({
	key: fs.readFileSync(__dirname + "/certs/cert.key"),
	cert: fs.readFileSync(__dirname + "/certs/cert.pem"),
},app).listen(443, () => 
{
	console.log("Server active @ port 443");
})



import { LeaderboardEntry } from "../server/leaderboard";
import { Coordinates, Workout, simplifiedWorkout } from "./workout.js";

//serve static files to client
const clientApp = path.join(__dirname, "compiled", "client");



/**
 * Test Endpoint 2
 */

/* Getting route given a workout ID*/
app.post('/requestImage', async function(req, res){

	/**
	 * Error checking
	 */

	


	var data = req.body;
    var id = (<any>data).id;
	console.log(id);
	id = id.toString(10);
    var collection = await usersDb.readAll();
    var found = false;
    var foundWorkOut;

	if(data.id == undefined){
		res.status(400).send;
		return;
	}

    for (var doc of <any>collection){
		// console.log(doc)
        var worksOut = doc.uploads.find(element => element.id == id);
        if (worksOut != undefined){
            found = true;
            foundWorkOut = worksOut
            break;
        }
    }
	// console.log(found);
    if (!found){
        res.status(400).send("workout id not found");
    }
    else {
		// console.log(":Found");
        var coor: number[][] = [];
        for (var latlong of foundWorkOut.locationRecord){
			let coord  = [<number> latlong.lng, <number> latlong.lat]
            coor.push(coord)
        }
        coor.push(coor[0])
        // console.log(coor);
        var map = new StaticMap({ width: 120, height: 200 });
        var imgLink = "image.png";
        const polygon = {
            coords: coor,
            color: '#0000FF',
            // width: 3
        };
        (<any>map).addPolygon(polygon);
        console.log("RENDERING ...");
        await (<any>map).render(12);
        await (<any>map).image.save(imgLink);
        console.log("DONE RENDERING");
        var arrayPixel = getPixels("image.png");
        res.status(200);
        res.send(JSON.stringify(arrayPixel));
    }
});


//endpoint for accepting workouts from de1
app.post("/receiveWorkout", (req, res) => 
{
	try
	{
		let incomingWorkout: simplifiedWorkout = req.body;
		if(incomingWorkout.de1Serial == undefined || 
			incomingWorkout.workoutDistance == undefined || 
			incomingWorkout.workoutTime == undefined)
		{
			res.status(400).send();
			return;
		}
		
		//find database entry with corresponding de1 serial number
		usersDb.read({de1serial: incomingWorkout.de1Serial.toString()}).then((document) => {
			if(document == null)
			{
				res.status(404).send("No one has registered this serial number");
				console.log("unknown serial");
				return;
			}
			
			let user : fitnessUser = <any> document
			let copyWorkouts = user.uploads

			if(copyWorkouts == undefined)
			{
				res.status(400).send("Failed to copy workouts");
				return;
			}
			for(let workout of copyWorkouts)
			{
				if(workout.id == incomingWorkout.id)
				{
					workout.completed = true;
					workout.workoutDate = new Date();
					workout.workoutDistance = incomingWorkout.workoutDistance;
					workout.workoutTime = incomingWorkout.workoutTime;

					usersDb.update({de1serial: incomingWorkout.de1Serial.toString()}, 
						{uploads: copyWorkouts}).then((result) => 
						{
							console.log("success")
							res.status(200).send("Successfully uploaded from de1.")
						}, (err) => 
						{
							console.log("failure")
							res.status(500).send("Failed to update database");
						});
					
					return;
				}
			}
		}, (err) => 
		{
			res.status(500).send("failed to search database for de1 serial");
		});
	} catch(e)
	{
		if(!res.headersSent)
		{
			res.status(400).send();
		}
	}

})

app.post("/logout", session.middleware, (req, res) => 
{
	try
	{
		session.deleteSession(req);
		res.status(200).send("Successfully logged out.");
	}
	catch(e){
		if(!res.headersSent)
		{
			res.status(400).send();
		}
		
	}
	

});

app.post("/deleteWorkout", session.middleware, (req, res) => 
{
	try{
		console.log("get delete req");
		let usr = (<any> req).username;
		let params = req.query;
		let workoutID = params.id;
		let currSer: number;

		if (usr == undefined || params == undefined || workoutID == undefined)
		{
			res.status(400).send();
			return;
		}


		usersDb.read({username:usr}).then((document) => 
		{
			return new Promise ((resolve, reject) => 
			{
				if(document == null)
				{
					res.status(404).send("Could not find user");
					reject(new Error("Could not find user"));
				}
				else 
				{
					let workoutsArr: Workout [] = (<fitnessUser> <unknown>document).uploads;
					let index = workoutsArr.findIndex(o => o.id == workoutID);
					currSer = (<fitnessUser> <unknown>document).currentSerial - 1;

					workoutsArr.splice(index, 1);

					for(let p = index; p < workoutsArr.length; p++)
					{
						workoutsArr[p].id = (parseInt(workoutsArr[p].id) - 1).toString();
					}
					resolve(workoutsArr);
				}	
			})
		}, (err)=> 
		{
			res.status(500).send(err);
		}).then((workoutArr) =>
		{
			return usersDb.update({username: usr}, {uploads: workoutArr, currentSerial: currSer});
		}, (err) => 
		{
			res.status(404).send(err)
		}).then((updatedDoc) => 
		{
			return new Promise ((resolve, reject) =>
			{
				if(updatedDoc == undefined)
				{
					reject(new Error("Could not update doc"))
				}
				else if((<any> updatedDoc).modifiedCount == 1)
				{
					res.status(200).send("Change permission OK")
				}

				else
				{
					res.status(304).send("Change permission OK");
				}

			})
			
		}, (err) => 
		{
			res.status(500).send("could not update db");
		});
	}
	catch(e){
		if(!res.headersSent)
		{
			res.status(400).send();
		}
	}
});


//endpoint for changing the permissions of the workout
app.post("/changePerms", session.middleware, (req, res) => 
{
	try{
		let usr = (<any> req).username;
		let params = req.query;
		let workoutID = params.id;
		let changeToPublic = <boolean> (params.changeToPublic === "true");
		let before: boolean;

		if (usr == undefined || params == undefined || workoutID == undefined || changeToPublic == undefined)
		{
			res.status(400).send();
			return;
		}

		usersDb.read({username:usr}).then((document) => 
		{
			return new Promise ((resolve, reject) => 
			{
				if(document == null)
				{
					res.status(404).send("Could not find user");
					reject(new Error("Could not find user"));
				}
				else 
				{
					let workoutsArr: Workout [] = (<fitnessUser> <unknown>document).uploads;
					let index = workoutsArr.findIndex(o => o.id == workoutID);

					if(index == -1)
					{
						reject(new Error("could not find that workout"));
					}
					before = workoutsArr[index].isPublic;
					workoutsArr[index].isPublic = changeToPublic;
					resolve(workoutsArr);
				}	
			})
		}, (err)=> 
		{
			res.status(500).send(err);
		}).then((workoutArr) =>
		{
			return usersDb.update({username: usr}, {uploads: workoutArr});
		}, (err) => 
		{
			res.status(404).send(err)
		}).then((updatedDoc) => 
		{
			return new Promise ((resolve, reject) =>
			{
				if(updatedDoc == undefined)
				{
					reject(new Error("Could not update doc"))
				}
				else if((<any> updatedDoc).modifiedCount == 1)
				{
					res.status(200).send("Change permission OK")
				}

				else
				{
					res.status(304).send("Change permission OK");
				}

			})
			
		}, (err) => 
		{
			res.status(500).send("could not update db");
		});
	}
	catch(e)
	{
		if(!res.headersSent)
		{
			res.status(400).send();
		}
	}
});


//endpoint for "stealing workouts" from public listed workouts
app.post("/steal", session.middleware, (req, res) => 
{
	try{

		
		let usr = (<any> req).username;
		let params = req.query;
		let workoutID = params.workoutID;
		let type = params.type;

		if(usr == undefined || params == undefined || workoutID == undefined || type == undefined)
		{
			res.status(400).send();
			return
		}

		//first check if workout ID already exists in current user
		usersDb.read({username: usr}).then((document) => {
			
			return new Promise((resolve, reject) =>
			{
				//check if workout exists
				let upload = (<fitnessUser> <unknown> document).uploads.find(o => o.id == workoutID)
				let workout = (<fitnessUser> <unknown> document).workouts.find(o => o.id == workoutID)

				if(upload == undefined && workout == undefined)
				{
					resolve((<fitnessUser> <unknown> document).uploads);
				}
				else 
				{
					reject("Entry already exists");
				}
			})
		}).then((uploads) => 
		{
			
			//add entry to uploads
			usersDb.readAll().then((docs) =>
			{
				let foundWorkout;
				mainloop:
				for (let user of (<fitnessUser[]> <unknown> docs))
				{
					for(let workout of user.workouts)
					{
						if(workoutID == workout.id)
						{
							foundWorkout = workout;
							break mainloop;
						}
					}
				}

				return new Promise((resolve, reject) =>
				{
					if (foundWorkout != null)
					{
						(<Workout[]>uploads).push(foundWorkout);
						resolve(uploads);
					} else
					{
						reject(new Error("Could not find workout with said ID"))
					}
				})

			}).then((updatedUpload) =>
			{
				//teapot
				usersDb.update({username: usr}, {uploads: updatedUpload});
				res.status(200).send();
			}).catch((err) =>
			{
				res.status(500).send("Server error");
			});
		}, (err) => 
		{
			res.status(409).send(err)
		});
	}
	catch(e)
	{
		if(!res.headersSent)
		{
			res.status(400).send();
		}
	}
});

//endpoint for getting workouts
app.get("/workouts", session.middleware, (req, res) => {

	try{

	
	let usr = (<any> req).username;

	usersDb.read({username: usr}).then((document) =>
	{
		let usrWorkouts = (<fitnessUser> <unknown>document).uploads; 

		let actualWorkouts = usrWorkouts.filter((workout) => {return workout.completed == true});

		res.status(200).send(JSON.stringify(actualWorkouts));
	}, (err) => 
	{
		res.status(500).send("Failed to access database");
	})
	} catch(e)
	{
		if(!res.headersSent)
		{
			res.status(400).send();
		}
	}
	
})

app.get("/publicworkouts", session.middleware, (req, res) => 
{
	try{
		let usr = req.query.username;

		usersDb.read({username: usr}).then((user) =>
		{
			if(user == null)
			{
				res.status(404).send("Could not find uploads");
				return;
			}

			let publicWorkouts = (<fitnessUser> <unknown> user).uploads.filter(workout => workout.isPublic && workout.completed);

			res.status(200).send(JSON.stringify(publicWorkouts));
			return;
		}, (error) =>
		{
			res.status(404).send("Could not find uploads");
			return;
		});
	}
	catch(e)
	{
		if(!res.headersSent)
		{
			res.status(400).send();
		}
	}


})

//open endpoint for getting stats
app.get("/stats", session.middleware, (req, res) => {
	try
	{

		let usr = (<any> req).username;

		usersDb.read({username: usr}).then((document) =>
		{
			let documents: fitnessUser[] = [];
			documents.push(<fitnessUser> <unknown> document);
			let LeaderboardObject:LeaderboardEntry[];

			LeaderboardObject = fitnessUser.computeStatistics(<fitnessUser[]> <unknown> documents);
			res.status(200).send(JSON.stringify(LeaderboardObject[0]));
		}, (err) => 
		{
			res.status(500).send("Failed to access database");
		})
	} catch(e)
	{
		if(!res.headersSent)
		{
			res.status(400).send();
		}
	}
	
})

//open endpoint for getting the leaderboard
app.get("/leaderboard", session.middleware, (req, res) => {
	try
	{
		usersDb.readAll().then((documents) =>
		{
			let LeaderboardObject:LeaderboardEntry[];

			LeaderboardObject = fitnessUser.computeStatistics(<fitnessUser[]> <unknown> documents);
			res.status(200).send(JSON.stringify(LeaderboardObject));
		}, (err) => 
		{
			res.status(500).send("Failed to access database");
		})
	}
	catch(e)
	{
		if(!res.headersSent)
		{
			res.status(400).send();
		}
	}

	
})

/**
 * Endpoint to receive workouts history
 */
app.get("/uploads", session.middleware, (req, res)=>{
	try{
		usersDb.read({username: (<any>req).username}).then((user) =>
		{
			if(user == null)
			{
				res.status(404).send("Could not find uploads");
				return;
			}

			let notDoneUploads = (<fitnessUser> <unknown>user).uploads.filter((workout) => {return !workout.completed})
			res.status(200).send(JSON.stringify(notDoneUploads));
			return;
		}, (error) =>
		{
			res.status(404).send("Could not find uploads");
			return;
		});
	} catch(e)
	{
		if(!res.headersSent)
		{
			res.status(400).send();
		}

	}

	
})

/**
 * Endpoint to send serial number 
 */
app.get("/serial", session.middleware, (req, res)=>{

	try{


		usersDb.read({username: (<any>req).username}).then((user) =>
		{
			if(user == null)
			{
				res.status(404).send("Could not find uploads");
				return;
			}
			res.status(200).send(JSON.stringify((<fitnessUser> <unknown>user).de1serial));
		}, (err) =>
		{
			res.status(404).send("Could not find serials");
			return;
		});
	} catch(e){
		if(!res.headersSent)
		{
			res.status(400).send();
		}
	}

})


/**
 *  Endpoint to receive uploaded route from map interface. 
 */
app.post("/routeupload", session.middleware, async (req, res) => {
	try{
		let user = (<any>req).username;

		if(req.body.route[0].lat == undefined || req.body.route[0].lng == undefined
			|| req.body.len == undefined)
		{
			res.status(400).send();
			return;
		}

		//sort the cities
		let query = {latitude: req.body.route[0].lat,longitude: req.body.route[0].lng};
		let cities = nearbyCities(query);

		let generatedID = crypto.randomUUID();
		let town = cities[0].name + ", " + cities[0].adminCode + ", " + countryCode.byIso(cities[0].country)?.country;
		
		usersDb.read({username: (<any>req).username}).then(async (user) =>
		{
			if(user == null)
			{
				res.status(404).send("Could not find workouts");
				return;
			}

			let workoutsArr: Workout [] = (<fitnessUser> <unknown>user).uploads;


			let serial = (await (<any> usersDb.read({currentSerial: {$exists: true}}))).currentSerial;
			(await (<any> usersDb.update({currentSerial: {$exists: true}}, {currentSerial: serial + 1})))

			workoutsArr.push(
				{
					id: serial.toString(10),
					completed: false,
					locationRecord: req.body.route,
					workoutDistance: req.body.len,
					workoutDate: new Date(),
					workoutTime: -1,
					nearestTown: town,
					isPublic: false,
				}
			)

			usersDb.update({username: (<any>req).username}, {uploads: workoutsArr}).then((success) =>
			{
				res.status(200).send(JSON.stringify({id: serial, nearestTown: town}));
				return;
			}, (err) =>
			{
				res.status(503).send("Failed to update user");
				return;
			})


		}, (err) =>
		{
			res.status(503).send("Could not find workouts");
			return;
		});
	}
	catch(e)
	{
		if(!res.headersSent)
		{
			res.status(400).send();
		}
	}

})

app.post("/submitserial", session.middleware, (req, res) =>
{
	try{
		let username = (<any>req).username;

		if(req.body.serial.length == undefined || req.body.serial.length < 1)
		{
			res.status(400).send("Cannot register an empty serial!");
			return;
		}

		usersDb.read({de1serial: req.body.serial}).then((user) =>
		{
			if(user == undefined)
			{
				usersDb.update({username: username}, {de1serial: req.body.serial}).then((success) =>
				{
					res.status(200).send("Successfully updated");
				}, (err) => 
				{
					res.status(500).send("could not update de1 serial registration.");
				});
			} 
			else 
			{
				res.status(400).send("serial number already exists!");
			}
		}, (err) => 
		{
			res.status(500).send("could not access db");
		})
	} catch(e)
	{
		if(!res.headersSent)
		{
			res.status(400).send();
		}
	}

	
})




/**
 * Endpoint to submit login details
 */
app.post("/login", (req, res) =>
{
	try{
		if(req.body == undefined)
		{
			res.status(400).send();
		}
		let user = req.body.username;

		if(user == null)
		{
			res.redirect("/login");
			res.send();
		}
		else 
		{
			//check db for user
			usersDb.read({username: user}).then((fitnessUser)=> 
			{
				if(fitnessUser == null)
				{
					res.status(401).send(`User combination does not exist, please try again. <br><a href="/login"> Go back to home page <\a>`);
					res.send();
					return;
				}
				let correctPassword = isCorrectPassword(req.body.password, (<fitnessUser> <unknown>fitnessUser).password); 
			
			if(correctPassword)
			{
				session.createSession(res, user);
				res.redirect("/");
				res.send();
				return;
			}
			else {
				res.status(401).send(`User password combination does not exist, please try again. <br><a href="/login"> Go back to home page <\a>`);
				res.send();
				return;
			}

			}, (err) => 
			{
				res.redirect("/login");
				res.send();
			});		
		}
	}
	catch(e){
		if(!res.headersSent)
		{
			res.status(400).send();
		}
	}
});

/**
 * Endpoint to register a user
 */

app.post("/register", (req, res) =>
{
	try {
		let user = req.body.username;
		let password = req.body.password;
		let confirm = req.body.passwordConfirm;

		if(user == undefined || password == undefined || confirm == undefined)
		{
			res.status(401).send(`Fields must not be blank, please try again. <br> <a href = "/register"> Go back to register page <\a>`);
			return;
		}

		//check if fields are valid
		if(user == "" || password == "" || confirm == "")
		{
			res.status(401).send(`Fields must not be blank, please try again. <br> <a href = "/register"> Go back to register page <\a>`);
			return;
		}

		if(password != confirm)
		{
			res.status(400).send(`Passwords must be the same, please try again. <br>
			<a href = "/register"> Go back to register page <\a>`);
			return;
		}

		//Check if user already exists
		usersDb.read({username: user}).then((ftUser => 
			{
				//register new user if dne
				if(ftUser == null)
				{
					usersDb.create(new fitnessUser(user, createSaltedHash(password), null)).then((success) =>
					{
						res.status(200).send(`Sucessfully Registered! <br>
						<a href = "/login"> Go back to login page <\a>`)
					}, (err) => 
					{
						res.status(500).send(`User creation failed, please try again later.<br>
						<a href = "/login"> Go back to login page <\a>`);
					})
				}
				else
				{
					res.status(401).send(`Username already exists, please try again. <br>
					<a href = "/register"> Go back to register page <\a>`);
					return;
				}
			}));
	} catch(e){
		if(!res.headersSent)
		{
			res.status(400).send();
		}	
	}
});


//Force redirect to https
// app.use((req, res, next) => {
//     req.secure ? next() : res.redirect('https://' + req.headers.host + req.url)
// })



/**
 * Static endpoints to serve HTML files. 
 */
app.use('/login', express.static(clientApp + "/login", {extensions: ['html'] }));
app.use('/register', express.static(clientApp + "/register", {extensions: ['html'] }));
app.use('/', session.middleware, express.static(clientApp, { extensions: ['html'] }));
app.use('/map', session.middleware, express.static(clientApp + "/map", { extensions: ['html'] }));
app.use('/routeDisplay', session.middleware, express.static(clientApp + "/routeDisplay", { extensions: ['html'] }));




// listen to incoming requests on port 3000
app.listen(port, () => {
  console.log(`Example app listening on port ${port}`);
})

/**
 * Listen to incoming requests on port 80
 */
app.listen(80, () => {
	console.log(`Example app listening on port ${80}`);
});


//handle errors
app.use((err, req, res, next) => {
	
	if(err instanceof session.Error)
	{
		if(req.headers.accept == "application/json")
		{
			res.status(401).send(err);
		}
		else 
		{
			res.redirect("/login");
			req.greatSuccess = false;
		}
		
	}
	else 
	{
		console.log(err);
		res.status(500).send("Not SessionError Object");
	}
})


function getPixels(fileName) {
    var data = fs.readFileSync(fileName);
    var png = (<any>PNG).sync.read(data);
    var arrayPixel = "[";
    
    // create a new PNG instance with the given width and height
    // const width = 30;
    // const height = 40;
    // const png_new = new PNG({ width: width, height: height });

    for (var i = 0; i < png.data.length; i = i + 4) {
        // var b = ((png.data[i+2]>>6)&0x00000003);
        // var g = ((png.data[i+1]>>3)&0x0000001C);
        // var r =  ((png.data[i]) &0x000000E0);
        // var color = b | g | r;

        const red = png.data[i] & 0xFF;
        const green = png.data[i+1] & 0xFF;
        const blue = png.data[i+2] & 0xFF;
      
        // Convert each color value to an 8-bit value by dividing by 32 and rounding down
        // const r = Math.round(red*8 / 255);
        // const g = Math.round(green*8 / 255);
        // const b = Math.round(blue*4 / 255);

        const r = (red >> 5) & 0x7;
        const g = (green >> 5) & 0x7;
        const b = (blue >> 6) & 0x3;
      
        // Combine the 8-bit color values into a single integer in the format of BBGGGRRR
        var color = (r << 5) | (g << 2) | b;
        color = color & 0xff;

        var c = color.toString(16);
        if (c.length === 1) {
            c = "0" + c;
        }

        // console.log(png.data[i],png.data[i+1],png.data[i+2]);
        // console.log(png.data[i].toString(16),png.data[i+1].toString(16),png.data[i+2].toString(16));
        // console.log(color,c)

        // var c = String.fromCharCode(color);
        arrayPixel = arrayPixel + c;

        // //write to png_new 
        // (<any>png_new.data)[i] = r << 5; 
        // png_new.data[i+1] = g << 5; 
        // png_new.data[i+2] = b << 6; 
        // png_new.data[i+3] = 255;

    }
    //arrayPixel = arrayPixel + "]";
    // console.log(arrayPixel);

    // write the PNG data to a file
    // const stream = fs.createWriteStream('image_8bit.png');
    // png_new.pack().pipe(stream);

    // console.log(arrayPixel);
    return arrayPixel;
}


//function to log incoming requests
function logRequest(req, res, next){
	console.log(`${new Date()}  ${req.ip} : ${req.method} ${req.path}`);
	next();
} 