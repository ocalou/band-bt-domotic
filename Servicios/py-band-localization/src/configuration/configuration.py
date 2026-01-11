import yaml
from pydantic import BaseModel, Field
from typing import Dict


class MqttParams(BaseModel):

    host: str
    port: int
    subscribe_topic: str


class Configuration(BaseModel):

    MQTT_PARAMS: MqttParams = Field(alias = 'mqtt')

    @classmethod
    def read(cls, file: str):

        with open(file) as f:

            params: Dict = yaml.safe_load(f)

            return Configuration(**params)