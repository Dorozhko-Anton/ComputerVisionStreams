from pydantic_settings import BaseSettings
import json 

class AppConfig(BaseSettings):
    api_server_port: int
    redis_host: str
    redis_port: int 
    redis_stream: str 
    redis_output_interval: int
    log_level: str 
    jetson_ip: str
    video_port: int
    max_images: int
    streamer_port: int
    ingress_port: int
    segment_time: int
    openai_api_key: str
    base_url: str 
    model: str 

def load_config(config_path: str) -> AppConfig:
    with open(config_path, 'r') as file:
        config_data = json.load(file)
    return AppConfig(**config_data)