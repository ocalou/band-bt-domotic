import yaml
from pydantic import BaseModel, Field
from typing import Dict


class MqttParams(BaseModel):

    host: str
    port: int
    subscribe_topic: str

class ServiceParams(BaseModel):

    delay: int

class Configuration(BaseModel):

    MQTT_PARAMS: MqttParams = Field(alias = 'mqtt')
    SERVICE_PARAMS: ServiceParams = Field(alias = 'service')

    @classmethod
    def read(cls, file: str):

        with open(file) as f:

            params: Dict = yaml.safe_load(f)

            return Configuration(**params)