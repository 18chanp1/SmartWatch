type Workout =  
{
    id: string;
    completed: boolean;
    locationRecord: Coordinates [];
    workoutDistance: number;
    workoutDate: Date;
    workoutTime: number;
    nearestTown: string;
    isPublic: boolean;
    
}

type simplifiedWorkout = 
{
    id: string;
    workoutDistance: number;
    de1Serial: string;
    workoutTime: number;
}

type Coordinates = {latitude: number, longitude: number};

export {Workout, Coordinates, simplifiedWorkout}