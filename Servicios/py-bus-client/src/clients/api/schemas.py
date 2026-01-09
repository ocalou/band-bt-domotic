from pydantic import BaseModel, Field
from typing import List, Dict


# Base model
class APIModel(BaseModel):
    """Common API configuration."""
    class Config:
        populate_by_name = True
        extra = 'ignore'


# Stop schemas
class BusArrival(APIModel):
    """Single bus arrival at a stop."""
    bus_id: int = Field(alias='bus')
    time: str = Field(alias='tiempo')
    distance: str = Field(alias='distancia')

class StopLine(APIModel):
    """Buses of a specific line at a stop."""
    line_id: int = Field(alias='linea')
    buses: List[BusArrival] = Field(default_factory=list)

class BusStop(APIModel):
    """All lines available at a stop."""
    lines: List[StopLine] = Field(alias='lineas', default_factory=list)

class BusStopInfo(APIModel):
    """Information of Bus Stop"""
    id: int
    name: str = Field(alias='nombre')
    x_coord: float = Field(alias='posx')
    y_coord: float = Field(alias='posy')


# Line schemas
class BusLine(APIModel):
    """Static bus line information."""
    id: int | None = None
    line_number: str = Field(alias='lin_comer')
    origin: str = Field(alias='nombre_orig')
    destination: str = Field(alias='nombre_dest')


class BusNetwork(APIModel):
    """All available bus lines."""
    lines: List[BusLine] = Field(alias='lineas')
    stops: List[BusStopInfo] = Field(alias='paradas')

# Response Buses
class BusInfoResponse(APIModel):
    """Model for buses data to send via MQTT"""
    bus: str
    time: str

class Response(APIModel):
    """Model for data to send via MQTT"""
    buses: Dict[str, List[BusInfoResponse]]
