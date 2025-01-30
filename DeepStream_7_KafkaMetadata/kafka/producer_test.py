import json
from kafka import KafkaProducer
 
# Create a producer with JSON serializer
producer = KafkaProducer(
    bootstrap_servers='localhost:9092',
    value_serializer=lambda v: json.dumps(v).encode('utf-8')
)
 
# Sending JSON data
producer.send('test', {'field': 'value'})

def on_send_success(record_metadata):
    print(record_metadata.topic)
    print(record_metadata.partition)
    print(record_metadata.offset)
 
def on_send_error(excp):
    print(excp)
    log.error('I am an errback', exc_info=excp)
 
# Sending a message with callbacks
producer.send('test',  {'field2': 'value2'}).add_callback(on_send_success).add_errback(on_send_error)

producer.flush()