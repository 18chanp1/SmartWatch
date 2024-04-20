type LeaderboardEntry = 
{
    user: string,
    avgspd: number,
    dist: number,
    time: number
};

let LeaderboardObject : LeaderboardEntry[] = [];


export{LeaderboardEntry, LeaderboardObject};