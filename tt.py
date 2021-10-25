import paho.mqtt.client

def on_connect(client, userdata, flags, rc):
	print('connected (%s)' % client._client_id)
	client.subscribe(topic='control/rfid', qos=2)
	client.subscribe(topic= 'entrada/ultra0', qos=2)
	client.subscribe(topic= 'calle/pir', qos=2)
	client.subscribe(topic= 'room/temperature', qos=2)
	client.subscribe(topic= 'calle/lluvia', qos=2)
	client.subscribe(topic= 'control/rfid', qos=2)
	

def on_message(client, userdata, message):
	print('------------------------------')
	print('topic: %s' % message.topic)
	print('payload: %s' % message.payload)
	print('qos: %d' % message.qos)
	top = str(message.topic)
	ms = str(message.payload.decode())
	print(top)
	print(ms)
	if top == "control/rfid" and (ms == "39-62-1b-b4" or ms == "b9-8f-7a-c1"):
		print("funciona")
		result = client.publish("porton/rfig","on")
	if top == "entrada/ultra0" and float(ms)<15.0:
		print("abrir puerta")
		result2 = client.publish("casa/entrada", "on")
	elif top == "entrada/ultra0" and float(ms)>15.0:
		print("cerrar puerta")
		result3 = client.publish("casa/entrada", "off")
	if top == "room/temperature" and float(ms)>23:
		print("Ventilador encendido")
		result4 = client.publish("casa/cuarto2", "off")
	if top == "room/temperature" and float(ms)<23:
		print("Ventilador apagado")
		result5 = client.publish("casa/cuarto2", "on")
    
	if top == "calle/pir" and(ms=="on"):
		print("Se detecto movimiento")
		result6 = client.publish("casa/entrada", "on")
	if top == "calle/pir" and(ms=="off"):
		print("No hay movimiento")
		result7 = client.publish("casa/entrada", "off")

def main():
	client = paho.mqtt.client.Client(client_id='12', clean_session=False)
	client.on_connect = on_connect
	client.on_message = on_message
	client.connect(host='192.168.2.170', port=1883)
	client.loop_forever()

if __name__ == '__main__':
	main()

sys.exit(0)