from pydantic import BaseModel, Field
from typing import List, Optional


class BusLineAPI(BaseModel):
    """
    Schema for receiving the Bus Line information from the API
    """

    id: int = Field(alias ='id')
    line_number: str = Field(alias = 'lin_comer')
    origin: str = Field(alias = 'nombre_orig')
    destination: str = Field(alias = 'nombre_dest')

    class Config:
        populate_by_name = True
        extra = 'ignore'

class BusLine(BaseModel):
    """
    Schema for organizing the information of a Bus Line
    """

    line_number: str
    origin: str
    destination: str

    class Config:
        extra = 'ignore'

class BusInfoAPI(BaseModel):
    """
    Schema for receiving the Bus information in a specific Stop
    """

    bus: int = Field(alias = 'bus')
    time: str = Field(alias = 'tiempo')
    distance: str = Field(alias = 'distancia')

    class Config:
        populate_by_name = True
        extra = 'ignore'

class BusStopLinesAPI(BaseModel):
    """
    Schema for receving the Bus Line's list from the API
    """

    line: int = Field(alias = 'linea')
    buses: List[BusInfoAPI] = Field(alias = 'buses',
                                 default_factory = list)

    class Config:
        populate_by_name = True
        extra = 'ignore'

class BusStop(BaseModel):
    """
    Schema for receiving the list of Bus Line's available un the Bus Stop
    """

    lines: List[BusStopLinesAPI] = Field(alias = 'lineas',
                                 default_factory = list)

    class Config:
        populate_by_name = True
        extra = 'ignore'

class AllBuses(BaseModel):
    """
    Schema for receiving buses received from API
    """

    bus_lines: List[BusLineAPI] = Field(alias = 'lineas')

    class Config:
        extra = 'ignore'