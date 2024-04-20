export default class fitnessUser {
    username;
    password; //TODO salting and hashing
    workouts;
    de1serial;
    uploads;
    currentSerial;
    constructor(user, pass, wkouts) {
        this.username = user;
        this.password = pass;
        this.workouts = wkouts === null ? [] : wkouts;
        this.uploads = [];
        this.de1serial = -1;
    }
    static computeStatistics(userArray) {
        let leaderboard = [];
        for (let curr of userArray) {
            //compute avg spd
            let totaldist = 0;
            let totaltime = 0;
            for (let workout of curr.uploads) {
                if (workout.completed) {
                    totaldist += workout.workoutDistance;
                    totaltime += workout.workoutTime;
                }
            }
            let avgV = totaltime == 0 ? totaldist : totaldist / totaltime;
            let currLeaderboard = {
                user: curr.username,
                avgspd: avgV,
                dist: totaldist,
                time: totaltime
            };
            leaderboard.push(currLeaderboard);
        }
        return leaderboard;
    }
}
//# sourceMappingURL=user.js.map