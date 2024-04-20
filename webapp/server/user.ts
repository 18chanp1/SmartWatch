import {Workout} from "./workout.js"
import { LeaderboardEntry } from "./leaderboard.js";

export default class fitnessUser 
{
    username: string;
    password: string; //TODO salting and hashing
    workouts: Workout[];
    de1serial: number;
    uploads: Workout[];
    currentSerial: number;

    constructor(user: string, pass:string, wkouts:Workout[] | null)
    {
        this.username = user;
        this.password = pass;
        this.workouts = wkouts === null ? [] : wkouts;
        this.uploads = []
        this.de1serial = -1;
    }

    static computeStatistics(userArray: fitnessUser[]): LeaderboardEntry []
    {
        let leaderboard:LeaderboardEntry[] = [];
        for(let curr of userArray)
        {
            //compute avg spd
            let totaldist: number = 0;
            let totaltime: number = 0;
            for(let workout of curr.uploads)
            {
                if(workout.completed)
                {
                    totaldist+= workout.workoutDistance;
                    totaltime+= workout.workoutTime;
                }
            }

            let avgV = totaltime == 0 ? totaldist : totaldist / totaltime;

            let currLeaderboard: LeaderboardEntry = 
            {
                user: curr.username,
                avgspd: avgV,
                dist: totaldist,
                time: totaltime
            }

            leaderboard.push(currLeaderboard);

        }

        return leaderboard;
    }

}