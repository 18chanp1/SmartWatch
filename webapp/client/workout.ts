type Workout =  
{
    id: string;
    completed: boolean;
    locationRecord: Coordinates [];
    workoutDistance: Number;
    workoutDate: Date;
    workoutTime: Number;
    nearestTown: string;
    isPublic: boolean;
}

type Coordinates = {lat: number, lng: number};

export {Workout, Coordinates}